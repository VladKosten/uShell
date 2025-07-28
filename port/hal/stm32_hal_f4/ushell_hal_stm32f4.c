/**
 * \file      ushell_hal_port.c
 * \brief     The uShell HAL port layer implementation for the STM32F4 HAL.
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
#include "ushell_hal_stm32f4.h"

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
    const UART_HandleTypeDef* uart;

} UShellPortLink_s;

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Flush the RX buffer.
 * \param[in] halPort - Pointer to the HAL port instance.
 */
static inline void uShellHalPortRxBuffFlush(UShellHalPort_s* const halPort);

/**
 * \brief Flush the TX buffer.
 * \param[in] halPort - Pointer to the HAL port instance.
 */
static inline void uShellHalPortTxBuffFlush(UShellHalPort_s* const halPort);

/**
 * \brief Initialize the ring buffer.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferInit(UShellHalPortRingBuffer_s* const ringBuffer);

/**
 * \brief Push data into the ring buffer.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \param[in] data - Data to be pushed into the ring buffer.
 * \param[in] dataSize - Size of the data to be pushed.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferPush(UShellHalPortRingBuffer_s* const ringBuffer,
                                                      const uint8_t* const data,
                                                      const size_t dataSize);

/**
 * \brief Pop data from the ring buffer.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \param[out] buff - Buffer to store the popped data.
 * \param[in] buffSize - Size of the buffer.
 * \param[out] usedSize - Pointer to store the size of the data used.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferPop(UShellHalPortRingBuffer_s* const ringBuffer,
                                                     uint8_t* const buff,
                                                     const size_t buffSize,
                                                     size_t* const usedSize);

/**
 * \brief Check if the ring buffer is empty.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \param[out] isEmpty - Pointer to store the empty status.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferIsEmpty(const UShellHalPortRingBuffer_s* const ringBuffer,
                                                         bool* const isEmpty);
/**
 * \brief This function to add a new link to the pool of the relationship of a specific uart to a specific  hal port
 * \param[in] parent - the parent object (the instance hal port) to be added to the pool;
 * \param[in] uart - the uart object to be added to the pool ;
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolAdd(const void* const parent,
                                               const UART_HandleTypeDef* const uart);

/**
 * \brief This function to remove a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] parent - the parent object (the instance hal port) to be removed from the pool ;
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolRemoveByParent(const void* const parent);

/**
 * \brief This function to get a link from the pool of the relationship of a specific uart to a specific hal port
 * \param[in] uart - the uart object to find link in the pool ;
 * \param[out] parent - pointer to stored parent object (the instance hal port);
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolParentGet(const UART_HandleTypeDef* const uart,
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
 * \param[in] usart - the usart descriptor  which has an error;
 * \param[out] no;
 */
static void uShellHalPortErrorCb(UART_HandleTypeDef* usart);

/**
 * \brief Callback for Tx complete (portable)
 * \param[in] usart - the usart descriptor which has completed the transmission;
 * \param[out] no;
 */
static void uShellHalPortTxCompleteCb(UART_HandleTypeDef* usart);

/**
 * \brief Callback for Rx received (portable)
 * \param[in] huart - the usart descriptor which has received data;
 * \param[in] Pos - the position in the buffer where the data was received;
 * \param[out] no;
 */
static void uShellHalPortRxReceivedCb(UART_HandleTypeDef* huart,
                                      uint16_t Pos);

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
static UShellPortLink_s uShellPortLink = {
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
                                     const UART_HandleTypeDef* uart,
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

        /* Clear HAL port */
        memset(halPort, 0, sizeof(UShellHalPort_s));

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
        halPort->uart = (UART_HandleTypeDef*) uart;

        /* Create link in the pool */
        UShellHalPortErr_e statusHalPort = uShellHalPortPoolAdd(halPort, halPort->uart);
        if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
        {
            /* Add link to the pool failed */
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
 * \brief Flush the RX buffer.
 * \param[in] halPort - Pointer to the HAL port instance.
 */
static inline void uShellHalPortRxBuffFlush(UShellHalPort_s* const halPort)
{
    do
    {
        if (halPort == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            return;
        }

        /* Flush RX buffer */
        memset(halPort->rx.buff, 0, sizeof(USHELL_HAL_PORT_IO_BUFFER_SIZE));
        halPort->rx.buffSize = 0;

    } while (0);
}

/**
 * \brief Flush the TX buffer.
 * \param[in] halPort - Pointer to the HAL port instance.
 */
static void uShellHalPortTxBuffFlush(UShellHalPort_s* const halPort)
{
    do
    {
        if (halPort == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            return;
        }

        /* Flush TX buffer */
        memset(halPort->tx.buff, 0, sizeof(USHELL_HAL_PORT_IO_BUFFER_SIZE));
        halPort->tx.buffSize = 0;

    } while (0);
}

/**
 * \brief Initialize the ring buffer.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferInit(UShellHalPortRingBuffer_s* const ringBuffer)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameters */
        if (ringBuffer == NULL)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Clear ring buffer */
        memset(ringBuffer, 0, sizeof(UShellHalPortRingBuffer_s));
        ringBuffer->head = 0;
        ringBuffer->tail = 0;

    } while (0);

    return status;
}

