/*
* c_Spindle_Drive.cpp
*
* Created: 8/8/2018 1:35:00 PM
* Author: jeff_d
*/


#include "c_Spindle_Drive.h"
#include "..\Time\c_Time_Keeper.h"
#include "..\FeedBack\c_Encoder_RPM.h"
#include "..\Parser\c_Parser.h"
#include "..\c_Processor.h"
#include "..\PID\c_PID.h"

#define PWM_OUTPUT_PIN PD6 //(pin 6 )
#define ENABLE_PIN PD5 //(pimn 5)
#define BRAKE_PIN PD4 //(pin 4)
#define DIRECTION_PIN PD7 //(pin 7)
#define CONTROL_PORT PORTD
#define MINIMUM_OUTPUT 100
#define MAXIMUM_OUTPUT 255

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

uint8_t c_Spindle_Drive::current_output = 0;
c_Spindle_Drive::s_drive_control c_Spindle_Drive::Drive_Control;
//double c_Spindle_Drive::Kp=2;
//double c_Spindle_Drive::Ki=5;
//double c_Spindle_Drive::Kd=1; //<--setup PID scalers
double c_Spindle_Drive::Kp=0.0015;
double c_Spindle_Drive::Ki=0.0001;
double c_Spindle_Drive::Kd=0.01;


int32_t c_Spindle_Drive::Target_Position=0; //<--encoder target value
double c_Spindle_Drive::B=0;
uint32_t c_Spindle_Drive::last_millis = 0;

static uint32_t accel_millis=0;
static float accel_output=0;

void c_Spindle_Drive::Initialize()
{
	/*
	CS02	CS01	CS00	DESCRIPTION
	0		0 		0		Timer/Counter2 Disabled
	0		0 		1 		No Prescaling
	0		1 		0 		Clock / 8
	0		1 		1 		Clock / 64
	1		0 		0 		Clock / 256
	1		0 		1 		Clock / 1024
	*/

	// make sure to make OC0 pin (pin PB6 for atmega328) as output pin
	//DDRD |= (1<<DDD6);
	//DDRD |= (1<<PWM_OUTPUT_PIN);

	TCCR0A |= (1<<COM0A1) ;//| (1<<WGM00);
	TCCR0A |= (1<<WGM01) | (1<<WGM00);
	TCCR0B |= (1<<CS01);// | _BV(WGM02);
	
	//set spindle motor enable pin as output
	DDRD |= (1<<ENABLE_PIN); //uno pin 5
	//set spindle motor brake pin as output
	DDRD |= (1<<BRAKE_PIN); //uno pin 4
	//set spindle motor reverse pin as output
	DDRD |= (1<<DIRECTION_PIN); //uno pin 7
	
	c_Spindle_Drive::Drive_Control.State=0;
	c_Spindle_Drive::Drive_Control.accel_time = 15;
	c_Spindle_Drive::Enable_Drive();
	c_Spindle_Drive::Brake_Drive();
	
	
}

void c_Spindle_Drive::Adjust(uint8_t output_value)
{
	//If the brake is on, set pwm output to zero
	if (c_Spindle_Drive::Get_State(STATE_BIT_BRAKE))
	{
		c_Spindle_Drive::current_output = MINIMUM_OUTPUT;
		return;
	}
	
	c_Spindle_Drive::current_output = output_value;
}

void c_Spindle_Drive::Enable_Drive()
{
	// make sure to make OC0 pin (pin PB6 for atmega328) as output pin
	DDRD |= (1<<PWM_OUTPUT_PIN);

	//set enable pin high
	CONTROL_PORT |= (1<<ENABLE_PIN);
	//Turn off brake pin (if it was on)
	CONTROL_PORT &= ~(1<<BRAKE_PIN);
	c_Spindle_Drive::Set_State(STATE_BIT_ENABLE); //<--set the enable bit to 1
	c_Spindle_Drive::Clear_State(STATE_BIT_DISABLE); //<--set the disable bit to 0
	/*
	braking bit is not cleared here. The drive can be enabled, and the brake be on
	but until the brake is turned off the PWM output will hold at zero.
	*/
}

