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
extern  uint32_t time_of_last;
extern  uint32_t over_flows;
extern  uint32_t span;
extern  uint32_t avg_span;
extern  int32_t count;
extern  uint32_t signal_monitor;
extern  bool signal_lost;
extern  uint32_t rev_count_x1;
extern  uint32_t rev_count_x1mil;
extern  volatile uint32_t running_tick;
//////////////////////////////////////////////////////////////////////////

extern uint32_t time_at_tick[TIME_ARRAY_SIZE];
extern uint8_t time_index;
extern  uint16_t encoder_count;





#endif /* GLOBALS_H_ */