#include "MKL05Z4.h"
//----------------------------------------------
// SIM_SOPT2
//----------------------------------------------
// Zródlo zegara
//
#define REFSEL				6
#define ADC_RESET			10
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
	struct {
		uint32_t word1;
		uint32_t word2;
	} word64;
	char bytes[8];
} data_ex2;

void BUSY_Toggle(void);
void BUSY_EN(void);
void BUSY_DIS(void);
void SetAddress(void);
void AD7606B_Init(void);
void ResetDelay(void);
void ResetDiodeON(void);
void ResetDiodeOFF(void);
void Set_DOUT(void);
void SDI_config(void);