void c_Spindle_Drive::Disable_Drive()
{
	//set pwm output pin to input so that no output will go to the driver.
	//if the drive does not have an enable control line, this should effectively set the output to 0
	DDRD &= ~(1<<PWM_OUTPUT_PIN);

	//set enable pin low
	CONTROL_PORT &= ~(1<<ENABLE_PIN);
	c_Spindle_Drive::Set_State(STATE_BIT_DISABLE); //<--set the disable bit to 1
	c_Spindle_Drive::Clear_State(STATE_BIT_ENABLE); //<--set the enable bit to 0
}

void c_Spindle_Drive::Brake_Drive()
{
	/*
	braking may be accomplished by connecting motor output leads together via a relay/resistor combo.
	There should be no output to the driver during braking.
	WARNING HERE... If you do not use a braking resistor and employ a relay only for motor braking, the motor
	will come to a hault IMMEDIATLEY! This can result in broken mounts, twisted shafts and all kind of bad things.
	*/
	
	//disable drive
	c_Spindle_Drive::Disable_Drive();
	//set brake pin high
	CONTROL_PORT |= (1<<BRAKE_PIN);
	c_Spindle_Drive::Set_State(STATE_BIT_BRAKE);//<--set braking bit
	c_Spindle_Drive::current_output = MINIMUM_OUTPUT;
}

void c_Spindle_Drive::Release_Drive()
{
	//set brake pin low
	CONTROL_PORT &= ~(1<<BRAKE_PIN); //(uno pin 6)
	c_Spindle_Drive::Clear_State(STATE_BIT_BRAKE);//<--clear braking bit
}

void c_Spindle_Drive::Reverse_Drive()
{
	CONTROL_PORT |= (1<<DIRECTION_PIN); //(uno pin 13)
	c_Spindle_Drive::Set_State(STATE_BIT_DIRECTION_CCW);//<--set CCW direction bit
	c_Spindle_Drive::Clear_State(STATE_BIT_DIRECTION_CW);//<--clear CW direction bit
}

void c_Spindle_Drive::Forward_Drive()
{
	CONTROL_PORT &= ~(1<<DIRECTION_PIN); //(uno pin 13)
	c_Spindle_Drive::Set_State(STATE_BIT_DIRECTION_CW);//<--set CW direction bit
	c_Spindle_Drive::Clear_State(STATE_BIT_DIRECTION_CCW); //<--clear CCW direction bit
}

void c_Spindle_Drive::Set_State(uint8_t State_Bit_Flag)
{
	(bit_set(c_Spindle_Drive::Drive_Control.State,BIT(State_Bit_Flag)));
	//c_Spindle_Drive::Drive_Control.State = (1<<State_Bit_Flag);
}

uint8_t c_Spindle_Drive::Get_State(uint8_t State_Bit_Flag)
{
	return (bit_get(c_Spindle_Drive::Drive_Control.State,BIT(State_Bit_Flag)));
	//return bit_get(c_Spindle_Drive::Drive_Control.State,BIT(State_Bit_Flag));
}

void c_Spindle_Drive::Clear_State(uint8_t State_Bit_Flag)
{
	bit_clear(c_Spindle_Drive::Drive_Control.State,BIT(State_Bit_Flag));
	//c_Spindle_Drive::Drive_Control.State &= ~(1<<State_Bit_Flag);
}

/*
Can't always act on inputs immediately. For example, we cannot just change direction immediately.
The spindle has to first be stopped, then reversed. This is the 'state machine' for controlling
that behavior.
*/
static float avg_rpm = 0.0;
static uint32_t reads = 0;

