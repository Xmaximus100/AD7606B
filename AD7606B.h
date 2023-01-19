#include "MKL05Z4.h"
//----------------------------------------------
// SIM_SOPT2
//----------------------------------------------
// Zr�dlo zegara
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
	struct{
		uint8_t address;
		struct{
			uint8_t RANGE2_5V;
			uint8_t RANGE5V;
			uint8_t RANGE10V;
		} value;
	} channel[4];
	struct{
		uint8_t address;
		uint8_t data;
		char status_header;
		char ext_os_clock;
		char dout_format;
		char operation_mode;
	} config;
} registers;

typedef union{
	struct{ //mozliwe ze przez wstawienie struktury nieanonimowej, unia stracila 
		//mozliwosc sklejania bitow - trzeba sprawdzic
		uint16_t byte1;
		uint16_t byte2;
		int8_t fault;
	} extraction;
	struct{
	uint32_t word;
	int8_t fault;
	} package;
} data_ex;

void BUSY_Toggle(void);
void BUSY_EN(void);
void BUSY_DIS(void);
void SetAddress(void);
void AD7606B_Init(void);
void ResetDelay(void);
void ResetDiodeON(void);
void ResetDiodeOFF(void);
void Set_DOUT(void);
data_ex LoadBuffer(char, char);
data_ex Extract(uint32_t);


