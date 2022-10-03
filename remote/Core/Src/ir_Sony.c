#include "IRremote.h"
#include "IRremoteInt.h"

// Code based on https://github.com/z3t0/Arduino-IRremote !
//
//==============================================================================
//                           SSSS   OOO   N   N  Y   Y
//                          S      O   O  NN  N   Y Y
//                           SSS   O   O  N N N    Y
//                              S  O   O  N  NN    Y
//                          SSSS    OOO   N   N    Y
//==============================================================================

#define IR_SONY_BITS                   12
#define IR_SONY_HDR_MARK             2400
#define IR_SONY_HDR_SPACE             600
#define IR_SONY_ONE_MARK             1200
#define IR_SONY_ZERO_MARK             600
#define IR_SONY_RPT_LENGTH          45000
#define IR_SONY_DOUBLE_SPACE_USECS    500  // usually ssee 713 - not using ticks as get number wrapround

//+=============================================================================
void  IRsend_sendSony (unsigned long data,  int nbits)
{
	// Header
	IRsend_mark(IR_SONY_HDR_MARK);
	IRsend_space(IR_SONY_HDR_SPACE);

	// Data
	for (unsigned long  mask = 1UL;  mask != (1UL << nbits);  mask <<= 1)
	{
		if (data & mask)
		{
			IRsend_mark(IR_SONY_ONE_MARK);
			IRsend_space(IR_SONY_HDR_SPACE);
		}
		else
		{
			IRsend_mark(IR_SONY_ZERO_MARK);
			IRsend_space(IR_SONY_HDR_SPACE);
    	}
  	}
}
