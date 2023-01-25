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
#define RANGE_2V5 0x0
#define RANGE_5V 0x1
#define RANGE_10V 0x2

//UART baudrate=115200
uint32_t samples_amount = 0;
uint32_t SAMPLES_MAX =  UINT32_MAX;
uint32_t sample_iter = 0;

char Too_Long[]="Zbyt dlugi ciag";
char Error[]="Zla komenda";	
	
uint8_t rx_buf_pos=0;
char temp_uart,buf;
uint8_t rx_FULL=FALSE;
uint8_t too_long=0;
uint16_t main_iter = 0;
uint16_t main_iter2 = 0;
char rx_buf[16];
data_ex2 unionR = {0}; 
uint8_t i =0;

void adc_config(uint8_t, uint8_t);
void CommunicationSetup(void);
void Reset(char value);
void CheckUART(void);
void PORTB_IRQHandler(void);
void SPI0_IRQHandler(void);
void UART0_IRQHandler(void);
void TPM0_IRQHandler(void);
void range_confg(uint8_t,uint8_t,uint8_t,uint8_t);
uint8_t uart_flag=0;
//void UART_Transmission(void);
//void AD7606_Set(uint8_t, uint8_t);

char hello_word[]= "hello";
char sending_data[]= "W";

void Send_uart(char data[], uint32_t size)
{
	
		for(i=0;i<size;i++) {
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = data[i];
		}	
}
void Send_uart_temp(char data)
{
	
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = data;
}

int main (void) { 
	CommunicationSetup();
	//Send_uart(hello_word, 5);
	while(1) {

		
		//if (sample_iter<samples_amount){
			if (main_iter2>16){ 
				if(uart_flag !=0){
					CheckUART();
					uart_flag =0;
				}
				// calosc wysylania 64 bitow z baudrate 115k trwa 600us
		//Send_uart(sending_data, 1);
				main_iter=0;
				main_iter2=0;
				//PTB->PSOR |= 1<<RANGE;
		//Send_uart(unionR.bytes, 8);
				//PTB->PCOR |= 1<<RANGE;
				unionR.word64.word1 = 0;
				unionR.word64.word2 = 0;

				//adc_config(0x03,0x22); //ustawienie zakresu napiec na +/- 2.5V
				//adc_config(0x04,0x22); //ustawienie zakresu napiec na +/- 2.5V
				//range_confg(RANGE_10V,RANGE_10V,RANGE_5V,RANGE_5V);
				BUSY_EN();
					//sample_iter++;

				}
		//}
		}
}

void range_confg(uint8_t RANGE_CH1,uint8_t RANGE_CH2,uint8_t RANGE_CH3,uint8_t RANGE_CH4)
{
	uint16_t value = RANGE_CH1 | (RANGE_CH2<<4);
	//value = RANGE_CH1 + (RANGE_CH2<<4);
	adc_config(0x03,value);
	value = RANGE_CH3 | (RANGE_CH4<<4);
	adc_config(0x04,value);
}


void SPI_ON(void)
{
	BUSY_DIS();    //wylaczenie przerwania na pinie busy
	ClockOFF();    //wylaczenie timera do SCK software'owy
	//CONVST_OFF();  //wylaczenie timera wywolujacego przerwanie na convst
	SPI0_Init();   //Inicjalizacja SPI hardwarowego
	CS_On();       //stan niski na pinie CS
}

void SPI_OFF()
{
	CS_Off();      //stan wysoki na pinie CS
	Set_DOUT();    //ustawienie pinow dout oraz SPI na I/O
	SDI_config();  //ustawienie pinu SDI w stan niski  po zakonczeniu rozmow po SPI
	//CONVST_ON();   //wylaczenie timera wywolujacego przerwanie na convst
	BUSY_EN();     //wlaczenie przerwania na pinie busy
}

void delay()
{
	volatile int cokolwiek = 0;
	for(int i=0;i<10;i++)
	{
		cokolwiek ++;
	}
}

