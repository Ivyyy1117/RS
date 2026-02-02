#ifndef EMG_FILTER_H
#define EMG_FILTER_H

#include "arm_math.h"
#include <stdint.h>

#define EMG_FS 2000
#define EMG_BIQUAD_SECTIONS 1
#define EMG_BLOCK_SIZE 256

typedef struct
{
    arm_biquad_casd_df1_inst_f32 biquad;
    float state[4]; // Hardcoded 4
} emg_filter_t;

void emg_filter_init(emg_filter_t *filt);

void emg_filter_process(emg_filter_t *filt,
                        float *input,
                        float *output,
                        uint32_t block_size);

#endif
