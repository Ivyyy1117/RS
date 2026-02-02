//完整流程测试脚本

#include <stdio.h>
#include "adc_sim.h"
#include "app/emg_filter.h"
#include "app/feature_extract.h"

// Configuration
#define NUM_CHANNELS 4
#define DURATION_SECONDS 5

// Filter State per Channel
typedef struct {
    BiquadFilter notch;    // 50Hz Notch
    BiquadFilter highpass; // 20Hz High Pass
    BiquadFilter lowpass;  // 450Hz Low Pass
    RMS_Calculator rms;    // RMS Extractor
} ChannelProcessor;

ChannelProcessor channels[NUM_CHANNELS];

void System_Init(void) {
    ADC_Init();
    
    for(int i = 0; i < NUM_CHANNELS; i++) {
        // 滤波器初始化
        Filter_Init(&channels[i].notch);
        Filter_Init(&channels[i].highpass);
        Filter_Init(&channels[i].lowpass);
        
        // 计算参数
        // 1. Notch at 50Hz (remove mains hum) - Q=5.0 for narrow notch
        Filter_CalcNotch(&channels[i].notch, SAMPLE_RATE, 50.0f, 5.0f);
        
        // 2. High Pass at 20Hz (remove motion artifacts) - Butterworth Q=0.707
        Filter_CalcHighPass(&channels[i].highpass, SAMPLE_RATE, 20.0f, 0.707f);
        
        // 3. Low Pass at 450Hz (anti-aliasing/noise) - Butterworth Q=0.707
        Filter_CalcLowPass(&channels[i].lowpass, SAMPLE_RATE, 450.0f, 0.707f);
        
        // Initialize RMS (Window size 200ms)
        Feature_RMS_Init(&channels[i].rms, 200.0f, SAMPLE_RATE);
    }
}

int main(void) {
    System_Init();
    
    printf("Time,Raw_CH0,Filtered_CH0,RMS_CH0,Raw_CH1,Filtered_CH1,RMS_CH1\n");
    
    float raw_buffer[NUM_CHANNELS];
    int total_samples = DURATION_SECONDS * (int)SAMPLE_RATE;
    
    for(int t = 0; t < total_samples; t++) {
        // 1. Get new data
        ADC_Read(raw_buffer, NUM_CHANNELS);
        
        float filtered_values[NUM_CHANNELS];
        float rms_values[NUM_CHANNELS];
        
        for(int i = 0; i < NUM_CHANNELS; i++) {
            float signal = raw_buffer[i];
            
            // 2. Apply Filters Chain
            // Notch -> HighPass -> LowPass
            signal = Filter_Process(&channels[i].notch, signal);
            signal = Filter_Process(&channels[i].highpass, signal);
            signal = Filter_Process(&channels[i].lowpass, signal);
            
            filtered_values[i] = signal;
            
            // 3. Extract Features
            rms_values[i] = Feature_RMS_Update(&channels[i].rms, signal);
        }
        
        // Output for Channel 0 and 1 (CSV format)
        // Time in seconds
        float time = (float)t / SAMPLE_RATE;
        printf("%.3f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n", 
               time, 
               raw_buffer[0], filtered_values[0], rms_values[0],
               raw_buffer[1], filtered_values[1], rms_values[1]);
    }
    
    return 0;
}
