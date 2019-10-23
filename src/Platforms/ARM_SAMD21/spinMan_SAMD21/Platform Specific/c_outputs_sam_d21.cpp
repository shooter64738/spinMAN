#include "c_outputs_sam_d21.h"
#include "../../../../core/c_configuration.h"

//#define OUT_TCCRA TCCR1A
//#define OUT_TCCRB TCCR1B
//#define OUT_OCR	 OCR1A

PortGroup *Pwm_Output_Port = &PORT->Group[0];
Tcc *Pwm_Timer_counter_Control = TCC0;

void HardwareAbstractionLayer::Outputs::initialize()
{
	//set mode to output
	//DDRB |= (1<<DDB5) | (1<<DDB4);
	//enable pullups
	//PORTB |= (1<<PORTB5) | (1<<PORTB4);
	//I cannot start this in free mode with the power solutions driver
	//Defaulting to forward mode because a direction must be set at the
	//time we start a pwm signal, even if the signal will not make the
	//motor move.
	HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Forward);
	asm("nop");
	HardwareAbstractionLayer::Outputs::configure_pwm_output_timer();
	asm("nop");
	HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Reverse);
	asm("nop");
	HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions::Forward);
		
}

void HardwareAbstractionLayer::Outputs::configure_pwm_output_timer()
{

	//DDRB |= (1<<DDB1);
	//ICR1 = 65535;
	//OCR1A = 65535;
	//TCCR1A = (1 << COM1A1)|(1 << COM1B1);
	//TCCR1A |= (1 << WGM11);
	//TCCR1B |= (1 << WGM12)|(1 << WGM13);
	//TCCR1B |= (1 << CS10);
	//
	uint32_t period = 48 - 1;

	// Because we are using TCC0, limit period to 24 bits
	period = ( period < 0x00ffffff ) ? period : 0x00ffffff;
	
	// Enable and configure generic clock generator 4
	GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |          // Improve duty cycle
	GCLK_GENCTRL_GENEN |        // Enable generic clock gen
	GCLK_GENCTRL_SRC_DFLL48M |  // Select 48MHz as source
	GCLK_GENCTRL_ID(4);         // Select GCLK4
	while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
	
	// Set clock divider of 1 to generic clock generator 4
	GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |         // Divide 48 MHz by 1
	GCLK_GENDIV_ID(4);           // Apply to GCLK4 4
	while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
	
	// Enable GCLK4 and connect it to TCC0 and TCC1
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |        // Enable generic clock
	GCLK_CLKCTRL_GEN_GCLK4 |    // Select GCLK4
	GCLK_CLKCTRL_ID_TCC0_TCC1;  // Feed GCLK4 to TCC0/1
	while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
	
	// Divide counter by 1 giving 48 MHz (20.83 ns) on each TCC0 tick
	Pwm_Timer_counter_Control->CTRLA.reg |= TCC_CTRLA_PRESCALER(TCC_CTRLA_PRESCALER_DIV1_Val);
	
	// Use "Normal PWM" (single-slope PWM): count up to PER, match on CC[n]
	Pwm_Timer_counter_Control->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;         // Select NPWM as waveform
	while (Pwm_Timer_counter_Control->SYNCBUSY.bit.WAVE);                // Wait for synchronization
	
	// Set the period (the number to count to (TOP) before resetting timer)
	Pwm_Timer_counter_Control->PER.reg = period;
	while (Pwm_Timer_counter_Control->SYNCBUSY.bit.PER);
	
	// Set PWM signal to output 50% duty cycle
	// n for CC[n] is determined by n = x % 4 where x is from WO[x]
	Pwm_Timer_counter_Control->CC[2].reg = period / 2;
	while (Pwm_Timer_counter_Control->SYNCBUSY.bit.CC2);
	
	// Configure PA18 (D10 on Arduino Zero) to be output
	//PORTA.DIRSET.reg = PORT_PA18;
	//PORT->Group[PORTA].DIRSET.reg = PORT_PA18;      // Set pin as output
	Pwm_Output_Port->DIRSET.reg = PORT_PA18;
	
	Pwm_Output_Port->OUTCLR.reg = PORT_PA18;      // Set pin to low
	
	// Enable the port multiplexer for PA18
	Pwm_Output_Port->PINCFG[18].reg |= PORT_PINCFG_PMUXEN;
	
	// Connect TCC0 timer to PA18. Function F is TCC0/WO[2] for PA18.
	// Odd pin num (2*n + 1): use PMUXO
	// Even pin num (2*n): use PMUXE
	Pwm_Output_Port->PMUX[9].reg = PORT_PMUX_PMUXE_F;
	
	// Enable output (start PWM)
	Pwm_Timer_counter_Control->CTRLA.reg |= (TCC_CTRLA_ENABLE);
	while (Pwm_Timer_counter_Control->SYNCBUSY.bit.ENABLE);              // Wait for synchronization

}

void HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions direction )
{
	if (direction == Spin::Enums::e_directions::Forward)
	{
		//PORTB &= ~(1<<PINB4);//make brake pin low
		//PORTB |= (1<<PINB5);//make direction pin high
	}
	else if (direction == Spin::Enums::e_directions::Reverse)
	{
		//PORTB &= ~(1<<PINB4);//make brake pin low
		//PORTB &= ~(1<<PINB5);//make direction pin low
	}
	else if (direction == Spin::Enums::e_directions::Free)
	{
		//freewheel
		//PORTB |= (1<<PINB5);//make direction pin high
		//PORTB |= (1<<PINB4);//make brake pin high
	}
}

void HardwareAbstractionLayer::Outputs::disable_output()
{
	//disable pwm output
	//TCCR1B &= ~(1 << CS10);
	//OCR1A = 65535;
	//PORTB &= ~(1<<PINB1);//make pwm pin low
	
}

void HardwareAbstractionLayer::Outputs::enable_output()
{
	//TCCR1B |= (1 << CS10);
}

void HardwareAbstractionLayer::Outputs::update_output(uint16_t value)
{
	//OCR1A = value;
}