
if(USHELL_UNIT_TEST_BUILD OR USHELL_INTEGRATION_TEST_BUILD)
    # Add library freertos_config
    add_library(freertos_config INTERFACE)
    target_include_directories(freertos_config SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

    # Consume FreeRTOS
    FetchContent_Declare(
        freertos_kernel
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG V10.6.0
    )

    # Select the heap port
    message(STATUS "Setting FREERTOS_HEAP=3")
    set(FREERTOS_HEAP "3" CACHE STRING "" FORCE)

    # Select the native compile PORT
    message(STATUS "UNIX, setting FREERTOS_PORT=GCC_POSIX")
    set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)

    # Make it available
    FetchContent_MakeAvailable(freertos_kernel)
endif()

# Add library DioMonitorConfig
add_library(UShellConfig INTERFACE)
target_include_directories(UShellConfig INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UShellConfig INTERFACE freertos_config INTERFACE freertos_kernel)