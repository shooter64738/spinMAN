#include "c_inputs_win.h"
#include "../../../../c_input.h"
#include "../../../../c_controller.h"

volatile uint32_t freq_count_ticks = 0;


volatile uint16_t pid_count_ticks = 0;
volatile uint16_t freq_interval = 0;
volatile uint16_t _ref_timer_count = 0;

volatile uint16_t enc_ticks_at_current_time = 0;
volatile uint32_t enc_count = 0;
static const int8_t encoder_table[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
static uint8_t enc_val = 0;


void HardwareAbstractionLayer::Inputs::get_rpm()
{
}

void HardwareAbstractionLayer::Inputs::synch_hardware_inputs()
{

}

void HardwareAbstractionLayer::Inputs::initialize()
{

}

void HardwareAbstractionLayer::Inputs::configure()
{
}

//This timer ticks every 2ms. 500 of these is 1 second

void HardwareAbstractionLayer::Inputs::update_encoder()
{

}



