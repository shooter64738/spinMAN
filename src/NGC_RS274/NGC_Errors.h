#ifndef NGC_Interpreter_Errors_h
#define NGC_Interpreter_Errors_h
#include "NGC_Groups.h"
class NGC_Interpreter_Errors
{
public:
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Negative error values indicate an error that the interpreter cant process. This is    |
	| most likely because the interpreter code needs modified to understand how to process  |
	| the line data. This would be a bug of some kind. To make it easy to locate where the  |
	| problem is within the interpreter code ONLY ONE of these errors is given per method in|
	| the interpreter code. Just search for the const value and it will take you to the     |
	| method that returned that error value.                                                |
	| Ie this error is returned:                                                            |
	| 'INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE' the raw value is '-1' which is what you|
	| might see in a debugger. Search the code using Ctrl+F for                             |
	| 'INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE' and you will find the method that the  |
	| error was set in.                                                                     |
	|***************************************************************************************|
	*/
	/*
	  Errors for interpreter failure are all negative values, and are not used more than once!
	*/
	static const int INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE = -1;
	static const int INTERPRETER_DOES_NOT_UNDERSTAND_M_WORD_VALUE = -2;
	static const int INTERPRETER_DOES_NOT_UNDERSTAND_CHARACTER_IN_BLOCK = -3;

	static const char OK = 0; //<--Everything parsed fine
	static const char NO_OP = 1; //<--Nothing parsed/to do, but no errors
	static const int NO_AXIS_DEFINED_FOR_MOTION = 2;
	static const int NO_FEED_RATE_SPECIFIED = 3;
	static const int NO_FEED_RATE_SPECIFIED_FOR_G93 = 4;
	static const int MISSING_CIRCLE_OFFSET_IJ = 5;
	static const int MISSING_CIRCLE_OFFSET_IK = 6;
	static const int MISSING_CIRCLE_OFFSET_JK = 7;
	static const int MISSING_CIRCLE_AXIS_XY = 8;
	static const int MISSING_CIRCLE_AXIS_XZ = 9;
	static const int MISSING_CIRCLE_AXIS_YZ = 10;
	static const int ACTIVE_PLANE_UNSPECIFIED = 11;

	//These errors are obviously for group issues. I don't want to have to change it in multiple
	//places/code files so I am connecting the error values together. I add 15 to each one to make
	//each group error unique
	#define BASE_ERROR 15
	static const unsigned int G_CODE_GROUP_NON_MODAL_ALREADY_SPECIFIED = NGC_Gcode_Groups::NON_MODAL + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_MOTION_GROUP_ALREADY_SPECIFIED = NGC_Gcode_Groups::MOTION_GROUP + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_PLANE_SELECTION_ALREADY_SPECIFIED = NGC_Gcode_Groups::PLANE_SELECTION + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_DISTANCE_MODE_ALREADY_SPECIFIED = NGC_Gcode_Groups::DISTANCE_MODE + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_FEED_RATE_MODE_ALREADY_SPECIFIED = NGC_Gcode_Groups::FEED_RATE_MODE + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_UNITS_ALREADY_SPECIFIED = NGC_Gcode_Groups::UNITS + 21;
	static const unsigned int G_CODE_GROUP_CUTTER_RADIUS_COMPENSATION_ALREADY_SPECIFIED = NGC_Gcode_Groups::CUTTER_RADIUS_COMPENSATION + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_TOOL_LENGTH_OFFSET_ALREADY_SPECIFIED = NGC_Gcode_Groups::TOOL_LENGTH_OFFSET + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_RETURN_MODE_CANNED_CYCLE_ALREADY_SPECIFIED = NGC_Gcode_Groups::RETURN_MODE_CANNED_CYCLE + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_COORDINATE_SYSTEM_SELECTION_ALREADY_SPECIFIED = NGC_Gcode_Groups::COORDINATE_SYSTEM_SELECTION + BASE_ERROR;
	static const unsigned int G_CODE_GROUP_PATH_CONTROL_MODE_ALREADY_SPECIFIED = NGC_Gcode_Groups::PATH_CONTROL_MODE + BASE_ERROR;