uint8_t c_Spindle_Drive::Check_State()
{
	//Check the current RPM
	float this_rpm = 0.0;
	uint16_t illegal_rotations = 0;
	
	this_rpm = c_Encoder_RPM::CurrentRPM();
	
	//Check for a direction change
	while (c_Parser::spindle_input.rotation_direction != c_Spindle_Drive::Drive_Control.direction)
	{
		//Clear all PID values because we are changing direction
		c_PID::Clear(c_PID::spindle_terms);
		accel_output = c_PID::spindle_terms.Min_Val;
		
		/*
		There is a difference in the programmed rotation, and the current rotation.
		We want to end the state of current rotation, and start the state of
		programmed rotation
		*/
		
		//determine the state of current rotation
		switch (c_Spindle_Drive::Drive_Control.direction)
		{
			case PARSER_DIRECTION_CCW: //<--drive is currently in CCW rotation (M04)
			{
				/*
				since we were in a rotating state, and now we are going to be stopping/changing direction,
				this is obviously deceleration.
				*/
				//Set state to decelerate
				c_Spindle_Drive::Set_State(STATE_BIT_DECELERATE);
				
				//new direction is either stop, or cw. Either way, we must brake first
				c_Spindle_Drive::Brake_Drive();
				//brake is active, and we continue in this state until rpm reaches 0.
				//If a stop is all we needed, we are essentially done. If we are going a new direction
				//however we set the direction pin, and then set directions to be =

				if ( this_rpm < 1 && c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CW) //(M03)
				{
					//if this is a new direction, set the direction pin, and then match the directions
					//between the driver and the parser
					c_Spindle_Drive::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
					
					//Set direction to forward
					c_Spindle_Drive::Forward_Drive();
					//Set state to accelerate
					c_Spindle_Drive::Set_State(STATE_BIT_ACCELERATE);
					//Clear decelerate bit
					c_Spindle_Drive::Clear_State(STATE_BIT_DECELERATE);
				}
				//If the spindle was rotating, and then set to an off state, the brake will remain on.
			}
			break;
			case PARSER_DIRECTION_CW: //<--drive is currently in CW rotation (M03)
			{
				/*
				since we were in a rotating state, and now we are going to be stopping/changing direction,
				this is obviously deceleration.
				*/
				//Set state to decelerate
				c_Spindle_Drive::Set_State(STATE_BIT_DECELERATE);
				
				//new direction is either stop, or ccw. Either way, we must brake first
				c_Spindle_Drive::Brake_Drive();
				//brake is active, and we continue in this state until rpm reaches 0.
				//If a stop is all we needed, we are essentially done. If we are going a new direction
				//however we set the direction pin, and then set directions to be =

				if ( this_rpm < 1 && c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CCW) //(M04)
				{
					//if this is a new direction, set the direction pin, and then match the directions
					//between the driver and the parser
					c_Spindle_Drive::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
					
					//Set direction to reverse
					c_Spindle_Drive::Reverse_Drive();
					//Set state to accelerate
					c_Spindle_Drive::Set_State(STATE_BIT_ACCELERATE);
					//Clear decelerate bit
					c_Spindle_Drive::Clear_State(STATE_BIT_DECELERATE);
				}
				//If the spindle was rotating, and then set to an off state, the brake will remain on.
			}
			break;
			case PARSER_DIRECTION_OFF: //<--drive is currently not rotating at all (we think) (M05)
			{
				//This one is simple. Just set the direction and enable the drive. Just in case
				//we check the rpm anyway, because we expect spindle to not be rotating.
				
				if ( this_rpm > 0)
				{
					illegal_rotations++;
					//rotation detected but we dont expect it while in an off state
					break; //<--brake out of the switch/case here. loop again
				}
				
				//new direction is either stop, cw/ccw. Either way, brake first
				c_Spindle_Drive::Brake_Drive();
				//assume new rpm of zero for now
				c_Spindle_Drive::Drive_Control.target_rpm = 0; //(M05) spindle stop
				c_Spindle_Drive::Drive_Control.target_rpm = c_Parser::spindle_input.target_rpm;
				
				//Set the rotation directions to match
				c_Spindle_Drive::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
				
				if(c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CW) //(M03)
				{
					c_Spindle_Drive::Forward_Drive();
				}

				if(c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CCW) //(M04)
				{
					c_Spindle_Drive::Reverse_Drive();
				}
				
				//since we were in an off state, and now we are going to be rotating, this is obviously acceleration
				//Set state to accelerate
				c_Spindle_Drive::Set_State(STATE_BIT_ACCELERATE);
				//Clear decelerate bit
				c_Spindle_Drive::Clear_State(STATE_BIT_DECELERATE);
			}
			break;
		}
		//With every loop check the RPM
		this_rpm = c_Encoder_RPM::CurrentRPM();
		
		/*
		When we do NOT expect rotations, we count the number of times we looped and detected movement.
		If this value reaches the maximum, we will throw an error. This might happen if the operator
		is moving the spindle by hand when we expect it to be off, and an M3/M4 command is trying to
		execute. It may also happen if there is electrical interference between the encoder	and the controller.
		*/
		
		if (illegal_rotations>650) //<-- processor will loop incredibly fast. 650 should be hit in about .00001 seconds
		{
			return NGC_Machine_Errors::SPINDLE_ROTATION_NOT_EXPECTED;
		}
		//Last state to check. We were rotating, and now we have commanded to stop
		if (c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_OFF)
		{
			//Since our brake is on, we can just let it come to a stop. Nothing else for us to do.
			//Set the rotation directions to match
			c_Spindle_Drive::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
			break; //<--brake out of our loop
		}
	}
	
	if (c_Spindle_Drive::Drive_Control.direction == PARSER_DIRECTION_OFF)
	return NGC_Machine_Errors::OK;
	
	c_Spindle_Drive::Process_State(this_rpm);
	
	return NGC_Machine_Errors::OK;
}

