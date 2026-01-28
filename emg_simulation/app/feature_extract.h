#ifndef FEATURE_EXTRACT_H
#define FEATURE_EXTRACT_H

typedef struct {
    float alpha; // Smoothing factor (0.0 - 1.0)
    float mean_square; // Current mean square value
} RMS_Calculator;

// Initialize RMS calculator
// window_size_ms: approximate window size in milliseconds
// sample_rate: sampling rate in Hz
void Feature_RMS_Init(RMS_Calculator *rms, float window_size_ms, float sample_rate);

// Update RMS with new sample and return current RMS value
float Feature_RMS_Update(RMS_Calculator *rms, float sample);

#endif // FEATURE_EXTRACT_H
