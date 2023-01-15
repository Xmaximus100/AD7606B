/**************************************************************
 * This file is a part of the PWM Demo (C).                   *
 **************************************************************/
/**
 * @file main.c
 * @author Koryciak
 * @date Nov 2020 
 * @brief File containing the main function. 
 * @ver 0.5
 */
 
#include "defines.h"
#include "spi.h"
#include "uart0.h"
#include "tpm.h"
#include "AD7606B.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//UART baudrate=115200

uint8_t temp = 0;
uint8_t data = 0;
uint8_t write_data = 0;
data_ex test;
int output[4] = {0,0,0,0};
char temp_data[] = {0x00, 0x00, 0x00, 0x00};
char abc_buf[] = {"Jazda jazda jazda\r\n"};
char def_buf[] = {"Biala gwiazda\r\n"};
char data_buf[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
char tekst = 1;
char data_ok = FALSE;
char interrupt_enable = FALSE;

uint8_t i =0;
uint8_t rx_FULL=FALSE;
/******************************************************************************\
* Private prototypes
\******************************************************************************/	
void SPI0_IRQHandler(void);
void UART0_IRQHandler(void);
void TPM0_IRQHandler(void);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/


/**
 * @brief The main loop. 
 * 
 * @return NULL 
 */
int main (void) { 
	TPM0_Init();
	//AD7606B_Init();
	//SPI0_Init();
	Set_DOUT();
	UART0_Init();
	//write_data = SPI0_Write(0xAA);
	//SPI0->M = write_data;
	int iter = 0;
	while(1) {
		
		//write_data = SPI0_Write(0xFF); //gdy tu byla ustawiona i++ to ramka tworzyla sie w nast konfiguracji [i][0x00//albo inne wewnatrz fkcji][i+1]
		
		//jednak przy konkretnej wartosci jest ona kopiowana na koniec, np. [0xFF][0x00][0xFF]
		//write_data = SPI0_Read();
		
		//for(int iter = 0; iter<1000; iter++);
		
		//while((SPI0->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK){} //fig_1
		//while(!SPI0->S>>SPI_S_SPRF_SHIFT){}
			//SPI0->S |= SPI_S_SPMF_MASK;
		
		/* //sending data through uart
					while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy anadajnik gotowy?
		switch (tekst){
			case 1:
				UART0->D = abc_buf[i];
				i=(i+1)%sizeof(abc_buf);
			for(int i=0; i<300000; i++);
			if (i == 0) {tekst = 2; for(int i=0; i<5000000; i++);}
				break;
			case 2:
				UART0->D = def_buf[i];
				i=(i+1)%sizeof(def_buf);
			for(int i=0; i<400000; i++);
			if (i == 0) {tekst = 1; for(int i=0; i<5000000; i++);}
				break;
			}	
		*/
		if (data_ok) {
			for(i=0; i<4; i++) output[i] = LoadBuffer(temp_data[i], i).word; 
			test = Extract(output[1]);
			if (output[0] != -1 || output[1] != -1 || output[2] != -1 || output[3] != -1)
			interrupt_enable = TRUE;
			for(i=0;i<40;i++)
			{
				while(!(UART0->S1 & UART0_S1_TDRE_MASK));
				UART0->D = data_buf[i];
			}
			data_ok = FALSE;
		}
	}
}

void SPI0_IRQHandler(void) {
	if ((SPI0->S & SPI_S_SPTEF_MASK) == SPI_S_SPTEF_MASK){
		SPI0->D = write_data;
	}
	if ((SPI0->S & SPI_S_SPRF_MASK) == SPI_S_SPRF_MASK){
		temp = SPI0->D;
	}
	if ((SPI0->S & SPI_S_SPMF_MASK) == SPI_S_SPMF_MASK){
		data = temp;
			//SPI0->S |= SPI_S_SPMF_MASK;
	}
}


void UART0_IRQHandler() {
	if(UART0->S1 & UART0_S1_RDRF_MASK)
	{
		temp=UART0->D;	// Odczyt wartosci z bufora odbiornika i skasowanie flagi RDRF
		if(!rx_FULL)
		{
			rx_FULL=TRUE;
		}
		ClockToggle();
	}
	NVIC_EnableIRQ(UART0_IRQn);
}


void TPM0_IRQHandler() {
	temp_data[0] = ((PTA->PDIR & 0x0100)>>8);
	temp_data[1] = ((PTA->PDIR & 0x0200)>>9);
	//temp_data[2] = ((PTA->PDIR & 0x0400)>>10) + '0';
	temp_data[2] = ((PTA->PDIR & 0x0400)>>10);
	temp_data[3] = ((PTA->PDIR & 0x0800)>>11);
	if (output[0] != -1 || output[1] != -1 || output[2] != -1 || output[3] != -1) { 
	/*
	output[0] += '0'; 
	output[1] += '0';
	output[2] += '0';
	output[3] += '0';
	*/
	sprintf(data_buf, "%d %d %d %d %d\r\n", output[0], test.extraction.byte1, test.extraction.byte2 , output[2], output[3]); }
	//sprintf(data_buf, "XX=%d%d%d%d\r\n", temp_data[0], temp_data[1], temp_data[2], temp_data[3]);}
	//sprintf(data_buf,"U=%.4fV\n",20.512310);
	data_ok = TRUE;
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK; // ToDo 2.1.8: Clear channel flag
}
