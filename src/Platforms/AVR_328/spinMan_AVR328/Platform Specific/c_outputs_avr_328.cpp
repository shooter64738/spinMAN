#include "c_outputs_avr_328.h"
#include "../../../../c_controller.h"

void HardwareAbstractionLayer::Outputs::initialize()
{
	OCR0A = 255;
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);

}

void HardwareAbstractionLayer::Outputs::configure()
{
}

void HardwareAbstractionLayer::Outputs::disable_output(uint16_t off_value)
{
	//disable pwm output
	OCR0A = off_value;
	DDRD &= ~(1<<PWM_OUTPUT_PIN);
	TCCR0B = 0;
}

void HardwareAbstractionLayer::Outputs::disable_output()
{
	//disable pwm output
	DDRD &= ~(1<<PWM_OUTPUT_PIN);
	TCCR0B = 0;
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