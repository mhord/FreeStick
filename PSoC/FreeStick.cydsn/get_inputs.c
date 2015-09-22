#include <project.h>
#include <stdlib.h>
#include "freestick.h"
#include "get_inputs.h"
#include "sx1509.h"
#include "keycodes.h"

uint8_t REG_I_ON[16] = {REG_I_ON_0, REG_I_ON_1, REG_I_ON_2, REG_I_ON_3,
					REG_I_ON_4, REG_I_ON_5, REG_I_ON_6, REG_I_ON_7,
					REG_I_ON_8, REG_I_ON_9, REG_I_ON_10, REG_I_ON_11,
					REG_I_ON_12, REG_I_ON_13, REG_I_ON_14, REG_I_ON_15};
					
uint8_t REG_T_ON[16] = {REG_T_ON_0, REG_T_ON_1, REG_T_ON_2, REG_T_ON_3,
					REG_T_ON_4, REG_T_ON_5, REG_T_ON_6, REG_T_ON_7,
					REG_T_ON_8, REG_T_ON_9, REG_T_ON_10, REG_T_ON_11,
					REG_T_ON_12, REG_T_ON_13, REG_T_ON_14, REG_T_ON_15};
					
uint8_t REG_OFF[16] = {REG_OFF_0, REG_OFF_1, REG_OFF_2, REG_OFF_3,
					REG_OFF_4, REG_OFF_5, REG_OFF_6, REG_OFF_7,
					REG_OFF_8, REG_OFF_9, REG_OFF_10, REG_OFF_11,
					REG_OFF_12, REG_OFF_13, REG_OFF_14, REG_OFF_15};

uint8_t REG_T_RISE[16] = {0xFF, 0xFF, 0xFF, 0xFF,
					REG_T_RISE_4, REG_T_RISE_5, REG_T_RISE_6, REG_T_RISE_7,
					0xFF, 0xFF, 0xFF, 0xFF,
					REG_T_RISE_12, REG_T_RISE_13, REG_T_RISE_14, REG_T_RISE_15};
					
uint8_t REG_T_FALL[16] = {0xFF, 0xFF, 0xFF, 0xFF,
					REG_T_FALL_4, REG_T_FALL_5, REG_T_FALL_6, REG_T_FALL_7,
					0xFF, 0xFF, 0xFF, 0xFF,
					REG_T_FALL_12, REG_T_FALL_13, REG_T_FALL_14, REG_T_FALL_15};

void readJoystick(void)
{
  uint8_t i = 0;
  uint8_t x = 0;
  int32_t adcVal = 0;

  // Catch an average measurement for each load cell and store them away.
  for (i = 0; i < 4; i++)
  {
    Cell_Select_Mux_Select(i);
    for (x = 0; x < AVE_CT; x++)
    {
      ADC_StartConvert();
      ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
      adcVal = ADC_Read32();
      channelData[i] += adcVal - channelOffset[i];      
      rawChannelData[i] += adcVal;
    }
    channelData[i] /= AVE_CT;
    rawChannelData[i]  /= AVE_CT;
  }
  // Calculation of sign; is positive if stick pushed right,
  //  negative if stick pushed left. Negative when pulled back, positive when
  //  pushed forward.

  signX = (channelData[EAST] > channelData[WEST]) ? -1 : 1;
  signY = (channelData[NORTH] > channelData[SOUTH]) ? -1 : 1;

  // We want a dead zone to stop values on the threshold from bouncing back
  //  and forth if the difference between the cell values on an axis is small.
  //  We use an R-S value; this is to strip the sign and weight smaller
  //  changes more heavily when the values are larger. 
  if ( (abs(channelData[SOUTH]-channelData[NORTH]) > DEADZONE) )
  {
    //magY = (int32_t) sqrt( (channelData[NORTH]*channelData[NORTH]) 
    //    + (channelData[SOUTH])*(channelData[SOUTH]) );
    magY = abs(channelData[SOUTH]-channelData[NORTH])-DEADZONE;
    if (magY > 127)
    {
      magY = 127;
    }
    magY *= signY;
  }
  else
  {
    magY = 0;
  }
  if ( (abs(channelData[WEST]-channelData[EAST]) > DEADZONE) )
  {
    //magX = (int32_t) sqrt( (channelData[EAST]*channelData[EAST]) 
     //   + (channelData[WEST])*(channelData[WEST]) );
    magX =  abs(channelData[WEST]-channelData[EAST]) - DEADZONE;
    if (magX > 127)
    {
      magX = 127;
    }
    magX *= signX;
  }
  else
  {
    magX = 0;
  }

#if 0
  sprintf(buffer, "$,"      // start of packet
      "%ld,"    // magX*signX
      "%ld,"    // magY*signY
      "%ld,"    // east
      "%ld,"    // west
      //"0,0,"
      "%ld,"    // north
      "%ld,"    // south
      "#"      // end of packet
      //"\r"     // carriage return
      ,
      magX, 
      magY, 
      channelData[EAST], 
      channelData[WEST], 
      channelData[NORTH], 
      channelData[SOUTH]);
      /*rawChannelData[EAST], 
      rawChannelData[WEST], 
      rawChannelData[NORTH], 
      rawChannelData[SOUTH]);*/
  UART_PutString(buffer);
#endif

}

