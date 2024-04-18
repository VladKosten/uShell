#ifndef USHELL_HAL_PORT_H_
#define USHELL_HAL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

/* Project includes */
#include "ushell_hal.h"             ///< UShell HAL layer
#include "stm32f4xx_hal_conf.h"     ///< Device-specific HAL configuration file

/**
 * \note Need USE_HAL_UART_REGISTER_CALLBACKS set to 1U
 * */

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief Check define USHELL_HAL_PORT_USART
 */
#if !defined(USHELL_HAL_PORT_USART)
    #error USART not define.
#endif

/**
 * \brief Check define USHELL_HAL_PORT_BAUDRATE
 */
#if !defined(USHELL_HAL_PORT_BAUDRATE)
    #error Baudrate not define.
#endif

/**
 * \brief Check define USE_HAL_UART_REGISTER_CALLBACKS (need set to 1U)
 */
#if !defined(USE_HAL_UART_REGISTER_CALLBACKS)
    #error USE_HAL_UART_REGISTER_CALLBACKS not define.
#endif

/**
 * \brief Check define USHELL_HAL_PORT_WORD_LENGTH
 */
#if !defined(USHELL_HAL_PORT_WORD_LENGTH)
    #error USHELL_HAL_PORT_WORD_LENGTH not define.
#endif

/**
 * \brief Check define USHELL_HAL_PORT_STOP_BITS
 */
#if !defined(USHELL_HAL_PORT_STOP_BITS)
    #error USHELL_HAL_PORT_STOP_BITS not define.
#endif

/**
 * \brief Check define USHELL_HAL_PORT_PARITY
 */
#if !defined(USHELL_HAL_PORT_PARITY)
    #error USHELL_HAL_PORT_PARITY not define.
#endif

/**
 * \brief Check define USHELL_HAL_PORT_NAME
 */
#if !defined(USHELL_HAL_PORT_NAME)
    #error USHELL_HAL_PORT_NAME not define.
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
* \brief Port HAL errors enumerator
*/
typedef enum
{
    USHELL_HAL_PORT_NO_ERR = 0,           ///< Exit: no errors
    USHELL_HAL_PORT_INVALID_ARGS_ERR,     ///< Exit: error - invalid pointers
    USHELL_HAL_PORT_INIT_ERR,             ///< The HAL PORT was not initialized
    USHELL_HAL_PORT_IO_ERR,               ///< IO error
} UShellHalPortErr_e;

/**
 * \brief Port HAL obj
 */
typedef struct
{
    /* The generic HAL object must be the first! */
    UShellHal_s base;        ///< Object parent

    /* The port-specific part */
    UART_HandleTypeDef huart;   ///< The HAL UART handle

}UShellHalPort_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

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
UShellHalPortErr_e UShell_HalPortInit(UShellHalPort_s* const halPort, const void* const parent,
                                                                      const char* const name);

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] UShellHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return UShellHalErr_e  - error code. non-zero = an error has occurred.
 */
UShellHalPortErr_e UShell_HalPortDeinit(UShellHalPort_s* const halPort);

#ifdef __cplusplus
}
#endif

#endif /* USHELL_HAL_PORT_H_ */