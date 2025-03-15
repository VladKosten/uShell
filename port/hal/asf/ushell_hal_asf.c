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
#include <string.h>
#include <stdbool.h>

/* Standard includes */
#include "uShell_hal_asf.h"

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
static UShellHalPortErr_e uShellHalPortPoolRemove(const void* const parent);

/**
 * \brief This function to get a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] uart - the uart object to find link in the pool (from Atmel Start);
 * \param[out] parent - pointer to stored parent object (the instance hal port);
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolParentGet(const struct usart_async_descriptor* const uart,
                                                     void** const parent);

/**
 * @brief Open function.
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortOpen(void* const hal);

/**
 * @brief Close function.
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortClose(void* const hal);

/**
 * @brief Write function.
 * @param[in] hal Pointer to the HAL instance.
 * @param[in] data Pointer to the data to be written.
 * @param[in] size Size of the data to be written.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortWrite(void* const hal,
                                         const UShellHalItem_t* const data,
                                         const size_t size);

/**
 * @brief Read function.
 * @param[in] hal Pointer to the HAL instance.
 * @param[out] data Pointer to the buffer to store the read data.
 * @param[in] size Size of the buffer.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortRead(void* const hal,
                                        UShellHalItem_t* const data,
                                        const size_t size);

/**
 * @brief Flush function.
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortFlush(void* const hal);

/**
 * @brief Set Tx mode
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 *
 */
static UShellHalErr_e uShellHalPortSetTxMode(void* const hal);

/**
 * @brief Set Rx mode
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortSetRxMode(void* const hal);

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
 * @brief Table of uShell HAL port operation functions.
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
    .flush = uShellHalPortFlush,
    .setTxMode = uShellHalPortSetTxMode,
    .setRxMode = uShellHalPortSetRxMode,
};

/**
 * @brief For store context from interrupt
 */
static USHellPortLink_s ushellPortLink = {
    .parent = NULL,
    .uart NULL};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

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
                                     const char* const name)
{
    /* Check input parameters */
    if ((halPort == NULL) ||
        (uart == NULL))
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Exit: invalid arguments
    }

    /* Init HAL base object */
    DwinHalErr_e statusHal = DwinHalInit(&halPort->base, parent, name);
    if (statusHal != DWIN_HAL_NO_ERR)
    {
        return DWIN_HAL_PORT_INIT_ERR;    // Exit: invalid arguments
    }

    /* Store uart */
    halPort->uart = (struct usart_async_descriptor*) uart;

    /* Create link in the pool */
    UShellHalPortErr_e statusHalPort = dwinHalPortPoolAdd(halPort, halPort->uart);
    if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
    {
        return DWIN_HAL_PORT_INIT_ERR;    // Exit: invalid arguments
    }

    /* Attach callbacks to Asf port object */
    int32_t statusAsf = usart_async_register_callback(halPort->uart, USART_ASYNC_TXC_CB, dwinHalPortTxCompleteCb);
    if (statusAsf != ERR_NONE)
    {
        return DWIN_HAL_PORT_INIT_ERR;    // Exit: invalid arguments
    }
    statusAsf = usart_async_register_callback(halPort->uart, USART_ASYNC_ERROR_CB, dwinHalPortErrorCb);
    if (statusAsf != ERR_NONE)
    {
        return DWIN_HAL_PORT_INIT_ERR;    // Exit: invalid arguments
    }
    statusAsf = usart_async_register_callback(halPort->uart, USART_ASYNC_RXC_CB, dwinHalPortRxReceiveCb);
    if (statusAsf != ERR_NONE)
    {
        return DWIN_HAL_PORT_INIT_ERR;    // Exit: invalid arguments
    }

    /* Flush rx buffer */
    statusAsf = usart_async_flush_rx_buffer(halPort->uart);
    if (statusAsf != ERR_NONE)
    {
        return DWIN_HAL_PORT_LAYER_ERR;    // Error: port layer
    }

    /* Enable USART */
    statusAsf = usart_async_enable(halPort->uart);
    if (statusAsf != ERR_NONE)
    {
        return DWIN_HAL_PORT_INIT_ERR;    // Exit: invalid arguments
    }
    /* Init HAL portable functions */
    halPort->base.portable = &dwinHalPortableFuncTab;

    return USHELL_HAL_PORT_NO_ERR;    // Exit: no errors
}

