#include "c_inputs_avr_328.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"

volatile uint32_t freq_count_ticks = 0;


volatile uint16_t pid_count_ticks = 0;
volatile uint16_t freq_interval = 0;
volatile uint16_t _ref_timer_count = 0;

volatile uint16_t enc_ticks_at_current_time = 0;
volatile uint32_t enc_count = 0;
static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static uint8_t enc_val = 0;


void HardwareAbstractionLayer::Inputs::get_rpm()
{
	if (freq_count_ticks>0)
	{
		float timer_ticks_at_current_time = _ref_timer_count;
		//find factor of encoder tick over time.
		float f_tcnt1_encoder = ((float)enc_ticks_at_current_time/(float)freq_count_ticks);
		//find factor of frequency tick over time.
		float f_tcnt1_req_speed = ((float)timer_ticks_at_current_time/(float)freq_count_ticks)*FRQ_GATE_TIME_MS;
		//calculate number of encoder ticks this would equate to, and calculate
		//how many revolutions that would be in 1 second
		float rps=(f_tcnt1_encoder/ENCODER_TICKS_PER_REV)* FRQ_GATE_TIME_MS;
		//float sig=(f_tcnt1_req_speed/fre)* frq_gate_time_ms;
		//multiiply rps *60 to get rpm.
		Spin::Input::Controls.sensed_rpm = rps *60.0;
		Spin::Input::Controls.step_counter = f_tcnt1_req_speed;
		//Spin::Input::Controls::host_serial.print_string("\r");
	}
}

void HardwareAbstractionLayer::Inputs::initialize()
{
	
}

void HardwareAbstractionLayer::Inputs::configure()
{
	//Pin D5 (PIND5) is connected to timer1 count. We use that as a hardware counter
	//set pin to input
	STEP_PORT_DIRECTION &= ~((1 << DDD5));//| (1<<DDD4));
	//enable pull up
	STEP_PORT |= (1<<PORTD5);// | (1<<PORTD4);
	
	//Setup timer 1 as a simple counter
	TCCR1A = 0;	TCCR1B = 0;	TCNT1 = 0;
	TCCR1B |= (1 << CS12) | (1 << CS11) | (1 << CS10);
	// Turn on the counter, Clock on Rise
	
	//Setup timer 2 as a timer for 1000ms
	TCCR2A=0; TCCR2B=0; TCNT2=0;
	TCCR2B = (1<<CS22) | (1<<CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
	// by setting CS22=1, CS21=0, CS20=1
	TCCR2A = (1<<WGM21);
	OCR2A = 249; //124                     // CTC divider will divide 125Kz by 125
	GTCCR |= (1<<PSRASY);
	TIMSK2 = (1<<OCIE2A);
	
	//Set pins on port for inputs
	CONTROl_PORT_DIRECTION &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2));
	//Enable pull ups
	CONTROl_PORT |= (1<<DIRECTION_PIN)|(1<<MODE_PIN_A) |(1<<MODE_PIN_B) |(1<<ENABLE_PIN);
	
	
	//Set the mask to check pins PB0-PB5
	PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2)|(1<<PCINT3);
	//Set the interrupt for PORTB (PCIE0)
	PCICR |= (1<<PCIE0);
	PCIFR |= (1<<PCIF0);
	
	//Setup encoder capture
	DDRD &= ~(1 << DDD2);	//input mode
	PORTD |= (1 << PORTD2);	//enable pullup
	DDRD &= ~(1 << DDD3);	//input mode
	PORTD |= (1 << PORTD3);	//enable pullup
	////
	//Any change triggers
	EICRA |= (1 << ISC00);	// Trigger on any change on INT0
	EICRA |= (1 << ISC10);	// Trigger on any change on INT1
	
	//EICRA |= (1 << ISC00) | (1 << ISC01);	// Trigger on rising change on INT0
	//EICRA |= (1 << ISC10) | (1 << ISC11);	// Trigger on rising change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on falling change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on falling change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on hi change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on hi change on INT1
	
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs()
{
	uint8_t current = CONTROL_PORT_PIN_ADDRESS ;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs(uint8_t current)
{
	//mode is read from 2 pins
	uint8_t mode_pins = BitTst(current, MODE_PIN_A);
	mode_pins += BitTst(current, MODE_PIN_B);
	Spin::Input::Controls.in_mode = (Spin::Controller::e_drive_modes)mode_pins;
	Spin::Input::Controls.enable = (Spin::Controller::e_drive_states)BitTst(current, ENABLE_PIN);
	Spin::Input::Controls.direction = (Spin::Controller::e_directions)BitTst(current, DIRECTION_PIN);
}

ISR (PCINT0_vect)
{
	uint8_t current = CONTROL_PORT_PIN_ADDRESS ;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
	
}

//This timer ticks every 2ms. 500 of these is 1 second
ISR(TIMER2_COMPA_vect)
{
	//Ive stripped this ISR down to the bare minimum. It seems to run fast enough to read a 2mhz signal reliably.
	_ref_timer_count = TCNT1;
	
	if (pid_count_ticks >= RPM_GATE_TIME_MS)
	{
		Spin::Controller::pid_interval = 1;
		pid_count_ticks = 0;
	}
	
	if (freq_count_ticks >= FRQ_GATE_TIME_MS)
	{
		//Leave timers enabled, just reset the counters for them
		//Spin::Control::Input::Actions.Step.Value = TCNT1;
		//TCCR1B = 0;//<--turn off counting on timer 1
		//TCCR2B = 0;//<--turn off timing on timer 2
		
		TCNT1 = 0;//<-- clear the counter for freq read (desired rpm)
		TCNT2 = 0;//<-- clear the counter for time keeping
		freq_count_ticks = 0;//	<--reset this to 0, but we will count this as a tick
		enc_ticks_at_current_time = 0;
		return;
		
	}

	freq_count_ticks++;
	pid_count_ticks++;
}


void HardwareAbstractionLayer::Inputs::update_encoder()
{
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	int8_t encoder_direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	enc_count += encoder_direction;
	
	if (enc_count<= 0)
	enc_count = ENCODER_TICKS_PER_REV;
	else if (enc_count >=ENCODER_TICKS_PER_REV)
	enc_count = 0;

	Spin::Input::Controls.encoder_count = enc_count;
	
	//So long as the timer remains enabled we can track rpm.
	enc_ticks_at_current_time++;
	
	
}

ISR (INT0_vect)
{
	//UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	HardwareAbstractionLayer::Inputs::update_encoder();
	
}

ISR(INT1_vect)
{
	//	UDR0='b';
	HardwareAbstractionLayer::Inputs::update_encoder();
}

