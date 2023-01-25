#include "tpm.h"


void TPM0_Init(void) {
		
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	PORTB->PCR[SCK_TPM] = PORT_PCR_MUX(0x02);  
	TPM0->SC |= TPM_SC_PS(0x01);
	TPM0->SC |= TPM_SC_CMOD(0x01);
	//0XAFF DAJE NAM co 265us przerwanie czyli 3,75kHz 
	TPM0->MOD = 0x08F; 
	//MOD = 0x8F daje nam ok 73kHz i to jest bezpieczna wartosc
	//
	TPM0->SC &= ~TPM_SC_CPWMS_MASK;
	TPM0->CONTROLS[0].CnSC &= ~ (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK);
	TPM0->CONTROLS[0].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);  
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK; 
	NVIC_SetPriority(TPM0_IRQn, 1); 

	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);

}
//TPM1 STERUJE PINEM CONVST DO WYTKONANIA POMIARU PRZEZ ADC'KA
uint32_t fclk = 41943040;
uint32_t TPM1_MOD=0x39A;
void TPM1_Init(uint32_t target_freq) {
	
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;		
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);
	 
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	//PORTA->PCR[CONVST_ITR] = PORT_PCR_MUX(0x02);  
	//Zegar MCGFLLCLK=41943040Hz
	/*
	Obliczanie czestotliwosci tpm'a
	(prescaler*wartosc MOD)/42MHz
	czyli dla SC_PS 0x7 czyli prescaler 128 oraz MOD 0xF000
	mamy (128*61440)/42MHz = 187ms
	*/
	TPM1_MOD = fclk/(128*target_freq);
	TPM1->SC |= TPM_SC_PS(0x07); //ustawienie prescaller'a ma byc 0x7
	
	TPM1->CNT = 0x0000;
	
	TPM1->MOD = TPM1_MOD; //zostawic 0x19A
	TPM1->CONTROLS[0].CnV = 0xfa; // oraz 0xfa
	TPM1->SC &= ~TPM_SC_CPWMS_MASK; 
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);
	
	volatile int cokolwiek = 0;
	for(int i=0;i<1000;i++)
	{
		cokolwiek ++;
	}
	PORTA->PCR[CONVST_ITR] = PORT_PCR_MUX(0x02);
	TPM1->SC |= TPM_SC_CMOD(0x01);
	
}


/*
void TPM1_freq(uint32_t target_freq)
{
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;		
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	//PORTA->PCR[CONVST_ITR] = PORT_PCR_MUX(0x02);  
	
	
	TPM1_MOD = fclk/(128*target_freq);
	//TPM1->CONTROLS[0].CnV = TPM1_MOD;
	TPM1->MOD = TPM1_MOD;
	//TPM1->CONTROLS[0].CnV = (TPM1_MOD-0x10); // oraz 0xfa
	
	TPM1->SC &= ~TPM_SC_CPWMS_MASK; 
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK);
	PORTA->PCR[CONVST_ITR] = PORT_PCR_MUX(0x02);
}
*/
/*
void ClockToggle(void){
	TPM0->SC ^=  TPM_SC_CMOD(0x01);
}
*/
void ClockON(void){
	TPM0->SC |=  TPM_SC_CMOD(0x01);
}

void ClockOFF(void){ //wyzwolic przy odczytaniu pelnego bufora 32bity
	TPM0->CNT = 0x00; //zerowanie licznika
	TPM0->SC &=  ~(TPM_SC_CMOD(0x01));
}

void CONVST_OFF(void){
	PORTA->PCR[12] = PORT_PCR_MUX(1);
	PTA->PDDR |= 1<<12; 
	PTA->PSOR |= 1<<12; //SET
	TPM1->SC &=  ~TPM_SC_CMOD(0x01);
	TPM1->CNT = 0x00;
	//Ustawienie pinu CONV na stan wysoki zeby w trakcie wylaczenia pin nie wisial w powietrzu bo wtedy sie randomowo wykonuja pomiary

	
}
/*
void CONVST_ON(void){
	//TPM1->SC |=  TPM_SC_CMOD(0x01);
	TPM1_Init(TPM1_MOD);
} */

