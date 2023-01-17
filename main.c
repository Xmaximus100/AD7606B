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

#define LF 0xa //Enter

//UART baudrate=115200

uint8_t temp_spi = 0;
uint8_t data = 0;
uint8_t write_data = 0;
data_ex test;
data_ex output[4] = {0,0,0,0};
char temp_data_dout[] = {0x00, 0x00, 0x00, 0x00};
char abc_buf[] = {"Jazda jazda jazda\r\n"};
char def_buf[] = {"Biala gwiazda\r\n"};
char data_buf[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
char Too_Long[]="Zbyt dlugi ciag";
char Error[]="Zla komenda";
uint8_t rx_buf_pos=0;
char temp_uart,buf;
uint8_t rx_FULL=FALSE;
uint8_t too_long=0;
char rx_buf[16];
char tekst = 1;
char data_ok = FALSE;
char interrupt_enable = FALSE;

uint8_t i =0;
/******************************************************************************\
* Private prototypes
\******************************************************************************/	
void PORTB_IRQHandler(void);
void SPI0_IRQHandler(void);
void UART0_IRQHandler(void);
void TPM0_IRQHandler(void);
void UART_Transmission(void);
void AD7606_Set(uint8_t, uint8_t);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/


/**
 * @brief The main loop. 
 * 
 * @return NULL 
 */
int main (void) { 
	AD7606B_Init();
	SPI0_Init();
	AD7606_Set(0xAA,0xF2);
	Set_DOUT(); 
	UART0_Init();
	TPM0_Init();
	TPM1_Init();
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
		
		//UART_Transmission();
		
		
		if (data_ok) {
			for(i=0; i<4; i++) output[i].word = LoadBuffer(temp_data_dout[i], i).word; 
			//test = Extract(output[1]);
			if (output[0].fault != -1 || output[1].fault != -1 || output[2].fault != -1 || output[3].fault != -1){

				//interrupt_enable = TRUE;
				//for(i=0;data_buf[i]!='\0';i++) //trying to find spot where i can stop iterating the list
				for(i=0;i<40;i++)
				{
					while(!(UART0->S1 & UART0_S1_TDRE_MASK));
					UART0->D = data_buf[i];
				}
			}
			data_ok = FALSE;
		}
		
	}
}

void AD7606_Set(uint8_t address, uint8_t data){
	SPI0_Write(address, data);
}

void Send_SPI(uint16_t data_spi) {
	LoadBufferSPI(data_spi);	
}

void SPI0_IRQHandler() {
	if ((SPI0->S & SPI_S_SPTEF_MASK) == SPI_S_SPTEF_MASK){
		SPI0->D = write_data;
	}
	if ((SPI0->S & SPI_S_SPRF_MASK) == SPI_S_SPRF_MASK){
		temp_spi = SPI0->D;
	}
	if ((SPI0->S & SPI_S_SPMF_MASK) == SPI_S_SPMF_MASK){
		data = temp_spi;
			//SPI0->S |= SPI_S_SPMF_MASK;
	}
}


void UART0_IRQHandler() {
	if(UART0->S1 & UART0_S1_RDRF_MASK)
	{
		//PTB->PCOR |= 1<<VDRIVE;
		temp_uart=UART0->D;	// Odczyt wartosci z bufora odbiornika i skasowanie flagi RDRF
		//ClockToggle(); 
		if(!rx_FULL)
		{
			if(temp_uart!=LF)
			{
				if(!too_long)	// Jesli za dlugi ciag, ignoruj reszte znak�w
				{
					rx_buf[rx_buf_pos] = temp_uart;	// Kompletuj komende
					rx_buf_pos++;
					if(rx_buf_pos==16)
						too_long=1;		// Za dlugi ciag
				}
			}
			else
			{
				if(!too_long)	// Jesli za dlugi ciag, porzuc tablice
					rx_buf[rx_buf_pos] = 0;
				rx_FULL=1;
			}
		}
	NVIC_EnableIRQ(UART0_IRQn);
	}
}

void PORTB_IRQHandler(){
	if( PORTB->ISFR & (1 << BUSY) ) {
		CS_On();
		ClockON();
		PORTB->PCR[BUSY] &= ~PORT_PCR_ISF_SHIFT;
	}
	NVIC_EnableIRQ(PORTB_IRQn);
}

void TPM0_IRQHandler() {
	temp_data_dout[0] = ((PTA->PDIR & 0x0080)>>(D_OUT_A-1));
	temp_data_dout[1] = ((PTA->PDIR & 0x0100)>>(D_OUT_B-1));
	temp_data_dout[2] = ((PTA->PDIR & 0x0200)>>(D_OUT_C-1));
	temp_data_dout[3] = ((PTA->PDIR & 0x0400)>>(D_OUT_D-1));
	if (output[0].fault != -1 || output[1].fault != -1 || output[2].fault != -1 || output[3].fault != -1) { 
	sprintf(data_buf, "data=%x %x %x %x %x %x %x %x\r\n", output[0].extraction.byte1, output[0].extraction.byte2, 
	output[1].extraction.byte1, output[1].extraction.byte2, output[2].extraction.byte1, output[2].extraction.byte2, 
	output[3].extraction.byte1, output[3].extraction.byte2); 
	ClockOFF(); 
	CS_Off();
	}
	//sprintf(data_buf, "XX=%d%d%d%d\r\n", temp_data[0], temp_data[1], temp_data[2], temp_data[3]);}
	//sprintf(data_buf,"U=%.4fV\n",20.512310);
	data_ok = TRUE;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; // ToDo 2.1.8: Clear channel flag
}

void UART_Transmission(){
	if(rx_FULL)		// Czy dana gotowa?
	{
		if(too_long)
		{
			for(i=0;Too_Long[i]!=0;i++)	// Zbyt dlugi ciag
				{
					while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy nadajnik gotowy?
					UART0->D = Too_Long[i];
				}
				while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy nadajnik gotowy?
				UART0->D = 0xa;		// Nastepna linia
				too_long=0;
		}
		else
		{
			for(i=0;Error[i]!=0;i++)	// Zla komenda
				{
					while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy nadajnik gotowy?
					UART0->D = Error[i];
				}
			while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy nadajnik gotowy?
			UART0->D = 0xa;		// Nastepna linia
		}
		rx_buf_pos=0;
		rx_FULL=0;	// Dana skonsumowana
	}
}


