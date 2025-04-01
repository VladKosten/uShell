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
 * \brief Port configuration
 */
typedef struct
{
    bool transceiverEnabled;    ///< Flag indicating if a transceiver is used.

    struct TransceiverPins_s
    {
        uint8_t rxPin;       ///< RX pin number.
        bool rxPinActive;    ///< RX pin state.
        uint8_t txPin;       ///< TX pin number.
        bool txPinActive;    ///< TX pin state.
    } transceiverPins;

} UShellHalPortCfg_s;

/**
 * \brief Port HAL obj
 */
typedef struct
{
    /* The generic HAL object must be the first! */
    UShellHal_s base;    ///< Object parent

    /* The port-specific part */
    UShellHalPortCfg_s cfg;                 ///< port configuration;
    struct usart_async_descriptor* uart;    ///< uart descriptor for the port;

} UShellHalPort_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init the HAL portable layer for the particular platform
 * \param[in] halPort - the HAL port descriptor;
 * \param[in] uart - the USART peripheral to be used;
 * \param[in] portCfg - the port configuration;
 * \param[in] parent - the parent object;
 * \param[in] name - the name of the object;
 * \param[out] no
 * \return UShellHalPortErr_e  - error code. non-zero = an error has occurred.
 */
UShellHalPortErr_e UShellHalPortInit(UShellHalPort_s* const halPort,
                                     const struct usart_async_descriptor* uart,
                                     const UShellHalPortCfg_s portCfg,
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