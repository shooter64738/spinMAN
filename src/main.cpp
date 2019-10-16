#include "driver/c_controller.h"
#define FACTOR 1/F_CPU


int main(void)
{
		//c_Processor::Start();
		Spin::Driver::Controller::initialize();
		Spin::Driver::Controller::run();
		
}
