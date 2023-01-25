#include "MKL05Z4.h"

#define SCK_TPM		11
#define CONVST_ITR	12

void TPM0_Init(void);
void TPM1_Init(uint32_t);
	
void ClockToggle(void);
void ClockON(void);
void ClockOFF(void);
void CONVST_ON(void);
void CONVST_OFF(void);
void TPM1_freq(uint32_t);

