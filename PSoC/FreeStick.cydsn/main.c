#include <stdio.h>
#include <project.h>
#include <stdlib.h>
#include "sx1509.h"
#include "init.h"
#include "freestick.h"
#include "i2c_support.h"
#include "get_inputs.h"

#define DEBUG

CY_ISR_PROTO(Keypad_ISR);

int8_t joystickData[8] = {0,0,0,0,0,0,0,0};
uint8_t keyboardData[8] = {0,0,0,0,0,0,0,0};
uint8_t buttons[8] = {0,0,0,0,0,0,0,0};
uint8_t throttlePos = 0;
int32_t magX = 0;
int32_t magY = 0;

int32_t channelOffset[4] = {0,0,0,0};
int32_t signX = 1;
int32_t signY = 1;
int32_t channelData[4] = {0,0,0,0};
int32_t rawChannelData[4] = {0,0,0,0};

volatile uint8_t keypadRowData = 0;
volatile uint8_t keypadColData = 0;
volatile uint8_t keypadInterrupt = 0;

char buffer[128];

void main()
{
  Keypad_Int_StartEx(Keypad_ISR);
  startup(); 								/* Calls the proper start API for all the components */

  for(;;)
  {
    while(!USBFS_bGetEPAckState(1)) 	/* Wait for ACK before loading data */
    {}

    readJoystick();						/* Calls function to read joystick movement */
    readButtons();						/* Calls function to monitor button presses */
    readThrottle();

    joystickData[0] = throttlePos;
    joystickData[1] = magX;		
    joystickData[2] = magY;
    joystickData[3] = buttons[0];
    joystickData[4] = buttons[1];

    USBFS_LoadInEP(1, (uint8 *)joystickData, JS_MESSAGE_SIZE );
    
    while(!USBFS_bGetEPAckState(2)) 	/* Wait for ACK before loading data */
    {}
    if (keypadInterrupt != 0)
    {
      keypadInterrupt = 0;
      keyboardData[0] = 0;
      keyboardData[2] = 0;
      readKeypad();
    }
    USBFS_LoadInEP(2, (uint8 *)keyboardData, KB_MESSAGE_SIZE);
    keyboardData[0] = 0;
    keyboardData[2] = 0;
  }
}

CY_ISR(Keypad_ISR)
{
  keypadInterrupt = 1;
  keypadColData = readByte(KEYPAD_ADDR, REG_KEY_DATA_1);
  keypadRowData = readByte(KEYPAD_ADDR, REG_KEY_DATA_2);
  Keypad_Int_Pin_ClearInterrupt();
}

/* End of File */

