/*
* Utilities.h
*
* Created: 1/5/2018 12:11:54 PM
*  Author: jeff_d
*/


#ifndef UTILITIES_H
#define UTILITIES_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Serial/c_Serial.h"

class c_Utiliities
{
	private:
	static int8_t count_digits(uint32_t Value)
	{
		int length = 1;

		if (Value > 0)
		for (length = 0; Value > 0; length++)
		Value = Value / 10;

		return length;
	}

	public:
	
	/*
	*ReturnBuffer array must be 18
	Format a float into a string. Sign the number +/-, using specified number of Whole and Decimal digits. Include a 3 char unit at the end.
	Default Whole digits is 8, Decimal digits is 4
	*/
	static void float_to_string(float FloatValue, char*ReturnBuffer,uint8_t WholeNumbers, uint8_t DecimalPlaces, const char *UnitString)
	{
		bool neg = FloatValue<0?true:false;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		//   01234567890123
		char formater[15] = "%+08d.%04d mm ";
		formater[3] = 48+WholeNumbers;//ascii 48=0, 57=9
		formater[8] = 48+DecimalPlaces;
		formater[11]= (UnitString[0] <31?' ':UnitString[0]);
		formater[12]= (UnitString[1] <31?' ':UnitString[1]);
		formater[13]= (UnitString[2] <31?' ':UnitString[2]);

		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;
		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : '+';
		
	}

	/*
	*ReturnBuffer array must be 18
	Format a float into a string. Sign the number +/-, using specified number of Whole and Decimal digits.
	Default Whole digits is 8, Decimal digits is 4
	*/
	static void float_to_string(float FloatValue, char*ReturnBuffer,uint8_t WholeNumbers, uint8_t DecimalPlaces)
	{
		bool neg = FloatValue<0?true:false;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		//   0123456789
		char formater[11] = "%+08d.%04d";
		formater[3] = 48+WholeNumbers;//ascii 48=0, 57=9
		formater[8] = 48+DecimalPlaces;
		
		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;
		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : '+';
		
	}

	/*
	*ReturnBuffer array must be 18
	Format a float into a string. Sign the number +/-, using specified number of Decimal digits. Include a 3 char unit at the end.
	Default Whole digits is 8, Decimal digits is 4
	*/
	static void float_to_string(float FloatValue, char*ReturnBuffer, uint8_t DecimalPlaces)
	{
		bool neg = FloatValue<0?true:false;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		// 0123456789
		char formater[11] = "%+08d.%04d";
		formater[8] = 48+DecimalPlaces;
		
		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;
		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : '+';
		
	}
	
	static void float_to_string_no_sign(float FloatValue, char*ReturnBuffer, uint8_t DecimalPlaces)
	{
		memset(ReturnBuffer,0,18);
		bool neg = FloatValue < 0 ? true : false;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		// 0123456789
		char formater[10] = "%08d.%04d";

		uint32_t whole = FloatValue;
		FloatValue = FloatValue - whole;
		uint32_t mantissa = (FloatValue) * 10000;

		//formater[2] = 48 + (count_digits(whole) + (neg?1:0)); //<--Leave one space for a '-' if needed
		//formater[7] = 48 + DecimalPlaces;//(count_digits(mantissa));

		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : ReturnBuffer[0];
		
	}
	
	static void float_to_string_no_sign(float FloatValue, char*ReturnBuffer)
	{
		bool neg = FloatValue < 0 ? true : false;
		FloatValue = neg ? FloatValue * -1 : FloatValue;
		// 0123456789
		char formater[10] = "%08d.%04d";

		int whole = FloatValue;
		FloatValue = FloatValue - whole;
		int mantissa = (FloatValue) * 10000;

		formater[2] = 48 + (count_digits(whole) + (neg?1:0)); //<--Leave one space for a '-' if needed
		formater[7] = 48 + (count_digits(mantissa));

		sprintf(ReturnBuffer, formater, whole, mantissa);
		//Cant change the format string +/-, but we can change the resulting string after the format is done!
		ReturnBuffer[0] = neg ? '-' : ReturnBuffer[0];
		
		//Last thing, trim off trailing zero's. the less serial data being sent the better
		for (int i=strlen(ReturnBuffer);i>0;i--)
		{
			if (ReturnBuffer[i]=='0' || ReturnBuffer[i]==' ')
			{
				ReturnBuffer[i]='\0';
			}
			else
			{
				break;
			}
		}
	}

	//This uses a global variable for conversion. eats up .2% of ram... 
	//static char* float_to_string_no_sign(float FloatValue)
	//{
	//memset(ConversionBuffer, 0, sizeof(char)*CONVERSION_BUFFER_SIZE);
	//bool neg = FloatValue < 0 ? true : false;
	//FloatValue = neg ? FloatValue * -1 : FloatValue;
	//// 0123456789
	//char formater[10] = "%08d.%04d";
	//
	//int whole = FloatValue;
	//FloatValue = FloatValue - whole;
	//int mantissa = (FloatValue) * 10000;
	//
	//formater[2] = 48 + (count_digits(whole) + (neg?1:0)); //<--Leave one space for a '-' if needed
	//formater[7] = 48 + (count_digits(mantissa));
	//
	//sprintf(ConversionBuffer, formater, whole, mantissa);
	////Cant change the format string +/-, but we can change the resulting string after the format is done!
	//ConversionBuffer[0] = neg ? '-' : ConversionBuffer[0];
	//
	////Last thing, trim off trailing zero's. the less serial data being sent the better
	//for (int i=strlen(ConversionBuffer)-1;i>0;i--)
	//{
	//if (ConversionBuffer[i]=='0' || ConversionBuffer[i]==' ')
	//{
	//ConversionBuffer[i]='\0';
	//}
	//else
	//{
	//break;
	//}
	//}
	//return ConversionBuffer;
	//}

	static void integer_to_string(int IntegerValue, char*ReturnBuffer, uint8_t Width)
	{
		bool neg = IntegerValue<0?true:false;
		IntegerValue = neg ? IntegerValue * -1 : IntegerValue;
		// 0123456789
		char formater[11] = "%8d";
		formater[1] = 48+Width;

		sprintf(ReturnBuffer, formater, IntegerValue);
		
	}
	
	
};



#endif