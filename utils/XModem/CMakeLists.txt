cmake_minimum_required(VERSION 3.10)

# Set the project name
project(XModem C ASM)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Export compile database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Add subdirectories
add_subdirectory(main)

