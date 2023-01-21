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
#define RST 0x20 //Space
#define TOG 'a'

//UART baudrate=115200

uint8_t temp_spi = 0;
uint8_t data = 0;
uint8_t write_data = 0;
data_ex test;
data_ex output[4] = {0,0,0,0};
char temp_data_dout[] = {0x00, 0x00, 0x00, 0x00};
char temp_buf[2][32];
uint32_t samples_amount = 0;
uint32_t SAMPLES_MAX =  UINT32_MAX;
uint32_t sample_iter = 0;

char abc_buf[] = {"Jazda jazda jazda\r\n"};
char def_buf[] = {"Biala gwiazda\r\n"};
char data_buf[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, 0x20,0x20,0x20,0x20,0x20};
char Too_Long[]="Zbyt dlugi ciag";
char Error[]="Zla komenda";	
	
uint8_t rx_buf_pos=0;
char temp_uart,buf;
uint8_t rx_FULL=FALSE;
uint8_t too_long=0;
uint16_t main_iter = 0;
uint16_t main_iter2 = 0;
char rx_buf[16];
char tekst = 1;
data_ex2 unionR = {0}; 
uint8_t i =0;

void CommunicationSetup(void);
void Reset(char value);
void CheckUART(void);
void PORTB_IRQHandler(void);
void SPI0_IRQHandler(void);
void UART0_IRQHandler(void);
void TPM0_IRQHandler(void);
void UART_Transmission(void);
void AD7606_Set(uint8_t, uint8_t);

char hello_word[]= {"hello\r\n"};;
char sending_data[]= {"wysylam dane\r\n"};;

void Send_uart(char data[], uint32_t size)
{
	
		for(i=0;i<size;i++) {
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = data[i];
		}	
}

int main (void) { 
	CommunicationSetup();
	Send_uart(hello_word, 7);
	while(1) {
		CheckUART();
		//if (sample_iter<samples_amount){
			if (main_iter2>16){ //moze sie wyjebac i trzeba dac 16
				Send_uart(sending_data, 11);
				main_iter=0;
				main_iter2=0;
				BUSY_EN();
				Send_uart(unionR.bytes, 8);
				unionR.word64.word1 = 0;
				unionR.word64.word2 = 0;
					//sample_iter++;
				}
		//}
		}
}

void CommunicationSetup(){
	AD7606B_Init();
	ResetDelay();
	SPI0_Init();
	AD7606_Set(0x02,0x10);
	for(int i=0;i<100;i++);
	AD7606_Set(0x03,0x0);
	for(int i=0;i<100;i++);
	AD7606_Set(0x04,0x0);
	for(int i=0;i<100;i++);
	Set_DOUT(); 
	UART0_Init();
	TPM0_Init();
	TPM1_Init();
}

void Reset(char value){
	BUSY_DIS();
	ClockOFF();
	CONVST_OFF();
	ResetDelay();
	ResetDiodeON();
	for(int i=0; i<10000;i++);
	ResetDiodeOFF();
	SPI0_Init();
	AD7606_Set(0x02,0x10);
	for(int i=0;i<100;i++);
	AD7606_Set(0x03,value);
	for(int i=0;i<100;i++);
	AD7606_Set(0x04,value);
	for(int i=0;i<100;i++);
	Set_DOUT(); 
	ClockON();
	CONVST_ON();
	BUSY_EN();
}

void AD7606_Set(uint8_t address, uint8_t data){
	SPI0_Write(address, data);
}

void CheckUART() {
	if(temp_uart == TOG)	{BUSY_Toggle(); ClockOFF();}
	else if(temp_uart == RST) {BUSY_DIS(); Reset(0x22);}
	else if(temp_uart == '1') {BUSY_DIS(); sample_iter=0; samples_amount=10; Reset(0x22);}
	else if(temp_uart == '2') {BUSY_DIS(); sample_iter=0; samples_amount=20; Reset(0x22);}
	else if(temp_uart == '3') {BUSY_DIS(); sample_iter=0; samples_amount=5; Reset(0x22);}
	temp_uart = 0;
}

