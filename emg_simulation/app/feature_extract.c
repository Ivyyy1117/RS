#include "feature_extract.h"
#include <math.h>

void Feature_RMS_Init(RMS_Calculator *rms, float window_size_ms, float sample_rate) {
    // Calculate alpha for Exponential Moving Average
    // Time constant tau = window_size_ms / 1000.0
    // alpha = 1 - exp(-1 / (sample_rate * tau))
    // Approximation: alpha = 1 / (sample_rate * tau) for large tau
    
    float tau = window_size_ms / 1000.0f;
    if (tau <= 0.0f) tau = 0.1f; // Default 100ms
    
    // Using simple smoothing factor approximation for efficiency
    // N = sample_rate * tau
    // alpha = 1/N
    float N = sample_rate * tau;
    rms->alpha = 1.0f / N;
    
    rms->mean_square = 0.0f;
}

float Feature_RMS_Update(RMS_Calculator *rms, float sample) {
    float square = sample * sample;
    
    // EMA: y[n] = alpha * x[n] + (1-alpha) * y[n-1]
    rms->mean_square = rms->alpha * square + (1.0f - rms->alpha) * rms->mean_square;
    
    return sqrtf(rms->mean_square);
}
