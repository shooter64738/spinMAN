/*
* grblRTC.c
*
* Created: 1/1/2018 12:03:25 PM
* Author : jeff_d
*/
#include "c_Encoder_RPM.h"
#include "..\Driver\c_Spindle_Drive.h"
#include "..\globals.h"
#include "..\c_Processor.h"
uint32_t over_flows=0;
uint32_t signal_monitor = 0;
bool signal_lost=false;
uint32_t time_at_tick[TIME_ARRAY_SIZE] = {0};
uint8_t time_index = 0;
int16_t count = 0;
int8_t encoder_direction = 0;

/*
Conversion factor is to save some math speed
CONVERSION_FACTOR = 1/(F_CPU/Prescale)
i.e. 1/(16,000,000/8) or 1/(16,000,000/1)
*/



/*
Instance methods
*/

static const int8_t encoder_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
//                                 *              *     *          *
//static const int8_t encoder_table[] = {0,-1,1,0,-1,0,1,0,1,-1,0,0,0,0,0};
//////////////////////////////////////////////////////////////////////////


float c_Encoder_RPM::encoder_rpm_multiplier = 0;
float c_Encoder_RPM::encoder_angle_multiplier = 0;
int16_t c_Encoder_RPM::encoder_ticks_per_rev = 0;

void c_Encoder_RPM::Initialize(uint16_t encoder_ticks_per_rev)
{
	
	//Following code enables interrupts to determine direction of motor rotation
	////////////////////////////////////////////////////////////////////////////
	DDRD &= ~(1 << DDD2);	//input mode
	PORTD |= (1 << PORTD2);	//enable pullup
	DDRD &= ~(1 << DDD3);	//input mode
	PORTD |= (1 << PORTD3);	//enable pullup
	////
	//Any change triggers
	EICRA |= (1 << ISC00);	// Trigger on any change on INT0
	EICRA |= (1 << ISC10);	// Trigger on any change on INT1
	
	//EICRA |= (1 << ISC00) | (1 << ISC01);	// Trigger on rising change on INT0
	//EICRA |= (1 << ISC10) | (1 << ISC11);	// Trigger on rising change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on falling change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on falling change on INT1
	
	//EICRA |= (1 << ISC01);	// Trigger on hi change on INT0
	//EICRA |= (1 << ISC11);	// Trigger on hi change on INT1
	
	EIMSK |= (1 << INT0) | (1 << INT1);     // Enable external interrupt INT0, INT1
	////////////////////////////////////////////////////////////////////////////
	
	/*
	PINB0 is the ONLY pin connected to the ICP mode of timer1 on the 328p.
	You MUST use PINB0 (uno pin D8) for Input Capture on timer1
	*/
	DDRB &= ~(1 << PINB0);//input mode
	PORTB |= (1 << PINB0); //enable pullup
	
	TCCR1A = 0; //(1<<WGM11);
	//prescale 8. @ 16,000,000/8 = 2,000,000
	//prescale 1. @ 16,000,000/1 = 16,000,000
	//Currently running NO prescaler gives best resolution. prescale 8 is suitable as well though
	//prescale 1 (none)
	//TCCR1B |= (1 << CS10); // | (1 << CS12);
	//prescale 8
	TCCR1B |= (1 << CS11);
	//prescale 64
	//TCCR1B |= (1 << CS10) | (1 << CS11);
	
	//TCCR1B |= (1<<ICNC1) | (0<<ICES1); //<-- noise cancel and edge select. Off for now
	TIMSK1 = (1<< TOIE1) |(1 << ICIE1);  //<-- (Input Capture Interrupt Enable) and (Timer Overflow Interrupt Enable)
	
	//Just shortening the math here so when we need the rpm, there is less math to do.
	c_Encoder_RPM::encoder_rpm_multiplier = TIME_FACTOR * (float)encoder_ticks_per_rev;
	c_Encoder_RPM::encoder_angle_multiplier = 360.0/(float)encoder_ticks_per_rev;
	c_Encoder_RPM::encoder_ticks_per_rev = encoder_ticks_per_rev;
	
	
}

