cmake_minimum_required(VERSION 3.10)
project(EMG_Simulation)

set(CMAKE_C_STANDARD 99)

# Include directories
include_directories(app)
include_directories(drivers)

# Main Simulation Sources
set(SOURCES
    main.c
    app/emg_filter.c
    app/feature_extract.c
    app/state_machine.c
    drivers/adc_sim.c
)

# Main Executable
add_executable(emg_sim ${SOURCES})

# Tool: Dataset Processor
add_executable(process_dataset 
    tools/process_dataset.c
    app/emg_filter.c
)

# Math library might be needed for standard C math functions
if(UNIX)
    target_link_libraries(emg_sim m)
    target_link_libraries(process_dataset m)
endif()
