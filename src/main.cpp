//#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <stdbool.h>
//#include <util/delay.h>
//#include "FeedBack/c_Encoder_RPM.h"
//#include "Serial/c_Serial.h"
//#include "Driver/c_Spindle_Drive.h"
//#include "Time/c_Time_Keeper.h"
//#include "PID/c_PID.h"
//#include "Parser/c_Parser.h"
#include "c_Processor.h"
#define FACTOR 1/F_CPU



int main(void)
{
	c_Processor::Start();
}
