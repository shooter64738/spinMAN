/*
* c_readers.h
*
* Created: 10/20/2019 1:09:45 PM
* Author: Family
*/


#ifndef __C_READERS_H__
#define __C_READERS_H__

#include <stdint.h>

class c_readers
{
	//variables
	public:
	protected:
	private:

	//functions
	public:
	static void no_vect();
	static void read_cha();
	static void read_chb();
	static void read_chz();
	//static void test_channels();
	static void read_quad();
	static void read_quad_with_z();
	static uint8_t read_active_channels();
	protected:
	private:

}; //c_readers

#endif //__C_READERS_H__
