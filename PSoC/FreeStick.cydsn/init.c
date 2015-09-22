#include <project.h>
#include "freestick.h"
#include "init.h"
#include "i2c_support.h"
#include "sx1509.h"

void startup(void)
{
  CYGlobalIntEnable;           					/* Enable Global interrupts */
  ADC_Start();        					/* Initialize ADC */
  Throttle_ADC_Start();
  Throttle_ADC_StartConvert();
  CyIntEnable(ADC_IRQ__INTC_NUMBER ); 	/* Enables ADC interrupts */
  Cell_Select_Mux_Start( );         						/* Reset all channels */
  Cell_Select_Mux_Select(0);        						/* Connect channel 0 */

  West_Load_Cell_Amp_Start();
  East_Load_Cell_Amp_Start();
  North_Load_Cell_Amp_Start();
  South_Load_Cell_Amp_Start();

  QuadDec_1_Start();
  QuadDec_2_Start();
  QuadDec_3_Start();
  QuadDec_4_Start();

  UART_Start();
  I2C_Start();

  USBFS_Start(0, USBFS_DWR_VDDD_OPERATION);	/* Start USBFS operation/device 0 and with 5V operation */
  while(!USBFS_bGetConfiguration());			/* Wait for Device to enumerate */
  {}
  USBFS_LoadInEP(1, (uint8 *)joystickData, JS_MESSAGE_SIZE); /* Loads an inital value into EP1 and sends it out to the PC */
  USBFS_LoadInEP(2, (uint8 *)keyboardData, KB_MESSAGE_SIZE);


  calibrateLoadCellOffsets();

  initKeypad();
}

void initButtons(void)
{
}

void calibrateLoadCellOffsets(void)
{
  uint8_t i = 0;
  uint8_t x = 0;
  int32_t adcVal = 0;

  for (i = 0; i < 4; i++)
  {
    channelOffset[i] = 0;
    Cell_Select_Mux_Select(i);
    for (x = 0; x < 16; x++)
    {
      ADC_StartConvert();
      ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
      adcVal = ADC_Read32();
      channelOffset[i] += adcVal;     
    }
    channelOffset[i] = channelOffset[i] / 16;
  }
}

void initKeypad(void)
{

  // Reset the port expander
  writeByte(KEYPAD_ADDR, REG_RESET, 0x12);	
  writeByte(KEYPAD_ADDR, REG_RESET, 0x34);	
  
	// Set regDir 0:7 outputs, 8:15 inputs:
  writeByte(KEYPAD_ADDR, REG_DIR_A, 0x00);
	writeByte(KEYPAD_ADDR, REG_DIR_B, 0xff);
	
	// Set regOpenDrain on 0:7:
	writeByte(KEYPAD_ADDR, REG_OPEN_DRAIN_A, 0xff);
	
	// Set regPullUp on 8:15:
	writeByte(KEYPAD_ADDR, REG_PULL_UP_B, 0xff);
	
	// Enable and configure debouncing on 8:15:
	writeByte(KEYPAD_ADDR, REG_DEBOUNCE_ENABLE_B, 0xff);
	writeByte(KEYPAD_ADDR, REG_DEBOUNCE_CONFIG, 0);	// Debounce must be less than scan time
	
	// RegKeyConfig1 sets the auto sleep time and scan time per row
	writeByte(KEYPAD_ADDR, REG_KEY_CONFIG_1, 0);
	
	// RegKeyConfig2 tells the SX1509 how many rows and columns we've got going
	writeByte(KEYPAD_ADDR, REG_KEY_CONFIG_2, 0b00111011);

  // RegClock needs to be set up to enable the internal 2MHz oscillator.
  // 7:   0  (reserved)
  // 6:5  10 (internal osc)
  // 4    0  (OSCIO pin input)
  // 3:0  0x0 (no output on OSCIO, OSCIO set to 0)
  writeByte(KEYPAD_ADDR, REG_CLOCK, 0b01000000);
}

