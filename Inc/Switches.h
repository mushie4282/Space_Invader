/**
 * @file Switches.h
 * @author Michelle Tran
 * @brief Initialize the onboard switches to use rising edge interrupt to start
 *      the Space Invader game or shoot a bullet
 */

#ifndef SWITCHES_H
#define SWITCHES_H

#include "tm4c123gh6pm.h"

#define SW2_FLAG	(GPIO_PORTF_MIS_R & 0x01) // PF0 (start game)
#define SW1_FLAG	(GPIO_PORTF_MIS_R & 0x10) // PF4 (shoot laser)

// Initialize Port F to use Rising Edge interrupt to start/end the game
void Switch_Init(void);
#endif