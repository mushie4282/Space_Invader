/**
 * @file Prompt.h
 * @author Grecia Francisco
 * @brief Functions that contains the game's starting and ending prompt
 */
#ifndef PROMPT_H
#define PROMPT_H

#include "Nokia5110.h"
#include <stdint.h>

void Start_Prompt(void); // starting the game prompt
void End_Prompt(uint8_t num); // end game prompt

#endif