/*
 * globals.h
 *
 * Created: 2/2/2019 9:01:36 PM
 *  Author: Family
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_
#define TIME_ARRAY_SIZE 15
//#define TIME_FACTOR 0.0000000625 //<-- using prescaler 1
#define TIME_FACTOR 0.000000125 //<-- using prescaler 8
extern  uint32_t over_flows;
extern  int16_t count;
extern  uint32_t signal_monitor;
extern  bool signal_lost;
extern uint32_t time_at_tick[TIME_ARRAY_SIZE];
extern uint8_t time_index;
extern int8_t encoder_direction;

#endif /* GLOBALS_H_ */