/**
 * @file SysTick.c
 * @author Michelle Tran
 * @brief Initialize SysTick timer
 */

#include "SysTick.h"

void SysTick_Init(void)
{
	// 1. Clear ENABLE to stop counter
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE; 
	// 2. Load Reload Value
	NVIC_ST_RELOAD_R = REFRESH10HZ; 
	// 3. Clear the counter via CURRENT register
	NVIC_ST_CURRENT_R = 0; // writing anything will clear
	// 4. Set interrupt priority
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000; // priority 1
	// 5. Set CLK_SRC = 1 to use system clock
	// set INTEN = 1 to enable interrupt
	// set ENABLE = 1 to start timer
	NVIC_ST_CTRL_R |= (NVIC_ST_CTRL_CLK_SRC + NVIC_ST_CTRL_INTEN + NVIC_ST_CTRL_ENABLE); 
}                     