/**
 * \brief Deinit the HAL portable layer for the particular platform
 * \param[in] UShellHalPort_s* const halPort - const halPort - the HAL port descriptor
 * \param[out] no
 * \return UShellHalErr_e  - error code. non-zero = an error has occurred.
 */
UShellHalPortErr_e UShellHalPortDeinit(UShellHalPort_s* const halPort)
{
    /* Check input parameters */
    if (halPort == NULL)
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Exit: invalid arguments
    }

    /* Deinit HAL base object */
    DwinHalErr_e statusHal = DwinHalDeinit(&halPort->base);
    (void) statusHal;
    USHELL_HAL_PORT_ASSERT(statusHal == DWIN_HAL_NO_ERR);
    int32_t statusAsf = ERR_NONE;
    (void) statusAsf;
    if (halPort->uart != NULL)
    {
        /* Detach callbacks from ASf port object */
        statusAsf = usart_async_register_callback(halPort->uart, USART_ASYNC_RXC_CB, NULL);
        USHELL_HAL_PORT_ASSERT(statusAsf == ERR_NONE);
        statusAsf = usart_async_register_callback(halPort->uart, USART_ASYNC_TXC_CB, NULL);
        USHELL_HAL_PORT_ASSERT(statusAsf == ERR_NONE);
        statusAsf = usart_async_register_callback(halPort->uart, USART_ASYNC_ERROR_CB, NULL);
        USHELL_HAL_PORT_ASSERT(statusAsf == ERR_NONE);

        /* Disable USART */
        statusAsf = usart_async_disable(halPort->uart);
        USHELL_HAL_PORT_ASSERT(statusAsf == ERR_NONE);
    }

    /* Remove from the pool */
    halPort->uart = NULL;
    UShellHalPortErr_e statusHalPort = dwinHalPortPoolRemove(halPort);
    (void) statusHalPort;
    USHELL_HAL_PORT_ASSERT(statusHalPort == USHELL_HAL_PORT_NO_ERR);

    return USHELL_HAL_PORT_NO_ERR;    // Exit: no errors
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
    /* Check input parameters */
    if ((parent == NULL) ||
        (uart == NULL))
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Exit: invalid arguments
    }

    /* Check if the link already exist*/
    if ((ushellPortLink.uart == uart) ||
        (ushellPortLink.parent == parent))
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Exit: invalid arguments
    }

    /* Add new link to the pool */
    if ((ushellPortLink.uart == NULL) ||
        (ushellPortLink.parent == NULL))
    {
        ushellPortLink.uart = uart;
        ushellPortLink.parent = parent;
    }

    return USHELL_HAL_PORT_NO_ERR;
}

/**
 * \brief This function to remove a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] parent - the parent object (the instance hal port) to be removed from the pool ;
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolRemove(const void* const parent)
{
    /* Check input parameters */
    if (parent == NULL)
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Exit: invalid arguments
    }

    /* Remove link from the pool */
    for (uint8_t ind = 0; ind < DWIN_HAL_PORT_POOL_SIZE; ind++)
    {
        if (dwinHalPortLinkPool [ind].parent == parent)
        {
            dwinHalPortLinkPool [ind].uart = NULL;
            dwinHalPortLinkPool [ind].parent = NULL;
        }
    }

    return DWIN_HAL_PORT_POOL_ERR;    // Exit: no errors
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
    /* Check input parameters */
    if ((uart == NULL) || (parent == NULL))
    {
        return USHELL_HAL_PORT_INVALID_ARGS_ERR;    // Exit: invalid arguments
    }

    /* Get link from the pool */
    for (uint8_t ind = 0; ind < DWIN_HAL_PORT_POOL_SIZE; ind++)
    {
        if (dwinHalPortLinkPool [ind].uart == uart)
        {
            *parent = (void*) dwinHalPortLinkPool [ind].parent;
            return USHELL_HAL_PORT_NO_ERR;
        }
    }

    return DWIN_HAL_PORT_POOL_ERR;    // Exit: link not found in the pool
}