void readButtons(void)
{
  Chl_4_Write(0);
  
  if (Chl_0_Read() != 0) buttons[0] &= ~0x02;
  else buttons[0] |= 0x02;
  if (Chl_1_Read() != 0) buttons[0] &= ~0x04;
  else buttons[0] |= 0x04;
  if (Chl_2_Read() != 0) buttons[0] &= ~0x08;
  else buttons[0] |= 0x08;
  if (Chl_3_Read() != 0) buttons[0] &= ~0x01;
  else buttons[0] |= 0x01;

  Chl_4_Write(1);
  
  Chl_5_Write(0);
  
  if (Chl_0_Read() != 0) buttons[0] &= ~0x20;
  else buttons[0] |= 0x20;
  if (Chl_1_Read() != 0) buttons[0] &= ~0x40;
  else buttons[0] |= 0x40;
  if (Chl_2_Read() != 0) buttons[0] &= ~0x80;
  else buttons[0] |= 0x80;
  if (Chl_3_Read() != 0) buttons[0] &= ~0x10;
  else buttons[0] |= 0x10;

  Chl_5_Write(1);

  buttons[1] = 0;
  int8_t counterTemp = QuadDec_1_GetCounter();
  if (counterTemp < 0) buttons[1] |= 0x01;
  else if (counterTemp > 0) buttons[1] |= 0x02;

  counterTemp = QuadDec_2_GetCounter();
  if (counterTemp < 0) buttons[1] |= 0x04;
  else if (counterTemp > 0) buttons[1] |= 0x08;

  counterTemp = QuadDec_3_GetCounter();
  if (counterTemp < 0) buttons[1] |= 0x10;
  else if (counterTemp > 0) buttons[1] |= 0x20;

  counterTemp = QuadDec_4_GetCounter();
  if (counterTemp < 0) buttons[1] |= 0x40;
  else if (counterTemp > 0) buttons[1] |= 0x80;

  QuadDec_1_SetCounter(0);
  QuadDec_2_SetCounter(0);
  QuadDec_3_SetCounter(0);
  QuadDec_4_SetCounter(0);

}

void readThrottle(void)
{
  int16_t adcVal = 0;
  Throttle_ADC_IsEndConversion(Throttle_ADC_WAIT_FOR_RESULT);
  adcVal = Throttle_ADC_GetResult16();
  if (adcVal < THROTTLE_MIN)
  {
    throttlePos = 0;
    return;
  }
  if (adcVal > THROTTLE_MAX)
  {
    throttlePos = 100;
    return;
  }
  else
  {
    throttlePos = ( (adcVal - THROTTLE_MIN) * 100 ) / 
                  ( THROTTLE_MAX - THROTTLE_MIN );
  }   
}



