add_library(XModem STATIC xmodem.c)
add_library(XModemServer STATIC xmodem_server.c)
add_library(XModemClient STATIC xmodem_client.c)

target_include_directories(XModem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(XModemServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(XModemClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(XModem PUBLIC UShellConfig)

target_link_libraries(XModemServer PUBLIC XModem)
target_link_libraries(XModemClient PUBLIC XModem)