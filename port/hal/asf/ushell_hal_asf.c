/**
 * \file      ushell_hal_port.c
 * \brief     The uShell HAL port layer implementation for the Atmel Start framework.
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning   A warning may be placed here...
 * \bug       Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/* Standard includes */
#include "ushell_hal_asf.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief USHELL_HAL_PORT_ASSERT macro definition
 */
#ifndef USHELL_HAL_PORT_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_HAL_PORT_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_HAL_PORT_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

/**
 * \brief The link of a specific uart to a specific instance hal port
 */
typedef struct
{
    const void* parent;
    const struct usart_async_descriptor* uart;

} USHellPortLink_s;

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief This function to add a new link to the pool of the relationship of a specific uart to a specific  hal port
 * \param[in] parent - the parent object (the instance hal port) to be added to the pool;
 * \param[in] uart - the uart object to be added to the pool (from Atmel Start);
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolAdd(const void* const parent,
                                               const struct usart_async_descriptor* const uart);

/**
 * \brief This function to remove a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] parent - the parent object (the instance hal port) to be removed from the pool ;
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolRemoveByParent(const void* const parent);

/**
 * \brief This function to get a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] uart - the uart object to find link in the pool (from Atmel Start);
 * \param[out] parent - pointer to stored parent object (the instance hal port);
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolParentGet(const struct usart_async_descriptor* const uart,
                                                     void** const parent);

/**
 * \brief Open function.
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortOpen(void* const hal);

/**
 * \brief Close function.
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortClose(void* const hal);

/**
 * \brief Write function.
 * \param[in] hal Pointer to the HAL instance.
 * \param[in] data Pointer to the data to be written.
 * \param[in] size Size of the data to be written.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortWrite(void* const hal,
                                         const UShellHalItem_t* const data,
                                         const size_t size);

/**
 * \brief Read function.
 * \param hal - Pointer to the HAL instance.
 * \param data - Pointer to the buffer to store the read data.
 * \param buffSize - Size of the buffer.
 * \param usedSize - Pointer to the variable to store the size of the data read.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortRead(void* const hal,
                                        UShellHalItem_t* const data,
                                        const size_t buffSize,
                                        size_t* const usedSize);

/**
 * \brief Set Tx mode
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 *
 */
static UShellHalErr_e uShellHalPortSetTxMode(void* const hal);

/**
 * \brief Set Rx mode
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortSetRxMode(void* const hal);

/**
 * \brief Check if data is available for reading.
 * \param[in] hal - Pointer to the HAL instance.
 * \param[in] isAvailable - Pointer to a boolean indicating if data is available.
 * \return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalErr_e uShellHalPortIsReadDataAvailable(void* const hal,
                                                       bool* const isAvailable);

/**
 * \brief Callback for error in the usart (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has an error;
 * \param[out] no;
 */
static void uShellHalPortErrorCb(const struct usart_async_descriptor* const usart);

/**
 * \brief Callback for Rx receive (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has an error;
 * \param[out] no;
 */
static void uShellHalPortRxReceiveCb(const struct usart_async_descriptor* const usart);

/**
 * \brief Callback for Tx complete (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has completed the transmission;
 * \param[out] no;
 */
static void uShellHalPortTxCompleteCb(const struct usart_async_descriptor* const usart);

/**
 * \brief Table of uShell HAL port operation functions.
 *
 * This structure holds pointers to the functions implementing the uShell HAL
 * port operations. These include opening, closing, reading from, writing to the
 * and flushing the port, as well as setting the port's transmit (Tx) and receive (Rx) modes.
 */
static UShellHalPortTable_s ushellHalPortTable = {
    .open = uShellHalPortOpen,
    .close = uShellHalPortClose,
    .write = uShellHalPortWrite,
    .read = uShellHalPortRead,
    .setTxMode = uShellHalPortSetTxMode,
    .setRxMode = uShellHalPortSetRxMode,
    .isReadDataAvailable = uShellHalPortIsReadDataAvailable,
};

