#include "emg_filter.h"

/*
 * 20-450 Hz Bandpass (fs = 2000 Hz)
 * Generated via Scipy butter(1, [20, 450], fs=2000, btype='band')
 * CMSIS-DSP order: {b0, b1, b2, a1, a2}
 * where y[n] = b0*x[n] + ... + a1*y[n-1] + a2*y[n-2]
 */
static const float emg_biquad_coeffs[5 * EMG_BIQUAD_SECTIONS] =
{
    0.4447994861f, 0.0000000000f, -0.4447994861f, 
    1.0523515234f, -0.1104010278f
};

// Init function
void emg_filter_init(emg_filter_t *filt)
{
    /* Clear state */
    for (uint32_t i = 0; i < 4; i++)
    {
        filt->state[i] = 0.0f;
    }

    /* Init CMSIS biquad */
    arm_biquad_cascade_df1_init_f32(
        &filt->biquad,
        1,
        (float32_t *)emg_biquad_coeffs,
        filt->state
    );
}

// Process function
void emg_filter_process(emg_filter_t *filt,
                        float *input,
                        float *output,
                        uint32_t block_size)
{
    arm_biquad_cascade_df1_f32(
        &filt->biquad,
        input,
        output,
        block_size
    );
}
