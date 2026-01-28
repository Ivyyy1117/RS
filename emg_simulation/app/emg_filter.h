#ifndef EMG_FILTER_H
#define EMG_FILTER_H

#include <stdint.h>

// Biquad filter structure
// Implements Direct Form I
// y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
typedef struct {
    float b0, b1, b2;
    float a1, a2;
    float x1, x2; // Previous inputs
    float y1, y2; // Previous outputs
} BiquadFilter;

// Initialize a filter with zero state
void Filter_Init(BiquadFilter *filter);

// Calculate coefficients for a Low Pass Filter
void Filter_CalcLowPass(BiquadFilter *filter, float sampleRate, float cutoffFreq, float q);

// Calculate coefficients for a High Pass Filter
void Filter_CalcHighPass(BiquadFilter *filter, float sampleRate, float cutoffFreq, float q);

// Calculate coefficients for a Notch Filter
void Filter_CalcNotch(BiquadFilter *filter, float sampleRate, float notchFreq, float q);

// Process a single sample through the filter
float Filter_Process(BiquadFilter *filter, float input);

#endif // EMG_FILTER_H