/**
 * \brief For store context from interrupt
 */
static USHellPortLink_s uShellPortLink = {
    .parent = NULL,
    .uart = NULL};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

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
                                     const char* const name)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Init HAL base object */
        UShellHalErr_e statusHal = UShellHalInit(&halPort->base,
                                                 parent,
                                                 name,
                                                 &ushellHalPortTable);
        if (statusHal != USHELL_HAL_NO_ERR)
        {
            /* HAL base object init failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }

        /* Store configuration */
        halPort->cfg = portCfg;

        /* Store uart */
        halPort->uart = (struct usart_async_descriptor*) uart;

        /* Create link in the pool */
        UShellHalPortErr_e statusHalPort = uShellHalPortPoolAdd(halPort, halPort->uart);
        if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
        {
            /* Add link to the pool failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }

        /* Flush rx buffer */
        int32_t statusAsf = usart_async_flush_rx_buffer(halPort->uart);
        if (statusAsf != ERR_NONE)
        {
            /* Flush failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }

        /* Enable USART */
        statusAsf = usart_async_enable(halPort->uart);
        if (statusAsf != ERR_NONE)
        {
            /* Enable USART failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }
    } while (0);

    /* Deinit HAL port if error */
    if (status != USHELL_HAL_PORT_NO_ERR)
    {
        UShellHalPortDeinit(halPort);
    }

    return status;
}

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] UShellHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return UShellHalErr_e  - error code. non-zero = an error has occurred.
 */
UShellHalPortErr_e UShellHalPortDeinit(UShellHalPort_s* const halPort)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;
    UShellHalErr_e statusHal = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameters */
        if (halPort == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Close port */
        uShellHalPortClose(halPort);

        /* Remove from the pool */
        uShellHalPortPoolRemoveByParent(halPort);

        /* Deinit HAL base object */
        statusHal = UShellHalDeinit(&halPort->base);
        if (statusHal != USHELL_HAL_NO_ERR)
        {
            /* HAL base object deinit failed */
            USHELL_HAL_PORT_ASSERT(0);
        }

        /* Clear */
        memset(halPort, 0, sizeof(UShellHalPort_s));

    } while (0);

    return status;
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================

/**
 * \brief This function to add a new link to the pool of the relationship of a specific uart to a specific  hal port
 * \param[in] parent - the parent object (the instance hal port) to be added to the pool;
 * \param[in] uart - the uart object to be added to the pool (from Atmel Start);
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolAdd(const void* const parent,
                                               const struct usart_async_descriptor* const uart)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((parent == NULL) ||
            (uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Add new link to the pool */
        if ((uShellPortLink.uart != NULL) ||
            (uShellPortLink.parent != NULL))
        {
            /* Link already exists */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }

        /* Add new link to the pool */
        uShellPortLink.uart = uart;
        uShellPortLink.parent = parent;

    } while (0);

    return status;
}

/**
 * \brief This function to remove a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] parent - the parent object (the instance hal port) to be removed from the pool ;
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolRemoveByParent(const void* const parent)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    /* Remove link from the pool */
    do
    {
        /* Check input parameters */
        if (parent == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Check parent is the same */
        if (uShellPortLink.parent != parent)
        {
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }

        /* Remove link from the pool */
        uShellPortLink.uart = NULL;
        uShellPortLink.parent = NULL;

    } while (0);

    return status;
}

