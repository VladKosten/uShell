add_subdirectory(help)
message("-- USHELL HELP CMD INCLUDED")
add_subdirectory(clear)
message("-- USHELL CLEAR CMD INCLUDED")
add_subdirectory(whoami)
message("-- USHELL WHOAMI CMD INCLUDED")


if (USHELL_CMD_FS_LITTLEFS)
     add_subdirectory(fs_littlefs)
    message("-- USHELL FS LITTLEFS CMD INCLUDED")
endif()
