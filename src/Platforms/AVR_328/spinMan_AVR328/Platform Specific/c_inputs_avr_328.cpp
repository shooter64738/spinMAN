#include "c_inputs_avr_328.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"
#include "../../../../c_configuration.h"
#include "../../../../bit_manipulation.h"
#include "..\..\..\..\volatile_encoder_externs.h"
#define __INPUT_VOLATILES__
#include "..\..\..\..\volatile_input_externs.h"

volatile uint32_t local_overflow_accumulator = 0;



void HardwareAbstractionLayer::Inputs::get_set_point()
{
	if (!BitTst(extern_input__intervals,ONE_INTERVAL_BIT))
	return;//<--return if its not time
	
	BitClr_(extern_input__intervals,ONE_INTERVAL_BIT);
	
	//find factor of frequency tick over time.
	//float f_req_value = ((float)extern_input__time_count / (SET_GATE_TIME_MS/MILLISECONDS_PER_SECOND));
	//Spin::Input::Controls.step_counter = f_req_value;
	
	
}

void HardwareAbstractionLayer::Inputs::initialize()
{
	
}

void HardwareAbstractionLayer::Inputs::configure_signal_input_timer()
{
	//Setup timer 1 as a simple counter
	IN_TCCRA = 0;	IN_TCCRB = 0;	IN_TCNT = 0;
	IN_TCCRB |= (1 << CS12) | (1 << CS11) | (1 << CS10);
	// Turn on the counter, Clock on Rise
}

void HardwareAbstractionLayer::Inputs::configure_interval_timer()
{
	//Setup timer 2 as a timer for 1000ms
	TCCR2A=0; TCCR2B=0; TCNT2=0;
	TCCR2B = TIMER_PRESCALE_MASK;//(1<<CS22) | (1<<CS20);  // set prescale factor of counter2 to 128 (16MHz/128 = 125000Hz)
	// by setting CS22=1, CS21=0, CS20=1
	TCCR2A = (1<<WGM21);
	OCR2A = ((F_CPU / PRE_SCALER) / TIMER_FRQ_HZ) - 1; //249; //124                     // CTC divider will divide 125Kz by 125
	GTCCR |= (1<<PSRASY);
	TIMSK2 = (1<<OCIE2A);
}

void HardwareAbstractionLayer::Inputs::configure()
{
	//Pin D5 (PIND5) is connected to timer1 count. We use that as a hardware counter
	//set pin to input
	//STEP_PORT_DIRECTION &= ~((1 << DDD5));//| (1<<DDD4));
	////enable pull up
	//STEP_PORT |= (1<<PORTD5);// | (1<<PORTD4);
	
	HardwareAbstractionLayer::Inputs::configure_signal_input_timer();
	HardwareAbstractionLayer::Inputs::configure_interval_timer();
	//Set pins on port for inputs
	CONTROl_PORT_DIRECTION &= ~((1 << DIRECTION_PIN) | (1 << MODE_PIN_A) | (1 << MODE_PIN_B) | (1<<ENABLE_PIN));
	//Enable pull ups
	CONTROl_PORT |= ((1<<DIRECTION_PIN)|(1<<MODE_PIN_A) |(1<<MODE_PIN_B) |(1<<ENABLE_PIN));
	//Set the mask to check pins PB0-PB5
	PCMSK1 = ((1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10)|(1<<PCINT11));
	
	//Set the interrupt for PORTC (PCIE1)
	PCICR |= (1<<PCIE1);
	PCIFR |= (1<<PCIF1);
}



void HardwareAbstractionLayer::Inputs::synch_hardware_inputs()
{
	//This method is called by the main program, and doesnt know which
	//port or pins to read, so it grabs it and passes it to the overlaoded
	//version
	uint8_t current = CONTROL_PORT_PIN_ADDRESS ;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs(uint8_t current)
{
	//mode is read from 2 pins
	uint8_t mode_pins = 0;
	if (BitTst(current, MODE_PIN_A))
	mode_pins ++;
	if (BitTst(current, MODE_PIN_B))
	mode_pins ++;
	
	mode_pins++;
	Spin::Input::Controls.in_mode = (Spin::Enums::e_drive_modes)mode_pins;
	
	if (BitTst(current, ENABLE_PIN))
	Spin::Input::Controls.enable = Spin::Enums::e_drive_states::Enabled;
	else
	Spin::Input::Controls.enable = Spin::Enums::e_drive_states::Disabled;

	if (BitTst(current, DIRECTION_PIN))
	Spin::Input::Controls.direction = Spin::Enums::e_directions::Forward;
	else
	Spin::Input::Controls.direction = Spin::Enums::e_directions::Reverse;
	
	Spin::Input::Controls.in_mode = (Spin::Enums::e_drive_modes)mode_pins;
	
}

ISR (PCINT1_vect)
{
	uint8_t current = CONTROL_PORT_PIN_ADDRESS ;
	HardwareAbstractionLayer::Inputs::synch_hardware_inputs(current);
	
}

//This timer ticks every 1ms. 1000 of these is 1 second
ISR(TIMER2_COMPA_vect)
{
	if (BitTst(IN_TIFR,IN_TOV))
	{
		(BitSet_(IN_TIFR,IN_TOV));//<--clear over flow
		local_overflow_accumulator += 256;	//<--add overflow & current count
	}
	sei();
	//Ive stripped this ISR down to the bare minimum. It seems to run fast enough to read a 2mhz signal reliably.
	if (pid_count_ticks >= PID_GATE_TIME_MS)
	{
		pid_count_ticks = 0;
		extern_input__intervals |=(1<<PID_INTERVAL_BIT);
	}
	if (rpm_count_ticks >= RPM_GATE_TIME_MS)
	{
		/*
		Encoder has been counting ticks while waiting on timer to reach rpm gate.
		Now that we have gotten to the gate time, we transfer the count of ticks
		in that time period over to the encoder struct, so that the rpm calculator
		inside the c_encoder class can do its math.
		*/
		spindle_encoder.period_ticks = extern_encoder__ticks_at_time;
		/*
		Extern_encoder__tick_at_time changes in the encoder isr, not the timer isr.
		Now that we have saved off the value for the rpm computation we can reset it
		for the next rpm gate time
		*/
		extern_encoder__ticks_at_time = 0;
		//clear the gate counter
		rpm_count_ticks = 0;
		//flag that the rpm time has expired.
		extern_input__intervals |=(1<<RPM_INTERVAL_BIT);
	}

	if (tmr_count_ticks >= SET_GATE_TIME_MS)
	{
		tmr_count_ticks = 0;
		
		extern_input__intervals |=(1<<ONE_INTERVAL_BIT);//<--flag that 1 second has passed
		extern_input__intervals |=(1<<RPT_INTERVAL_BIT);//<--flag that its time to report
		
		extern_input__time_count = IN_TCNT + local_overflow_accumulator;
		IN_TCNT = 0;//<-- clear the counter for freq read (desired rpm)
		local_overflow_accumulator = 0;
	}

	tmr_count_ticks++;
	pid_count_ticks++;
	rpm_count_ticks++;
}
