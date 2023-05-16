/**
 * @file SysTick.h
 * @author Michelle Tran
 * @brief Initialize SysTick timer to be able to refresh the LCD at 10Hz
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include "Nokia5110.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define REFRESH10HZ 8000000 - 1 // 80MHz / 10 Hz = 8,000,000

// initialize the interrupt to control screen refresh rate
void SysTick_Init(void); 

#endif