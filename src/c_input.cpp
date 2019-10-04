/*
* c_input.cpp
*
* Created: 10/3/2019 4:14:44 PM
* Author: jeff_d
*/


#include "c_input.h"
#include "c_controller.h"

Spin::Control::Input::s_flags Spin::Control::Input::Actions;


volatile uint8_t control_old_states = 255;
volatile uint8_t step_old_states = 255;

volatile uint32_t freq_count_ticks = 0;
volatile uint32_t enc_count = 0;
volatile uint16_t enc_ticks_in_period = 0;
volatile uint16_t pid_count_ticks = 0;


static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static uint8_t enc_val = 0;
//static uint8_t rpm_slot = 0;



void Spin::Control::Input::update_rpm()
{
	if (freq_count_ticks>0)
	{
		float timer_count = TCNT1;
		//find factor of encoder tick over time.
		float f_tcnt1_encoder = ((float)enc_ticks_in_period/(float)freq_count_ticks);
		//find factor of frequency tick over time.
		float f_tcnt1_req_speed = ((float)timer_count/(float)freq_count_ticks)*frq_gate_time_ms;
		//calculate number of encoder ticks this would equate to, and calculate
		//how many revolutions that would be in 1 second
		float rps=(f_tcnt1_encoder/encoder_ticks_per_rev)* frq_gate_time_ms;
		//float sig=(f_tcnt1_req_speed/fre)* frq_gate_time_ms;
		//multiiply rps *60 to get rpm.
		Spin::Control::Input::Actions.Rpm.Value = rps *60.0;
		Spin::Control::Input::Actions.Step.Value = f_tcnt1_req_speed;
		//Spin::Input::Controls::host_serial.print_string("\r");
	}
	return;
}

void Spin::Control::Input::initialize()
{
	Spin::Control::Input::setup_pulse_inputs();
	Spin::Control::Input::setup_control_inputs();
	Spin::Control::Input::setup_encoder_capture();
	
	Spin::Controller::host_serial.print_string("input initialized\r");//<-- Send hello message
}

void Spin::Control::Input::setup_pulse_inputs()
{
	Spin::Control::Input::Actions.Step.Dirty = 0;        // reset

	//Pin D5 (PIND5) is connected to timer1 count. We use that as a hardware counter
	//set pin to input
	STEP_PORT_DIRECTION &= ~((1 << DDD5));//| (1<<DDD4));
	//enable pull up
	STEP_PORT |= (1<<PORTD5);// | (1<<PORTD4);
	
	Spin::Control::Input::timer_re_start();
}

void Spin::Control::Input::timer_re_start()
{
	freq_count_ticks = 0;//	<--reset this to 0, but count this as a tick
	enc_ticks_in_period = 0;
	pid_count_ticks = 0;
	//rpm_slot = 0;
	
	Spin::Control::Input::timer1_reset();
	Spin::Control::Input::timer2_reset();
}

void Spin::Control::Input::timer1_reset()
{
	//Setup timer 1 as a simple counter
	TCCR1A = 0;	TCCR1B = 0;	TCNT1 = 0;
	TCCR1B |= (1 << CS12) | (1 << CS11) | (1 << CS10);
	// Turn on the counter, Clock on Rise
}

void Spin::Control::Input::timer2_reset()
{
	//Setup timer 2 as a timer for 1000ms
	TCCR2A=0; TCCR2B=0; TCNT2=0;
	TCCR2B = (1<<CS22) | (1<<CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
	// by setting CS22=1, CS21=0, CS20=1
	TCCR2A = (1<<WGM21);
	OCR2A = 249; //124                     // CTC divider will divide 125Kz by 125
	GTCCR |= (1<<PSRASY);
	TIMSK2 = (1<<OCIE2A);
}

void Spin::Control::Input::setup_control_inputs()
{
	//Set pins on port for inputs
	CONTROl_PORT_DIRECTION &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2));
	//Enable pull ups
	CONTROl_PORT |= (1<<Direction_Pin)|(1<<Mode_Pin)|(1<<Enable_Pin);
	//Set the mask to check pins PB0-PB5
	PCMSK0 = (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2);
	//Set the interrupt for PORTB (PCIE0)
	PCICR |= (1<<PCIE0);
	PCIFR |= (1<<PCIF0);
	//Moved step PCINT to its own ISR because it will probably get hit the most frequently
	//and cause the biggest performance hit.
	
	//set pin to input
	STEP_PORT_DIRECTION &= ~((1 << DDD5));
	//enable pullup
	STEP_PORT |= (1<<Step_Pin_on_Timer);
	
	////set pin to input
	//STEP_PORT_DIRECTION &= ~((1 << DDD7));
	////enable pullup
	//STEP_PORT |= (1<<Step_Pin);
	//PCMSK2 = (1<<PCINT23);
	//
	////Set the interrupt for PORTD (PCIE2)
	//PCICR |= (1<<PCIE2);
	////Set interrupt flag register.
	//PCIFR|=(1<<PCIF2);
}

