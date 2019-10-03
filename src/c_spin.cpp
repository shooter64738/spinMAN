/*
* c_spin.cpp
*
* Created: 9/30/2019 7:34:46 PM
* Author: Family
*/


#include "c_spin.h"
Spin::Input::Controls::s_flags Spin::Input::Controls::Control;
Spin::Input::Controls::s_pid_terms Spin::Input::Controls::as_position;
Spin::Input::Controls::s_pid_terms Spin::Input::Controls::as_velocity;

volatile uint8_t control_old_states = 255;
volatile uint8_t step_old_states = 255;

volatile uint8_t overflow_counter = 0;
//volatile uint32_t pulse_count_at_interval = 0;
volatile uint32_t  freq_count_ticks = 0;

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
volatile uint32_t enc_count = 0;
volatile uint16_t enc_ticks_in_period = 0;
volatile uint16_t rpm_count_ticks = 0;
static uint8_t enc_val = 0;
static uint8_t rpm_slot = 0;
static uint8_t pid_interval = 0;
static Spin::Input::Controls::s_pid_terms * active_pid_mode;

c_Serial Spin::Input::Controls::host_serial;

void Spin::Input::Controls::run()
{
	Spin::Input::Controls::host_serial = c_Serial(0,115200);//<-- Start serial at 115,200 baud on port 0
	Spin::Input::Controls::host_serial.print_string("proto type\r");//<-- Send hello message
	active_pid_mode = &as_velocity;
	
	while(1)
	{
		Spin::Input::Controls::update_vitals();
		if (Spin::Input::Controls::Control.Enable.Dirty == 1)
		{
			Spin::Input::Controls::host_serial.print_string("en dirty\r");
			Spin::Input::Controls::Control.Enable.Dirty = 0;
		}
		if (Spin::Input::Controls::Control.Rpm.Dirty == 1)
		{
			//Spin::Input::Controls::host_serial.print_string("rpm dirty\r");
			Spin::Input::Controls::Control.Rpm.Dirty = 0;
			//Spin::Input::Controls::update_vitals();
			Spin::Input::Controls::host_serial.print_string("reported rpm: ");
			Spin::Input::Controls::host_serial.print_float(Spin::Input::Controls::Control.Rpm.Value);
			Spin::Input::Controls::host_serial.print_string("\r");
			//count in 1 second/400*60 = rpm
			//time/1000 *400*60
			//
		}
		if (Spin::Input::Controls::Control.Direction.Dirty == 1)
		{
			Spin::Input::Controls::host_serial.print_string("dr dirty\r");
			Spin::Input::Controls::Control.Direction.Dirty = 0;
		}
		if (Spin::Input::Controls::Control.Mode.Dirty == 1)
		{
			Spin::Input::Controls::host_serial.print_string("md dirty\r");
			Spin::Input::Controls::Control.Mode.Dirty = 0;
		}
		
		if (pid_interval)
		{
			pid_interval = 0;
			update_pid(Spin::Input::Controls::Control.Step.Value, Spin::Input::Controls::Control.Rpm.Value);
			
			{
				//Spin::Input::Controls::host_serial.print_string("new pid value:");
				//Spin::Input::Controls::host_serial.print_float(active_pid_mode->Max_Val-abs(active_pid_mode->cv_control_variable));
				//Spin::Input::Controls::host_serial.print_string("\r");
				//
				//Spin::Input::Controls::host_serial.print_string("read rpm:");Spin::Input::Controls::host_serial.print_float(Spin::Input::Controls::Control.Rpm.Value);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("control rpm:");Spin::Input::Controls::host_serial.print_float(Spin::Input::Controls::Control.Step.Value);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("Kp:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->Kp);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("Ki:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->Ki);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("Kd");Spin::Input::Controls::host_serial.print_float(active_pid_mode->Kd);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("cv_control_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->cv_control_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("pv_process_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->pv_process_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("p_proportional_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->p_proportional_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("i_integral_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->i_integral_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("d_derivative_variable:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->d_derivative_variable);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("e_error_last:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->e_error_last);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("e_error:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->e_error);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("set_last:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->actual_last);Spin::Input::Controls::host_serial.print_string("\r");
				//Spin::Input::Controls::host_serial.print_string("actual_last:");Spin::Input::Controls::host_serial.print_float(active_pid_mode->set_last);Spin::Input::Controls::host_serial.print_string("\r");
				//while(1){}
			}
			OCR0A = 200;// active_pid_mode->Max_Val -abs(active_pid_mode->cv_control_variable);
		}
		if (Spin::Input::Controls::Control.Step.Dirty == 1)
		{
			//this is only here for testing. I need a 50khz input to the motor
			//drive and I am also borrowing that pulse generator as an rpm request
			//Spin::Input::Controls::Control.Step.Value = Spin::Input::Controls::Control.Step.Value/100;
			
			//Spin::Input::Controls::host_serial.print_string("requested rpm:");
			//Spin::Input::Controls::host_serial.print_int32(Spin::Input::Controls::Control.Step.Value);
			//Spin::Input::Controls::host_serial.print_string("\r");
			Spin::Input::Controls::Control.Step.Dirty = 0;
			Spin::Input::Controls::timer_re_start();
		}
		if (Spin::Input::Controls::host_serial.HasEOL())
		{
			Spin::Input::Controls::host_serial.Get();
			active_pid_mode->Kp+=0.01;
		}
		
		//OCR0A = 200;//256-active_pid_mode->cv_control_variable;
	}
}

