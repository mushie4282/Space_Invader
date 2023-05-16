/**
 * @file Prompt.c
 * @author Grecia Francisco
 * @brief Game's starting and ending prompt implementations on the Nokia5110
 */
#include "Prompt.h"

// Game start prompt
void Start_Prompt(void){ 
	Nokia5110_Init();
	Nokia5110_Clear();
	Nokia5110_SetCursor(3,0);
	Nokia5110_OutString((unsigned char *)"Space" );
	Nokia5110_SetCursor(3,1); 
	Nokia5110_OutString((unsigned char *)"Invader");
	Nokia5110_SetCursor(3,2); 
	Nokia5110_OutString((unsigned char *)"Press");
	Nokia5110_SetCursor(3,3); 
	Nokia5110_OutString((unsigned char *)"SW 2");
	Nokia5110_SetCursor(3,4); 
	Nokia5110_OutString((unsigned char *)"To Start");
} 

// Game end prompt for 3 seconds
void End_Prompt(uint8_t num){
	Nokia5110_Init();
	Nokia5110_Clear();
	Nokia5110_SetCursor(4,0);
	Nokia5110_OutString((unsigned char *)"Game" );
	Nokia5110_SetCursor(4,1);
	Nokia5110_OutString((unsigned char *)"Over");
	Nokia5110_SetCursor(4,2);
	Nokia5110_OutString((unsigned char *)"Nice");
	Nokia5110_SetCursor(4,3);
	Nokia5110_OutString((unsigned char *)"Try! ");
	Nokia5110_SetCursor(2,4);
	Nokia5110_OutString((unsigned char *)"Score:");
	Nokia5110_OutUDec(num);
	
	// 3 second busy wait delay based on 80 MHz system clock
	for(uint32_t n = 0; n < (10908600); ++n){}
}