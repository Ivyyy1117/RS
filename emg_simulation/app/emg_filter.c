#include "emg_filter.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void Filter_Init(BiquadFilter *filter) {
    filter->b0 = 0.0f; filter->b1 = 0.0f; filter->b2 = 0.0f;
    filter->a1 = 0.0f; filter->a2 = 0.0f;
    filter->x1 = 0.0f; filter->x2 = 0.0f;
    filter->y1 = 0.0f; filter->y2 = 0.0f;
}

float Filter_Process(BiquadFilter *filter, float input) {
    // Direct Form I
    float output = filter->b0 * input + filter->b1 * filter->x1 + filter->b2 * filter->x2
                 - filter->a1 * filter->y1 - filter->a2 * filter->y2;

    // Shift state
    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = output;

    return output;
}

// Formulas from Audio EQ Cookbook (Robert Bristow-Johnson)

void Filter_CalcLowPass(BiquadFilter *filter, float sampleRate, float cutoffFreq, float q) {
    float w0 = 2.0f * M_PI * cutoffFreq / sampleRate;
    float alpha = sinf(w0) / (2.0f * q);
    float cosw0 = cosf(w0);

    float a0 = 1.0f + alpha;
    filter->b0 = ((1.0f - cosw0) / 2.0f) / a0;
    filter->b1 = (1.0f - cosw0) / a0;
    filter->b2 = ((1.0f - cosw0) / 2.0f) / a0;
    filter->a1 = (-2.0f * cosw0) / a0;
    filter->a2 = (1.0f - alpha) / a0;
    
    // printf("LP: b0=%f, a1=%f, a0=%f\n", filter->b0, filter->a1, a0);
}

void Filter_CalcHighPass(BiquadFilter *filter, float sampleRate, float cutoffFreq, float q) {
    float w0 = 2.0f * M_PI * cutoffFreq / sampleRate;
    float alpha = sinf(w0) / (2.0f * q);
    float cosw0 = cosf(w0);

    float a0 = 1.0f + alpha;
    filter->b0 = ((1.0f + cosw0) / 2.0f) / a0;
    filter->b1 = -(1.0f + cosw0) / a0;
    filter->b2 = ((1.0f + cosw0) / 2.0f) / a0;
    filter->a1 = (-2.0f * cosw0) / a0;
    filter->a2 = (1.0f - alpha) / a0;
    
    // printf("HP: b0=%f, a1=%f, a0=%f\n", filter->b0, filter->a1, a0);
}

void Filter_CalcNotch(BiquadFilter *filter, float sampleRate, float notchFreq, float q) {
    float w0 = 2.0f * M_PI * notchFreq / sampleRate;
    float alpha = sinf(w0) / (2.0f * q);
    float cosw0 = cosf(w0);

    float a0 = 1.0f + alpha;
    filter->b0 = 1.0f / a0;
    filter->b1 = (-2.0f * cosw0) / a0;
    filter->b2 = 1.0f / a0;
    filter->a1 = (-2.0f * cosw0) / a0;
    filter->a2 = (1.0f - alpha) / a0;
    
    // printf("Notch: b0=%f, a1=%f, a0=%f\n", filter->b0, filter->a1, a0);
}
