#ifndef USHELL_STARTUP_CFG_H_
#define USHELL_STARTUP_CFG_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
* \brief Matrix Kbd assert definition
*/
#define USHELL_STARTUP_ASSERT(cond)                              assert((cond))

/* Define the OSAL port type */

/**
* \brief FreeRTOS OSAL port
* \note Uncomment this line to use the FreeRTOS OSAL port
*       Recommended use CMake to define this option
 */
// #define USHELL_STARTUP_OSAL_PORT_FREERTOS


/* Define the HAL port type */

/**
* \brief STM32 HAL port
* \note Uncomment this line to use the STM32 HAL port
*       Recommended use CMake to define this option
 */
// #define USHELL_STARTUP_HAL_PORT_STM32

/**
 * \brief ATMEL HAL port
 * \note Uncomment this line to use the ATMEL HAL port
 *      Recommended use CMake to define this option
 * \todo Add the ATMEL HAL port implementation
 */
// #define USHELL_STARTUP_HAL_PORT_ATMEL


/* Include port-specific headers: */

/* Include OSAL port-specific headers here: */
#ifdef USHELL_STARTUP_OSAL_PORT_FREERTOS
    #include "dwin_osal_port.h"
#endif

/* Include HAL port-specific headers here: */
#ifdef USHELL_STARTUP_HAL_PORT_STM32 || USHELL_STARTUP_HAL_PORT_ATMEL || USHELL_STARTUP_HAL_PORT_CMOCK
    #include "dwin_hal_port.h"
#endif

/* Include other port-specific headers here: */
// ... //

/**
* \brief uShell name by default
*/
#ifndef USHELL_STARTUP_NAME
    #define USHELL_STARTUP_NAME                       "uShell"
#endif

/**
* \brief Define the OSAL port type
 */
#ifndef USHELL_STARTUP_OSAL_PORT_TYPE
    #ifdef USHELL_STARTUP_OSAL_PORT_FREERTOS

        /* For startup */
        #define USHELL_STARTUP_OSAL_PORT_TYPE         DwinOsalFreertos_s
        #define USHELL_STARTUP_OSAL_PORT_NAME         "uShellOsal"
        /* For OSAL port */
        #define USHELL_OSAL_PORT_THREAD_NAME          "uShellTask"
        #define USHELL_OSAL_PORT_THREAD_PRIO          (tskIDLE_PRIORITY + 1)
        #define USHELL_OSAL_PORT_THREAD_STACK_SIZE    256
    #endif
    /* Add definition for other ports here: */
    // ... //
#endif

/**
* \brief Define the HAL port type
 */
#ifndef USHELL_STARTUP_HAL_PORT_TYPE
    #ifdef USHELL_STARTUP_HAL_PORT_ATMEL
        #define USHELL_STARTUP_HAL_PORT_TYPE          DwinHalPort_s
        #define USHELL_STARTUP_HAL_PORT_UART_DESC     &USART0

    #elif USHELL_STARTUP_HAL_PORT_STM32

        /* For startup */
        #define USHELL_STARTUP_HAL_PORT_TYPE          UShellHalPort_s
        #define USHELL_STARTUP_HAL_PORT_NAME          "uShellHal"

        /* For HAL port */
        #define USHELL_HAL_PORT_UART_DESC     USART1
        #define USHELL_HAL_PORT_BAUDRATE      115200
        #define USHELL_HAL_PORT_WORD_LENGTH   UART_WORDLENGTH_8B
        #define USHELL_HAL_PORT_STOP_BITS     UART_STOPBITS_1
        #define USHELL_HAL_PORT_PARITY        UART_PARITY_NONE
        #define USHELL_HAL_PORT_NAME          "uShellHal"

    #elif USHELL_STARTUP_HAL_PORT_CMOCK
        #define USHELL_STARTUP_HAL_PORT_TYPE          DwinHalPort_s

    #endif
    /* Add definition for other ports here: */
    // ... //
#endif

#ifdef __cplusplus
    }
#endif

#endif