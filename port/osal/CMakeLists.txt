# OSAL port for FreeRTOS
if((OSAL_PORT_FREERTOS))
    message("-- DIO_MONITOR using FreeRTOS OSAL port")
    add_subdirectory(freertos)
elseif((OSAL_PORT_POSIX))
    message("-- DIO_MONITOR using FreeRTOS OSAL port")
    add_subdirectory(freertos)
endif()