void c_Spindle_Drive::Process_State(float current_rpm)
{
	/*
	If we get to here, the spindle is expected to be on. Keep calculating the PID loop.
	any input from the host that requires a direction change, stop, rpm adjustment, feed rate
	change will cause the PID to update automatically.
	*/
	
	
	//If we don't call release_drive, the pwm output will be ignored.
	if (c_Spindle_Drive::Get_State(STATE_BIT_BRAKE)) c_Spindle_Drive::Release_Drive();
	
	//Set the drive to enabled if it was disabled.
	if (c_Spindle_Drive::Get_State(STATE_BIT_DISABLE)) c_Spindle_Drive::Enable_Drive();
	
	
	if (c_Spindle_Drive::Get_State(STATE_BIT_ACCELERATE))
	{
		if (accel_output< c_PID::spindle_terms.Min_Val)
		{
			accel_output = c_PID::spindle_terms.Min_Val;
		}
		c_Spindle_Drive::Set_Acceleration(current_rpm);
	}
	else if (c_Spindle_Drive::Get_State(STATE_BIT_DECELERATE))
	{
		c_Spindle_Drive::Set_Decelerate(current_rpm);
	}
	else
	{
		//save current accel value in case we increase rpm from here.
		accel_output = c_Spindle_Drive::current_output;
		//If we are above X rpm, switch to average rpm. It is more stable
		if (current_rpm>500)
		{
			current_rpm = c_Encoder_RPM::CurrentRPM();
		}
			c_Spindle_Drive::current_output = c_PID::Calculate(current_rpm,c_Spindle_Drive::Drive_Control.target_rpm,c_PID::spindle_terms);
	}
	
	OCR0A = c_Spindle_Drive::current_output;
}


