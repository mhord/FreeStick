#ifndef __get_inputs_h__
#define __get_inputs_h__

#define DEADZONE 50
#define AVE_CT   8

#define THROTTLE_MAX 720
#define THROTTLE_MIN 20

void readJoystick(void);  
void readButtons(void);  
void readThrottle(void);
void readKeypad(void);

#endif

