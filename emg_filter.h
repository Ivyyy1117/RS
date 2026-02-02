#ifndef EMG_FILTER_H
#define EMG_FILTER_H

#include "arm_math.h"
#include <stdint.h>

/* ================== 配置区 ================== */

// 采样率（Hz）
#define EMG_FS               2000

// biquad 节数（一个二阶 = 1）
#define EMG_BIQUAD_SECTIONS  1

// block 处理长度（DMA / 仿真）
#define EMG_BLOCK_SIZE       256 //一次处理多少个采样点

/* ================== 结构体 ================== */

typedef struct
{
    arm_biquad_cascade_df1_instance_f32 biquad;  //官方滤波器结构体
    float state[4 * EMG_BIQUAD_SECTIONS]; //记忆
} emg_filter_t;  //一个完整的 EMG IIR 滤波器实例

/* ================== API ================== */

// 初始化（必须在使用前调用一次）
void emg_filter_init(emg_filter_t *filt); //绑定阶数、系数、state数组、清零状态

// block 滤波
void emg_filter_process(emg_filter_t *filt,
                        float *input,
                        float *output,
                        uint32_t block_size); 

#endif /* EMG_FILTER_H */
