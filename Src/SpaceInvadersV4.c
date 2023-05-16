/**
 * @file SpaceInvadersV4.c
 * @author Dr. Min He, Michelle Tran, Grecia Fransico
 * @brief Space Invader project's main file
 * @date 2023-05-09
 */
// Reference:
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* The original example code comes the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
 // ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to pne side of the 1k resistor
// other side of the 1k resistor is connected to +3.3V 
// Onboard sw1(left push button): fire button 
// Onboard sw2(right push button): game start button

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3  [1k ohm]
// Reset         (RST, pin 4) connected to PA7  [10k ohm]
// Data/Command  (D/C, pin 5) connected to PA6  [10k ohm]
// SSI0Tx        (DN,  pin 6) connected to PA5  [10k ohm]
// SSI0Clk       (SCLK, pin 7) connected to PA2 [10k ohm]
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "tm4c123gh6pm.h" // provided predefined marcos of Launchpad registers
#include "Nokia5110.h"
#include "PLL.h" 
#include <stdint.h>
#include "ADC.h"
#include "SysTick.h"
#include "Prompt.h"
#include "Switches.h"
#include "Image_Array.h"

enum game_status{OVER, ON};
enum life_status{DEAD, ALIVE};
enum enemy_posture{CLOSE, OPEN};

#define PLAYERW     ((unsigned char)PlayerShip0[18]) // ship's width
#define PLAYERH     ((unsigned char)PlayerShip0[22]) // ship's height
#define ENEMY10W    	16
#define NUM_OF_ENEMY 	5
#define LASERH      	9 // laser's height
#define LASERW      	2 // laster's width
#define ENEMY_WIDTH 	18
#define ALL_ENEMY_DEAD 	if(num_of_enemy == 0) game_s = 0 // check if all enemies are dead to end the game
#define TOP_OF_NOKIA 	0 // represent the value at the top of the Nokia5110 LCD

// Basic struct of all sprites in the game
typedef struct Sprite {
  uint8_t x;      				// x coordinate
  uint8_t y;      				// y coordinate
  const unsigned char *image; 	// ptr -> image
  uint8_t life;            		// 0=dead, 1=alive
} STyp; // use `STyp` in alias of `struct Sprite`
STyp Enemy[NUM_OF_ENEMY];
STyp PlayerShip;
STyp Bullet;

// Function prototypes
extern void EnableInterrupts(void);  // defined in startup.s
extern void DisableInterrupts(void); // defined in startup.s
void Game_Init(void);
void Move(void);
void Draw(void);
void Switch_Init(void);
void System_Init(void); 

// global variables used for game control
volatile uint8_t time_to_draw = 0; // refresh rate flag
uint8_t game_s; // starting game flag
uint8_t score = 0; // player's score
// added global variables
uint16_t ADC_value; // variable storing potentiometer value
uint8_t num_of_enemy = NUM_OF_ENEMY; // number of enemy flags

int main(void){
	System_Init();

  while(1){
    Start_Prompt();
		
	while(game_s==OVER){};
     
	Game_Init(); // game initialization
		
    Draw(); // screen refresh
    while (game_s==ON) {
      if (time_to_draw){
        Move(); // increment images position
        Draw(); // screen refresh
        time_to_draw = 0;
      }
    }
	// pass in the tallied score to be displayed at the end of the game
    End_Prompt(score);
  }
}

/**
 * @brief Call all functions to initialize TM4C123 system
 * 
 */
void System_Init(void){
	DisableInterrupts();
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
	SysTick_Init();				// SysTick Clock used for 10Hz refresh rate 
	Switch_Init();				// Port F Init (onboard swtiches)
	ADC_Init();					// Ain2 (PE1) ADC initialize		
	Nokia5110_Init();
	Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();  // draw buffer
	EnableInterrupts();
	game_s = OVER; 
}

// Initialize the game: initialize all sprites and 
// reset refresh control and game status.
void Game_Init(void){
	time_to_draw = 0; // 0 = do NOT draw
	score = 0; // reset score 
	uint8_t i; // for-loop variable
	num_of_enemy = NUM_OF_ENEMY; // set number of enemies 

	// Version 2: enemy initialization.
	for(i=0 ; i < NUM_OF_ENEMY; i++){
		if(i > 2){ // 2nd row of enemies
			Enemy[i].x = (18*i) - 40;
			Enemy[i].y = 20;
			Enemy[i].image = SmallEnemy[4-i][0];
		}
		else{ // 1st row of enemies
			Enemy[i].x = 20*i;
			Enemy[i].y = 10;
			Enemy[i].image = SmallEnemy[i][0];
		}
		Enemy[i].life = ALIVE;
	}
	
  	// Version 3: player ship initialization
	// initialize the x position by reading the potentiometer then translating it to be within the LCD's range
	Pot_Conversion(&ADC_value);
	PlayerShip.x = ADC_value * (MAX_X-PLAYERW) / MAX_ADC_VALUE;
	PlayerShip.y = 48;
	PlayerShip.image = PlayerShip0;
	PlayerShip.life = ALIVE; 
	
	// Version 4:  Laser initialization
	Bullet.life = DEAD; 
	Bullet.image = Laser0;
}

