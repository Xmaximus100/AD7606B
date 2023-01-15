#include "spi.h"


void SPI0_Init(void)
{
	SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;							//Enabling SPI clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;							/* clock for PORTB */
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;							/* clock for PORTA */
	
	PORTA->PCR[MOSI] = PORT_PCR_MUX(ALT3);							//PTA7=MOSI
	PORTA->PCR[MISO] = PORT_PCR_MUX(ALT3);							//PTA6=MISO
	PORTA->PCR[SS] = PORT_PCR_MUX(ALT3);							//PTA5=SS
	//PORTA->PCR[SS] = PORT_PCR_MUX(ALT1);						//manual SS
	//PTA->PDDR |= 1<<SS;
	PORTB->PCR[SCK] = PORT_PCR_MUX(ALT3);							//PTB0=SCK
			
	
	//SPI0->C1 = ~(SPI_C1_MSTR_MASK | SPI_C1_LSBFE_MASK) & 0xFF;
	//SPI0->C2 &= SPI_C2_SPMIE_MASK;
	
	(void)SPI0->C1;
	(void)SPI0->C2;
	(void)SPI0->BR;
	
	SPI0->C1 = 0x5E & 0xFF; //dziala wysylanie MOSI
	//SPI0->C1 = 0xF6 & 0xFF;
	//SPI0->C2 = 0x10 & 0xFF; //dziala wysylanie MOSI - MODFEN(1)
	//SPI0->C2 = 0x91 & 0xFF; //dziala SPTF
	SPI0->C2 = 0x90 & 0xFF; //data sie zmienia, nie dziala SPTF
	SPI0->BR = 0x10 & 0xFF;
	SPI0->M = 0xAA & 0xFF;
	
	//NVIC_EnableIRQ(SPI0_IRQn);
	//NVIC_ClearPendingIRQ(SPI0_IRQn);
}

void CS_On(void){
	PTA->PSOR |= 1<<SS;
}

void CS_Off(void){
	PTA->PCOR |= 1<<SS;
}

uint8_t SPI0_Write(uint8_t data)
{
	//SPI0->C1 |= SPI_C1_SPIE_MASK;
	do{ //odkomentowac z oscyloskopem //fig_1
	while((SPI0->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK){}
	SPI0->D = data;											//mozemy utworzyc max 24-bitowa ramke
	while(!SPI0->S>>SPI_S_SPRF_SHIFT){} //kazde nadanie danych musi zostac potwierdzone przez odczytanie SPI0->S
	SPI0->D = 0x00;											//po odczycie mozemy dokleic jeszcze jedna ramke 
	while(!SPI0->S>>SPI_S_SPRF_SHIFT){}
	SPI0->D = 0xAA;	
	}while((SPI0->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK);
	//while(!SPI0->S>>SPI_S_SPRF_SHIFT){}
	return data;
}

uint8_t SPI0_Read(void)
{
	uint8_t data;
	do{ //odkomentowac z oscyloskopem //fig_1
		while((SPI0->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK);
		while(!SPI0->S>>SPI_S_SPRF_SHIFT){}
		data = SPI0->D; 
		while(!SPI0->S>>SPI_S_SPRF_SHIFT){}
		data = SPI0->D;
	}while((SPI0->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK);		
	return data;
}


