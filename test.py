
import scipy.io as sio
import numpy as np
import ctypes
import matplotlib.pyplot as plt

# ================== Configuration ==================
# Choose which movement and repetition to process
# Stimulus: 0=Rest, 1-12=Basic Finger Movements (for DB1)
TARGET_STIMULUS = 1    #第几个动作
TARGET_REPETITION = 2  #第几次重复
# ===================================================

# Define C Structure
class EMGFilter(ctypes.Structure):
    _fields_ = [
        ("biquad", ctypes.c_byte * 64),   # CMSIS instance size
        ("state", ctypes.c_float * (4 * 1))  # 1 section state
    ]

# Load DLL
try:
    lib = ctypes.CDLL(r"E:\RS\emg_simulation\build\Debug\emg_filter.dll")
except Exception as e:
    print(f"Error loading DLL: {e}")
    exit(1)

# Define Argument Types
lib.emg_filter_init.argtypes = [ctypes.POINTER(EMGFilter)]
lib.emg_filter_process.argtypes = [
    ctypes.POINTER(EMGFilter),
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.c_uint32
]

# Load Data
mat_path = "Ninapro_dataset/s1/S1_A1_E1.mat"
print(f"Loading {mat_path}...")
mat = sio.loadmat(mat_path)

emg_full = mat['emg'].astype(np.float32)
stimulus = mat['restimulus']
repetition = mat['rerepetition']

# Detect Sampling Rate
if 'fs' in mat:
    fs = int(mat['fs'][0][0])
else:
    fs = 100 # Default for Ninapro DB1
print(f"Data Shape: {emg_full.shape}, fs={fs}")

# Find indices for the target action and repetition
mask = (stimulus.flatten() == TARGET_STIMULUS) & (repetition.flatten() == TARGET_REPETITION)
indices = np.where(mask)[0]

if len(indices) == 0:
    print(f"Error: No data found for Stimulus {TARGET_STIMULUS}, Repetition {TARGET_REPETITION}")
    # Show available options
    avail_stims = np.unique(stimulus)
    print(f"Available Stimuli: {avail_stims}")
    if TARGET_STIMULUS in avail_stims:
        avail_reps = np.unique(repetition[stimulus == TARGET_STIMULUS])
        print(f"Available Repetitions for Stimulus {TARGET_STIMULUS}: {avail_reps}")
    exit(1)

start_idx = indices[0]
end_idx = indices[-1]
duration = len(indices) / fs
print(f"Processing Stimulus {TARGET_STIMULUS}, Repetition {TARGET_REPETITION}")
print(f"Time Window: {start_idx} to {end_idx} ({len(indices)} samples, {duration:.2f}s)")

# Select Channel 1 for this window
channel_idx = 0
x_segment = emg_full[start_idx:end_idx+1, channel_idx]

# Filter Init
filt = EMGFilter()
lib.emg_filter_init(ctypes.byref(filt))

# Prepare Output
y_segment = np.zeros_like(x_segment)
BLOCK = 256

# Process Loop (Chunked)
current_idx = 0
total_len = len(x_segment)

while current_idx < total_len:
    remaining = total_len - current_idx
    blk_size = BLOCK if remaining >= BLOCK else remaining
    
    x_blk = x_segment[current_idx : current_idx + blk_size]
    y_out = np.zeros(blk_size, dtype=np.float32)
    
    lib.emg_filter_process(
        ctypes.byref(filt),
        x_blk.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
        y_out.ctypes.data_as(ctypes.POINTER(ctypes.c_float)),
        ctypes.c_uint32(blk_size)
    )
    
    y_segment[current_idx : current_idx + blk_size] = y_out
    current_idx += blk_size

# Visualization
# t = np.arange(len(x_segment)) / fs

# plt.figure(figsize=(10, 8))

# plt.subplot(2, 1, 1)
# plt.plot(t, x_segment)
# plt.title(f"Raw EMG (Stim {TARGET_STIMULUS}, Rep {TARGET_REPETITION})")
# plt.xlabel("Time (s)")
# plt.ylabel("Amplitude")
# plt.grid(True)

# plt.subplot(2, 1, 2)
# plt.plot(t, y_segment)
# plt.title(f"Filtered EMG")
# plt.xlabel("Time (s)")
# plt.ylabel("Amplitude")
# plt.grid(True)

# plt.tight_layout()
# plt.savefig("result_action.png")
# print("Saved result_action.png")

# Save Result
out = {
    "emg_raw_segment": x_segment,
    "emg_filtered_segment": y_segment,
    "fs": fs,
    "stimulus": TARGET_STIMULUS,
    "repetition": TARGET_REPETITION
}
sio.savemat(
    rf"E:\RS\emg_simulation\filtered_dataset\{TARGET_STIMULUS}-{TARGET_REPETITION}.mat",
    out
)


print("Saved {TARGET_STIMULUS}-{TARGET_REPETITION}.mat")