/**
 * \brief Push data into the ring buffer.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \param[in] data - Data to be pushed into the ring buffer.
 * \param[in] dataSize - Size of the data to be pushed.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferPush(UShellHalPortRingBuffer_s* const ringBuffer,
                                                      const uint8_t* const data,
                                                      const size_t dataSize)
{
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    do
    {
        if ((ringBuffer == NULL) ||
            (data == NULL) ||
            (dataSize == 0) ||
            (dataSize > USHELL_HAL_PORT_IO_BUFFER_SIZE))
        {
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        // Calculate used space
        size_t used = (ringBuffer->head >= ringBuffer->tail)
                          ? (ringBuffer->head - ringBuffer->tail)
                          : (USHELL_HAL_PORT_IO_BUFFER_SIZE - ringBuffer->tail + ringBuffer->head);

        // Calculate free space
        size_t free = USHELL_HAL_PORT_IO_BUFFER_SIZE - used;

        // If not enough space, move tail forward to make room
        if (dataSize > free)
        {
            ringBuffer->tail = (ringBuffer->tail + (dataSize - free)) % USHELL_HAL_PORT_IO_BUFFER_SIZE;
        }

        // Push data
        for (size_t i = 0; i < dataSize; i++)
        {
            ringBuffer->buff [ringBuffer->head] = data [i];
            ringBuffer->head = (ringBuffer->head + 1) % USHELL_HAL_PORT_IO_BUFFER_SIZE;
        }

    } while (0);

    return status;
}

/**
 * \brief Pop data from the ring buffer.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \param[out] buff - Buffer to store the popped data.
 * \param[in] buffSize - Size of the buffer.
 * \param[out] usedSize - Pointer to store the size of the data used.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferPop(UShellHalPortRingBuffer_s* const ringBuffer,
                                                     uint8_t* const buff,
                                                     const size_t buffSize,
                                                     size_t* const usedSize)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameters */
        if ((ringBuffer == NULL) ||
            (buff == NULL) ||
            (buffSize == 0) ||
            (usedSize == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Check if the ring buffer is empty */
        bool isEmpty = false;
        status = uShellHalPortRingBufferIsEmpty(ringBuffer, &isEmpty);
        if (status != USHELL_HAL_PORT_NO_ERR)
        {
            break;
        }

        if (isEmpty)
        {
            *usedSize = 0;    // No data to pop
            return USHELL_HAL_PORT_NO_ERR;
        }

        /* Pop data from the ring buffer */
        size_t i = 0;
        while ((i < buffSize) && (ringBuffer->tail != ringBuffer->head))
        {
            buff [i++] = ringBuffer->buff [ringBuffer->tail];
            ringBuffer->tail = (ringBuffer->tail + 1) % USHELL_HAL_PORT_IO_BUFFER_SIZE;
        }

        *usedSize = i;

    } while (0);

    return status;
}

/**
 * \brief Check if the ring buffer is empty.
 * \param[in] ringBuffer - Pointer to the ring buffer instance.
 * \param[out] isEmpty - Pointer to store the empty status.
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortRingBufferIsEmpty(const UShellHalPortRingBuffer_s* const ringBuffer,
                                                         bool* const isEmpty)
{
    /* Local variable */
    UShellHalPortErr_e status = USHELL_HAL_PORT_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameters */
        if ((ringBuffer == NULL) ||
            (isEmpty == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_INVALID_ARGS_ERR;
            break;
        }

        /* Check if the ring buffer is empty */
        *isEmpty = (ringBuffer->head == ringBuffer->tail);

    } while (0);

    return status;
}