float Spin::Input::Controls::update_pid(uint32_t target, uint32_t current)
{
	active_pid_mode->get_pid(target,current);

}

void Spin::Input::Controls::update_vitals()
{
//400/500=0.8
//0.8/400 = 0.002
//0.002 * 500 = 1
float rps=((enc_ticks_in_period/TCNT1)/encoder_ticks_per_rev)* frq_gate_time_ms;
Spin::Input::Controls::Control.Rpm.Value = rps *60;
return;
	for (int i=0;i<rpm_buffer_size;i++)
	{
		//Spin::Input::Controls::host_serial.print_int32(Spin::Input::Controls::Control.Rpm.Value_Buffer[i]);
		//Spin::Input::Controls::host_serial.print_string(", ");
		Spin::Input::Controls::Control.Rpm.Value +=	Spin::Input::Controls::Control.Rpm.Value_Buffer[i];
	}
	//Spin::Input::Controls::host_serial.print_string("\r");
	float mean =  Spin::Input::Controls::Control.Rpm.Value / rpm_buffer_size;
	Spin::Input::Controls::Control.Rpm.Value = (((float)mean / encoder_ticks_per_rev) * 60.0)*rpm_buffer_size;
	
}

void Spin::Input::Controls::initialize()
{
	Spin::Input::Controls::setup_pulse_inputs();
	Spin::Input::Controls::setup_control_inputs();
	Spin::Input::Controls::setup_encoder_capture();
	Spin::Input::Controls::set_pid_defaults();
	Spin::Input::Controls::setup_pwm_out();
	//enable interrupts
	sei();
	
}

void Spin::Input::Controls::set_pid_defaults()
{
	as_position.reset();
	as_position.Kp=0.0555;
	as_position.Ki=0.0001;
	as_position.Kd=-0.01;
	as_position.Max_Val=255;
	as_position.Min_Val=-255;
	as_position.invert_output=1;
	as_position.t_time=.25;
	
	as_velocity.reset();
	as_velocity.Kp=0.001;
	as_velocity.Ki=0.05;
	as_velocity.Kd=0.02;
	as_velocity.Max_Val=240;
	as_velocity.Min_Val=0;
	as_velocity.invert_output=1;
	as_velocity.t_time=.25;
}

void Spin::Input::Controls::setup_pulse_inputs()
{
	Spin::Input::Controls::Control.Step.Dirty = 0;        // reset

	//Pin D5 (PIND5) is connected to timer1 count. We use that as a hardware counter
	//set pin to input
	STEP_PORT_DIRECTION &= ~((1 << DDD5));//| (1<<DDD4));
	//enable pull up
	STEP_PORT |= (1<<PORTD5);// | (1<<PORTD4);
	
	Spin::Input::Controls::timer_re_start();
}

void Spin::Input::Controls::timer_re_start()
{
	overflow_counter=0;//<clear over flow counter
	freq_count_ticks = 0;//	<--reset this to 0, but count this as a tick
	enc_ticks_in_period = 0;
	rpm_count_ticks = 0;
	//rpm_slot = 0;
	
	Spin::Input::Controls::timer1_reset();
	Spin::Input::Controls::timer2_reset();
}

void Spin::Input::Controls::timer1_reset()
{
	//Setup timer 1 as a simple counter
	TCCR1A = 0;	TCCR1B = 0;	TCNT1 = 0;
	TCCR1B |= (1 << CS12) | (1 << CS11) | (1 << CS10);
	// Turn on the counter, Clock on Rise
}

void Spin::Input::Controls::timer2_reset()
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

