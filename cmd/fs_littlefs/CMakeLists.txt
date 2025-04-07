# Create a library for the fs command
add_library(UShellCmdFs STATIC ushell_cmd_fs.c)

# Include directories
target_include_directories(UShellCmdFs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link dependencies
target_link_libraries(UShellCmdFs PUBLIC UShellCmd
                                  PUBLIC UShellConfig
                                  PUBLIC LittleFs
                                  PUBLIC XModemServer
                                  PUBLIC XModemClient)