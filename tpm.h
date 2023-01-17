#include "MKL05Z4.h"

#define SCK_TPM		11
#define BUSY_ITR	12

void TPM0_Init(void);
void TPM1_Init(void);
	
void ClockToggle(void);
void ClockON(void);
void ClockOFF(void);

