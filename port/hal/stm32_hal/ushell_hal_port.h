#ifndef USHELL_HAL_PORT_H_
#define USHELL_HAL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

/* Project includes */
#include "uShell_hal.h"             ///< UShell HAL layer
#include "stm32f4xx_hal_conf.h"     ///< Device-specific HAL configuration file

/**
 * \note Need USE_HAL_UART_REGISTER_CALLBACKS set to 1U
 * */

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief The USART peripheral to be used (USART1, USART2, USART3, etc.)
 */
#ifndef USHELL_HAL_PORT_USART
    #define USHELL_HAL_PORT_USART USART1
#endif

/**
 * \brief The baudrate to be used
 */
#ifndef USHELL_HAL_PORT_BAUDRATE
    #define USHELL_HAL_PORT_BAUDRATE 115200
#endif

/**
 * \brief The word length to be used
 */
#ifndef USHELL_HAL_PORT_WORD_LENGTH
    #define USHELL_HAL_PORT_WORD_LENGTH UART_WORDLENGTH_8B
#endif

/**
 * \brief The stop bits to be used
 */
#ifndef USHELL_HAL_PORT_STOP_BITS
    #define USHELL_HAL_PORT_STOP_BITS UART_STOPBITS_1
#endif

/**
 * \brief The parity to be used
 */
#ifndef USHELL_HAL_PORT_PARITY
    #define USHELL_HAL_PORT_PARITY UART_PARITY_NONE
#endif

/**
 * \brief The HAL PORT name
 */
#ifndef USHELL_HAL_PORT_NAME
    #define USHELL_HAL_PORT_NAME "uShellHal"
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