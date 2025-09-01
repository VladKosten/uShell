add_library(UShellHalPort STATIC ushell_hal_stm32f4.c)

target_include_directories(UShellHalPort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellHalPort PUBLIC UShellConfig
                                    PUBLIC UShellHal)