add_library(UShellCmdClear STATIC ushell_cmd_clear.c)

target_include_directories(UShellCmdClear PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellCmdClear PUBLIC UShellCmd
                                     PUBLIC UShellConfig)