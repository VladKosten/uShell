#ifndef USHELL_HAL_PORT_H_
#define USHELL_HAL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

/* Project includes */
#include "ushell_hal.h"    ///< UShell HAL layer
#include "atmel_start.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Port HAL errors enumerator
 */
typedef enum
{
    USHELL_HAL_PORT_NO_ERR = 0,          ///< Exit: no errors
    USHELL_HAL_PORT_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers
    USHELL_HAL_PORT_INIT_ERR,            ///< The HAL PORT was not initialized
    USHELL_HAL_PORT_IO_ERR,              ///< IO error
} UShellHalPortErr_e;

/**
 * \brief Port HAL obj
 */
typedef struct
{
    /* The generic HAL object must be the first! */
    UShellHal_s base;    ///< Object parent

    /* The port-specific part */
    struct usart_async_descriptor* uart;    ///< uart descriptor for the port;
} UShellHalPort_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init the HAL portable layer for the particular platform
 * \param[in] halPort - the HAL port descriptor;
 * \param[in] uart - the USART peripheral to be used;
 * \param[in] parent - the parent object;
 * \param[in] name - the name of the object;
 * \param[out] no
 * \return UShellHalPortErr_e  - error code. non-zero = an error has occurred.

 */
UShellHalPortErr_e UShellHalPortInit(UShellHalPort_s* const halPort,
                                     const struct usart_async_descriptor* uart,
                                     const void* const parent,
                                     const char* const name);

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] UShellHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return UShellHalErr_e  - error code. non-zero = an error has occurred.
 */
UShellHalPortErr_e UShellHalPortDeinit(UShellHalPort_s* const halPort);

#ifdef __cplusplus
}
#endif

#endif /* USHELL_HAL_PORT_H_ */