#ifndef NGC_Gcode_Groups_h
#define NGC_Gcode_Groups_h



class NGC_Gcode_Groups
{
public:
	static const uint8_t NON_MODAL = 0;
	static const uint8_t MOTION_GROUP = 1;
	static const uint8_t PLANE_SELECTION = 2;
	static const uint8_t DISTANCE_MODE = 3;
	static const uint8_t FEED_RATE_MODE = 5;
	static const uint8_t UNITS = 6;
	static const uint8_t CUTTER_RADIUS_COMPENSATION = 7;
	static const uint8_t TOOL_LENGTH_OFFSET = 8;
	static const uint8_t RETURN_MODE_CANNED_CYCLE = 10;
	static const uint8_t COORDINATE_SYSTEM_SELECTION = 12;
	static const uint8_t PATH_CONTROL_MODE = 13;
	static const uint8_t RECTANGLAR_POLAR_COORDS_SELECTION = 15;
};
class NGC_Mcode_Groups
{
public:
	static const uint8_t STOPPING = 4;
	static const uint8_t TOOL_CHANGE = 6;
	static const uint8_t SPINDLE_TURNING = 7;
	static const uint8_t COOLANT = 8;
	static const uint8_t FEED_SPEED_OVERRIDE = 9;
};
#endif
