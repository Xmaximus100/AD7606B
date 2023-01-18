#include "MKL05Z4.h"
//----------------------------------------------
// SIM_SOPT2
//----------------------------------------------
// Zródlo zegara
//
#define REFSEL				6
#define VDRIVE				10
#define _PAR_SER			5
#define BUSY					3
#define CONTROL_DIODE	8
#define RANGE					7

#define D_OUT_A				8
#define D_OUT_B				9
#define D_OUT_C				10
#define D_OUT_D				11

#define MOSI			7
#define SCK				0
#define SS				5

typedef struct{
	uint8_t channel[8];
	uint8_t value;
	struct{
		uint8_t address;
		char status_header;
		char ext_os_clock;
		char dout_format;
		char operation_mode;
	} config;
} registers;

typedef union{
	int8_t fault;
	struct{ //mozliwe ze przez wstawienie struktury nieanonimowej, unia stracila 
		//mozliwosc sklejania bitow - trzeba sprawdzic
		uint16_t byte1;
		uint16_t byte2;
	} extraction;
	uint32_t word;
} data_ex;
void BUSY_EN(void);
void BUSY_DIS(void);
void AD7606B_Init(void);
void ResetDelay(void);
void Set_DOUT(void);
data_ex LoadBuffer(char, char);
data_ex Extract(uint32_t);