	//If the values to these word errors are changed at all the error get
	//logic in the c_Interpreter also needs to be changed!
	static const unsigned int WORD_A_ALREADY_DEFINED = 100;
	static const unsigned int WORD_B_ALREADY_DEFINED = 101;
	static const unsigned int WORD_C_ALREADY_DEFINED = 101;
	static const unsigned int WORD_D_ALREADY_DEFINED = 103;
	static const unsigned int WORD_E_ALREADY_DEFINED = 104;
	static const unsigned int WORD_F_ALREADY_DEFINED = 105;
	static const unsigned int WORD_G_ALREADY_DEFINED = 106;
	static const unsigned int WORD_H_ALREADY_DEFINED = 107;
	static const unsigned int WORD_I_ALREADY_DEFINED = 108;
	static const unsigned int WORD_J_ALREADY_DEFINED = 109;
	static const unsigned int WORD_K_ALREADY_DEFINED = 110;
	static const unsigned int WORD_L_ALREADY_DEFINED = 111;
	static const unsigned int WORD_M_ALREADY_DEFINED = 112;
	static const unsigned int WORD_N_ALREADY_DEFINED = 113;
	static const unsigned int WORD_O_ALREADY_DEFINED = 114;
	static const unsigned int WORD_P_ALREADY_DEFINED = 115;
	static const unsigned int WORD_Q_ALREADY_DEFINED = 116;
	static const unsigned int WORD_R_ALREADY_DEFINED = 117;
	static const unsigned int WORD_S_ALREADY_DEFINED = 118;
	static const unsigned int WORD_T_ALREADY_DEFINED = 119;
	static const unsigned int WORD_U_ALREADY_DEFINED = 120;
	static const unsigned int WORD_V_ALREADY_DEFINED = 121;
	static const unsigned int WORD_W_ALREADY_DEFINED = 122;
	static const unsigned int WORD_X_ALREADY_DEFINED = 123;
	static const unsigned int WORD_Y_ALREADY_DEFINED = 124;
	static const unsigned int WORD_Z_ALREADY_DEFINED = 125;

	/*
	These are errors (generally) for modal commands
	*/
	static const unsigned int CAN_CYCLE_MISSING_R_VALUE = 200;
	static const unsigned int CAN_CYCLE_WORD_L_NOT_POS_INTEGER = 201;
	static const unsigned int CAN_CYCLE_WORD_L_LESS_THAN_2 = 202;
	static const unsigned int CAN_CYCLE_MISSING_X_VALUE = 203;
	static const unsigned int CAN_CYCLE_MISSING_Y_VALUE = 204;
	static const unsigned int CAN_CYCLE_MISSING_Z_VALUE = 205;
	static const unsigned int CAN_CYLCE_ROTATIONAL_AXIS_A_DEFINED = 206;
	static const unsigned int CAN_CYLCE_ROTATIONAL_AXIS_B_DEFINED = 207;
	static const unsigned int CAN_CYLCE_ROTATIONAL_AXIS_C_DEFINED = 208;
	static const unsigned int CAN_CYLCE_CUTTER_RADIUS_COMPENSATION_ACTIVE = 209;
	static const unsigned int CAN_CYCLE_LINEAR_AXIS_UNDEFINED = 210;
	static const unsigned int CUTTER_RADIUS_COMP_NOT_XY_PLANE=211;
	static const unsigned int CAN_CYCLE_RETURN_MODE_UNDEFINED=212;
	static const unsigned int CAN_CYCLE_MISSING_P_VALUE=213;

	//static const unsigned int CAN_CYCLE_MISSING_X_VALUE = 205;
	static const unsigned int ADDRESS_VALUE_NOT_NUMERIC=300;

	static const unsigned int CUTTER_RADIUS_COMP_ALREADY_ACTIVE = 400;
	
	
};
#endif

#ifndef NGC_Machine_Errors_h
#define NGC_Machine_Errors_h

class NGC_Machine_Errors
{
public:
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| Negative error values indicate an error that the machine cant process. This is most   |
	| likely becasue the machine code needs modified to understand how to process the       |
	| interpreted data. This would be a bug of some kind. To make it easy to locate where   |
	| the problem is within the machine code ONLY ONE of these errors is given per method in|
	| the machine code. Just search for the const value and it will take you to the method  |
	| that returned that error value.                                                       |
	| Ie this error is returned:                                                            |
	| 'MACHINE_DOES_NOT_UNDERSTAND_ARC_FORMAT' the raw value is '-1' which is what you might|
	| see in a debugger. Search the code using Ctrl+F for                                   |
	| 'MACHINE_DOES_NOT_UNDERSTAND_ARC_FORMAT' and you will find the method that the        |
	| error was set in.                                                                     |
	|***************************************************************************************|
	*/

