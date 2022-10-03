/*
   You can change this value accordingly to the receiver module you use.
   The required value can be derived from the timings printed here.
   Keep in mind that the timings may change with the distance
   between sender and receiver as well as with the ambient light intensity.
*/
#define MARK_EXCESS_MICROS    20 // recommended for the cheap VS1838 modules

//#define DEBUG // Activate this for lots of lovely debug output from the decoders.
#define INFO // To see valuable informations from universal decoder for pulse width or pulse distance protocols

//steppers
#define MOTOR_L_DIRECTION_PIN 4
#define MOTOR_L_STEP_PIN 5
#define MOTOR_R_DIRECTION_PIN 6
#define MOTOR_R_STEP_PIN 7

#include <Arduino.h>
/*
   Define macros for input and output pin etc.
*/
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

//+=============================================================================
// Configure the Arduino
//
void setup()
{
  pinMode(MOTOR_L_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR_L_STEP_PIN, OUTPUT);
  pinMode(MOTOR_R_DIRECTION_PIN, OUTPUT);
  pinMode(MOTOR_R_STEP_PIN, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);   // Status message will be sent to PC at 9600 baud

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

  // infos for receive
  Serial.print(RECORD_GAP_MICROS);
  Serial.println(F(" us is the (minimum) gap, after which the start of a new IR packet is assumed"));
  Serial.print(MARK_EXCESS_MICROS);
  Serial.println(F(" us are subtracted from all marks and added to all spaces for decoding"));
}

//+=============================================================================
// The repeating section of the code
//
void loop() {
  if (IrReceiver.decode()) {  // Grab an IR code
    // Check if the buffer overflowed
    /*if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
      Serial.println(F("Overflow detected"));
      Serial.println(F("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__));
      // see also https://github.com/Arduino-IRremote/Arduino-IRremote#compile-options--macros-for-this-library
    } else {
      Serial.println();                               // 2 blank lines between entries
      Serial.println();
      IrReceiver.printIRResultShort(&Serial);
      Serial.println();
      IrReceiver.printIRSendUsage(&Serial);
      Serial.println();
      Serial.println(F("Raw result in internal ticks (50 us) - with leading gap"));
      IrReceiver.printIRResultRawFormatted(&Serial, false); // Output the results in RAW format
      Serial.println(F("Raw result in microseconds - with leading gap"));
      IrReceiver.printIRResultRawFormatted(&Serial, true);  // Output the results in RAW format
      Serial.println();                               // blank line between entries
      Serial.print(F("Result as internal ticks (50 us) array - compensated with MARK_EXCESS_MICROS="));
      Serial.println(MARK_EXCESS_MICROS);
      IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, false); // Output the results as uint8_t source code array of ticks
      Serial.print(F("Result as microseconds array - compensated with MARK_EXCESS_MICROS="));
      Serial.println(MARK_EXCESS_MICROS);
      IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true); // Output the results as uint16_t source code array of micros
      IrReceiver.printIRResultAsCVariables(&Serial);  // Output address and data as source code variables

      //IrReceiver.printIRResultMinimal(&Serial);

      IrReceiver.compensateAndPrintIRResultAsPronto(&Serial);

    }*/
    /*
       !!!Important!!! Enable receiving of the next value,
       since receiving has stopped after the end of the current received data packet.
    */
    IrReceiver.resume();                            // Prepare for the next value

    /*
       Finally check the received data and perform actions according to the received address and commands
    */
    if (IrReceiver.decodedIRData.protocol == SONY && IrReceiver.decodedIRData.address == 0 && IrReceiver.decodedIRData.command < 0x04)
    {
      Serial.println(IrReceiver.decodedIRData.command);
      if (IrReceiver.decodedIRData.command == 0x00) //forward
      {
        digitalWrite(MOTOR_L_DIRECTION_PIN, LOW);
        digitalWrite(MOTOR_R_DIRECTION_PIN, HIGH);
      }
      else if (IrReceiver.decodedIRData.command == 0x01) //backward
      {
        digitalWrite(MOTOR_L_DIRECTION_PIN, HIGH);
        digitalWrite(MOTOR_R_DIRECTION_PIN, LOW);
      }
      else if (IrReceiver.decodedIRData.command == 0x02) //left
      {
        digitalWrite(MOTOR_L_DIRECTION_PIN, HIGH);
        digitalWrite(MOTOR_R_DIRECTION_PIN, HIGH);
      }
      else if (IrReceiver.decodedIRData.command == 0x03) //right
      {
        digitalWrite(MOTOR_L_DIRECTION_PIN, LOW);
        digitalWrite(MOTOR_R_DIRECTION_PIN, LOW);
      }

      for (int i = 0; i < 10; i++)
      {
        digitalWrite(MOTOR_L_STEP_PIN, HIGH);
        digitalWrite(MOTOR_R_STEP_PIN, HIGH);
        delay(10);
        digitalWrite(MOTOR_L_STEP_PIN, LOW);
        digitalWrite(MOTOR_R_STEP_PIN, LOW);
        delay(10);
      }
    }
  }
}
