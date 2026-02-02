import scipy.io as sio
import numpy as np
import ctypes

class EMGFilter(ctypes.Structure):
    _fields_ = [
        ("biquad", ctypes.c_byte * 64),   # 足够放 CMSIS 结构体
        ("state", ctypes.c_float * (4 * 1))  # 1 个 biquad
    ]

lib = ctypes.CDLL("./emg_filter.dll")

lib.emg_filter_init.argtypes = [
    ctypes.POINTER(EMGFilter)
]

lib.emg_filter_process.argtypes = [
    ctypes.POINTER(EMGFilter),
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.c_uint32
]


mat = sio.loadmat("Ninapro_dataset\s1\S1_A1_E1.mat")

emg = mat['emg'].astype(np.float32)
fs = int(mat['fs'][0][0])
x = emg[:20000, 0]

print(emg.shape)   # (N, 12) 或 (N, 16)
print("fs =", fs)


BLOCK = 256

filt = EMGFilter()
lib.emg_filter_init(ctypes.byref(filt))

y = np.zeros_like(x)

for i in range(0, len(x) - BLOCK, BLOCK):
    x_blk = x[i:i+BLOCK]
    y_blk = y[i:i+BLOCK]

    lib.emg_filter_process(
        ctypes.byref(filt),
        x_blk.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
        y_blk.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
        BLOCK
    )

out = {
    "emg_raw": x,
    "emg_filtered": y,
    "fs": fs
}

sio.savemat("emg_filtered.mat", out)