void adc_config(uint8_t addres, uint8_t value)
{
	SPI_ON();
	SPI0_Write(addres,value); // 1 dout 
	delay();
	//for(int i=0;i<3000;i++); //delay
	SPI_OFF();

}



void CommunicationSetup(){
	AD7606B_Init();
	UART0_Init();
	TPM0_Init();
	TPM1_Init(800);
	SPI_OFF();
	Reset_ADC();
	adc_config(0x02,0x10); // ustawienei zeby dane szly na 4 wyjscia dout
	adc_config(0x03,0x00); //ustawienie zakresu napiec na +/- 2.5V
	adc_config(0x04,0x00); //ustawienie zakresu napiec na +/- 2.5V	
}
/*
void Reset(char value){
	BUSY_DIS();
	ClockOFF();
	CONVST_OFF();
	//Reset_ADC(); //reset adc nie jest potrzebny
	ResetDiodeON(); //zapalenie ledem czerwonym
	//for(int i=0; i<10000;i++); //nie wiem po co to bylo
	SPI0_Init();
	CS_On();
	AD7606_Set(0x02,0x10); // 1 dout 
	//AD7606_Set(0x02,0x10); // 4 dout - poprawny
	delay();
	//for(int i=0;i<3000;i++);
	AD7606_Set(0x03,value);
	//for(int i=0;i<3000;i++);
	delay();
	AD7606_Set(0x04,value);
	//for(int i=0;i<3000;i++);
	delay();
	CS_Off();
	ResetDiodeOFF();
	Set_DOUT(); 
	SDI_config();  //ustawienie pinu SDI w stan niski  po zakonczeniu rozmow po SPI
	//ClockON();
	CONVST_ON();
	BUSY_EN();
	
}
*/
/*
void AD7606_Set(uint8_t address, uint8_t data){
	SPI0_Write(address, data);
}
*/
uint16_t uart_data[4]={0};
uint16_t uart_iter = 0;
void CheckUART() {
	if(temp_uart == '4' || uart_iter>0)
	{
		uart_data[uart_iter] = temp_uart;
		uart_iter++;
		if(uart_iter == 2)
		{
			adc_config(uart_data[0],uart_data[1]);
			uart_iter=0;
		}
		
	}
		//Send_uart_temp(temp_uart);

	if(temp_uart == '1') {TPM1_Init(100);
	range_confg(RANGE_10V,RANGE_10V,RANGE_5V,RANGE_5V);
	}
	else if(temp_uart == '2') {TPM1_Init(400);}
	else if(temp_uart == '3') {TPM1_Init(800);}
	temp_uart = 0;
	
	
	/* stary kod
	if(temp_uart == TOG)	{BUSY_Toggle(); ClockOFF();}
	else if(temp_uart == RST) {BUSY_DIS();}
	else if(temp_uart == '1') {BUSY_DIS(); sample_iter=0; samples_amount=10;}
	else if(temp_uart == '2') {BUSY_DIS(); sample_iter=0; samples_amount=20;}
	else if(temp_uart == '3') {BUSY_DIS(); sample_iter=0; samples_amount=5;}
	temp_uart = 0;
	*/
}
/*
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
*/


void UART0_IRQHandler() {
	if(UART0->S1 & UART0_S1_RDRF_MASK)
	{
		temp_uart=UART0->D;
		uart_flag++;
		//Send_uart_temp(temp_uart);
		NVIC_EnableIRQ(UART0_IRQn);
	}
}

void PORTB_IRQHandler(){
	CS_On();
	ClockON();
	BUSY_DIS();
	PORTB->PCR[BUSY] &= ~PORT_PCR_ISF_SHIFT;
	NVIC_EnableIRQ(PORTB_IRQn);
}

void TPM0_IRQHandler() {
	if(!(PTB->PDIR & 0x01)){ //sprawdzenie czy zbocze jest narastajace 
		//Ta funckja wykonuje sie ok 1,5us TO WAZNE!!!!
		//PTB->PSOR |= 1<<RANGE;
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
		//PTB->PSOR |= 1<<RANGE;
		//PTB->PCOR |= 1<<RANGE;
	}
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
}