void Spin::Control::Input::setup_encoder_capture()
{
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

void Spin::Control::Input::encoder_update()
{
	
	//////////////////////////////////////////////////////////////////////////
	// static allows this value to persist across function calls
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	int8_t encoder_direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	enc_count += encoder_direction;
	
	if (enc_count<= 0)
	enc_count = encoder_ticks_per_rev;
	else if (enc_count >=encoder_ticks_per_rev)
	enc_count = 0;
	//////////////////////////////////////////////////////////////////////////
	Spin::Control::Input::Actions.Encoder.Value = enc_count;
	Spin::Control::Input::Actions.Encoder.Dirty = 1;
	
	//So long as the timer remains enabled we can track rpm.
	enc_ticks_in_period++;
	
	
}

//This timer ticks every 2ms. 500 of these is 1 second
ISR(TIMER2_COMPA_vect)
{
	//Ive stripped this ISR down to the bare minimum. It seems to run fast enough to read a 2mhz signal reliably.
	
	if (pid_count_ticks >= rpm_gate_time_ms)
	{
		Spin::Controller::pid_interval = 1;
		pid_count_ticks = 0;
	}
	if (freq_count_ticks >= frq_gate_time_ms)
	{
		//Spin::Control::Input::Actions.Step.Value = TCNT1;
		TCCR1B = 0;//<--turn off counting on timer 1
		TCCR2B = 0;//<--turn off timing on timer 2
		
		Spin::Control::Input::Actions.Rpm.Dirty = 1;//enc_ticks_in_period != 0;
		Spin::Control::Input::Actions.Step.Dirty = 1;
		
	}

	freq_count_ticks++;
	pid_count_ticks++;
}

ISR(PCINT0_vect)
{
	uint8_t updates = CONTROL_PORT_PIN_ADDRESS ^ control_old_states;
	control_old_states = CONTROL_PORT_PIN_ADDRESS;
	
	if (updates & (1 << Enable_Pin))
	{
		Spin::Control::Input::Actions.Enable.Value = (control_old_states & (1 << Enable_Pin));
		Spin::Control::Input::Actions.Enable.Dirty = 1;//(bool) (updates & (1 << Enable_Pin));;
	}
	
	if (updates & (1 << Direction_Pin))
	{
		Spin::Control::Input::Actions.Direction.Value = (bool) (updates & (1 << Direction_Pin));
		Spin::Control::Input::Actions.Direction.Dirty = 1;//(bool) updates & (1 << Direction_Pin);
	}
	
	if (updates & (1 << Mode_Pin))
	{
		Spin::Control::Input::Actions.Mode.Value = (bool) (updates & (1 << Mode_Pin));
		Spin::Control::Input::Actions.Mode.Dirty = 1;//(bool) updates & (1 << Mode_Pin);
	}
	
};

ISR(PCINT2_vect)
{
	uint8_t updates = STEP_PORT_PIN_ADDRESS ^ step_old_states;
	
	Spin::Control::Input::Actions.Step.Value = (STEP_PORT_PIN_ADDRESS & (1 << Step_Pin));
	Spin::Control::Input::Actions.Step.Dirty = 1;// updates & (1 << Step_Pin);
	
	step_old_states = STEP_PORT_PIN_ADDRESS;
	
};

ISR (INT0_vect)
{
	//UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	Spin::Control::Input::encoder_update();
	
}

ISR(INT1_vect)
{
	//	UDR0='b';
	Spin::Control::Input::encoder_update();
}