float c_Encoder_RPM::CurrentRPM()
{
	//return 0;
	/*
	using the overflow counter to rough track .5 seconds.
	if we haven't had a pulse for .5 seconds or more, then we assume signal has been lost.
	RPM should be set to zero.
	*/
	//c_Encoder_RPM::encoder_dirty = false;
	if (signal_lost)
	{
		return -1.0;
	}
	
	uint32_t avg=0;
	for (int i=0;i<TIME_ARRAY_SIZE;i++)
	{
		avg+=time_at_tick[i];
		//c_Processor::host_serial.print_int32(time_at_tick[i]);
		//c_Processor::host_serial.print_string(",");
	}
	float seconds = 60/((((((float)avg)/((float)TIME_ARRAY_SIZE))*c_Encoder_RPM::encoder_rpm_multiplier)));
	return  seconds/4;
}

float c_Encoder_RPM::CurrentAngle_DEG()
{
	float this_count = count; //<--Copy from volatile as a float
	return c_Encoder_RPM::encoder_angle_multiplier * this_count;
}

//float c_Encoder_RPM::CurrentAngle_RAD()
//{
	//return c_Encoder_RPM::CurrentAngle_DEG()* M_PI/180;
//}



ISR(TIMER1_OVF_vect)
{
	//UDR0='x';
	over_flows++;
	signal_monitor++;
	if (signal_monitor>65) signal_lost=true;
}

void c_Encoder_RPM::Encoder_Trigger()
{
	uint16_t this_tick = ICR1;
	signal_lost = false;
	signal_monitor=0;
	
	//calculate total tick count by factoring in over flows
	time_at_tick[time_index++]  =  (over_flows*65536) + this_tick;
	over_flows = 0;
	TCNT1 = 0;
	if (time_index == TIME_ARRAY_SIZE)
	time_index = 0;
}
ISR(TIMER1_CAPT_vect)
{
	c_Encoder_RPM::Encoder_Trigger();
}
//
//ISR (TIMER1_COMPA_vect)
//{
//}

//////////////////////////////////////////////////////////////////////////
int32_t c_Encoder_RPM::Encoder_Position()
{
	
	//if (c_Encoder_RPM::encoder_dirty)
	{
		int32_t this_count = count; //<--copy from volatile
		return this_count;
	}
}

// encoder interrupt function, fires on change of either encoder input signal
// see tutorial/gif for description about how this function works
void c_Encoder_RPM::Encoder_Update()
{
	uint16_t this_tick = TCNT1;
	signal_lost = false;
	signal_monitor=0;
	
	//calculate total tick count by factoring in over flows
	time_at_tick[time_index++]  =  (over_flows*65536) + this_tick;
	over_flows = 0;
	TCNT1 = 0;
	if (time_index == TIME_ARRAY_SIZE)
	time_index = 0;
	
	
	//////////////////////////////////////////////////////////////////////////
	static uint8_t enc_val = 0; // static allows this value to persist across function calls
	enc_val = enc_val << 2; // shift the previous state to the left
	enc_val = enc_val | ((PIND & 0b1100) >> 2); // or the current state into the 2 rightmost bits
	encoder_direction = encoder_table[enc_val & 0b1111];    // preform the table lookup and increment count accordingly
	count += encoder_direction;
	
	if (count<= 0)
	count = c_Encoder_RPM::encoder_ticks_per_rev;
	else if (count >=c_Encoder_RPM::encoder_ticks_per_rev)
	count = 0;
	//////////////////////////////////////////////////////////////////////////
}

ISR (INT0_vect)
{
	//UDR0='a';
	//c_Encoder_RPM::Encoder_Trigger();
	c_Encoder_RPM::Encoder_Update();
}
ISR(INT1_vect)
{
	//UDR0='b';
	c_Encoder_RPM::Encoder_Update();
}