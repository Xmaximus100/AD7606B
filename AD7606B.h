#include "MKL05Z4.h"
//----------------------------------------------
// SIM_SOPT2
//----------------------------------------------
// Zródlo zegara
//
#define REFSEL				11
#define VDRIVE				10
#define _PAR_SER			5

typedef union{
	int8_t fault;
	struct{ //mozliwe ze przez wstawienie struyktury nieanonimowej, unia stracila 
		//mozliwosc sklejania bitow - trzeba sprawdzic
		uint16_t byte1;
		uint16_t byte2;
	} extraction;
	uint32_t word;
} data_ex;

void AD7606B_Init(void);
void Set_DOUT(void);
data_ex LoadBuffer(char, char);
data_ex Extract(uint32_t);