void Spin::Input::Controls::setup_pwm_out()
{
	OCR0A = 255;
	// set PWM for 50% duty cycle
	DDRD |= (1<<PWM_OUTPUT_PIN);

	TCCR0A |= (1 << COM0A1);
	// set none-inverting mode

	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	// set fast PWM Mode

	TCCR0B |= (1 << CS00);
	// set prescaler to 8 and starts PWM
	
	return;
	
	/*
	CS02	CS01	CS00	DESCRIPTION
	0		0 		0		Timer/Counter2 Disabled
	0		0 		1 		No Prescaling
	0		1 		0 		Clock / 8
	0		1 		1 		Clock / 64
	1		0 		0 		Clock / 256
	1		0 		1 		Clock / 1024
	*/
	
	TCCR0A |= (1<<COM0A1) ;//| (1<<WGM00);
	TCCR0A |= (1<<WGM01) | (1<<WGM00);
	TCCR0B |= (1<<CS00);// | _BV(WGM02);
	
	// make sure to make OC0 pin (pin PB6 for atmega328) as output pin
	
	
	//set pwm output pin to input so that no output will go to the driver.
	//if the drive does not have an enable control line, this should effectively set the output to 0
	//DDRD &= ~(1<<PWM_OUTPUT_PIN);
	//OCR0A = 256-c_Spindle_Drive::current_output;
}

void Spin::Input::Controls::setup_control_inputs()
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

void Spin::Input::Controls::setup_encoder_capture()
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

void Spin::Input::Controls::encoder_update()
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
	Spin::Input::Controls::Control.Encoder.Value = enc_count;
	Spin::Input::Controls::Control.Encoder.Dirty = 1;
	
	//So long as the timer remains enabled we can track rpm.
	enc_ticks_in_period++;
	
	
}

//This timer ticks every 2ms. 500 of these is 1 second
ISR(TIMER2_COMPA_vect)
{
	//Grab the count value as soon as we enter. If the gate time has passed
	//we will want the most accurate reading of this we can get
	
	//Because the count (frequency) on timer 1 determines the rpm requested
	//I am not concerned with an over flow. If a 65,535 rpm speed is requested
	//then you obviously have a much higher grade machine than I intended to
	//control!

	if (rpm_count_ticks >= rpm_gate_time_ms)
	{
		Spin::Input::Controls::Control.Rpm.Value_Buffer[(++rpm_slot) & (rpm_buffer_size-1u)] = enc_ticks_in_period;
		rpm_count_ticks = 0;
		//rpm_slot ++;
		enc_ticks_in_period = 0;
		Spin::Input::Controls::Control.Rpm.Dirty = 1;//enc_ticks_in_period != 0;
		pid_interval = 1;
		//if (rpm_slot>rpm_buffer_size)
		//rpm_slot = 0;
	}
	
	if (freq_count_ticks >= frq_gate_time_ms)
	{
		Spin::Input::Controls::Control.Step.Value = TCNT1;
		TCCR1B = 0;//<--turn off counting on timer 1
		TCCR2B = 0;//<--turn off timing on timer 2
		
		
		Spin::Input::Controls::Control.Step.Dirty = 1;
		
	}

	freq_count_ticks++;
	rpm_count_ticks++;
	
}

ISR(PCINT0_vect)
{
	uint8_t updates = CONTROL_PORT_PIN_ADDRESS ^ control_old_states;
	control_old_states = CONTROL_PORT_PIN_ADDRESS;
	
	if (updates & (1 << Enable_Pin))
	{
		Spin::Input::Controls::Control.Enable.Value = (bool) (updates & (1 << Enable_Pin));
		Spin::Input::Controls::Control.Enable.Dirty = 1;//(bool) (updates & (1 << Enable_Pin));;
	}
	
	if (updates & (1 << Direction_Pin))
	{
		Spin::Input::Controls::Control.Direction.Value = (bool) (updates & (1 << Direction_Pin));
		Spin::Input::Controls::Control.Direction.Dirty = 1;//(bool) updates & (1 << Direction_Pin);
	}
	
	if (updates & (1 << Mode_Pin))
	{
		Spin::Input::Controls::Control.Mode.Value = (bool) (updates & (1 << Mode_Pin));
		Spin::Input::Controls::Control.Mode.Dirty = 1;//(bool) updates & (1 << Mode_Pin);
	}
	
};

ISR(PCINT2_vect)
{
	uint8_t updates = STEP_PORT_PIN_ADDRESS ^ step_old_states;
	
	Spin::Input::Controls::Control.Step.Value = (STEP_PORT_PIN_ADDRESS & (1 << Step_Pin));
	Spin::Input::Controls::Control.Step.Dirty = 1;// updates & (1 << Step_Pin);
	
	step_old_states = STEP_PORT_PIN_ADDRESS;
	
};

ISR (INT0_vect)
{
	//UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	Spin::Input::Controls::encoder_update();
	
}

ISR(INT1_vect)
{
	//	UDR0='b';
	Spin::Input::Controls::encoder_update();
}