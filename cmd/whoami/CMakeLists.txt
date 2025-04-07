add_library(UShellCmdWhoAmI STATIC ushell_cmd_whoami.c)

target_include_directories(UShellCmdWhoAmI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(UShellCmdWhoAmI PUBLIC UShellCmd
                                    PUBLIC UShellConfig)