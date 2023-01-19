#include "AD7606B.h"

uint32_t buffer_ch[4];
char load_enable = 1;
uint8_t block_iter[] = {0,0,0,0};
uint32_t temp_ch[8];
registers reg;

void BUSY_EN(void)
{
	PORTB->PCR[BUSY] |= 	PORT_PCR_IRQC(0xA);
}

void BUSY_DIS(void)
{
	PORTB->PCR[BUSY] &= 	~(PORT_PCR_IRQC(0xA));
}


void AD7606B_Init(void){							/* clock for PORTB */
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK); 
	PORTB->PCR[REFSEL] = PORT_PCR_MUX(1);		
	PORTB->PCR[VDRIVE] = PORT_PCR_MUX(1);	
	PORTB->PCR[BUSY] = PORT_PCR_MUX(1);
	PORTB->PCR[_PAR_SER] = PORT_PCR_MUX(1);
	PORTB->PCR[RANGE] = PORT_PCR_MUX(1);
	//PORTB->PCR[CONTROL_DIODE] = PORT_PCR_MUX(1); //ustawienie tej diody rowniez anuluje przerwanie 
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

data_ex LoadBuffer(char data, char block){
	data_ex something;
	buffer_ch[block] += data<<block_iter[block]; block_iter[block]++; 
	if(block_iter[block]>31) {
		block_iter[block] = 0;
		something.package.word = buffer_ch[block];
		buffer_ch[block] = 0;
		return something;
	}
	something.package.fault = -1;
	return something;
}

data_ex Extract(uint32_t word0){
	data_ex something;
	something.package.word = word0;
	return something;
}

//bity WriteEnable=0, Read/~Write=0, ADD(5-0), MSB(7-0)

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