	/*
	Errors for interpreter failure are all negative values, and are not used mroe than once!
	*/
	static const int MACHINE_DOES_NOT_UNDERSTAND_ARC_FORMAT = -1;


	static const unsigned char OK = 0;
	static const unsigned char NO_OP = 0;
	static const unsigned int X_AXIS_TRAVEL_IS_BEYOND_LIMITS = 11;
	static const unsigned int Y_AXIS_TRAVEL_IS_BEYOND_LIMITS = 12;
	static const unsigned int Z_AXIS_TRAVEL_IS_BEYOND_LIMITS = 13;
	static const unsigned int A_AXIS_TRAVEL_IS_BEYOND_LIMITS = 14;
	static const unsigned int B_AXIS_TRAVEL_IS_BEYOND_LIMITS = 15;
	static const unsigned int C_AXIS_TRAVEL_IS_BEYOND_LIMITS = 16;

	static const unsigned int X_AXIS_TRAVEL_LIMIT_NOT_SET = 17;
	static const unsigned int Y_AXIS_TRAVEL_LIMIT_NOT_SET = 18;
	static const unsigned int Z_AXIS_TRAVEL_LIMIT_NOT_SET = 19;
	static const unsigned int A_AXIS_TRAVEL_LIMIT_NOT_SET = 20;
	static const unsigned int B_AXIS_TRAVEL_LIMIT_NOT_SET = 21;
	static const unsigned int C_AXIS_TRAVEL_LIMIT_NOT_SET = 22;

	static const unsigned int X_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 30;
	static const unsigned int Y_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 31;
	static const unsigned int Z_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 32;
	static const unsigned int A_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 33;
	static const unsigned int B_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 34;
	static const unsigned int C_AXIS_DEFINED_BUT_DOES_NOT_EXIST = 35;

	//static const char NON_MODAL = 0;
	//static const char MOTION_GROUP = 1;
	static const unsigned int PLANE_SELECTION_UNSPECIFIED = 40;
	static const unsigned int DISTANCE_MODE_UNSPECIFIED = 41;
	static const unsigned int FEED_RATE_MODE_UNSPECIFIED = 42;
	static const unsigned int UNITS_UNSPECIFIED = 43;
	static const unsigned int CUTTER_RADIUS_COMPENSATION_UNSPECIFIED = 44;
	static const unsigned int TOOL_LENGTH_OFFSET_UNSPECIFIED = 45;
	static const unsigned int RETURN_MODE_CANNED_CYCLE_UNSPECIFIED = 46;
	static const unsigned int COORDINATE_SYSTEM_SELECTION_UNSPECIFIED = 47;
	static const unsigned int PATH_CONTROL_MODE_UNSPECIFIED = 48;
	static const unsigned int FEED_RATE_UNSPECIFIED = 49;
	static const unsigned int RADIUS_FORMAT_ARC_POINTS_NOT_EQUIDISTANT = 50;
	static const unsigned int RECTANGLAR_POLAR_COORDS_UNSPECIFIED = 51;

	static const unsigned int CAN_CYCLE_R_WORD_GREATER_THAN_Z = 60;
	static const unsigned int CAN_CYCLE_R_WORD_GREATER_THAN_X = 61;
	static const unsigned int CAN_CYCLE_R_WORD_GREATER_THAN_Y = 62;
	static const unsigned int CAN_CYCLE_L_WORD_INVALID = 63;
	static const unsigned int CAN_CYCLE_Q_WORD_UNDEFINED = 64;
	static const unsigned int CAN_CYCLE_Q_WORD_INVALID = 65;
	static const unsigned int CAN_CYCLE_ROTATIONAL_AXIS_DEFINED = 66;

	static const unsigned int INTERPRETER_FAILURE = 70;
	static const unsigned int SPINDLE_ROTATION_NOT_EXPECTED = 80;
};

#endif