void c_Spindle_Drive::Set_Acceleration(float current_rpm)
{
	if (c_Time_Keeper::millis()>accel_millis)
	{
		//accel_output grows fast at start, and slows as speed is reached
		
		accel_output += (1-(current_rpm/c_Spindle_Drive::Drive_Control.target_rpm));
		//let the pid calculate so that when we switch from accel to run, the PID is already in synch
		c_PID::Calculate(current_rpm,c_Spindle_Drive::Drive_Control.target_rpm,c_PID::spindle_terms);
		
		//We have either hit the rpm, or accel has wrapped.
		if ( current_rpm >= c_Spindle_Drive::Drive_Control.target_rpm || accel_output < c_Spindle_Drive::current_output)
		{
			c_Spindle_Drive::Clear_State(STATE_BIT_ACCELERATE);
			accel_output = c_Spindle_Drive::current_output;
		}
		c_Spindle_Drive::current_output = accel_output;
		accel_millis = c_Time_Keeper::millis()+c_Spindle_Drive::Drive_Control.accel_time;
	}
}

void c_Spindle_Drive::Set_Decelerate(float current_rpm)
{
	static uint32_t last_millis = 0;
	
	if (c_Time_Keeper::millis()>last_millis)
	{
		c_Spindle_Drive::current_output = c_PID::Calculate(current_rpm,c_Spindle_Drive::Drive_Control.target_rpm,c_PID::spindle_terms);
		last_millis = c_Time_Keeper::millis();
	}
}

static float ITerm;
static float lastInput;
//static uint8_t NewOutPut = 0;
static double errl=0;

static uint32_t last_millis=0;
void c_Spindle_Drive::Servo(int32_t destination)
{
	/*
	get time
	vf-vi 146.3-0
	t 5.50 = ------
	accel 26.6

	get distance
	26.6 * (5.50*5.50)
	d 402 = -----
	2 (half)

	feed = 200-0
	time = feed/accel
	*/

	uint32_t distance_pulses = destination-c_Encoder_RPM::Encoder_Position();
	

	//c_Processor::host_serial.Write("accel_time=");c_Processor::host_serial.print_float(accel_time);
	//c_Processor::host_serial.Write("accel_pulses=");c_Processor::host_serial.print_float(accel_pulses);
	//c_Processor::host_serial.Write("accel=");c_Processor::host_serial.print_float(accel);
	//c_Processor::host_serial.Write("target_rpm=");c_Processor::host_serial.print_float(c_Spindle_Drive::Drive_Control.target_rpm);
	//c_Processor::host_serial.Write("distance_pulses=");c_Processor::host_serial.print_float(distance_pulses);
	
	
	//if (c_Time_Keeper::millis() > last_millis)
	{
		//c_PID::servo_terms.Kp = 0;
		//float accel_remain = (c_Spindle_Drive::Drive_Control.accel_pulses - c_Encoder_RPM::Encoder_Position());
		//Get the output value for our target rpm
		uint8_t rpm_out = c_PID::Calculate(c_Encoder_RPM::CurrentRPM(),c_Spindle_Drive::Drive_Control.target_rpm, c_PID::spindle_terms);
		//c_PID::servo_terms.Max_Val = rpm_out;
		
		//uint8_t servo_out = c_PID::Calculate(c_Encoder_RPM::Encoder_Position(),c_Spindle_Drive::Target_Position, c_PID::servo_terms)
		//*(accel_remain>0?accel_remain*.01:1);
		//c_Processor::host_serial.Write("servo_out=");c_Processor::host_serial.print_float(servo_out);
		//c_Processor::host_serial.Write("accel_remain=");c_Processor::host_serial.print_float(accel_remain);
		
		//OCR0A = servo_out;
		//last_millis =  c_Time_Keeper::millis()+(c_Spindle_Drive::Drive_Control.accel_step_time);
	}
	last_millis = c_Time_Keeper::millis()+1;
}
// default constructor
//c_Spindle_Drive::c_Spindle_Drive()
//{
//} //c_Spindle_Drive
//
//// default destructor
//c_Spindle_Drive::~c_Spindle_Drive()
//{
//} //~c_Spindle_Drive

