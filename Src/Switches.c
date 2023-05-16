/**
 * @file Switches.c
 * @author Michelle Tran
 * @brief Implementation of the onboard switches
 */

#include "Switches.h"

// Initialize Port F Pin 0 and Pin 4 to use Rising Edge interrupt to start/end the game
void Switch_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;	// 2) activate port F
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; 
	delay = SYSCTL_RCGCGPIO_R;            		// allow time to finish activating
	// Configure GPIO
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R = 0x11; 					// unlock PF0 & PF4
	GPIO_PORTF_PCTL_R &= ~0x11; 				// clear PCTL to select GPIO
	GPIO_PORTF_AFSEL_R &= ~0x11;                // no alternate functions on PF0 and PF4
	GPIO_PORTF_AMSEL_R &= ~0x11;         	    // disable analog functionality on PF0 and PF4
	GPIO_PORTF_DIR_R &= ~0x11; 				    // set PF0 & PF4 as input
	GPIO_PORTF_PUR_R |= 0x11; 				    // enable pull-up on PF0 & PF4
	GPIO_PORTF_DEN_R |= 0x11;           	    // enable digital I/O on PF0, PF4
	// Configure Interrupt
	GPIO_PORTF_IS_R &= ~0x11; 					// PF4 & PF0 is edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11; 					// PF4 & PF0 is not both edges
	GPIO_PORTF_IEV_R |= 0x11; 					// PF0 (SW2) & PF4 (SW1) rising edge event
	GPIO_PORTF_ICR_R = 0x11; 					// clear flag 0 & flag 4
	GPIO_PORTF_IM_R |= 0x11; 					// arm interrupt on PF4
	NVIC_PRI7_R &= ~0x00F00000; 				// clear bits 23-21
	NVIC_PRI7_R |= (0x00 << 20); 				// set priority 0 
	NVIC_EN0_R |= (0x01 << 30);
}