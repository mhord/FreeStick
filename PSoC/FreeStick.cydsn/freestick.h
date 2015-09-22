#ifndef __freestick_h__
#define __freestick_h__

#define WEST 0
#define EAST 1
#define NORTH 2
#define SOUTH 3

#define JS_MESSAGE_SIZE 5
#define KB_MESSAGE_SIZE 8

extern int32_t channelOffset[4];
extern uint8_t keyboardData[8];
extern int8_t joystickData[8];
extern uint8_t throttlePos;
extern uint8_t buttons[8];
extern int32_t magX;
extern int32_t magY;
extern volatile uint8_t keypadRowData;
extern volatile uint8_t keypadColData;
extern volatile uint8_t keypadInterrupt;
extern int32_t signX;
extern int32_t signY;
extern int32_t channelData[4];
extern int32_t rawChannelData[4];

extern volatile uint8_t keypadRowData;
extern volatile uint8_t keypadColData;
extern volatile uint8_t keypadInterrupt;

extern char buffer[128];

#endif