// Update positions for all alive sprites.
// drawing reference is ALWAYS bottom left corner
void Move(void){
	uint8_t calculated_position = 0; // potentiometer's ADC reading
	uint8_t i; // for-loop iterator
    
	// Move Bullet
	if(Bullet.life == ALIVE)
	{
		if((Bullet.y + LASERH) > TOP_OF_NOKIA)
		{// if the bullet has NOT reached the enemy
			Bullet.y -= 2; // move the laser image up
		}
		else if((Bullet.y + LASERH) <= TOP_OF_NOKIA)
		{
			Bullet.life = DEAD; // remove the laser off the screen
		}
	}
	
 	// Move enemies if it is alive 
	// 1. check if a laser is touching the index enemy
	// 2. check if the index enemy's image is STILL a creature image
	for(i = 0; i < NUM_OF_ENEMY; i++) 
	{
		if(Enemy[i].life == ALIVE) // check if the enemy is alive
		{	
			// check if Bullet is within the y-range of an enemy & has not reach the top of the display & if it is active
			if((Bullet.y - LASERH) <= Enemy[i].y && (Bullet.y - LASERH) > TOP_OF_NOKIA && Bullet.life == ALIVE)
			{
				// check if the bullet is within the x-range of an enemy
				if((Bullet.x + LASERW) >= Enemy[i].x && (Bullet.x + LASERW) <= (Enemy[i].x + ENEMY_WIDTH))
				{
					// Version 4: add explosion initialization
					Enemy[i].image = SmallExplosion0; // change the enemy's image when one the bullet touches it 
					Bullet.life = DEAD; // deactivate the bullet
					++score; // increment the user's score
				}
			}
			// check if the enemy is still on the display with it's associated image
			if(Enemy[i].image != SmallExplosion0)
			{
				// check if the enemy is within the display's width
				if((Enemy[i].x + ENEMY_WIDTH - 1) < MAX_X)
				{
					Enemy[i].x += 1; // move the enemy's image to the right
				}
				else
				{ // Enemy reached the end of the display
					Enemy[i].life = DEAD;
					num_of_enemy--;
				}
			}
		}
	}
	
	// Read ADC and update player ship position: only x coordinate will be changed
	Pot_Conversion(&ADC_value); // read ADC value
	calculated_position = ADC_value * (MAX_X - PLAYERW)/MAX_ADC_VALUE; // using ratios, find the x-position in relations to the LCD
	// check if the calculated position is within range
	if(calculated_position < MAX_X && calculated_position > 0) PlayerShip.x = calculated_position; 
  
	// predefined marco that defines an
	// if statement checking if the number of enemies is equal to zero
	ALL_ENEMY_DEAD; 
}

// Update the screen: 
// clear display and update the screen with the 
// current positions of all sprites that are alive.
void Draw(void){
	uint8_t i; // for-loop iterator
  
	if (game_s == OVER) return;
  
	Nokia5110_ClearBuffer(); // delete everything off the display
  
  	// Update live enemies' positions in display buffer
  	// check if the image is still the sprite or explosion 
	for(i=0; i< NUM_OF_ENEMY; ++i)
	{
		if(Enemy[i].life == ALIVE)
		{
			// Change postures for the enemies: two postures: open/close
			if(Enemy[i].image != SmallExplosion0){
				// check if the enemy's row location
				if(Enemy[i].x % 2 == 1){
					if(i > 2){ 
						Enemy[i].image = SmallEnemy[4-i][OPEN];// 2nd row enemies
					} else {
						Enemy[i].image = SmallEnemy[i][OPEN]; // 1st row enemies
					}
					
				} else { 
					if(i > 2){
						Enemy[i].image = SmallEnemy[4-i][CLOSE];// 2nd row enemies
					} else {
						Enemy[i].image = SmallEnemy[i][CLOSE]; // 1st row enemies
					}
				}
				Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, Enemy[i].image, 0);
			} else {
				// display the explosion, then remove the enemy
				Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, Enemy[i].image, 0);
				Enemy[i].life = DEAD;
				--num_of_enemy;
			}
		}
	}
  
	// Update the player ship position in display buffer
	Nokia5110_PrintBMP(PlayerShip.x, PlayerShip.y, PlayerShip.image, 0);
	
	// Update the bullet position in display buffer if there is one.
	if(Bullet.life == ALIVE){
		Nokia5110_PrintBMP(Bullet.x, Bullet.y, Bullet.image, 0); 
	}	

	Nokia5110_DisplayBuffer(); // Update the display with information in display buffer
	 
}

/**
 * @brief SysTick timer handler to control refresh rate at 10Hz
 */
void SysTick_Handler(void){ time_to_draw = 1; }

/**
 * @brief handles rising edge interrupt on (PF0) SW2 & (PF4) SW1
 * SW1_FLAG & SW2_FLAG checks mask interrupt status register for associated bit (GPIO_PORTF_MIS_R)
 */
void GPIOPortF_Handler(void){
	// take care of button debounce
	for(int i = 0; i < 72424; i++){}

	// (PF4) SW1: shoot a bullet if there is NONE
	if(SW1_FLAG)
	{
		GPIO_PORTF_ICR_R = 0x10; // acknowledge interrupt Flag 4
		if(Bullet.life == DEAD) // make sure there is only 1 bullet per frame
		{
			Bullet.life = ALIVE;
			Bullet.x = PlayerShip.x + (PLAYERW/2) - 1;
			Bullet.y = PlayerShip.y - PLAYERH;
		}
	}
	// (PF0) SW2: start the game
	if(SW2_FLAG)
	{
		GPIO_PORTF_ICR_R = 0x01; // acknowledge interrupt Flag 0 
		game_s = ON; // set game flag
	}
}
