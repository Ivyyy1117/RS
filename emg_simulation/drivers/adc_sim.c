#include "adc_sim.h"
#include <stdlib.h>
#include <math.h>

static uint32_t tick_counter = 0;

void ADC_Init(void) {
    tick_counter = 0;
    // Seed random number generator
    srand(42); 
}

// Generate synthetic EMG signal
// Signal = Muscle_Signal + 50Hz_Noise + Random_Noise
void ADC_Read(float *buffer, int num_channels) {
    float t = (float)tick_counter / SAMPLE_RATE;
    
    for (int i = 0; i < num_channels; i++) {
        // 1. 50Hz Power Line Interference (Noise)
        float noise_50hz = 0.5f * sinf(2.0f * 3.14159f * 50.0f * t);
        
        // 2. Random broadband noise
        float white_noise = ((float)rand() / RAND_MAX - 0.5f) * 0.2f;
        
        // 3. Simulated Muscle Signal (Burst)
        // Simulate a burst every 2 seconds for channel 0, offset for others
        float muscle_activity = 0.0f;
        float burst_period = 2.0f;
        float burst_phase = fmodf(t + i * 0.5f, burst_period);
        
        if (burst_phase < 0.5f) { // Active for 0.5s
            // EMG looks like amplitude modulated noise
            float carrier = ((float)rand() / RAND_MAX - 0.5f);
            muscle_activity = 2.0f * carrier * sinf(2.0f * 3.14159f * 100.0f * t); // Higher freq content
        }
        
        buffer[i] = muscle_activity + noise_50hz + white_noise;
    }
    
    tick_counter++;
}
