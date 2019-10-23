#include "c_outputs_sam_d21.h"
#include "../../../../core/c_configuration.h"

//#define OUT_TCCRA TCCR1A
//#define OUT_TCCRB TCCR1B
//#define OUT_OCR	 OCR1A

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