// Comment on the layout of the keypad(s): this 8x4 keypad is really several
//  things at once: a 4x4 silicone keypad, a 3x4 phone-type keypad, and four
//  stand-alone buttons. Here's the key to how they correspond to return
//  values from the keypad controller:
//  Silicone:
//   O    O    O    O    row = 0xFE
//   O    O    O    O    row = 0xFD
//   O    O    O    O    row = 0xFB
//   O    O    O    O    row = 0xF7
//   0xFE 0xFD 0xFB 0xF7 = col
//   Phone pad:
//   1    2    3         row = 0xFE
//   4    5    6         row = 0xFD
//   7    8    9         row = 0xFB
//   *    0    #         row = 0xF7
//   0xEF 0xDF 0xBF      = col
//   Random buttons (below keypad) NB!!! these are rotated 90-deg from others!
//   O    O    O    O    col = 0x7F
//   0xFE 0xFD 0xFB 0xF7 = row
void readKeypad(void)
{
  switch (keypadColData)
  {
  case 0xFE:
    switch (keypadRowData)
    {
      case 0xFE:
        // Top left corner, silicone
        // "Select preset #1"
        keyboardData[2] = KEY_F5;
        break;
      case 0xFD:
        // Top left mid, silicone
        // "Select preset #2"
        keyboardData[2] = KEY_F6;
        break;
      case 0xFB:
        // Top right mid, silicone
        // "Select preset #3"
        keyboardData[2] = KEY_F7;
        break;
      case 0xF7:
        // Top right, silicone
        // "Nearest objective craft"
        keyboardData[2] = KEY_O;
        break;
      case 0xEF:
        // Keypad, 1
        keyboardData[2] = KEY_1;
        break;
      case 0xDF:
        // Keypad, 2
        keyboardData[2] = KEY_2;
        break;
      case 0xBF:
        // Keypad, 3
        keyboardData[2] = KEY_3;
        break;
      case 0x7F:
        // Left single button
        // "Cycle through nav buoys"
        keyboardData[2] = KEY_N;
        break;
      default:
        break;
    }
    break;

  case 0xFD:
    switch (keypadRowData)
    {
      case 0xFE:
        // Top mid left, silicone
        // "Set as preset 1"
        keyboardData[2] = KEY_F5;
        keyboardData[0] = KEY_LSHIFT;
        break;
      case 0xFD:
        // Top mid left mid, silicone
        // "Set as preset 2"
        keyboardData[2] = KEY_F6;
        keyboardData[0] = KEY_LSHIFT;
        break;
      case 0xFB:
        // Top mid right mid, silicone
        // "Set as preset 3"
        keyboardData[2] = KEY_F7;
        keyboardData[0] = KEY_LSHIFT;
        break;
      case 0xF7:
        // Top mid right, silicone
        // "Target attacker of target"
        keyboardData[2] = KEY_A;
        break;
      case 0xEF:
        // Keypad, 4
        // "Wait for orders"
        keyboardData[2] = KEY_4;
        break;
      case 0xDF:
        // Keypad, 5
        // "Go ahead"
        keyboardData[2] = KEY_5;
        break;
      case 0xBF:
        // Keypad, 6
        // "Board to reload"
        keyboardData[2] = KEY_6;
        break;
      case 0x7F:
        // Left mid single button
        // "Acknowledge"
        keyboardData[2] = KEY_SPC;
        break;
      default:
        break;
    }
    break;

  case 0xFB:
    switch (keypadRowData)
    {
      case 0xFE:
        // Bottom mid left, silicone
        // "Dock with target"
        keyboardData[2] = KEY_D;
        keyboardData[0] = KEY_LSHIFT;
        break;
      case 0xFD:
        // Bottom mid left mid, silicone
        // "Pick up target"
        keyboardData[2] = KEY_P;
        keyboardData[0] = KEY_LSHIFT;
        break;
      case 0xFB:
        // Bottom mid right mid, silicone
        // "Release carried object"
        keyboardData[2] = KEY_R;
        keyboardData[0] = KEY_LSHIFT;
        break;
      case 0xF7:
        // Bottom mid right, silicone
        // Countermeasures
        keyboardData[2] = KEY_C;
        break;
      case 0xEF:
        // Keypad, 7
        keyboardData[2] = KEY_7;
        break;
      case 0xDF:
        // Keypad, 8
        keyboardData[2] = KEY_8;
        break;
      case 0xBF:
        // Keypad, 9
        keyboardData[2] = KEY_9;
        break;
      case 0x7F:
        // Right mid single button
        // "Cycle gunner AI"
        keyboardData[2] = KEY_F;
        break;
      default:
        break;
    }
    break;

  case 0xF7:
    switch (keypadRowData)
    {
      case 0xFE:
        // Bottom left, silicone 
        // "Cycle forward through components"
        keyboardData[2] = KEY_PERIOD;
        break;
      case 0xFD:
        // Bottom left mid, silicone
        // "Cycle revers through components"
        keyboardData[0] = KEY_LSHIFT;
        keyboardData[2] = KEY_PERIOD;
        break;
      case 0xFB:
        // Bottom right mid, silicone
        // "Target nearest craft targeting me"
        keyboardData[2] = KEY_E;
        break;
      case 0xF7:
        // Bottom right, silicone
        // "Target incoming warhead"
        keyboardData[2] = KEY_I;
        break;
      case 0xEF:
        // Keypad, *
        // "See wingman commands MFD"
        keyboardData[2] = KEY_TAB;
        break;
      case 0xDF:
        // Keypad, 0
        keyboardData[2] = KEY_0;
        break;
      case 0xBF:
        // Keypad, #
        keyboardData[2] = KEY_LT;
        break;
      case 0x7F:
        // Right single button
        // "Switch to gunner position"
        keyboardData[2] = KEY_G;
        break;
      default:
        break;
    }
    break;
  }
  keypadColData = 0;
  keypadRowData = 0;
}
/* [] END OF FILE */

