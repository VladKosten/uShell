cmake_minimum_required(VERSION 3.10)

# Set the project name
project(uShell C ASM)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Export compile database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# If the standalone build option is enabled, add the test subdirectory
if(USHELL_UNIT_TEST_BUILD OR USHELL_UNIT_TEST_BUILD)
    option(HAL_PORT_CMOCK "Use CMOCK HAL port" ON)
    option(OSAL_PORT_POSIX "Use POSIX OSAL port" ON)
    add_subdirectory(test)
endif()

# Add subdirectories
add_subdirectory(main)
add_subdirectory(port)
add_subdirectory(cmd)
add_subdirectory(utils)