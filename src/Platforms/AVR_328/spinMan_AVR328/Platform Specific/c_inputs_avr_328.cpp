#include "c_inputs_avr_328.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"
#include "../../../../c_configuration.h"
#include "../../../../bit_manipulation.h"
#include "volatile_encoder_externs.h"
#define __INPUT_VOLATILES__
#include "volatile_input_externs.h"


void HardwareAbstractionLayer::Inputs::get_rpm()
{
	//In velocity mode we only care if the sensed rpm matches the input rpm
	//In position mode we only care if the sensed position matches the input position.

	BitClr_(intervals, RPM_INTERVAL_BIT);
	uint32_t mean_enc = 0;
	for (int i=0;i<ENCODER_SUM_ARRAY_SIZE;i++)
	{
		mean_enc +=enc_sum_array[i];
	}

	mean_enc = mean_enc/ENCODER_SUM_ARRAY_SIZE;
	//doing some scaling up and down trying to avoid float math as much as possible.
	int32_t rps = ((mean_enc * TIMER_FRQ_HZ) * INV_ENCODER_TICKS_PER_REV * 100 * 60) / 1000;
	//multiiply rps *60 to get rpm.
	Spin::Input::Controls.sensed_rpm = rps;
}

void HardwareAbstractionLayer::Inputs::get_set_point()
{
	BitClr_(intervals,ONE_INTERVAL_BIT);
	
	//find factor of frequency tick over time.
	float f_req_value = ((float)_ref_timer_count / (SET_GATE_TIME_MS/MILLISECONDS_PER_SECOND));
	Spin::Input::Controls.step_counter = f_req_value;
	
	
}

void HardwareAbstractionLayer::Inputs::initialize()
{
	
	mic++;
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
	TCCR2B = TIMER_PRESCALE_MASK;//(1<<CS22) | (1<<CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
	// by setting CS22=1, CS21=0, CS20=1
	TCCR2A = (1<<WGM21);
	OCR2A = ((F_CPU / PRE_SCALER) / TIMER_FRQ_HZ) - 1; //249; //124                     // CTC divider will divide 125Kz by 125
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
	
	//HardwareAbstractionLayer::Inputs::configure_encoder_quadrature();
	////Setup encoder capture
	//DDRD &= ~(1 << DDD2);	//input mode
	//PORTD |= (1 << PORTD2);	//enable pullup
	//DDRD &= ~(1 << DDD3);	//input mode
	//PORTD |= (1 << PORTD3);	//enable pullup
	//////
	////Any change triggers
	//EICRA |= (1 << ISC00);	// Trigger on any change on INT0 PD2 (pin D2)
	//EICRA |= (1 << ISC10);	// Trigger on any change on INT1 PD3 (pin D3)
	
	//EICRA |= (1 << ISC00) | (1 << ISC01);	// Trigger on rising change on INT0
	//EICRA |= (1 << ISC10) | (1 << ISC11);	// Trigger on rising change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on falling change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on falling change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on hi change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on hi change on INT1
	
	//EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
}



void HardwareAbstractionLayer::Inputs::synch_hardware_inputs()
{
	//This method is called by the main program, and doesnt know which
	//port or pins to read, so it grabs it and passed it to the overlaoded
	//version
	uint8_t current = CONTROL_PORT_PIN_ADDRESS ;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs(uint8_t current)
{
	//mode is read from 2 pins
	uint8_t mode_pins = BitTst(current, MODE_PIN_A);
	mode_pins += BitTst(current, MODE_PIN_B);
	Spin::Input::Controls.in_mode = (Spin::Enums::e_drive_modes)mode_pins;
	Spin::Input::Controls.enable = (Spin::Enums::e_drive_states)BitTst(current, ENABLE_PIN);
	Spin::Input::Controls.direction = (Spin::Enums::e_directions)BitTst(current, DIRECTION_PIN);
}

uint8_t HardwareAbstractionLayer::Inputs::get_intervals()
{
	uint8_t _intervals = intervals;
	return _intervals;
}

void HardwareAbstractionLayer::Inputs::check_intervals()
{
	uint8_t _intervals = intervals;

	Spin::Controller::one_interval = BitTst(_intervals,ONE_INTERVAL_BIT);
	Spin::Controller::pid_interval = BitTst(_intervals,PID_INTERVAL_BIT);
	
	if (BitTst(intervals,RPM_INTERVAL_BIT))
	HardwareAbstractionLayer::Inputs::get_rpm();
	
	if (Spin::Controller::one_interval)
	{
		HardwareAbstractionLayer::Inputs::get_set_point();
	}
	
	
}




ISR (PCINT0_vect)
{
	uint8_t current = CONTROL_PORT_PIN_ADDRESS ;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
	
	if (BitTst(current, INDEX_PIN_ON_TIMER))
	{
		//enc_active_channels |= ENC_CHZ_TRK_BIT;
	}
	
}

//This timer ticks every 1ms. 1000 of these is 1 second
ISR(TIMER2_COMPA_vect)
{
	//Ive stripped this ISR down to the bare minimum. It seems to run fast enough to read a 2mhz signal reliably.


	if (pid_count_ticks >= PID_GATE_TIME_MS)
	{
		pid_count_ticks = 0;
		intervals |=(1<<PID_INTERVAL_BIT);
	}
	if (rpm_count_ticks >= RPM_GATE_TIME_MS)
	{
		enc_sum_array[(++enc_sum_array_head) & ENCODER_SUM_SIZE_MSK] = enc_ticks_at_current_time;
		_ref_enc_count = enc_ticks_at_current_time;
		enc_ticks_at_current_time = 0; rpm_count_ticks = 0;
		intervals |=(1<<RPM_INTERVAL_BIT);
	}

	if (tmr_count_ticks >= SET_GATE_TIME_MS)
	{
		_ref_timer_count = TCNT1;
		//Leave timers enabled, just reset the counters for them
		TCNT1 = 0;//<-- clear the counter for freq read (desired rpm)
		//TIMER_2.TCNT = 0;//<-- clear the counter for time keeping
		//tmr_count_ticks = 0;//	<--reset this to 0, but we will count this as a tick
		tmr_count_ticks = 0;
		intervals |=(1<<ONE_INTERVAL_BIT);
	}

	tmr_count_ticks++;
	pid_count_ticks++;
	rpm_count_ticks++;
}

//ISR (INT0_vect)
//{
	////UDR0='a';
	////c_Encoder_RPM::Encoder_Trigger();
	//enc_active_channels |= ENC_CHA_TRK_BIT;
	//Spin::Configuration::Drive_Settings.Encoder_Config.call_vect();
	////HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
	//
//}
//
//ISR(INT1_vect)
//{
	////	UDR0='b';
	//enc_active_channels |= ENC_CHB_TRK_BIT;
	//Spin::Configuration::Drive_Settings.Encoder_Config.call_vect();
	////HardwareAbstractionLayer::Inputs::update_encoder_for_quad();
//}

