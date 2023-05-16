/**
 * @file ADC.h
 * @author Michelle Tran
 * @brief Initialize ADC to read a potentiometer's position to move the spaceship's position
 *      Pot's pin 2 connected to PE2/AIN1
 */

#ifndef ADC_H
#define ADC_H

#include "tm4c123gh6pm.h"
#include <stdint.h>

#define MAX_ADC_VALUE 4095
#define PE2 0x04 // 0000 0100

void ADC_Init();

void Pot_Conversion(uint16_t *ain2);

#endif