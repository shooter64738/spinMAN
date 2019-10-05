#include "c_inputs_win.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"

volatile uint32_t freq_count_ticks = 0;


volatile uint16_t pid_count_ticks = 0;
volatile uint16_t freq_interval = 0;
volatile uint16_t _ref_timer_count = 0;

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


void HardwareAbstractionLayer::Inputs::get_rpm()
{
	if (freq_count_ticks>0)	
	{
		float timer_ticks_at_current_time = _ref_timer_count;
		//find factor of encoder tick over time.
		float f_tcnt1_encoder = ((float)enc_ticks_at_current_time / (float)freq_count_ticks);
		//find factor of frequency tick over time.
		float f_tcnt1_req_speed = ((float)timer_ticks_at_current_time / (float)freq_count_ticks)*FRQ_GATE_TIME_MS;
		//calculate number of encoder ticks this would equate to, and calculate
		//how many revolutions that would be in 1 second
		float rps = (f_tcnt1_encoder / ENCODER_TICKS_PER_REV)* FRQ_GATE_TIME_MS;
		//float sig=(f_tcnt1_req_speed/fre)* frq_gate_time_ms;
		//multiiply rps *60 to get rpm.
		//Spin::Input::Controls.sensed_rpm = rps *60.0;
		Spin::Input::Controls.step_counter = f_tcnt1_req_speed;
		if (f_tcnt1_req_speed > 600)
		{
			int x = 0;
		}
		//Spin::Input::Controls::host_serial.print_string("\r");
	}
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
	TIMER_2.TCCRB = (1 << CS22) | (1 << CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
	// by setting CS22=1, CS21=0, CS20=1
	TIMER_2.TCCRA = (1 << WGM21);
	TIMER_2.OCRA = 249; //124                     // CTC divider will divide 125Kz by 125
	
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


static void TIMER_2_COMPA_vect()
{
	//Ive stripped this ISR down to the bare minimum. It seems to run fast enough to read a 2mhz signal reliably.
	_ref_timer_count = TIMER_1.TCNT;

	if (pid_count_ticks >= RPM_GATE_TIME_MS)
	{
		Spin::Controller::pid_interval = 1;
		pid_count_ticks = 0;
		Spin::Input::Controls.sensed_rpm++;
	}

	if (freq_count_ticks >= FRQ_GATE_TIME_MS)
	{
		//Leave timers enabled, just reset the counters for them
		//Spin::Control::Input::Actions.Step.Value = TCNT1;
		//TCCR1B = 0;//<--turn off counting on timer 1
		//TCCR2B = 0;//<--turn off timing on timer 2

		TIMER_1.TCNT = 0;//<-- clear the counter for freq read (desired rpm)
		TIMER_2.TCNT = 0;//<-- clear the counter for time keeping
		freq_count_ticks = 0;//	<--reset this to 0, but we will count this as a tick
		enc_ticks_at_current_time = 0;
		return;
	}

	freq_count_ticks++;
	pid_count_ticks++;
}

void HardwareAbstractionLayer::Inputs::timer2_compa_vect_thread()
{
	//put the thread to sleep for 1 second, and 'tick' at 1 second intervals. Thsi simulates the timer interrupt on the avr.
	while (true)
	{
		//this is a simulated pulse count from timer 1
		TIMER_1.TCNT++;
		//only run a step timer tick if the 'timer' is enabled
		if (TIMER_2.TIMSK & (1 << OCIE2A))
		{
			TIMER_2_COMPA_vect();
			std::this_thread::sleep_for(std::chrono::milliseconds (1));

		}
	}

}


