#include "AD7606B.h"

uint32_t buffer_ch[4];
char load_enable = 1;
uint8_t block_iter[] = {0,0,0,0};
uint32_t temp_ch[8];

void AD7606B_Init(void){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;							/* clock for PORTB */
	PORTB->PCR[REFSEL] = PORT_PCR_MUX(1);						
	PORTB->PCR[VDRIVE] = PORT_PCR_MUX(1);							
	PORTA->PCR[_PAR_SER] = PORT_PCR_MUX(1);
	
	
	PTB->PDDR |= 1<<REFSEL;
	PTB->PDDR |= 1<<VDRIVE;
	//PTB->PDDR |= 0<<_PAR_SER; //is input when reset
	
	PTB->PSOR |= 1<<REFSEL;	
	PTB->PSOR |= 1<<VDRIVE;	
}

void Set_DOUT(void){
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK); 
	PORTA->PCR[9] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[9] &= ~(PORT_PCR_PS_MASK);
	PORTA->PCR[10] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[10] &= ~(PORT_PCR_PS_MASK);
	PORTA->PCR[11] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[11] &= ~(PORT_PCR_PS_MASK);
	PORTA->PCR[12] = (PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK);
	PORTA->PCR[12] &= ~(PORT_PCR_PS_MASK);
	//PTA->PDDR &= (0<<9 | 0<<10 | 0<<11 | 0<<12); //reset value
}

data_ex LoadBuffer(char data, char block){
	data_ex something;
	if (load_enable) {
		buffer_ch[block] += data<<block_iter[block]; block_iter[block]++; 
	if(block_iter[block]>31) {
		block_iter[block] = 0;
		something.word = buffer_ch[block];
		//temp_ch[2*block+1] = buffer_ch[block] & 0xFF;
		buffer_ch[block] = 0;
		return something;
	}
	}
	something.fault = -1;
	return something;
}

data_ex Extract(uint32_t word0){
	data_ex something;
	something.word = word0;
	return something;
}



