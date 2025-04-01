# Add libraries
add_library(UShell STATIC ushell.c
                          ushell_vcp.c
                          ushell_history.c
                          )
add_library(UShellHal STATIC ushell_hal.c)
add_library(UShellOsal STATIC ushell_osal.c)
add_library(UShellCmd STATIC ushell_cmd.c)

# Set include directories for all libraries
target_include_directories(UShell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(UShellHal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(UShellOsal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(UShellCmd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
target_link_libraries(UShellHal PUBLIC UShellConfig)
target_link_libraries(UShellOsal PUBLIC UShellConfig)
target_link_libraries(UShellCmd PUBLIC UShellConfig)

# Link libraries to DioMonitor based on STANDALONE_BUILD
target_link_libraries(UShell PUBLIC UShellHal
                             PUBLIC UShellOsal
                             PUBLIC UShellCmd)