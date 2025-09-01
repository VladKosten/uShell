#ifndef USHELL_HAL_PORT_H_
#define USHELL_HAL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

/* Project includes */
#include "ushell_hal.h"       ///< UShell HAL layer
#include "stm32f4xx_hal.h"    ///< STM32F4 HAL definitions

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief Input/Output buffer size
 */
#ifndef USHELL_HAL_PORT_IO_BUFFER_SIZE
    #define USHELL_HAL_PORT_IO_BUFFER_SIZE 256    ///< Default IO buffer size
#endif

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
        GPIO_TypeDef* rxPort;         ///< RX port.
        uint32_t rxPin;               ///< RX pin number.
        GPIO_PinState rxPinActive;    ///< RX pin state.
        GPIO_TypeDef* txPort;         ///< TX port.
        uint32_t txPin;               ///< TX pin number.
        GPIO_PinState txPinActive;    ///< TX pin state.

    } transceiverPins;

} UShellHalPortCfg_s;

typedef struct
{
    size_t buffSize;                                  ///< Size of the buffer.
    uint8_t buff [USHELL_HAL_PORT_IO_BUFFER_SIZE];    ///< Buffer for IO operations.
} UShellHalPortIo_s;

typedef struct
{
    size_t head;                                      ///< Head index for the ring buffer.
    size_t tail;                                      ///< Tail index for the ring buffer.
    uint8_t buff [USHELL_HAL_PORT_IO_BUFFER_SIZE];    ///< Buffer for the ring buffer.

} UShellHalPortRingBuffer_s;

/**
 * \brief Port HAL obj
 */
typedef struct
{
    /* The generic HAL object must be the first! */
    UShellHal_s base;                          ///< Object parent
    UShellHalPortIo_s rx;                      ///< RX buffer
    UShellHalPortIo_s tx;                      ///< TX buffer
    UShellHalPortRingBuffer_s rxRingBuffer;    ///< RX ring buffer

    /* The port-specific part */
    UShellHalPortCfg_s cfg;      ///< port configuration;
    UART_HandleTypeDef* uart;    ///< Pointer to the USART peripheral handle;

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
                                     const UART_HandleTypeDef* uart,
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