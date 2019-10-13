#include "c_outputs_avr_328.h"
#include "..\..\..\..\c_enumerations.h"
#include "..\..\..\..\c_configuration.h"

#define OUT_TCCRA TCCR1A
#define OUT_TCCRB TCCR1B
#define OUT_OCR	 OCR1A

void HardwareAbstractionLayer::Outputs::initialize()
{
	
	HardwareAbstractionLayer::Outputs::configure_pwm_output_timer();
	
	DIRECTION_PORT_DIRECTION |= (1<<FORWARD_PIN) | (1<<REVERSE_PIN);
	DIRECTION_PORT |= (1<<PORTB5) | (1<<PORTB4);
}

void HardwareAbstractionLayer::Outputs::configure_pwm_output_timer()
{
	OUT_OCR = 255;
	OUT_TCCRA |= (1 << COM0A1);
	OUT_TCCRA |= (1 << WGM01) | (1 << WGM00);
}

void HardwareAbstractionLayer::Outputs::set_direction(Spin::Enums::e_directions direction )
{
	if (direction == Spin::Enums::e_directions::Forward)
	{
		DIRECTION_PORT &= ~(1<<REVERSE_PIN);//make reverse pin low
		DIRECTION_PORT |= (1<<FORWARD_PIN);//make forward pin high
	}
	else if (direction == Spin::Enums::e_directions::Reverse)
	{
		DIRECTION_PORT &= ~(1<<FORWARD_PIN);//make forward pin low
		DIRECTION_PORT |= (1<<REVERSE_PIN);//make reverse pin high
	}
	else if (direction == Spin::Enums::e_directions::Free)
	{
		//freewheel
		DIRECTION_PORT |= (1<<FORWARD_PIN);//make forward pin high
		DIRECTION_PORT |= (1<<REVERSE_PIN);//make reverse pin high
	}
}

void HardwareAbstractionLayer::Outputs::disable_output()
{
	//disable pwm output
	OUT_TCCRB = 0;
	OUT_OCR = Spin::Configuration::Drive_Settings.Drive_Turn_Off_Value;
}

void HardwareAbstractionLayer::Outputs::enable_output()
{
	//enable pwm output
	DDRD |= (1<<PWM_OUTPUT_PIN);
	OUT_TCCRB |= (1 << CS00);
}

void HardwareAbstractionLayer::Outputs::update_output(uint16_t value)
{
	OUT_OCR = value;
}