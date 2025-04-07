add_library(UShellStartup STATIC ushell_startup.c)

target_include_directories(UShellStartup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellStartup PUBLIC UShellConfig
                                    PUBLIC UShell
                                    PUBLIC UShellCmdHelp
                                    PUBLIC UShellCmdClear
                                    PUBLIC UShellCmdWhoAmI
                                    PUBLIC UShellCmdFs
                                    PUBLIC UShellOsalPort
                                    PUBLIC UShellHalPort
                                    PUBLIC Msp)