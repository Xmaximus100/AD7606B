#include "DMA.h"

void DMA_Init(void)
{
	DMA0->DMA->SAR =  0x00010000;
	DMA0->DMA->DAR =  0x00010018;
}

