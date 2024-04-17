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
UShellHalPortErr_e UShell_HalPortInit(UShellHalPort_s* const halPort, USART_TypeDef* const usart,
                                                                      uint32_t baudrate,
                                                                      uint32_t WordLenght,
                                                                      uint32_t stopBits,
                                                                      uint32_t parity,
                                                                      const void* const parent,
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