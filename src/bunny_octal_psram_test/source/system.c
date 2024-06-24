
/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_gpio.h"

volatile uint32_t g_systickCounter = 20U;
/*******************************************************************************
 * Code
 ******************************************************************************/

volatile uint32_t mS_DelayTicker = 0;
volatile uint32_t LED_Ticker = 0;
volatile uint32_t BlinkRate = 500;

void SysTick_Handler(void)
{
	mS_DelayTicker++;

	if(LED_Ticker++ >= BlinkRate)
	{
		LED_Ticker = 0;
		GPIO_PortToggle(GPIO3, 1u << 19);
	}
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}



void system__delay_ms(uint32_t mS)
{
	mS_DelayTicker = 0;

	while(mS_DelayTicker<mS)
	{
	}
}


void system__init()
{

	SysTick_Config(SystemCoreClock/1000);

}