/**
 * \brief This function to get a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] uart - the uart object to find link in the pool (from Atmel Start);
 * \param[out] parent - pointer to stored parent object (the instance hal port);
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolParentGet(const struct usart_async_descriptor* const uart,
                                                     void** const parent)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    /* Get link from the pool */
    do
    {
        /* Check input parameters */
        if ((uart == NULL) ||
            (parent == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Check uart is the same */
        if (uShellPortLink.uart != uart)
        {
            /* Link not found */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INIT_ERR;
            break;
        }

        /* Get parent */
        *parent = (void*) uShellPortLink.parent;

    } while (0);

    return status;
}

/**
 * \brief Open function.
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortOpen(void* const hal)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (halPort->uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Attach callback for TX complete */
        int32_t statusAsf = usart_async_register_callback(halPort->uart,
                                                          USART_ASYNC_TXC_CB,
                                                          uShellHalPortTxCompleteCb);
        if (statusAsf != ERR_NONE)
        {
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Attach callback for error */
        statusAsf = usart_async_register_callback(halPort->uart,
                                                  USART_ASYNC_ERROR_CB,
                                                  uShellHalPortErrorCb);
        if (statusAsf != ERR_NONE)
        {
            uShellHalPortClose(hal);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Attach callback for RX receive */
        statusAsf = usart_async_register_callback(halPort->uart,
                                                  USART_ASYNC_RXC_CB,
                                                  uShellHalPortRxReceiveCb);

        if (statusAsf != ERR_NONE)
        {
            uShellHalPortClose(hal);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Close function.
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortClose(void* const hal)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (halPort->uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Detach callback for TX complete */
        int32_t statusAsf = usart_async_register_callback(halPort->uart,
                                                          USART_ASYNC_TXC_CB,
                                                          NULL);
        if (statusAsf != ERR_NONE)
        {
            /* Attach failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Detach callback for error */
        statusAsf = usart_async_register_callback(halPort->uart,
                                                  USART_ASYNC_ERROR_CB,
                                                  NULL);
        if (statusAsf != ERR_NONE)
        {
            /* Attach failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Detach callback for RX receive */
        statusAsf = usart_async_register_callback(halPort->uart,
                                                  USART_ASYNC_RXC_CB,
                                                  NULL);
        if (statusAsf != ERR_NONE)
        {
            /* Attach failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Write function.
 * \param[in] hal Pointer to the HAL instance.
 * \param[in] data Pointer to the data to be written.
 * \param[in] size Size of the data to be written.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortWrite(void* const hal,
                                         const UShellHalItem_t* const data,
                                         const size_t size)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (data == NULL) ||
            (size == 0) ||
            (halPort->uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Write data */
        int32_t asfStatus = io_write((struct io_descriptor*) halPort->uart,
                                     (uint8_t*) data,
                                     size);
        if (asfStatus != size)
        {
            /* Write failed */
            status = USHELL_HAL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Read function.
 * \param[in] hal Pointer to the HAL instance.
 * \param[out] data Pointer to the buffer to store the read data.
 * \param[in] size Size of the buffer.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortRead(void* const hal,
                                        UShellHalItem_t* const data,
                                        const size_t buffSize,
                                        size_t* const usedSize)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (data == NULL) ||
            (buffSize == 0) ||
            (halPort->uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Read data */
        int32_t asfStatus = io_read((struct io_descriptor*) halPort->uart,
                                    (uint8_t*) data,
                                    buffSize);

        /* Return used size */
        *usedSize = asfStatus;

    } while (0);

    return status;
}

/**
 * \brief Set Tx mode
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 *
 */
static UShellHalErr_e uShellHalPortSetTxMode(void* const hal)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Open the port */
    do
    {
        /* Check input parameters */
        if ((halPort == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check we have the transceiver */
        if (halPort->cfg.transceiverEnabled == false)
        {
            break;
        }

        /* Set rx tx pin */
        gpio_set_pin_level(halPort->cfg.transceiverPins.txPin, halPort->cfg.transceiverPins.txPinActive);
        gpio_set_pin_level(halPort->cfg.transceiverPins.rxPin, !halPort->cfg.transceiverPins.rxPinActive);

    } while (0);

    /* Return status */
    return status;
}

/**
 * \brief Set Rx mode
 * \param[in] hal Pointer to the HAL instance.
 * \return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortSetRxMode(void* const hal)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Open the port */
    do
    {
        /* Check input parameters */
        if (halPort == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check we have the transceiver */
        if (halPort->cfg.transceiverEnabled == false)
        {
            break;
        }

        /* Set rx tx pin */
        gpio_set_pin_level(halPort->cfg.transceiverPins.txPin, !halPort->cfg.transceiverPins.txPinActive);
        gpio_set_pin_level(halPort->cfg.transceiverPins.rxPin, halPort->cfg.transceiverPins.rxPinActive);

    } while (0);

    /* Return status */
    return status;
}

/**
 * \brief Check if data is available for reading.
 * \param[in] hal - Pointer to the HAL instance.
 * \param[in] isAvailable - Pointer to a boolean indicating if data is available.
 * \return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalErr_e uShellHalPortIsReadDataAvailable(void* const hal,
                                                       bool* const isAvailable)
{
    /* Local variable */
    UShellHalPort_s* halPort = (UShellHalPort_s*) hal;
    UShellHalErr_e status = USHELL_HAL_NO_ERR;
    int32_t asfStatus = 0;

    /* Open the port */
    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (isAvailable == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check if data is available */
        asfStatus = usart_async_is_rx_not_empty(halPort->uart);

        /* Return status */
        *isAvailable = (asfStatus == 0) ? false : true;

    } while (0);

    /* Return status */
    return status;
}

/**
 * \brief Callback for error in the usart (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has an error;
 * \param[out] no;
 */
static void uShellHalPortErrorCb(const struct usart_async_descriptor* const usart)
{
    /* Local variable */
    void* parent = NULL;
    UShellHalPort_s* halPort = NULL;
    UShellHalPortErr_e statusHalPort = USHELL_HAL_PORT_NO_ERR;

    do
    {
        /* Check input parameter */
        if (usart == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Find link in the pool */
        statusHalPort = uShellHalPortPoolParentGet(usart, &parent);
        if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
        {
            /* Link not found */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Equating pointers */
        halPort = (UShellHalPort_s*) parent;

        /* Check error callback */
        if (NULL == halPort->base.rxTxErrorCb)
        {
            break;
        }

        /* Call error callback */
        halPort->base.rxTxErrorCb((void*) halPort, USHELL_HAL_CB_RX_TX_ERROR);

    } while (0);
}

/**
 * \brief Callback for Rx receive (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has an error;
 * \param[out] no;
 */
static void uShellHalPortRxReceiveCb(const struct usart_async_descriptor* const usart)
{
    /* Local variable */
    void* parent = NULL;
    UShellHalPort_s* halPort = NULL;
    UShellHalPortErr_e statusHalPort = USHELL_HAL_PORT_NO_ERR;

    do
    {
        /* Check input parameter */
        if (usart == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Find link in the pool */
        statusHalPort = uShellHalPortPoolParentGet(usart, &parent);
        if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
        {
            /* Link not found */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Equating pointers */
        halPort = (UShellHalPort_s*) parent;

        /* Check error callback */
        if (NULL == halPort->base.rxReceivedCb)
        {
            break;
        }

        /* Call error callback */
        halPort->base.rxReceivedCb((void*) halPort, USHELL_HAL_CB_RX_RECEIVED);

    } while (0);
}

/**
 * \brief Callback for Tx complete (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has completed the transmission;
 * \param[out] no;
 */
static void uShellHalPortTxCompleteCb(const struct usart_async_descriptor* const usart)
{
    /* Local variable */
    void* parent = NULL;
    UShellHalPort_s* halPort = NULL;
    UShellHalPortErr_e statusHalPort = USHELL_HAL_PORT_NO_ERR;

    do
    {
        /* Check input parameter */
        if (usart == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Find link in the pool */
        statusHalPort = uShellHalPortPoolParentGet(usart, &parent);
        if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
        {
            /* Link not found */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Equating pointers */
        halPort = (UShellHalPort_s*) parent;

        /* Check error callback */
        if (NULL == halPort->base.txCompleteCb)
        {
            break;
        }

        /* Call error callback */
        halPort->base.txCompleteCb((void*) halPort,
                                   USHELL_HAL_CB_TX_COMPLETE);

    } while (0);
}