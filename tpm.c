#include "tpm.h"


void TPM0_Init(void) {
		
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;		// ToDo 2.1.1: Enable TPM1 mask in SCGC6 register
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);// ToDo 2.1.1: Choose MCGFLLCLK clock source
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	PORTB->PCR[SCK_TPM] = PORT_PCR_MUX(0x02);  
	//prescaler 0x04 za szybki
	TPM0->SC |= TPM_SC_PS(0x01);  				// ToDo 2.1.3: Set prescaler to 128
	TPM0->SC |= TPM_SC_CMOD(0x01);					// ToDo 2.1.4: For TMP1, select the internal input clock source
	
	//TPM0->CNT = 0;
	TPM0->MOD = 0x001F; //0015
	
	TPM0->SC &= ~TPM_SC_CPWMS_MASK; 		/* up counting */
	TPM0->CONTROLS[0].CnSC &= ~ (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK); //Output Capture toggle on overload
	TPM0->CONTROLS[0].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);  
  ////TPM0->CONTROLS[2].CnSC |= (TPM_CnSC_MSA_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
	
	//We set TPM interrupt to gather GPIO data each pulse 
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK; 
	NVIC_SetPriority(TPM0_IRQn, 1);  /* TPM1 interrupt priority level  */

	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);	/* Enable Interrupts */

}

void TPM1_Init(void) {
		
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;		// ToDo 2.1.1: Enable TPM1 mask in SCGC6 register
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x01);// ToDo 2.1.1: Choose MCGFLLCLK clock source
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; 
	PORTA->PCR[BUSY_ITR] = PORT_PCR_MUX(0x02);  
	
	TPM1->SC |= TPM_SC_PS(0x02);  				// ToDo 2.1.3: Set prescaler to 128
	TPM1->SC |= TPM_SC_CMOD(0x01);					// ToDo 2.1.4: For TMP1, select the internal input clock source
	
	TPM1->CNT = 0x0000;
	TPM1->MOD = 0x00F0; 
	TPM1->CONTROLS[0].CnV = 0x00A0;
	
	TPM1->SC &= ~TPM_SC_CPWMS_MASK; 		/* up counting */
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);	//Output capture pulse on match
  //TPM0->CONTROLS[2].CnSC |= (TPM_CnSC_MSA_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
	
	//TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK; 
	//NVIC_SetPriority(TPM1_IRQn, 0);  /* TPM1 interrupt priority level  */

	//NVIC_ClearPendingIRQ(TPM1_IRQn); 
	//NVIC_EnableIRQ(TPM1_IRQn);	/* Enable Interrupts */

}

void ClockToggle(void){
	//TPM0->SC ^=  TPM_SC_CMOD(0x01);
	TPM0->CONTROLS[0].CnSC ^= (TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
}

void ClockON(void){
	TPM0->SC |=  TPM_SC_CMOD(0x01);
	//TPM0->CONTROLS[0].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
}

void ClockOFF(void){ //wyzwolic przy odczytaniu pelnego bufora 32bity
	TPM0->CNT = 0x00; //zerowanie licznika
	TPM0->SC &=  ~(TPM_SC_CMOD(0x01));
	//TPM0->CONTROLS[0].CnSC &= ~(TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
}

