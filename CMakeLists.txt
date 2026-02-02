cmake_minimum_required(VERSION 3.15)
project(emg_simulation C)

set(CMAKE_C_STANDARD 11)

# ====== CMSIS-DSP Path ======
set(CMSIS_DSP_PATH "E:/RS/CMSIS-DSP-main")

# ====== Source Files for Executable (Commented out as main.c is missing) ======
# add_executable(emg_sim
#     emg_filter.c
# )

# ====== Shared Library for Python ======
add_library(emg_filter_lib SHARED
    emg_filter.c
)
set_target_properties(emg_filter_lib PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    OUTPUT_NAME "emg_filter"
)

# ====== Include Directories ======
include_directories(
    inc
    ${CMSIS_DSP_PATH}/Include
    ${CMSIS_DSP_PATH}/PrivateInclude
)

# ====== CMSIS-DSP Sources ======
# We need to compile these into both the executable and the library
set(CMSIS_SOURCES
    ${CMSIS_DSP_PATH}/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c
    ${CMSIS_DSP_PATH}/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
)

# target_sources(emg_sim PRIVATE ${CMSIS_SOURCES})
target_sources(emg_filter_lib PRIVATE ${CMSIS_SOURCES})

# ====== Compile Definitions ======
# target_compile_definitions(emg_sim PRIVATE
#     ARM_MATH_CM4
#     __FPU_PRESENT=1
# )
target_compile_definitions(emg_filter_lib PRIVATE
    ARM_MATH_CM4
    __FPU_PRESENT=1
)
