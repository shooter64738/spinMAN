#include "c_inputs_win.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"
#include "../../../../c_configuration.h"
#include "../../../../bit_manipulation.h"
#include "../../../../bit_manipulation.h"
#include "../../../../volatile_encoder_externs.h"
#define __INPUT_VOLATILES__
#include "../../../../volatile_input_externs.h"

static const int8_t encoder_table[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
static uint8_t enc_val = 0;

std::thread HardwareAbstractionLayer::Inputs::timer2_comp_a_thread(HardwareAbstractionLayer::Inputs::timer2_compa_vect_thread);
#define CS00 0
#define CS01 1
#define CS02 2
#define CS10 0
#define CS11 1
#define CS12 2
#define CS20 0
#define CS21 1
#define CS22 2
#define WGM21 1
#define OCIE2A 1

#define DIRECTION_PIN 0	//Nano pin A0
#define MODE_PIN_A 1    //Nano pin A1
#define MODE_PIN_B 2    //Nano pin A2
#define ENABLE_PIN 3    //Nano pin A3
struct s_timer
{
	uint8_t TIMSK;
	uint8_t TCCRA;
	uint8_t TCCRB;
	uint16_t OCRA;
	uint16_t OCRB;
	uint16_t TCNT;
};
static s_timer TIMER_0;
static s_timer TIMER_1;
static s_timer TIMER_2;

volatile uint32_t local_overflow_accumulator = 0;

void HardwareAbstractionLayer::Inputs::get_set_point()
{
	if (!BitTst(extern_input__intervals, ONE_INTERVAL_BIT))
		return;//<--return if its not time

	BitClr_(extern_input__intervals, ONE_INTERVAL_BIT);

	//find factor of frequency tick over time.
	//float f_req_value = ((float)extern_input__time_count / (SET_GATE_TIME_MS/MILLISECONDS_PER_SECOND));
	//Spin::Input::Controls.step_counter = f_req_value;

}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs()
{
	uint8_t CONTROL_PORT_PIN_ADDRESS = 0;
	BitSet_(CONTROL_PORT_PIN_ADDRESS, ENABLE_PIN);
	//BitSet_(CONTROL_PORT_PIN_ADDRESS, MODE_PIN_A);
	//BitSet_(CONTROL_PORT_PIN_ADDRESS, MODE_PIN_B);

	uint8_t current = CONTROL_PORT_PIN_ADDRESS;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs(uint8_t current)
{
	//mode is read from 2 pins
	uint8_t mode_pins = 0;
	if (BitTst(current, MODE_PIN_A))
		mode_pins++;
	if (BitTst(current, MODE_PIN_B))
		mode_pins++;
	Spin::Input::Controls.in_mode = (Spin::Enums::e_drive_modes)(mode_pins + 1);

	if (BitTst(current, ENABLE_PIN))
		Spin::Input::Controls.enable = Spin::Enums::e_drive_states::Enabled;
	else
		Spin::Input::Controls.enable = Spin::Enums::e_drive_states::Disabled;

	if (BitTst(current, DIRECTION_PIN))
		Spin::Input::Controls.direction = Spin::Enums::e_directions::Forward;
	else
		Spin::Input::Controls.direction = Spin::Enums::e_directions::Reverse;
}

void HardwareAbstractionLayer::Inputs::initialize()
{
	//this makes a pseudo timer
	HardwareAbstractionLayer::Inputs::timer2_comp_a_thread.detach();
}

void HardwareAbstractionLayer::Inputs::configure()
{
	////Pin D5 (PIND5) is connected to timer1 count. We use that as a hardware counter
	////set pin to input
	//STEP_PORT_DIRECTION &= ~((1 << DDD5));//| (1<<DDD4));
	////enable pull up
	//STEP_PORT |= (1 << PORTD5);// | (1<<PORTD4);

	//Setup timer 1 as a simple counter
	TIMER_1.TCCRA = 0;	TIMER_1.TCCRB = 0;	TIMER_1.TCNT = 0;
	TIMER_1.TCCRB |= (1 << CS12) | (1 << CS11) | (1 << CS10);
	// Turn on the counter, Clock on Rise

	//Setup timer 2 as a timer for 1000ms
	TIMER_2.TCCRA = 0; TIMER_2.TCCRB = 0; TIMER_2.TCNT = 0;
	TIMER_2.TCCRB = TIMER_PRESCALE_MASK;  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
	// by setting CS22=1, CS21=0, CS20=1
	TIMER_2.TCCRA = (1 << WGM21);
	TIMER_2.OCRA = ((F_CPU / PRE_SCALER) / TIMER_FRQ_HZ) - 1;// 249; //124                     // CTC divider will divide 125Kz by 125

	TIMER_2.TIMSK = (1 << OCIE2A);

	////Set pins on port for inputs
	//CONTROl_PORT_DIRECTION &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2));
	////Enable pull ups
	//CONTROl_PORT |= (1 << DIRECTION_PIN) | (1 << MODE_PIN_A) | (1 << MODE_PIN_B) | (1 << ENABLE_PIN);


	////Set the mask to check pins PB0-PB5
	//PCMSK0 = (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);
	////Set the interrupt for PORTB (PCIE0)
	//PCICR |= (1 << PCIE0);
	//PCIFR |= (1 << PCIF0);

	////Setup encoder capture
	//DDRD &= ~(1 << DDD2);	//input mode
	//PORTD |= (1 << PORTD2);	//enable pullup
	//DDRD &= ~(1 << DDD3);	//input mode
	//PORTD |= (1 << PORTD3);	//enable pullup
	//////
	////Any change triggers
	//EICRA |= (1 << ISC00);	// Trigger on any change on INT0
	//EICRA |= (1 << ISC10);	// Trigger on any change on INT1

	////EICRA |= (1 << ISC00) | (1 << ISC01);	// Trigger on rising change on INT0
	////EICRA |= (1 << ISC10) | (1 << ISC11);	// Trigger on rising change on INT1

	////EICRA |= (1 << ISC01);	// Trigger on falling change on INT0
	////EICRA |= (1 << ISC11);	// Trigger on falling change on INT1

	////EICRA |= (1 << ISC01);	// Trigger on hi change on INT0
	////EICRA |= (1 << ISC11);	// Trigger on hi change on INT1

	//EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}

//This timer ticks every 2ms. 500 of these is 1 second

void HardwareAbstractionLayer::Inputs::update_encoder()
{

}

void HardwareAbstractionLayer::Inputs::start_wave_read()
{
	TIMER_1.TCNT = 0;
}

static void TIMER_2_COMPA_vect()
{
	//Ive stripped this ISR down to the bare minimum. It seems to run fast enough to read a 2mhz signal reliably.
	if (BitTst(TIMER_1.TIMSK, 0))
	{
		//(BitSet_(TIMER_1.TIMSK, 0));//<--clear over flow
		//local_overflow_accumulator += 256;	//<--add overflow & current count
	}

	if (pid_count_ticks >= PID_GATE_TIME_MS)
	{
		pid_count_ticks = 0;
		extern_input__intervals |= (1 << PID_INTERVAL_BIT);
	}
	if (rpm_count_ticks >= RPM_GATE_TIME_MS)
	{
		extern_encoder__ticks_at_time = 0; rpm_count_ticks = 0;
		extern_input__intervals |= (1 << RPM_INTERVAL_BIT);
	}

	if (tmr_count_ticks >= SET_GATE_TIME_MS)
	{
		extern_input__time_count = TIMER_1.TCNT + local_overflow_accumulator;
		//Leave timers enabled, just reset the counters for them
		TIMER_1.TCNT = 0;//<-- clear the counter for freq read (desired rpm)
		//TIMER_2.TCNT = 0;//<-- clear the counter for time keeping
		//tmr_count_ticks = 0;//	<--reset this to 0, but we will count this as a tick
		tmr_count_ticks = 0;
		extern_input__intervals |= (1 << ONE_INTERVAL_BIT);//<--flag that 1 second has passed
		extern_input__intervals |= (1 << RPT_INTERVAL_BIT);//<--flag that its time to report
	}

	tmr_count_ticks++;
	pid_count_ticks++;
	rpm_count_ticks++;
}

void HardwareAbstractionLayer::Inputs::timer2_compa_vect_thread()
{
	//put the thread to sleep for 1 second, and 'tick' at 1 second intervals. Thsi simulates the timer interrupt on the avr.
	while (true)
	{
		//this is a simulated pulse count from timer 1
		TIMER_1.TCNT += 2;
		spindle_encoder.position++;
		//only run a step timer tick if the 'timer' is enabled
		if (TIMER_2.TIMSK & (1 << OCIE2A))
		{
			TIMER_2_COMPA_vect();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		}
	}

}