/**
 * \brief This function to add a new link to the pool of the relationship of a specific uart to a specific  hal port
 * \param[in] parent - the parent object (the instance hal port) to be added to the pool;
 * \param[in] uart - the uart object to be added to the pool ;
 * \param[out] no;
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolAdd(const void* const parent,
                                               const UART_HandleTypeDef* const uart)
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
 * \param[in] uart - the uart object to find link in the pool ;
 * \param[out] parent - pointer to stored parent object (the instance hal port);
 * \return UShellHalPortErr_e - error code. non-zero = an error has occurred;
 */
static UShellHalPortErr_e uShellHalPortPoolParentGet(const UART_HandleTypeDef* const uart,
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
    HAL_StatusTypeDef stStatus = HAL_OK;

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

        /* Flush RX buffer */
        uShellHalPortRxBuffFlush(halPort);

        /* Flush TX buffer */
        uShellHalPortTxBuffFlush(halPort);

        /* Initialize RX ring buffer */
        UShellHalPortErr_e ringBufferStatus = uShellHalPortRingBufferInit(&halPort->rxRingBuffer);
        if (ringBufferStatus != USHELL_HAL_PORT_NO_ERR)
        {
            /* Ring buffer initialization failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Register callback for Rx received */
        stStatus = HAL_UART_RegisterRxEventCallback(halPort->uart,
                                                    uShellHalPortRxReceivedCb);
        if (stStatus != HAL_OK)
        {
            /* Register callback failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Register callback for TX complete */
        stStatus = HAL_UART_RegisterCallback(halPort->uart,
                                             HAL_UART_TX_COMPLETE_CB_ID,
                                             uShellHalPortTxCompleteCb);
        if (stStatus != HAL_OK)
        {
            /* Register callback failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Register callback for error */
        stStatus = HAL_UART_RegisterCallback(halPort->uart,
                                             HAL_UART_ERROR_CB_ID,
                                             uShellHalPortErrorCb);
        if (stStatus != HAL_OK)
        {
            /* Register callback failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
            break;
        }

        /* Enable receiver */
        stStatus = HAL_UARTEx_ReceiveToIdle_IT(halPort->uart,
                                               halPort->rx.buff,
                                               USHELL_HAL_PORT_IO_BUFFER_SIZE);
        if (stStatus != HAL_OK)
        {
            /* Enable receiver failed */
            USHELL_HAL_PORT_ASSERT(0);
            (void) uShellHalPortClose(halPort);
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
    HAL_StatusTypeDef stStatus = HAL_OK;
    (void) stStatus;

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

        /* Disable receiver */
        stStatus = HAL_UART_AbortReceive_IT(halPort->uart);
        USHELL_HAL_PORT_ASSERT(stStatus == HAL_OK);

        /* Disable transmitter */
        stStatus = HAL_UART_AbortTransmit_IT(halPort->uart);
        USHELL_HAL_PORT_ASSERT(stStatus == HAL_OK);

        /* Unregister callbacks */
        stStatus = HAL_UART_UnRegisterCallback(halPort->uart,
                                               HAL_UART_TX_COMPLETE_CB_ID);
        USHELL_HAL_PORT_ASSERT(stStatus == HAL_OK);

        stStatus = HAL_UART_UnRegisterCallback(halPort->uart,
                                               HAL_UART_ERROR_CB_ID);
        USHELL_HAL_PORT_ASSERT(stStatus == HAL_OK);

        stStatus = HAL_UART_UnRegisterRxEventCallback(halPort->uart);
        USHELL_HAL_PORT_ASSERT(stStatus == HAL_OK);

        /* Flush RX buffer */
        uShellHalPortRxBuffFlush(halPort);

        /* Flush TX buffer */
        uShellHalPortTxBuffFlush(halPort);

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
    HAL_StatusTypeDef stStatus = HAL_OK;

    do
    {
        /* Check input parameters */
        if ((halPort == NULL) ||
            (data == NULL) ||
            (size == 0) ||
            (size > USHELL_HAL_PORT_IO_BUFFER_SIZE) ||
            (halPort->tx.buffSize + size > USHELL_HAL_PORT_IO_BUFFER_SIZE) ||
            (halPort->uart == NULL))
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Clear TX buffer */
        uShellHalPortTxBuffFlush(halPort);

        /* Copy data to TX buffer */
        memcpy(&halPort->tx.buff [halPort->tx.buffSize], data, size);
        halPort->tx.buffSize += size;

        /* Write data */
        stStatus = HAL_UART_Transmit_IT(halPort->uart,
                                        halPort->tx.buff,
                                        halPort->tx.buffSize);
        if (stStatus != HAL_OK)
        {
            /* Transmit data failed */
            USHELL_HAL_PORT_ASSERT(0);
            status = USHELL_HAL_PORT_ERR;    // Exit: invalid arguments
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

        /* Check if buffer size is enough */
        if (buffSize < halPort->rx.buffSize)
        {
            /* Buffer size is not enough */
            *usedSize = 0;
            status = USHELL_HAL_SIZE_ERR;
            break;
        }

        /* Pop data from the ring buffer */
        status = uShellHalPortRingBufferPop(&halPort->rxRingBuffer,
                                            (uint8_t*) data,
                                            buffSize,
                                            usedSize);
        if (status != USHELL_HAL_PORT_NO_ERR)
        {
            /* Pop data from the ring buffer failed */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

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

        // Set rx pin inactive
        HAL_GPIO_WritePin(halPort->cfg.transceiverPins.rxPort,
                          halPort->cfg.transceiverPins.rxPin,
                          !halPort->cfg.transceiverPins.rxPinActive);

        /* Set tx rx pin */
        HAL_GPIO_WritePin(halPort->cfg.transceiverPins.txPort,
                          halPort->cfg.transceiverPins.txPin,
                          halPort->cfg.transceiverPins.txPinActive);

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

        // Set tx pin inactive
        HAL_GPIO_WritePin(halPort->cfg.transceiverPins.txPort,
                          halPort->cfg.transceiverPins.txPin,
                          !halPort->cfg.transceiverPins.txPinActive);

        /* Set rx pin */
        HAL_GPIO_WritePin(halPort->cfg.transceiverPins.rxPort,
                          halPort->cfg.transceiverPins.rxPin,
                          halPort->cfg.transceiverPins.rxPinActive);

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

        /* Check ring buffer status */
        status = uShellHalPortRingBufferIsEmpty(&halPort->rxRingBuffer, isAvailable);
        if (status != USHELL_HAL_PORT_NO_ERR)
        {
            /* Check ring buffer status failed */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

    } while (0);

    /* Return status */
    return status;
}

/**
 * \brief Callback for error in the usart (portable)
 * \param[in] usart - the usart descriptor  which has an error;
 * \param[out] no;
 */
static void uShellHalPortErrorCb(UART_HandleTypeDef* usart)
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
 * \brief Callback for Tx complete (portable)
 * \param[in] usart - the usart descriptor  which has completed the transmission;
 * \param[out] no;
 */
static void uShellHalPortTxCompleteCb(UART_HandleTypeDef* const usart)
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

/**
 * \brief Callback for Rx received (portable)
 * \param[in] huart - the usart descriptor which has received data;
 * \param[in] Pos - the position in the buffer where the data was received;
 * \param[out] no;
 */
static void uShellHalPortRxReceivedCb(UART_HandleTypeDef* huart,
                                      uint16_t Pos)
{
    /* Local variable */
    void* parent = NULL;
    UShellHalPort_s* halPort = NULL;
    UShellHalPortErr_e statusHalPort = USHELL_HAL_PORT_NO_ERR;
    HAL_StatusTypeDef stStatus = HAL_OK;
    (void) stStatus;

    do
    {
        /* Check input parameter */
        if (huart == NULL ||
            Pos == 0)
        {
            /* Input parameters are invalid */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Find link in the pool */
        statusHalPort = uShellHalPortPoolParentGet(huart, &parent);
        if (statusHalPort != USHELL_HAL_PORT_NO_ERR)
        {
            /* Link not found */
            USHELL_HAL_PORT_ASSERT(0);
            break;
        }

        /* Equating pointers */
        halPort = (UShellHalPort_s*) parent;

        /* save received data in the ring buffer */
        statusHalPort = uShellHalPortRingBufferPush(&halPort->rxRingBuffer,
                                                    halPort->rx.buff,
                                                    Pos);
        USHELL_HAL_PORT_ASSERT(statusHalPort == USHELL_HAL_PORT_NO_ERR);

        /* Enable receiver for the next data */
        stStatus = HAL_UARTEx_ReceiveToIdle_IT(huart,
                                               halPort->rx.buff,
                                               USHELL_HAL_PORT_IO_BUFFER_SIZE);
        USHELL_HAL_PORT_ASSERT(stStatus == HAL_OK);

        /* Check error callback */
        if (NULL == halPort->base.rxReceivedCb)
        {
            break;
        }

        /* Call error callback */
        halPort->base.rxReceivedCb((void*) halPort, USHELL_HAL_CB_RX_RECEIVED);

    } while (0);
}