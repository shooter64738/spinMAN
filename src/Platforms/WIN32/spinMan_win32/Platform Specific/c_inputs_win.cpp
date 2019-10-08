#include "c_inputs_win.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"

volatile uint32_t tmr_count_ticks = 0;


volatile uint16_t pid_count_ticks = 0;
volatile uint16_t rpm_count_ticks = 0;
volatile uint16_t freq_interval = 0;
volatile uint16_t _ref_timer_count = 0;
volatile uint16_t _ref_enc_count = 0;
volatile uint8_t intervals = 0;

volatile uint16_t enc_ticks_at_current_time = 0;
volatile uint32_t enc_count = 0;
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

volatile uint32_t enc_sum_array[ENCODER_SUM_ARRAY_SIZE];
volatile uint8_t enc_sum_array_head = 0;

void HardwareAbstractionLayer::Inputs::get_rpm()
{
	//In velocity mode we only care if the sensed rpm matches the input rpm
	//In position mode we only care if the sensed position matches the input position.

	BitClr_(intervals, RPM_INTERVAL_BIT);
	uint32_t mean_enc = 0;
	for (int i = 0; i<ENCODER_SUM_ARRAY_SIZE; i++)
	{
		mean_enc += enc_sum_array[i];
	}

	mean_enc = mean_enc / ENCODER_SUM_ARRAY_SIZE;
	//doing some scaling up and down trying to avoid float math as much as possible.
	int32_t rps = ((mean_enc * TIMER_FRQ_HZ) * INV_ENCODER_TICKS_PER_REV * 100 * 60) / 1000;
	//multiiply rps *60 to get rpm.
	Spin::Input::Controls.sensed_rpm = _ref_enc_count;
}


void HardwareAbstractionLayer::Inputs::get_set_point()
{
	BitClr_(intervals, ONE_INTERVAL_BIT);
	//find factor of frequency tick over time.
	float f_req_value = ((float)_ref_timer_count / (SET_GATE_TIME_MS / MILLISECONDS_PER_SECOND));
	Spin::Input::Controls.step_counter = f_req_value;
}

void HardwareAbstractionLayer::Inputs::check_intervals()
{
	uint8_t _intervals = intervals;

	Spin::Controller::one_interval = BitTst(_intervals, ONE_INTERVAL_BIT);
	Spin::Controller::pid_interval = BitTst(_intervals, PID_INTERVAL_BIT);

	if (BitTst(intervals, RPM_INTERVAL_BIT))
		HardwareAbstractionLayer::Inputs::get_rpm();
	if (BitTst(intervals, ONE_INTERVAL_BIT))
		HardwareAbstractionLayer::Inputs::get_set_point();
	//Spin::Input::Controls.sensed_position = enc_count;
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs()
{

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


	if (pid_count_ticks >= PID_GATE_TIME_MS)
	{
		pid_count_ticks = 0;
		intervals |= (1 << PID_INTERVAL_BIT);
	}
	if (rpm_count_ticks >= RPM_GATE_TIME_MS)
	{
		enc_sum_array[(++enc_sum_array_head) & ENCODER_SUM_SIZE_MSK] = 9;
		
		_ref_enc_count = enc_ticks_at_current_time;
		enc_ticks_at_current_time = 0; rpm_count_ticks = 0;
		intervals |= (1 << RPM_INTERVAL_BIT);
	}

	if (tmr_count_ticks >= SET_GATE_TIME_MS)
	{
		_ref_timer_count = TIMER_1.TCNT;
		//Leave timers enabled, just reset the counters for them
		TIMER_1.TCNT = 0;//<-- clear the counter for freq read (desired rpm)
		//TIMER_2.TCNT = 0;//<-- clear the counter for time keeping
		//tmr_count_ticks = 0;//	<--reset this to 0, but we will count this as a tick
		tmr_count_ticks = 0;
		intervals |= (1 << ONE_INTERVAL_BIT);
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

		//only run a step timer tick if the 'timer' is enabled
		if (TIMER_2.TIMSK & (1 << OCIE2A))
		{
			TIMER_2_COMPA_vect();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

		}
	}

}


