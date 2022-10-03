#ifndef IRremote_h
#define IRremote_h

#include "IRremoteInt.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// An enumerated list of all supported formats
// You do NOT need to remove entries from this list when disabling protocols!
//
typedef enum
{
	UNKNOWN      = -1,
	UNUSED       =  0,
	SONY
} ir_decode_type_t;

//------------------------------------------------------------------------------
// Set IR_DEBUG to 1 for debug output
//
#define IR_DEBUG  0

//------------------------------------------------------------------------------
// Debug directives
//
#if IR_DEBUG
#	define IR_DBG_PRINT_INT(integer)	printf("%d", integer)
#	define IR_DBG_PRINT(string)    		printf(string)
#	define IR_DBG_PRINTLN(string)  		{printf(string); printf("\n");}
#else
#	define IR_DBG_PRINT_INT(...)
#	define IR_DBG_PRINT(...)
#	define IR_DBG_PRINTLN(...)
#endif

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int  IR_MATCH       (int measured, int desired);
int  IR_MATCH_MARK  (int measured_ticks, int desired_us);
int  IR_MATCH_SPACE (int measured_ticks, int desired_us);

//------------------------------------------------------------------------------
// Functions for sending IR
//
extern void IRsend_enableIROut (uint32_t khz);
extern void IRsend_mark        (unsigned int usec);
extern void IRsend_space       (unsigned int usec);
extern void IRsend_sendRaw     (const unsigned int buf[], unsigned int len, unsigned int hz);

//......................................................................
extern void IRsend_sendSony (unsigned long data, int nbits);
//......................................................................

extern void Error_Handler(void);

#endif
