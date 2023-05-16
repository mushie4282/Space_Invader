/**
 * @file ADC.c
 * @author Michelle Tran
 * @brief ADC function implementation to read ADC value of the potentiometer
 */

#include "ADC.h"

/**
 * @brief Initialize Ain2 sampling at 125k max samples
 *    SS3 triggering event: software trigger -> busy wait sampling
 */
void ADC_Init()
{
	volatile unsigned long delay;
  SYSCTL_RCGCADC_R |= 0x01; 		// 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOE; 	// 1) activate clock for Port E					
	while ((SYSCTL_RCGCGPIO_R & SYSCTL_RCGC2_GPIOE) == 0){} // 2) allow time for clock to stabilize

	// GPIO Setup
  GPIO_PORTE_DIR_R &= ~PE2;      // 3) make PE2 input
  GPIO_PORTE_AFSEL_R |= PE2;     // 4) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~PE2;      // 5) disable digital I/O on PE2         
  GPIO_PORTE_PCTL_R = GPIO_PORTE_PCTL_R & 0xFF00FF0F;
  GPIO_PORTE_AMSEL_R |= PE2;     // 6) enable analog functionality on PE2
	// ADC Setup
  ADC0_PC_R &= ~ADC_PC_SR_M;     // 8) clear max sample rate field
  ADC0_PC_R |= ADC_PC_SR_125K;   //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;         // 9) make Sequencer 3 the HIGHEST priority
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3; // 10) disable sample sequencer 3 (ADC SS3)
  ADC0_EMUX_R &= ~ADC_EMUX_EM3_M;   // 11) seq3 is software trigger
  ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+1; // 12) set channel Ain1 (PE2) (if that line doesn't work try 0x0001)
	// 13) no TS0 D0, yes END0 IE0 
  ADC0_SSCTL3_R = ADC_SSCTL3_END0 + ADC_SSCTL3_IE0; // sample sequence control
  ADC0_IM_R &= ~ADC_IM_MASK3;     // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;// 15) enable sample sequencer 3 (0x0008)
}

/**
 * @brief Busy wait ADC conversion
 * 
 * @param ain2 12-bit referemce result of ADC conversion
 */
void Pot_Conversion(uint16_t *ain2){
	ADC0_PSSI_R = ADC_PSSI_SS3;					  // 1) initiate SS3 sample
	while((ADC0_RIS_R & 0x08) == 0){}; 		// 2) wait for conversion done
	*ain2 = ADC0_SSFIFO3_R & 0xFFF; 			// 3) read results (lower 12-bits are used)
	ADC0_ISC_R = ADC_PSSI_SS3; 						// 4) acknowledge completion
}
