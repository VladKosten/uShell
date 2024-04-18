# CMakeLists.txt
# Created on: 2024-04-15
# Author: V.Kosten (vladkosten@gmail.com)
# Brief: CMake for the uShell - a simple shell for embedded systems
# Description: This is the main CMakeLists.txt file for the uShell project
#             It is used to configure the project and generate the build files
#             for the build system of your choice.

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(uShell LANGUAGES C)

# Set the build type to standalone if not set by the user
if(STANDOLONE STREQUAL "")
    set(STANDOLONE "ON")
endif()

if(${STANDOLONE} STREQUAL "ON")
    message(STATUS "${PROJECT_NAME} building, standalone")
else()
    message(STATUS "${PROJECT_NAME} building as a library")

# Check if osal port if its not standalone
if(NOT OSAL_PORT && STANDOLONE STREQUAL "OFF")
    message(WARNING " OSAL_PORT is not set. Please specify it from top-level CMake file (example):\n"
        "  set(OSAL_PORT FREERTOS CACHE STRING \"\")\n"
        "  or from CMake command line option:\n"
        "  -DOSAL_PORT=FREERTOS\n"

        " Available port options:\n"
        " FREERTOS - FreeRTOS\n"
    )
endif()

if(NOT HAL_PORT && STANDOLONE STREQUAL "OFF")
    message(WARNING " HAL_PORT is not set. Please specify it from top-level CMake file (example):\n"
        "  set(HAL_PORT STM32 CACHE STRING \"\")\n"
        "  or from CMake command line option:\n"
        "  -DHAL_PORT=STM32\n"

        " Available port options:\n"
        " STM32 - STM32 series\n"
)

if(NOT TARGET CompileOptions && STANDOLONE STREQUAL "OFF")
    message( FATAL_ERROR "TARGET CompileOptions is not created. \n"
        " This is a mandatory target for the project. \n"
        " He save all compile options for the project. \n"
        " Please create this target in the top-level CMakeLists.txt file. \n"
        " Example: \n"
        " add_library(CompileOptions INTERFACE) \n"
        " target_compile_options(CompileOptions INTERFACE -Wall -Wextra -Werror) \n"
    )

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Set the output directory for the executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Set the output directory for the libraries
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Add subdirectories depending on the build type
if(${STANDOLONE} STREQUAL "ON")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    # add_subdirectory(test)
elseif(${STANDOLONE} STREQUAL "OFF")
    add_subdirectory(startup)
endif()

# Add the subdirectories
add_subdirectory(main)
add_subdirectory(portable)