void UART_Transmission() {
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
			if(atoi(rx_buf)>0 && atoi(rx_buf)<SAMPLES_MAX)	{
				samples_amount=atoi(rx_buf); 
				Reset(0x11);
			}
			else {
			for(i=0;Error[i]!=0;i++)	// Zla komenda
				{
					while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy nadajnik gotowy?
					UART0->D = Error[i];
				}
			while(!(UART0->S1 & UART0_S1_TDRE_MASK));	// Czy nadajnik gotowy?
			UART0->D = 0xa;		// Nastepna linia
			}
		}
		rx_buf_pos=0;
		rx_FULL=0;	// Dana skonsumowana
	}
}


void UART0_IRQHandler() {
	if(UART0->S1 & UART0_S1_RDRF_MASK)
	{
		//PTB->PCOR |= 1<<VDRIVE;
		temp_uart=UART0->D;	// Odczyt wartosci z bufora odbiornika i skasowanie flagi RDRF
		//if(temp_uart==Reset) BUSY_Toggle();
		//ClockToggle(); 
		/*
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
		*/
	NVIC_EnableIRQ(UART0_IRQn);
	}
}

void PORTB_IRQHandler(){
//proba zablokowania wywolania przerwania
//przez sygnal BUSY, gdy w trakcie pobierania danych
//FPTB->PSOR |= 1<<CONTROL_DIODE;
	CS_On();
	ClockON();
	BUSY_DIS();
	PORTB->PCR[BUSY] &= ~PORT_PCR_ISF_SHIFT;
	NVIC_EnableIRQ(PORTB_IRQn);
}
 uint32_t pomocnicze = 0;
void TPM0_IRQHandler() {	//Sygnal busy pojawia sie raz za razem, nie do konca rozumiem z czego to wynika
	//kompletnie nie czeka na przejscie danych, przerwanie jest jednak z pewnoscia za wolne, nalezy popracowac nad 
	//optymalizacja
	
	//PTB->PSOR |= 1<<RANGE;
	if(!(PTB->PDIR & 0x01)){ //sprawdzenie czy zbocze jest narastajace
		pomocnicze = PTA->PDIR;
		pomocnicze = pomocnicze  & 0x0300;
		pomocnicze = pomocnicze>>8;
		pomocnicze = pomocnicze<<main_iter;
		PTB->PSOR |= 1<<RANGE;
		//temp_buf[0][main_iter] = ((PTA->PDIR >>(D_OUT_A)) & 0x01); //wjednej linii //FPTA szybsze
		//temp_buf[1][main_iter] = ((PTA->PDIR >>(D_OUT_B)) & 0x01);
		if (main_iter!=16) {
			unionR.word64.word1 |= (((PTA->PDIR & 0x0300)>>(8)) << (main_iter*2));
			main_iter++;
		}
		else {
			unionR.word64.word2 |= (((PTA->PDIR & 0x0300)>>(8)) << (main_iter2*2));
			main_iter2++;
			if(main_iter2>16) {
				ClockOFF(); CS_Off();
			}
		}
	}
	PTB->PCOR |= 1<<RANGE;
	//na poczatku sprawdzalismy czy fault != -1 zarowno w petli glownej jak i tutaj
	//co sprawialo, ze wykonywal gdy dane jeszcze nie byly gotowe (w kolejnej iteracji)
	//roznicy jednak nie dalo sie zauwazyc, bez uzycia oscyloskopu
	
	//przerwanie trwalo jednak zbyt dlugo, postanowilismy ograniczyc je tylko do sprawdzenia pinow
	//FPTB->PCOR |= 1<<CONTROL_DIODE;
	//}
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK; // ToDo 2.1.8: Clear channel flag
}



