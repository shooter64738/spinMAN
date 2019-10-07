#include "c_outputs_avr_328.h"


void HardwareAbstractionLayer::Outputs::initialize()
{
	OCR0A = 255;
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	
	
	DIRECTION_PORT_DIRECTION |= (1<<FORWARD_PIN) | (1<<REVERSE_PIN);
	DIRECTION_PORT |= (1<<PORTB5) | (1<<PORTB4);
	

}

void HardwareAbstractionLayer::Outputs::set_direction(uint8_t direction )
{
	if (direction == 0)
	{
		DIRECTION_PORT &= ~(1<<REVERSE_PIN);//make reverse pin low
		DIRECTION_PORT |= (1<<FORWARD_PIN);//make forward pin high
	}
	else if (direction == 1)
	{
		DIRECTION_PORT &= ~(1<<FORWARD_PIN);//make forward pin low
		DIRECTION_PORT |= (1<<REVERSE_PIN);//make reverse pin high
	}
	else if (direction == 2)
	{
		//freewheel
		DIRECTION_PORT |= (1<<FORWARD_PIN);//make forward pin high
		DIRECTION_PORT |= (1<<REVERSE_PIN);//make reverse pin high
	}
}

void HardwareAbstractionLayer::Outputs::disable_output(uint16_t off_value)
{
	//disable pwm output
	OCR0A = off_value;
	HardwareAbstractionLayer::Outputs::disable_output();
}

void HardwareAbstractionLayer::Outputs::disable_output()
{
	//DIRECTION_PORT |= (1<<REVERSE_PIN);//make reverse pin high
	//DIRECTION_PORT |= (1<<FORWARD_PIN);//make forward pin high
	//disable pwm output
	//DDRD &= ~(1<<PWM_OUTPUT_PIN);
	//TCCR0B = 0;
}

void HardwareAbstractionLayer::Outputs::enable_output()
{
	//enable pwm output
	DDRD |= (1<<PWM_OUTPUT_PIN);
	TCCR0B |= (1 << CS00);
}

void HardwareAbstractionLayer::Outputs::update_output(uint16_t value)
{
	OCR0A = value;
}