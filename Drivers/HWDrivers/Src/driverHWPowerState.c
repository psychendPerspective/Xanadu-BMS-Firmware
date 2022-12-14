#include "driverHWPowerState.h"

const PowerStatePortStruct driverHWPorts[NoOfPowersSTATs] = 		            // Holds all status configuration data
{
	{GPIOB,RCC_AHBENR_GPIOBEN,GPIO_PIN_6,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL},		    //Not used, STM32_GPIO4 on BMSv1.0 // P_STAT_POWER_ENABLE
	{GPIOB,RCC_AHBENR_GPIOBEN,GPIO_PIN_4,GPIO_MODE_INPUT,GPIO_NOPULL},				// P_STAT_BUTTON_INPUT
	{GPIOB,RCC_AHBENR_GPIOBEN,GPIO_PIN_5,GPIO_MODE_INPUT,GPIO_NOPULL}				// P_STAT_CHARGE_DETECT, goes high when charger is detected
};

void driverHWPowerStateInit(void) {
	GPIO_InitTypeDef PowerStatePortHolder;
	uint8_t STATPointer;
	
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();	
	
	for(STATPointer = 0; STATPointer < NoOfPowersSTATs; STATPointer++) {
		RCC->AHBENR |= driverHWPorts[STATPointer].ClkRegister;								// Enable clock de desired port
		PowerStatePortHolder.Mode = driverHWPorts[STATPointer].Mode;					// Push pull output
		PowerStatePortHolder.Pin = driverHWPorts[STATPointer].Pin;						// Points to status pin
		PowerStatePortHolder.Pull = driverHWPorts[STATPointer].Pull;					// No pullup
		PowerStatePortHolder.Speed = GPIO_SPEED_HIGH;													// GPIO clock speed
		HAL_GPIO_Init(driverHWPorts[STATPointer].Port,&PowerStatePortHolder);	// Perform the IO init 
	};
};

void driverHWPowerStateSetOutput(PowerStateIDTypedef outputPort, PowerStateStateTypedef newState) {
	HAL_GPIO_WritePin(driverHWPorts[outputPort].Port,driverHWPorts[outputPort].Pin,(GPIO_PinState)newState); // Set desired pin to desired state
};

bool driverHWPowerStateReadInput(PowerStateIDTypedef inputPort) {	
	return (bool) HAL_GPIO_ReadPin(driverHWPorts[inputPort].Port,driverHWPorts[inputPort].Pin);
};
