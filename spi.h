#include "MKL05Z4.h"
//----------------------------------------------
// SIM_SOPT2
//----------------------------------------------
// Zródlo zegara
//
#define CLK_DIS 					0x00
#define MCGFLLCLK 				0x01
#define OSCERCLK					0x02
#define MCGIRCLK					0x03

#define ALT3 			3
#define ALT1 			1
#define MOSI			7
#define MISO			6
#define SCK				0
#define SS				5
#define _PAR_SEL	5


void SPI0_Init(void);

uint8_t SPI0_Write(uint8_t data);
uint8_t SPI0_Read(void);
