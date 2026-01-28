#ifndef ADC_SIM_H
#define ADC_SIM_H

#include <stdint.h>

#define ADC_CHANNELS 4
#define SAMPLE_RATE 1000 // 1kHz sampling rate

void ADC_Init(void);
void ADC_Read(float *buffer, int num_channels);

#endif // ADC_SIM_H
