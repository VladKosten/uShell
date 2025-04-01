add_library(UShellStartup STATIC ushell_startup.c)

target_include_directories(UShellStartup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellStartup PUBLIC UShellStartupConfig
                                    PUBLIC UShell
                                    PUBLIC UShellCmdHelp)