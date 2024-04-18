/**
* \file      uShell_hal_port.c
* \brief     The uShell HAL port layer implementation for STM32 HAL library.
* \authors   Vladislav Kosten (vladkosten@gmail.com)
* \copyright Copyright (c) 2024. Vlad Kosten. All rights reserved.
* \warning   A warning may be placed here...
* \bug       Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Standard includes */
#include "uShell_hal_port.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief USHELL_HAL_PORT_ASSERT macro definition
*/
#ifndef USHELL_HAL_PORT_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_HAL_PORT_ASSERT(cond)  USHELL_ASSERT(cond)
    #else
        #define USHELL_HAL_PORT_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================


//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================


//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

/**
 * \brief Init the HAL portable layer for the particular platform
 * \param[in] halPort - the HAL port descriptor;
 * \param[in] usart - the USART peripheral to be used;
 * \param[in] baudrate - the baudrate to be used;
 * \param[in] WordLenght - the word length to be used;
 * \param[in] stopBits - the stop bits to be used;
 * \param[in] parity - the parity to be used;
 * \param[out] no
 * \return UShellHalPortErr_e  - error code. non-zero = an error has occurred.

 */
UShellHalPortErr_e UShell_HalPortInit(UShellHalPort_s* const halPort, USART_TypeDef* const usart,
                                                                      uint32_t baudrate,
                                                                      uint32_t WordLenght,
                                                                      uint32_t stopBits,
                                                                      uint32_t parity,
                                                                      const void* const parent,
                                                                      const char* const name)
{
    /* Check input parametr */
    if((halPort == NULL) || (usart == NULL))
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Invalid pointers
    }
    
    switch (usart)
    {
    case USART1_BASE:
    case USART2_BASE:
    case USART3_BASE:
    case UART4_BASE:
    case UART5_BASE:
        break;

    default:
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Invalid USART
        break;
    }

    /* Initialize the HAL port object */
    memset(halPort, 0, sizeof(UShellHalPort_s));
    halPort->base.parent = parent;
    halPort->base.name = name;

    /* Initialize the HAL UART handle */
    halPort->huart.Instance = usart;
    halPort->huart.Init.BaudRate = 115200;

}

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] UShellHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return UShellHalErr_e  - error code. non-zero = an error has occurred.
 */
UShellHalPortErr_e UShell_HalPortDeinit(UShellHalPort_s* const halPort)



//============================================================================[PRIVATE FUNCTIONS]===================================================================================
