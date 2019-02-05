#ifndef NGC_Gcodes_X100_h
#define NGC_Gcodes_X100_h
/*
All codes defined in here are multiplied by 100 so they can be handled as ints or char's
Double and float tyeps consume too much memory to have a numberic variable with only one
decimal place. It is jsut not worth it.
Any value that would be below 256 after multiplication is a char. any value over 256 is an int
*/

class NGC_Gcodes_X100
{
public:
	static const unsigned int RAPID_POSITIONING = 0 * 100;
	static const unsigned int LINEAR_INTERPOLATION = 1 * 100;
	static const unsigned int CIRCULAR_INTERPOLATION_CW = 2 * 100;
	static const unsigned int CIRCULAR_INTERPOLATION_CCW = 3 * 100;
	static const unsigned int DWELL = 4 * 100;
	static const unsigned int COORDINATE_SYSTEM_ORIGIN = 10 * 100;
	static const unsigned int RECTANGULAR_COORDINATE_SYSTEM = 15 * 100;
	static const unsigned int POLAR_COORDINATE_SYSTEM = 16 * 100;
	static const unsigned int XY_PLANE_SELECTION = 17 * 100;
	static const unsigned int XZ_PLANE_SELECTION = 18 * 100;
	static const unsigned int YZ_PLANE_SELECTION = 19 * 100;
	static const unsigned int INCH_SYSTEM_SELECTION = 20 * 100;
	static const unsigned int MILLIMETER_SYSTEM_SELECTION = 21 * 100;
	static const unsigned int RETURN_TO_HOME = 28 * 100;
	static const unsigned int RETURN_TO_SECONDARY_HOME = 30 * 100;
	static const unsigned int STRAIGHT_PROBE = 38.2 * 100;
	static const unsigned int CANCEL_CUTTER_RADIUS_COMPENSATION = 40 * 100;
	static const unsigned int START_CUTTER_RADIUS_COMPENSATION_LEFT = 41 * 100;
	static const unsigned int START_CUTTER_RADIUS_COMPENSATION_RIGHT = 42 * 100;
	static const unsigned int TOOL_LENGTH_OFFSET = 43 * 100;
	static const unsigned int CANCEL_TOOL_LENGTH_OFFSET = 49 * 100;
	static const unsigned int MOTION_IN_MACHINE_COORDINATE_SYSTEM = 53 * 100;

	static const unsigned int MOTION_CANCELED = 80 * 100;
	static const unsigned int CANNED_CYCLE_DRILLING = 81 * 100;
	static const unsigned int CANNED_CYCLE_DRILLING_WITH_DWELL= 82 * 100;
	static const unsigned int CANNED_CYCLE_PECK_DRILLING = 83 * 100;
	static const unsigned int CANNED_CYCLE_RIGHT_HAND_TAPPING = 84 * 100;
	static const unsigned int CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT = 85 * 100;
	static const unsigned int CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT = 86 * 100;
	static const unsigned int CANNED_CYCLE_BACK_BORING = 87 * 100;
	static const unsigned int CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT = 88 * 100;
	static const unsigned int CANNED_CYCLE_BORING_DWELL_FEED_OUT = 89 * 100;
	static const unsigned int CANNED_CYCLE_RETURN_TO_Z = 98 * 100;
	static const unsigned int CANNED_CYCLE_RETURN_TO_R = 99 * 100;

	static const unsigned int ABSOLUTE_DISANCE_MODE = 90 * 100;
	static const unsigned int INCREMENTAL_DISTANCE_MODE = 91 * 100;

	static const unsigned int FEED_RATE_MINUTES_PER_UNIT_MODE = 93 * 100;
	static const unsigned int FEED_RATE_UNITS_PER_MINUTE_MODE = 94 * 100;
	static const unsigned int FEED_RATE_UNITS_PER_ROTATION = 95 * 100;
};
#endif

#ifndef NGC_Mcodes_X100_h
#define NGC_Mcodes_X100_h
class NGC_Mcodes_X100
{
public:
	static const unsigned int STOPPING = 4 * 100;
	static const unsigned int TOOL_CHANGE = 6 * 100;
	static const unsigned int SPINDLE_TURNING = 7 * 100;
	static const unsigned int COOLANT = 8 * 100;
	static const unsigned int FEED_SPEED_OVERRIDE = 9 * 100;
};
#endif