/**
 * @brief Open function.
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortOpen(void* const hal);

/**
 * @brief Close function.
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortClose(void* const hal);

/**
 * @brief Write function.
 * @param[in] hal Pointer to the HAL instance.
 * @param[in] data Pointer to the data to be written.
 * @param[in] size Size of the data to be written.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortWrite(void* const hal,
                                         const UShellHalItem_t* const data,
                                         const size_t size);

/**
 * @brief Read function.
 * @param[in] hal Pointer to the HAL instance.
 * @param[out] data Pointer to the buffer to store the read data.
 * @param[in] size Size of the buffer.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortRead(void* const hal,
                                        UShellHalItem_t* const data,
                                        const size_t size);

/**
 * @brief Flush function.
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortFlush(void* const hal);

/**
 * @brief Set Tx mode
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 *
 */
static UShellHalErr_e uShellHalPortSetTxMode(void* const hal);

/**
 * @brief Set Rx mode
 * @param[in] hal Pointer to the HAL instance.
 * @return Error code indicating the result of the operation.
 */
static UShellHalErr_e uShellHalPortSetRxMode(void* const hal);

/**
 * \brief Callback for error in the usart (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has an error;
 * \param[out] no;
 */
static void uShellHalPortErrorCb(const struct usart_async_descriptor* const usart)
{
    /* Check input parameters*/
    USHELL_HAL_PORT_ASSERT(usart != NULL);

    /* Find link in the pool */
    void* parent = NULL;
    UShellHalPortErr_e statusHalPort = dwinHalPortPoolParentGet(usart, &parent);
    (void) statusHalPort;
    USHELL_HAL_PORT_ASSERT(statusHalPort == USHELL_HAL_PORT_NO_ERR);

    /* Equating pointers */
    DwinHalPort_s* halPort = (DwinHalPort_s*) parent;

    /* Call error callback */
    if (NULL != halPort->base.errorCb)
    {
        halPort->base.errorCb((DwinHal_s*) halPort, DWIN_HAL_ERROR_CB_TYPE);
    }
}

/**
 * \brief Callback for Rx receive (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has an error;
 * \param[out] no;
 */
static void uShellHalPortRxReceiveCb(const struct usart_async_descriptor* const usart)
{
    /* Check input parameters*/
    USHELL_HAL_PORT_ASSERT(usart != NULL);

    /* Find link in the pool */
    void* parent = NULL;
    UShellHalPortErr_e statusHalPort = dwinHalPortPoolParentGet(usart, &parent);
    (void) statusHalPort;
    USHELL_HAL_PORT_ASSERT(statusHalPort == USHELL_HAL_PORT_NO_ERR);

    /* Equating pointers */
    DwinHalPort_s* halPort = (DwinHalPort_s*) parent;

    /* Call receive callback */
    if (NULL != halPort->base.rxReceiveCb)
    {
        halPort->base.rxReceiveCb((DwinHal_s*) halPort, DWIN_HAL_RX_RECEIVE_CB_TYPE);
    }
}

/**
 * \brief Callback for Tx complete (portable)
 * \param[in] usart - the usart descriptor (from Atmel Start) which has completed the transmission;
 * \param[out] no;
 */
static void uShellHalPortTxCompleteCb(const struct usart_async_descriptor* const usart)
{
    /* Check input parameters*/
    USHELL_HAL_PORT_ASSERT(usart != NULL);

    /* Find link in the pool */
    void* parent = NULL;
    UShellHalPortErr_e statusHalPort = dwinHalPortPoolParentGet(usart, &parent);
    (void) statusHalPort;
    USHELL_HAL_PORT_ASSERT(statusHalPort == USHELL_HAL_PORT_NO_ERR);

    /* Equating pointers */
    DwinHalPort_s* halPort = (DwinHalPort_s*) parent;

    /* Call error callback */
    if (NULL != halPort->base.txCompleteCb)
    {
        halPort->base.txCompleteCb((DwinHal_s*) halPort, DWIN_HAL_TX_COMPLETE_CB_TYPE);
    }
}