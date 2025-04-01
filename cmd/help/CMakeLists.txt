add_library(UShellCmdHelp STATIC ushell_cmd_help.c)

target_include_directories(UShellCmdHelp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellCmdHelp PUBLIC UShellCmd
                                    PUBLIC UShellConfig)