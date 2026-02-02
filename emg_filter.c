#include "emg_filter.h"

/*
 * 示例：20–450 Hz 带通（fs = 2000 Hz）
 * 由 Python / Matlab 生成
 */
static const float emg_biquad_coeffs[5 * EMG_BIQUAD_SECTIONS] =
{
    /* b0, b1, b2, a1, a2 */
    0.0675f, 0.1349f, 0.0675f,
   -1.1430f, 0.4128f
};

//初始化函数
void emg_filter_init(emg_filter_t *filt)
{
    /* 清状态 */
    for (uint32_t i = 0; i < 4 * EMG_BIQUAD_SECTIONS; i++)
    {
        filt->state[i] = 0.0f;
    }

    /* 初始化 CMSIS biquad */
    arm_biquad_cascade_df1_init_f32(
        &filt->biquad,
        EMG_BIQUAD_SECTIONS,
        (float32_t *)emg_biquad_coeffs,
        filt->state
    );
}

//滤波处理函数
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

