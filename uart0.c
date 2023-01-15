#include "uart0.h"

void UART0_Init(void)
{
	uint8_t osr_val = 16; //with osr value we can compensate baudrate error - for 115200 we use osr=16, for 9600 32, for 921600 osr=11
	uint32_t sbr_val;
	uint32_t uart0clk = 41943040;
	uint32_t baud_rate = 115200;
	uint32_t reg_temp = 0;
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;							//UART0 dolaczony do zegara
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;							//Port B dolaczony do zegara
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MCGFLLCLK);		//Zegar MCGFLLCLK=41943040Hz (CLOCK_SETUP=0)
	PORTB->PCR[1] = PORT_PCR_MUX(2);								//PTB1=TX_D
	PORTB->PCR[2] = PORT_PCR_MUX(2);								//PTB2=RX_D
	
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );		//Blokada nadajnika i o dbiornika
	
	//using NXP example code 
	reg_temp = UART0->C4;	
	reg_temp &= ~UART0_C4_OSR_MASK;
	reg_temp |= UART0_C4_OSR(osr_val-1);

	// Write reg_temp to C4 register
	UART0->C4 = reg_temp;
	
	reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
	sbr_val = (uint32_t)((uart0clk)/(baud_rate * (reg_temp)));
	
	 /* Save off the current value of the uartx_BDH except for the SBR field */
	reg_temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));

	UART0->BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
	UART0->BDL = (uint8_t)(sbr_val & UART0_BDL_SBR_MASK);
	
	//UART0->BDH = 0;
	//UART0->BDL =91;		//Dla CLOCK_SETUP=0 BR=28800	BDL=91	:	CLOCK_SETUP=1	BR=230400		BDL=13
	//UART0->C4 &= ~UART0_C4_OSR_MASK;
	//UART0->C4 |= UART0_C4_OSR(15);	//Dla CLOCK_SETUP=0 BR=28800	OSR=15	:	CLOCK_SETUP=1	BR=230400		OSR=15
	//UART0->C5 |= UART0_C5_BOTHEDGE_MASK;	//Próbkowanie odbiornika na obydwu zboczach zegara
	
	UART0->C2 |= UART0_C2_RIE_MASK;		// Wlacz przerwania od odbiornika
	UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);		//Wlacz nadajnik i odbiornik
	
	NVIC_SetPriority(UART0_IRQn, 1);
	
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}
