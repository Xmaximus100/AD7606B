#include "AD7606B.h"

uint32_t buffer_ch[4];
char load_enable = 1;
uint8_t block_iter[] = {0,0,0,0};
uint32_t temp_ch[8];
registers reg;

void SetAddress(void){
	reg.channel[0].address = 0x03;
	reg.channel[1].address = 0x03;
	reg.channel[2].address = 0x04;
	reg.channel[3].address = 0x04;
	reg.channel[0].value.RANGE2_5V = 0x00;
	reg.channel[0].value.RANGE5V = 0x01;
	reg.channel[0].value.RANGE10V = 0x02;
	reg.channel[1].value.RANGE2_5V = 0x00;
	reg.channel[1].value.RANGE5V = 0x01<<4;
	reg.channel[1].value.RANGE10V = 0x02<<4;
	reg.channel[2].value.RANGE2_5V = 0x00;
	reg.channel[2].value.RANGE5V = 0x01;
	reg.channel[2].value.RANGE10V = 0x02;
	reg.channel[3].value.RANGE2_5V = 0x00;
	reg.channel[3].value.RANGE5V = 0x01<<4;
	reg.channel[3].value.RANGE10V = 0x02<<4;
	reg.config.address = 0x02;
	reg.config.data = 0x10; 
}


//Wlaczenie/wylaczenie przerwania od pinu busy - to jest potrzebne bo na pin convst idzie sygnal cyklicznie co jakis czas
void BUSY_Toggle(void)
{
	PORTB->PCR[BUSY] ^= PORT_PCR_IRQC(0xA);
}

void BUSY_EN(void)
{
	PORTB->PCR[BUSY] |= PORT_PCR_IRQC(0xA);
}

void BUSY_DIS(void)
{
	PORTB->PCR[BUSY] &= ~(PORT_PCR_IRQC(0xA));
}


void AD7606B_Init(void){	
	SetAddress();
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK); 
	PORTB->PCR[REFSEL] = PORT_PCR_MUX(1);		
	PORTB->PCR[VDRIVE] = PORT_PCR_MUX(1);	
	PORTB->PCR[BUSY] = PORT_PCR_MUX(1);
	PORTB->PCR[_PAR_SER] = PORT_PCR_MUX(1);
	PORTB->PCR[RANGE] = PORT_PCR_MUX(1);
	PORTB->PCR[CONTROL_DIODE] = PORT_PCR_MUX(1); //ustawienie tej diody rowniez anuluje przerwanie 
//	| PORT_PCR_PE_MASK);
	PORTB->PCR[BUSY] |=  PORT_PCR_PE_MASK |		
											 PORT_PCR_PS_MASK;
	
	//PORTA->PCR[FIRSTDATA] = PORT_PCR_MUX(1);
	
	PORTB->PCR[BUSY] |= 	PORT_PCR_IRQC(0xA);
	
	
	PTB->PDDR |= 1<<RANGE; //ustawianie B7 jako RANGE, wywala przerwania na zegarze
	PTB->PDDR |= 1<<REFSEL;
	PTB->PDDR |= 1<<_PAR_SER;
	FPTB->PDDR |= 1<<CONTROL_DIODE;
	PTB->PDDR |= 1<<VDRIVE;
	//PTB->PDDR |= ~(1<<_PAR_SER); //is input when reset
	PTB->PDDR &= ~(1<<BUSY); //is input when reset
	
	//PTB->PSOR |= 1<<RANGE;
	FPTB->PSOR |= 1<<CONTROL_DIODE;
	PTB->PSOR |= 1<<REFSEL;	
	PTB->PSOR |= 1<<_PAR_SER;
	PTB->PCOR |= 1<<VDRIVE;	//przy resecie pin ma wartosc ~1V
	
	NVIC_ClearPendingIRQ(PORTB_IRQn);				/* Clear NVIC any pending interrupts on PORTC_B */
	NVIC_EnableIRQ(PORTB_IRQn);							/* Enable NVIC interrupts source for PORTC_B module */
	
	NVIC_SetPriority (PORTB_IRQn, 1);
}

void Set_DOUT(void){
	PORTA->PCR[SS] = PORT_PCR_MUX(0x01);	
	PORTA->PCR[MOSI] = PORT_PCR_MUX(0x01);	
	PORTB->PCR[SCK] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);	
	PORTB->PCR[SCK] &= ~(PORT_PCR_PS_MASK);
	//Setting up as GPIO input with pull down, unable to disactivate, line nieed to be bridged with pin B9
	PORTB->PCR[SCK] &= ~(PORT_PCR_PS_MASK); //nie ustawia sie!
	PORTA->PCR[D_OUT_A] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[D_OUT_A] &= ~(PORT_PCR_PS_MASK);
	PORTA->PCR[D_OUT_B] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[D_OUT_B] &= ~(PORT_PCR_PS_MASK);
	PORTA->PCR[D_OUT_C] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[D_OUT_C] &= ~(PORT_PCR_PS_MASK);
	PORTA->PCR[D_OUT_D] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[D_OUT_D] &= ~(PORT_PCR_PS_MASK);
	
	PTA->PDDR |= 1<<SS;
}




uint16_t SetRegister(uint8_t address, uint8_t data){	
	return ((address&0x3F)<<8) + (data&0xFF); 
}	

//error: busy - opadajace, a pojawiaja sie dane
void ResetDelay(void){
	PTB->PSOR |= 1<<VDRIVE;
	for(int i=0;i<1000;i++); //~170us
	PTB->PCOR |= 1<<VDRIVE;
	for(int i=0;i<2000;i++); //340us
}

void ResetDiodeON(void){
	FPTB->PCOR |= 1<<CONTROL_DIODE;
}

void ResetDiodeOFF(void){
	FPTB->PSOR |= 1<<CONTROL_DIODE;
}


