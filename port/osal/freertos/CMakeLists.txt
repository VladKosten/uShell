add_library(UShellOsalPort STATIC ushell_osal_freertos.c)

target_include_directories(UShellOsalPort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellOsalPort PUBLIC UShellConfig
                                     PUBLIC UShellOsal
                                     PUBLIC freertos_kernel)