#ifndef IRremoteint_h
#define IRremoteint_h

#include <stdint.h>
#include "stm32f7xx_hal.h"

//------------------------------------------------------------------------------
// Information for the Interrupt Service Routine
//
#define IR_RAWBUF  101  // Maximum length of raw duration buffer

typedef struct
{
	// The fields are ordered to reduce memory over caused by struct-padding
	uint8_t       rcvstate;     // State Machine state
	uint16_t      recvpin;      // Pin connected to IR data from detector
	GPIO_TypeDef* recvpinport;  // Port of pin connected to IR data from detector
	uint16_t      blinkpin;
	GPIO_TypeDef* blinkpinport;
	uint8_t       blinkflag;          // true -> enable blinking of pin on IR processing
	uint8_t       rawlen;             // counter of entries in rawbuf
	unsigned int  timer;              // State timer, counts 50uS ticks.
	unsigned int  rawbuf[IR_RAWBUF];  // raw data
	uint8_t       overflow;           // Raw buffer overflow occurred
} irparams_t;

// ISR State-Machine : Receiver States
#define IR_STATE_IDLE      2
#define IR_STATE_MARK      3
#define IR_STATE_SPACE     4
#define IR_STATE_STOP      5
#define IR_STATE_OVERFLOW  6

// Allow all parts of the code access to the ISR data
// NB. The data can be changed by the ISR at any time, even mid-function
// Therefore we declare it as "volatile" to stop the compiler/CPU caching it
extern volatile irparams_t irparams;

//------------------------------------------------------------------------------
// Pulse parms are ((X*50)-100) for the Mark and ((X*50)+100) for the Space.
// First MARK is the one after the long gap
// Pulse parameters in uSec
//

// Due to sensor lag, when received, Marks  tend to be 100us too long and
//                                   Spaces tend to be 100us too short
#define IR_MARK_EXCESS     100

// microseconds per clock interrupt tick
#define IR_USECPERTICK     50

// Upper and Lower percentage tolerances in measurements
#define IR_TOLERANCE       25
#define IR_LTOL            (1.0 - (IR_TOLERANCE/100.))
#define IR_UTOL            (1.0 + (IR_TOLERANCE/100.))

// Minimum gap between IR transmissions
#define IR_GAP             5000
#define IR_GAP_TICKS       (IR_GAP/IR_USECPERTICK)

#define IR_TICKS_LOW(us)   ((int)(((us)*IR_LTOL/IR_USECPERTICK)))
#define IR_TICKS_HIGH(us)  ((int)(((us)*IR_UTOL/IR_USECPERTICK + 1)))

//------------------------------------------------------------------------------
// IR detector output is active low
//
#define IR_MARK   0
#define IR_SPACE  1

#endif
