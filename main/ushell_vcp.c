/**
* \file         ushell_vcp.c
* \brief        The file contains the implementation of the UShell vcp module. The module is responsible for the initialization of the UShell
*               and the main loop of vcp UShell.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright    MIT License (c) 2025
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...

*/
//===============================================================================[ INCLUDE ]========================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Project includes */
#include "ushell_vcp.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_VCP_ASSERT
    #define USHELL_VCP_ASSERT(cond)
#endif

/**
 * \brief VCP messages used by the module.
 *
 * This enum represents the basic events that can occur in the VCP module,
 * excluding completion and error events.
 */
typedef enum
{
    USHELL_VCP_EVENT_NONE = USHELL_OSAL_EVENT_GROUP_BIT_NONE,     ///< No message
    USHELL_VCP_EVENT_ERROR = USHELL_OSAL_EVENT_GROUP_BIT_0,       ///< Error message
    USHELL_VCP_EVENT_RX_EVENT = USHELL_OSAL_EVENT_GROUP_BIT_1,    ///< Rx event
    USHELL_VCP_EVENT_TX_EVENT = USHELL_OSAL_EVENT_GROUP_BIT_2,    ///< Tx event
    USHELL_VCP_EVENT_INSPECT = USHELL_OSAL_EVENT_GROUP_BIT_3,     ///< Inspect event

} UShellVcpEvent_e;

/**
 * \brief VCP transfer-related messages.
 *
 * This enum represents transaction completion and error events.
 */
typedef enum
{
    USHELL_VCP_MSG_TX_NONE = 0,        ///< Tx/Rx error event
    USHELL_VCP_MSG_TX_COMPLETE = 1,    ///< Tx complete event
    USHELL_VCP_MSG_TX_RX_ERR = 2,      ///< Tx or Rx error event

} UShellVcpMsgTransfer_e;

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShell thread worker
 * \param[in] vcp - vcp object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module.
 *       It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellWorker(void* const arg);

/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[in] cbType - callback type
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
 */
static void uShellVcpRxReceivedCb(const void* const hal,
                                  const UShellHalCallback_e cbType);

/**
 * \brief Callback for the transmitted data
 * \param[in] hal - hal object
 * \param[in] cbType - callback type
 * \param[out] none
 * \return none
 * \note This function is called when the data is transmitted to the serial port.
 */
static void uShellVcpTxCompleteCb(const void* const hal,
                                  const UShellHalCallback_e cbType);

/**
 * \brief Callback for the error
 * \param[in] hal - hal object
 * \param[in] cbType - callback type
 * \param[out] none
 * \return none
 * \note This function is called when the error occurs in the serial port.
 */
static void uShellVcpXferErrorCb(const void* const hal,
                                 const UShellHalCallback_e cbType);

/**
 * \brief Initialize the runtime environment
 * \param vcp - vcp object
 * \param osal - osal object
 * \param hal - hal object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvInit(UShellVcp_s* const vcp,
                                         UShellOsal_s* const osal,
                                         UShellHal_s* const hal);

/**
 * \brief Deinitialize the runtime environment
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvDeInit(UShellVcp_s* const vcp);

/**
 * \brief Initialize the runtime environment HAL
 * \param vcp - vcp object
 * \param hal - hal object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalInit(UShellVcp_s* const vcp,
                                            UShellHal_s* const hal);

/**
 * \brief Deinitialize the runtime environment HAL
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalDeInit(UShellVcp_s* const vcp);

/**
 * \brief Initialize the runtime environment OSAL
 * \param[in] vcp - vcp object
 * \param[in] osal - osal object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpRtEnvOsalInit(UShellVcp_s* const vcp,
                                             UShellOsal_s* const osal);

/**
 * \brief Deinitialize the runtime environment OSAL
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvOsalDeInit(UShellVcp_s* const vcp);

/**
 * \brief Lock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpPrintLock(const UShellVcp_s* const vcp);

/**
 * \brief Unlock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpPrintUnlock(const UShellVcp_s* const vcp);

/**
 * \brief Lock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpReadLock(const UShellVcp_s* const vcp);

/**
 * \brief Unlock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpReadUnlock(const UShellVcp_s* const vcp);

/**
 * \brief Send message event to the vcp object
 * \param vcp - vcp object to be sent
 * \param msgEvent - message event to be sent
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpEventSend(UShellVcp_s* const vcp,
                                         UShellVcpEvent_e msgEvent);

/**
 * \brief Wait for message event from the vcp object (blocked )
 * \param vcp - vcp object
 * \param msgEvent - message event to be waited
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpEventWait(UShellVcp_s* const vcp,
                                         UShellVcpEvent_e* const msgEvent);

/**
 * \brief Send message transfer to the vcp object
 * \param vcp - vcp object to be sent
 * \param msgTransfer - message transfer to be sent
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgTransferSend(UShellVcp_s* const vcp,
                                               UShellVcpMsgTransfer_e msgTransfer);

/**
 * \brief Flush message transfer from the vcp object
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgTransferFlush(UShellVcp_s* const vcp);

/**
 * \brief Wait for message transfer from the vcp object (blocked )
 * \param vcp - vcp object
 * \param msgTransfer - message transfer to be waited
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgTransferPend(UShellVcp_s* const vcp,
                                               UShellVcpMsgTransfer_e* const msgTransfer,
                                               const uint32_t timeout);

/**
 * \brief Read from the port
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpReadFromPort(UShellVcp_s* const vcp);

/**
 * \brief Write to the port
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpWriteToPort(UShellVcp_s* const vcp);

/**
 * \brief Inspect the vcp object
 * \param vcp - vcp object to be inspected
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpInspect(UShellVcp_s* const vcp);

/**
 * \brief Flush the buffer of the vcp object
 * \param vcp - vcp object to be flushed
 * \return none
 */
static inline void uShellVcpIoBuffFlush(UShellVcp_s* const vcp);

/**
 * \brief Flush the rx buffer of the vcp object
 * \param vcp - vcp object to be flushed
 */
static inline void uShellVcpStreamRxFlush(UShellVcp_s* const vcp);

/**
 * \brief Flush the tx buffer of the vcp object
 * \param vcp - vcp object to be flushed
 */
static inline void uShellVcpStreamTxFlush(UShellVcp_s* const vcp);

/**
 * \brief Set tx direction of the vcp object
 * \param[in] vcp - vcp object to be set
 */
static inline void uShellVcpDirectTxSet(UShellVcp_s* const vcp);

/**
 * \brief Set rx direction of the vcp  object
 * \param[in] vcp - vcp object to be set
 */
static inline void uShellVcpDirectRxSet(UShellVcp_s* const vcp);

/**
 * \brief Callback function for the timer expiration
 * \param timerParam - parameter for the timer expiration callback
 * \return none
 */
static void uShellVcpTimerExpiredCb(void* const timerParam);

/**
 * \brief Print a block of data to the uShell vcp object.
 * \param[in] vcp   - uShell vcp object.
 * \param[in] data  - pointer to the data to be printed.
 * \param[in] len   - length of the data to send.
 * \return UShellVcpErr_e - error code. non-zero means an error occurred.
 */
static UShellVcpErr_e uShellVcpPrintBytes(UShellVcp_s* const vcp,
                                          const void* data,
                                          size_t len);

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)
/**
 * \brief Used for stdio redirection
 */
static UShellVcp_s* vcpStdIO = NULL;
#endif

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Init uShell vcp object
 * \param[in] vcp - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] usedForStdIO - flag to indicate if the object is used for stdio (ONLY ONE INSTANCE CAN BE USED FOR STDIO)
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellVcpInit(UShellVcp_s* const vcp,
                             const UShellOsal_s* const osal,
                             const UShellHal_s* const hal,
                             const bool usedForStdIO,
                             void* const parent,
                             const char* const name)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(osal != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Initialize the runtime environment */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            return USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Flush the objects */
        memset(vcp, 0, sizeof(UShellVcp_s));

        /* Set the parent object */
        vcp->parent = parent;
        vcp->name = name;

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)

        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
        /* Save the vcp object for stdio redirection */
        vcp->usedForStdIO = usedForStdIO;
        if ((usedForStdIO == true) && (vcpStdIO == NULL))
        {
            vcpStdIO = vcp;
        }
#endif

        /* Initialize the runtime environment */
        status = uShellVcpRtEnvInit(vcp,
                                    (UShellOsal_s*) osal,
                                    (UShellHal_s*) hal);
        if (status != USHELL_VCP_NO_ERR)
        {
            break;
        }

    } while (0);

    return USHELL_VCP_NO_ERR;
}

/**
 * \brief DeInit vcp object
 * \param[in] vcp - vcp object to be deinitialized
 * \param[out] none
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellVcpDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the runtime environment */
        uShellVcpRtEnvDeInit(vcp);

        /* Clear the object */
        memset(vcp, 0, sizeof(UShellVcp_s));

    } while (0);

    return status;
}

/**
 * \brief Print string to the uShell vcp object
 * \param[in] vcp - uShell object to be printed
 * \param[in] str - string to be printed
 * \param[out] none
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintStr(UShellVcp_s* const vcp,
                                 const char* const str)
{
    /* Check input */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (str == NULL))
        {
            return USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Lock the print lock */
        uShellVcpPrintLock(vcp);

        /* Print string to the vcp object */
        status = uShellVcpPrintBytes(vcp,
                                     str,
                                     strlen(str));

        /* Unlock the print lock */
        uShellVcpPrintUnlock(vcp);

    } while (0);

    return status;
}

/**
 * \brief Print char to the uShell vcp object
 * \param vcp - uShell object to be printed
 * \param ch - char to be printed
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintChar(UShellVcp_s* const vcp,
                                  const char ch)
{
    /* Check input */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (ch == NULL))
        {
            return USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Lock the print lock */
        uShellVcpPrintLock(vcp);

        /* Print string to the vcp object */
        status = uShellVcpPrintBytes(vcp,
                                     ch,
                                     1U);

        /* Unlock the print lock */
        uShellVcpPrintUnlock(vcp);

    } while (0);

    return status;
}

/**
 * \brief Scan char from the vcp vcp object
 * \param[in] vcp - vcp object to be scanned
 * \param[in] ch - char to be scanned
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanChar(UShellVcp_s* const vcp,
                                 char* const ch)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* const osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    size_t receivedBytes = 0;

    /* Print string to the vcp object */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (ch == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellVcpReadLock(vcp);

        do
        {
            /* Get the stream buffer */
            osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuff);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuff == NULL))
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Write to the stream buffer */
            receivedBytes = UShellOsalStreamBuffReceiveBlocking(osal, streamBuff, ch, 1U);
            if (receivedBytes == 0)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

        } while (0);

        /* Unlock */
        uShellVcpReadUnlock(vcp);
    }

    while (0);

    return status;
}

/**
 * \brief Scan character from the uShell vcp object in non-blocking mode
 * \note: This function is non-blocking and will return immediately.
 * \note: This function will return USHELL_VCP_EMPTY_ERR if no character is available.
 * \param[in] vcp - uShell object to be scanned
 * \param[in] ch - character to be scanned
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanCharNonBlock(UShellVcp_s* const vcp,
                                         char* const ch)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* const osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    size_t receivedBytes = 0;

    /* Print string to the vcp object */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (ch == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellVcpReadLock(vcp);

        do
        {
            /* Get the stream buffer */
            osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuff);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuff == NULL))
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Write to the stream buffer */
            receivedBytes = UShellOsalStreamBuffReceive(osal, streamBuff, ch, 1U, 0U);
            if (receivedBytes == 0)
            {
                status = USHELL_VCP_EMPTY_ERR;
                break;
            }

        } while (0);

        /* Unlock */
        uShellVcpReadUnlock(vcp);
    }

    while (0);

    return status;
}

/**
 * \brief Scan string from the vcp vcp object
 * \param[in] vcp - vcp object to be scanned
 * \param[in] str - string to be scanned
 * \param[in] size - size of the string to be scanned
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanStr(UShellVcp_s* const vcp,
                                char* const str,
                                const size_t maxSize)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* const osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    char ch = 0U;
    size_t strSize = 0U;
    size_t receivedBytes = 0U;

    /* Print string to the vcp object */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (str == NULL) ||
            (maxSize == 0U))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellVcpReadLock(vcp);

        do
        {
            /* Get the stream buffer */
            osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuff);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuff == NULL))
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            do
            {
                /* Read from the stream buffer */
                receivedBytes = UShellOsalStreamBuffReceiveBlocking(osal,
                                                                    streamBuff,
                                                                    (void*) &ch,
                                                                    1U);
                if (receivedBytes == 0U)
                {
                    status = USHELL_VCP_INVALID_ARGS_ERR;
                    break;
                }

                /* Copy the char to the string */
                str [strSize] = ch;

                /* Increment the size */
                strSize++;

                /* Check the size of the string */
                if (strSize >= maxSize)
                {
                    status = USHELL_VCP_INVALID_ARGS_ERR;
                    break;
                }

                /* Check is the end of the string */
                if (ch == '\0')
                {
                    break;
                }

            } while (1);

        } while (0);

        /* Unlock */
        uShellVcpReadUnlock(vcp);

    } while (0);

    return status;
}

/**
 * \brief Check if the uShell vcp object is empty
 * \param vcp - uShell object to be checked
 * \param isEmpty - pointer to store the result indicating if the object is empty
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanIsEmpty(UShellVcp_s* const vcp,
                                    bool* const isEmpty)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* const osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    bool isEmptyBuff = false;

    /* Print string to the vcp object */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (isEmpty == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellVcpReadLock(vcp);

        do
        {
            /* Get the stream buffer */
            osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuff);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuff == NULL))
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Read from the stream buffer */
            osalStatus = UShellOsalStreamBuffIsEmpty(osal, streamBuff, &isEmptyBuff);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Set the result */
            *isEmpty = isEmptyBuff;

        } while (0);

        /* Unlock */
        uShellVcpReadUnlock(vcp);

    } while (0);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief UShell thread worker
 * \param[in] vcp - vcp object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellWorker(void* const arg)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variables */
    UShellVcp_s* const vcp = (UShellVcp_s*) arg;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellVcpEvent_e msgEvent = USHELL_VCP_EVENT_NONE;

    /* Main loop */
    while (1)
    {
        /* Wait for the event */
        status = uShellVcpEventWait(vcp, &msgEvent);
        if (status != USHELL_VCP_NO_ERR)
        {
            continue;
        }

        /* Process the msg */
        switch (msgEvent)
        {

            /* Process the received data */
            case USHELL_VCP_EVENT_RX_EVENT :

                /* Process the received data */
                status = uShellVcpReadFromPort(vcp);
                if (status != USHELL_VCP_NO_ERR)
                {
                    /* Send error msg */
                    uShellVcpEventSend(vcp, USHELL_VCP_EVENT_ERROR);
                    USHELL_VCP_ASSERT(0);
                }
                break;

            /* Process the transmitted data */
            case USHELL_VCP_EVENT_TX_EVENT :

                /* Process the transmitted data */
                status = uShellVcpWriteToPort(vcp);
                if (status != USHELL_VCP_NO_ERR)
                {
                    /* Send error msg */
                    uShellVcpEventSend(vcp, USHELL_VCP_EVENT_ERROR);
                    USHELL_VCP_ASSERT(0);
                }

                break;

            /* Process the error */
            case USHELL_VCP_EVENT_ERROR :

                /* Process the error */
                status = uShellVcpMsgTransferFlush(vcp);
                USHELL_VCP_ASSERT(status == USHELL_VCP_NO_ERR);

                /* Flush the rx stream buffers */
                uShellVcpStreamRxFlush(vcp);

                /* Flush the tx stream buffers */
                uShellVcpStreamTxFlush(vcp);

                /* Flush the buffer */
                uShellVcpIoBuffFlush(vcp);

                /* Set the rx mode in hal */
                uShellVcpDirectRxSet(vcp);

                break;

            /* Process the inspect event */
            case USHELL_VCP_EVENT_INSPECT :
            {
                status = uShellVcpInspect(vcp);
                if (status != USHELL_VCP_NO_ERR)
                {
                    /* Send error msg */
                    uShellVcpEventSend(vcp, USHELL_VCP_EVENT_ERROR);
                    USHELL_VCP_ASSERT(0);
                }
                break;
            }

            /* None is none */
            case USHELL_VCP_EVENT_NONE :
            default :
            {
                break;
            }
        }
    }
}

/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[in] cbType - callback type
 * \param[out] none
 */
static void uShellVcpRxReceivedCb(const void* const hal,
                                  const UShellHalCallback_e cbType)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL) ||
            (cbType != USHELL_HAL_CB_RX_RECEIVED))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        status = uShellVcpEventSend(ushell, USHELL_VCP_EVENT_RX_EVENT);
        if (status != USHELL_VCP_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
        }

    } while (0);
}

/**
 * \brief Callback for the transmitted data
 * \param[in] hal - hal object
 * \param[in] cbType - callback type
 * \param[out] none
 * \return none
 * \note This function is called when the data is transmitted to the serial port.
 */
static void uShellVcpTxCompleteCb(const void* const hal,
                                  const UShellHalCallback_e cbType)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL) ||
            (cbType != USHELL_HAL_CB_TX_COMPLETE))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        status = uShellVcpMsgTransferSend(ushell, USHELL_VCP_MSG_TX_COMPLETE);
        if (status != USHELL_VCP_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
        }

    } while (0);
}

/**
 * \brief Callback for the error
 * \param[in] hal - hal object
 * \param[in] cbType - callback type
 * \param[out] none
 * \return none
 * \note This function is called when the error occurs in the serial port.
 */
static void uShellVcpXferErrorCb(const void* const hal,
                                 const UShellHalCallback_e cbType)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL) ||
            (cbType != USHELL_HAL_CB_RX_TX_ERROR))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        status = uShellVcpMsgTransferSend(ushell, USHELL_VCP_MSG_TX_RX_ERR);
        if (status != USHELL_VCP_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
        }

    } while (0);
}

/**
 * \brief Initialize the runtime environment
 * \param vcp - vcp object
 * \param osal - osal object
 * \param hal - hal object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvInit(UShellVcp_s* const vcp,
                                         UShellOsal_s* const osal,
                                         UShellHal_s* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(osal != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Rt env init */
    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            return USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Initialize the runtime environment HAL */
        status = uShellVcpRtEnvHalInit(vcp, hal);
        if (status != USHELL_VCP_NO_ERR)
        {
            break;
        }

        /* Initialize the runtime environment OSAL */
        status = uShellVcpRtEnvOsalInit(vcp, osal);
        if (status != USHELL_VCP_NO_ERR)
        {
            break;
        }

    } while (0);

    /* Check status */
    if (status != USHELL_VCP_NO_ERR)
    {
        /* Deinitialize the runtime environment */
        uShellVcpRtEnvDeInit(vcp);
    }

    return status;
}

/**
 * \brief Deinitialize the runtime environment
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the runtime environment OSAL */
        uShellVcpRtEnvOsalDeInit(vcp);

        /* Deinitialize the runtime environment HAL */
        uShellVcpRtEnvHalDeInit(vcp);

    } while (0);

    return status;
}

/**
 * \brief Initialize the runtime environment HAL
 * \param vcp - vcp object
 * \param hal - hal object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalInit(UShellVcp_s* const vcp,
                                            UShellHal_s* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (hal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Save the hal object */
        vcp->hal = hal;

        /* Attach the parent object */
        halStatus = UShellHalParentSet(hal, vcp);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the received data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_RECEIVED,
                                      uShellVcpRxReceivedCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the transmitted data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_TX_COMPLETE,
                                      uShellVcpTxCompleteCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the error */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_TX_ERROR,
                                      uShellVcpXferErrorCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Open the hal */
        halStatus = UShellHalOpen(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Deinitialize the runtime environment HAL
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;

    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (vcp->hal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Detach callback for the received data */
        halStatus = UShellHalCbDetach(hal,
                                      USHELL_HAL_CB_RX_RECEIVED);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach callback for the transmitted data */
        halStatus = UShellHalCbDetach(hal,
                                      USHELL_HAL_CB_TX_COMPLETE);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach callback for the error */
        halStatus = UShellHalCbDetach(hal,
                                      USHELL_HAL_CB_RX_TX_ERROR);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach the parent object */
        halStatus = UShellHalParentSet(hal, NULL);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Close */
        halStatus = UShellHalClose(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Remove the hal object */
        vcp->hal = NULL;

    } while (0);

    return status;
}

/**
 * \brief Initialize the runtime environment OSAL
 * \param[in] vcp - vcp object
 * \param[in] osal - osal object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpRtEnvOsalInit(UShellVcp_s* const vcp,
                                             UShellOsal_s* const osal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(osal != NULL);

    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* thisOsal = (UShellOsal_s*) osal;

    /* Attach osal object */
    vcp->osal = thisOsal;

    /* Create osal objects */
    do
    {
        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, vcp);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create mutex for print */
        UShellOsalLockObjHandle_t lockObj = NULL;
        osalStatus = UShellOsalLockObjCreate(thisOsal, &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create mutex for read */
        UShellOsalLockObjHandle_t lockObjRead = NULL;
        osalStatus = UShellOsalLockObjCreate(thisOsal, &lockObjRead);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObjRead != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* Create the events */
        UShellOsalEventGroupHandle_t event = NULL;
        osalStatus = UShellEventGroupCreate(thisOsal, &event);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(event != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* Create the queue for transfer to read from VCP */
        UShellOsalQueueHandle_t queueRx = NULL;
        osalStatus = UShellOsalQueueCreate(thisOsal,
                                           sizeof(UShellVcpMsgTransfer_e),
                                           4U,
                                           &queueRx);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(queueRx != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* Create stream buffer for transfer to write in VCP */
        UShellOsalStreamBuffHandle_t streamBufferTx = NULL;
        osalStatus = UShellOsalStreamBuffCreate(thisOsal,
                                                USHELL_VCP_BUFFER_SIZE,
                                                1U,
                                                &streamBufferTx);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBufferTx == NULL))
        {
            break;
        }

        /* Create stream buffer for transfer to read from VCP */
        UShellOsalStreamBuffHandle_t streamBufferRx = NULL;
        osalStatus = UShellOsalStreamBuffCreate(thisOsal,
                                                USHELL_VCP_BUFFER_SIZE,
                                                1U,
                                                &streamBufferRx);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBufferRx == NULL))
        {
            break;
        }

        /* Create timer for inspect the state*/
        UShellOsalTimerHandle_t timer = NULL;
        UShellOsalTimerCfg_s timerCfg =
            {
                .name = USHELL_VCP_TIMER_NAME,
                .timerParam = vcp,
                .periodMs = USHELL_VCP_TIMER_INSPECT_PERIOD_MS,
                .autoReloadState = true,
                .timerExpiredCb = uShellVcpTimerExpiredCb};

        osalStatus = UShellOsalTimerCreate(thisOsal, &timer, timerCfg);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(timer != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* Timer start */
        osalStatus = UShellOsalTimerStart(thisOsal, timer);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create task */
        UShellOsalThreadHandle_t thread = NULL;
        UShellOsalThreadCfg_s threadCfg =
            {
                .name = USHELL_VCP_THREAD_NAME,
                .stackSize = USHELL_VCP_THREAD_STACK_SIZE,
                .threadParam = vcp,
                .threadPriority = USHELL_VCP_THREAD_PRIORITY,
                .threadWorker = uShellWorker};
        osalStatus = UShellOsalThreadCreate(thisOsal, &thread, threadCfg);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(thread != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

    } while (0);

    if (osalStatus != USHELL_OSAL_NO_ERR)
    {
        uShellVcpRtEnvOsalDeInit(vcp);
        return USHELL_VCP_PORT_ERR;
    }

    return USHELL_VCP_NO_ERR;
}

/**
 * \brief Deinitialize the runtime environment OSAL
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvOsalDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(vcp->osal != NULL);

    /* Status obj */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    (void) osalStatus;
    UShellOsal_s* thisOsal = (UShellOsal_s*) vcp->osal;

    do
    {
        /* Destroy osal objects */
        /* : Delete the thread */
        do
        {
            /* : : Find the thread handle */
            UShellOsalThreadHandle_t thread = NULL;
            osalStatus = UShellOsalThreadHandleGet(thisOsal, 0U, &thread);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (thread == NULL))
            {
                break;
            }

            /* : : Delete the thread */
            osalStatus = UShellOsalThreadDelete(thisOsal, thread);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* : Delete the lock object */
        do
        {
            /* : : Find the lock object handle */
            UShellOsalLockObjHandle_t lockObj = NULL;
            osalStatus = UShellOsalLockObjHandleGet(thisOsal, 0U, &lockObj);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (lockObj == NULL))
            {
                break;
            }

            /* : : Delete the lock object */
            osalStatus = UShellOsalLockObjDelete(thisOsal, lockObj);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        } while (0);

        /* : Delete the lock object */
        do
        {
            /* : : Find the lock object handle */
            UShellOsalLockObjHandle_t lockObj = NULL;
            osalStatus = UShellOsalLockObjHandleGet(thisOsal, 1U, &lockObj);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (lockObj == NULL))
            {
                break;
            }

            /* : : Delete the lock object */
            osalStatus = UShellOsalLockObjDelete(thisOsal, lockObj);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* : Delete the queue for event msg */
        do
        {
            /* : : Find the event group handle */
            UShellOsalQueueHandle_t queueEvent = NULL;
            osalStatus = UShellOsalQueueHandleGet(thisOsal, 0U, &queueEvent);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (queueEvent == NULL))
            {
                break;
            }

            /* : : Delete the event group */
            osalStatus = UShellOsalQueueDelete(thisOsal, queueEvent);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* : Delete the queue for transfer */
        do
        {
            /* : : Find the event group handle */
            UShellOsalQueueHandle_t queueEvent = NULL;
            osalStatus = UShellOsalQueueHandleGet(thisOsal, 1U, &queueEvent);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (queueEvent == NULL))
            {
                break;
            }

            /* : : Delete the event group */
            osalStatus = UShellOsalQueueDelete(thisOsal, queueEvent);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, NULL);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        /* Flush */
        vcp->osal = NULL;

    } while (0);

    return status;
}

/**
 * \brief Lock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpPrintLock(const UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                0U,
                                                                &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Lock */
        osalStatus = UShellOsalLock(osal, lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * \brief Unlock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpPrintUnlock(const UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                0U,
                                                                &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Unlock */
        osalStatus = UShellOsalUnlock(osal, lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * \brief Lock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpReadLock(const UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                1U,
                                                                &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Lock */
        osalStatus = UShellOsalLock(osal, lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * \brief Unlock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpReadUnlock(const UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                1U,
                                                                &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Unlock */
        osalStatus = UShellOsalUnlock(osal, lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * \brief Send message event to the vcp object
 * \param vcp - vcp object to be sent
 * \param msgEvent - message event to be sent
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpEventSend(UShellVcp_s* const vcp,
                                         UShellVcpEvent_e msgEvent)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalEventGroupHandle_t event = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    bool eventExist = false;

    /* Send the message to the queue */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Check we have these event */
        if (msgEvent & USHELL_VCP_EVENT_NONE)
        {
            eventExist = true;
        }
        if (msgEvent & USHELL_VCP_EVENT_RX_EVENT)
        {
            eventExist = true;
        }
        if (msgEvent & USHELL_VCP_EVENT_TX_EVENT)
        {
            eventExist = true;
        }
        if (msgEvent & USHELL_VCP_EVENT_ERROR)
        {
            eventExist = true;
        }
        if (msgEvent & USHELL_VCP_EVENT_INSPECT)
        {
            eventExist = true;
        }

        if (!eventExist)
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalEventGroupHandleGet(osal, 0U, &event);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (event == NULL))
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Send the message to the queue */
        osalStatus = UShellEventGroupSetBits(osal, event, msgEvent);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Wait for message event from the vcp object (blocked )
 * \param vcp - vcp object
 * \param msgEvent - message event to be waited
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpEventWait(UShellVcp_s* const vcp,
                                         UShellVcpEvent_e* const msgEvent)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalEventGroupHandle_t eventHandle = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsalEventGroupBits_e receivedBit = 0U;
    UShellOsalEventGroupBits_e bitsToWait = USHELL_VCP_EVENT_ERROR |
                                            USHELL_VCP_EVENT_RX_EVENT |
                                            USHELL_VCP_EVENT_TX_EVENT |
                                            USHELL_VCP_EVENT_INSPECT;

    /* Send the message to the queue */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (msgEvent == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalEventGroupHandleGet(osal, 0U, &eventHandle);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (eventHandle == NULL))
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Get the event */
        osalStatus = UShellEventGroupBitsWait(osal,
                                              eventHandle,
                                              bitsToWait,
                                              &receivedBit,
                                              true,
                                              false);

        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        if (receivedBit & USHELL_VCP_EVENT_RX_EVENT)
        {
            *msgEvent = USHELL_VCP_EVENT_RX_EVENT;
        }
        else if (receivedBit & USHELL_VCP_EVENT_TX_EVENT)
        {
            *msgEvent = USHELL_VCP_EVENT_TX_EVENT;
        }
        else if (receivedBit & USHELL_VCP_EVENT_ERROR)
        {
            *msgEvent = USHELL_VCP_EVENT_ERROR;
        }
        else if (receivedBit & USHELL_VCP_EVENT_INSPECT)
        {
            *msgEvent = USHELL_VCP_EVENT_INSPECT;
        }
        else
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

    }

    while (0);

    return status;
}

/**
 * \brief Send message transfer to the vcp object
 * \param vcp - vcp object to be sent
 * \param msgTransfer - message transfer to be sent
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgTransferSend(UShellVcp_s* const vcp,
                                               UShellVcpMsgTransfer_e msgTransfer)
{
    {
        /* Check input parameters */
        USHELL_VCP_ASSERT(vcp != NULL);

        /* Local variable */
        UShellVcpErr_e status = USHELL_VCP_NO_ERR;
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
        UShellOsalQueueHandle_t queue = NULL;
        UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;

        /* Send the message to the queue */
        do
        {
            /* Check input parameters */
            if ((vcp == NULL) ||
                (osal == NULL))
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Check msg event */
            if (!(msgTransfer == USHELL_VCP_MSG_TX_NONE) &&
                !(msgTransfer == USHELL_VCP_MSG_TX_COMPLETE) &&
                !(msgTransfer == USHELL_VCP_MSG_TX_RX_ERR))

            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Get the queue handle */
            osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (queue == NULL))
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Send the message to the queue */
            osalStatus = UShellOsalQueueItemPut(osal, queue, &msgTransfer);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

        } while (0);

        return status;
    }
}

/**
 * \brief Flush message transfer from the vcp object
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgTransferFlush(UShellVcp_s* const vcp)
{
    {
        /* Check input parameters */
        USHELL_VCP_ASSERT(vcp != NULL);

        /* Local variable */
        UShellVcpErr_e status = USHELL_VCP_NO_ERR;
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
        UShellOsalQueueHandle_t queue = NULL;
        UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;

        /* Send the message to the queue */
        do
        {
            /* Check input parameters */
            if ((vcp == NULL) ||
                (osal == NULL))
            {
                status = USHELL_VCP_INVALID_ARGS_ERR;
                break;
            }

            /* Get the queue handle */
            osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (queue == NULL))
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Flush the queue */
            osalStatus = UShellOsalQueueReset(osal, queue);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

        } while (0);

        return status;
    }
}

/**
 * \brief Wait for message transfer from the vcp object (blocked )
 * \param vcp - vcp object
 * \param msgTransfer - message transfer to be waited
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgTransferPend(UShellVcp_s* const vcp,
                                               UShellVcpMsgTransfer_e* const msgTransfer,
                                               const uint32_t timeout)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalQueueHandle_t queue = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellVcpMsgTransfer_e msgTxLocal = USHELL_VCP_EVENT_NONE;

    /* Send the message to the queue */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (msgTransfer == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Get the message from the queue */
        osalStatus = ushellOsalQueueItemPend(osal, queue, &msgTxLocal, timeout);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            *msgTransfer = USHELL_VCP_MSG_TX_RX_ERR;
            break;
        }

        /* Check msg event */
        if (!(msgTxLocal == USHELL_VCP_MSG_TX_NONE) &&
            !(msgTxLocal == USHELL_VCP_MSG_TX_COMPLETE) &&
            !(msgTxLocal == USHELL_VCP_MSG_TX_RX_ERR))

        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            *msgTransfer = USHELL_VCP_MSG_TX_RX_ERR;
            break;
        }

        /* Set the message event */
        *msgTransfer = msgTxLocal;
    }

    while (0);

    return status;
}

/**
 * \brief Read from the port
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpReadFromPort(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    size_t sendByte = 0U;

    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellOsalStreamBuffHandle_t streamBuffer = NULL;

    /* Read from the port */

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the stream buffer handle */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuffer);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuffer == NULL))
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Process send */
        do
        {

            /* Read from hal */
            halStatus = UShellHalRead(hal, vcp->io.buffer, USHELL_VCP_BUFFER_SIZE, &vcp->io.ind);
            if (halStatus != USHELL_HAL_NO_ERR)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Check we have to send */
            if (vcp->io.ind <= 0U)
            {
                break;
            }

            /* Send to stream buffer */
            sendByte = UShellOsalStreamBuffSendBlocking(osal,
                                                        streamBuffer,
                                                        vcp->io.buffer,
                                                        vcp->io.ind);

            if (sendByte != vcp->io.ind)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

        } while (1);

        /* Send the message to the queue */
    } while (0);

    return status;
}

/**
 * \brief Write to the port
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpWriteToPort(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellOsalStreamBuffHandle_t streamBuffer = NULL;
    UShellVcpMsgTransfer_e msgTxLocal = USHELL_VCP_MSG_TX_NONE;

    /* Read from the port */

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the stream buffer handle */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 0U, &streamBuffer);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuffer == NULL))
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Open tx mode */
        uShellVcpDirectTxSet(vcp);

        /* Process send */
        do
        {
            /* Flush the buffer */
            uShellVcpIoBuffFlush(vcp);

            /* Read from stream buffer */
            vcp->io.ind = UShellOsalStreamBuffReceive(osal,
                                                      streamBuffer,
                                                      vcp->io.buffer,
                                                      USHELL_VCP_BUFFER_SIZE,
                                                      0U);

            /* Check we have to send */
            if (vcp->io.ind <= 0U)
            {
                break;
            }

            /* Flush the tx queue */
            status = uShellVcpMsgTransferFlush(vcp);
            if (status != USHELL_VCP_NO_ERR)
            {
                break;
            }

            /* Send to hal */
            halStatus = UShellHalWrite(hal, vcp->io.buffer, vcp->io.ind);
            if (halStatus != USHELL_HAL_NO_ERR)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Wait for tx complete */
            status = uShellVcpMsgTransferPend(vcp, &msgTxLocal, USHELL_VCP_TX_TIMEOUT_MS);
            if ((msgTxLocal != USHELL_VCP_MSG_TX_COMPLETE) ||
                (status != USHELL_VCP_NO_ERR))
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

        } while (1);

        /* Set rx mode */
        uShellVcpDirectRxSet(vcp);

    } while (0);

    return status;
}

/**
 * \brief Inspect the vcp object
 * \param vcp - vcp object to be inspected
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpInspect(UShellVcp_s* const vcp)
{
    /* Check input */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

    bool isRxDataAvailable = false;
    UShellOsalStreamBuffHandle_t streamBuffTx = NULL;
    bool isTxStreamEmpty = false;

    do
    {

        /* Check we have data in hal */
        do
        {

            /* Get handle of rx stream */
            halStatus = UShellHalIsReadDataAvailable(hal, &isRxDataAvailable);
            USHELL_VCP_ASSERT(halStatus == USHELL_HAL_NO_ERR);
            if ((halStatus != USHELL_HAL_NO_ERR))
            {
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Check we have data in hal */
            if (isRxDataAvailable == false)
            {
                break;
            }

            /* Set the rx event */
            status = uShellVcpEventSend(vcp, USHELL_VCP_EVENT_RX_EVENT);
            if (status != USHELL_VCP_NO_ERR)
            {
                USHELL_VCP_ASSERT(0);
                break;
            }

        } while (0);

        /* Check we have data in the stream buffer tx */
        do
        {
            /* Get handle of tx stream */
            osalStatus = UShellOsalStreamBuffHandleGet((UShellOsal_s*) vcp->osal,
                                                       0U,
                                                       &streamBuffTx);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuffTx == NULL))
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Check we have data in the stream buffer tx */
            osalStatus = UShellOsalStreamBuffIsEmpty((UShellOsal_s*) vcp->osal,
                                                     streamBuffTx,
                                                     &isTxStreamEmpty);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Check we have data in the stream buffer tx */
            if (isTxStreamEmpty == true)
            {
                break;
            }

            /* Set the tx event */
            status = uShellVcpEventSend(vcp, USHELL_VCP_EVENT_TX_EVENT);
            if (status != USHELL_VCP_NO_ERR)
            {
                USHELL_VCP_ASSERT(0);
                break;
            }

        } while (0);

    } while (0);

    return status;
}

/**
 * \brief Flush the buffer of the vcp object
 * \param vcp - vcp object to be flushed
 * \return none
 */
static inline void uShellVcpIoBuffFlush(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Flush the buffer */
    do
    {
        /* Check input parameters */
        if (vcp == NULL)
        {
            break;
        }

        /* Flush the buffer */
        memset(&vcp->io, 0, sizeof(UShellVcpIo_s));

    } while (0);
}

/**
 * \brief Flush the rx buffer of the vcp object
 * \param vcp - vcp object to be flushed
 */
static inline void uShellVcpStreamRxFlush(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuffer = NULL;

    /* Flush the buffer */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            break;
        }

        /* Get the stream buffer handle */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuffer);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuffer == NULL))
        {
            break;
        }

        /* Flush the stream buffer */
        osalStatus = UShellOsalStreamBuffReset(osal, streamBuffer);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

    } while (0);
}

/**
 * \brief Flush the tx buffer of the vcp object
 * \param vcp - vcp object to be flushed
 */
static inline void uShellVcpStreamTxFlush(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuffer = NULL;

    /* Flush the buffer */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            break;
        }

        /* Get the stream buffer handle */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 0U, &streamBuffer);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuffer == NULL))
        {
            break;
        }

        /* Flush the stream buffer */
        osalStatus = UShellOsalStreamBuffReset(osal, streamBuffer);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

    } while (0);
}

/**
 * \brief Set tx direction of the vcp object
 * \param[in] vcp - vcp object to be set
 */
static inline void uShellVcpDirectTxSet(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (hal == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set tx direction */
        halStatus = UShellHalSetTxMode(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief Set rx direction of the vcp  object
 * \param[in] vcp - vcp object to be set
 */
static inline void uShellVcpDirectRxSet(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (hal == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set tx direction */
        halStatus = UShellHalSetRxMode(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief Callback function for the timer expiration
 * \param timerParam - parameter for the timer expiration callback
 * \return none
 */
static void uShellVcpTimerExpiredCb(void* const timerParam)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellVcp_s* const ushell = (UShellVcp_s*) timerParam;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if (ushell == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send the message to the queue */
        status = uShellVcpEventSend(ushell, USHELL_VCP_EVENT_INSPECT);
        if (status != USHELL_VCP_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
        }

    } while (0);
}

/**
 * \brief Print a block of data to the uShell vcp object.
 * \param[in] vcp   - uShell vcp object.
 * \param[in] data  - pointer to the data to be printed.
 * \param[in] len   - length of the data to send.
 * \return UShellVcpErr_e - error code. non-zero means an error occurred.
 */
static UShellVcpErr_e uShellVcpPrintBytes(UShellVcp_s* const vcp,
                                          const void* data,
                                          size_t len)
{
    /* Check input */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(data != NULL);
    USHELL_VCP_ASSERT(len > 0U);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* const osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    const uint8_t* ptr = (const uint8_t*) data;
    size_t totalSent = 0U;
    size_t remaining = len;
    size_t chunkSize = 0U;
    size_t chunkWriteCount = 0U;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (data == NULL) ||
            (len == 0U))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the stream buffer for TX (index 0) */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 0U, &streamBuff);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuff == NULL))
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Lock */
        uShellVcpPrintLock(vcp);

        /* Loop to send the data in chunks */
        while (remaining > 0U)
        {
            chunkSize = (remaining < USHELL_VCP_BUFFER_SIZE)
                            ? remaining
                            : USHELL_VCP_BUFFER_SIZE;
            chunkWriteCount = UShellOsalStreamBuffSendBlocking(osal,
                                                               streamBuff,
                                                               (void*) &ptr [totalSent],
                                                               chunkSize);
            if (chunkWriteCount == 0U)
            {
                status = USHELL_VCP_PORT_ERR;
                break;
            }
            totalSent += chunkWriteCount;
            remaining -= chunkWriteCount;
            status = uShellVcpEventSend(vcp, USHELL_VCP_EVENT_TX_EVENT);
            if (status != USHELL_VCP_NO_ERR)
            {
                break;
            }
        }

        /* Unlock */
        uShellVcpPrintUnlock(vcp);

    } while (0);

    return status;
}

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)
/**
 * \brief Reads data from the virtual COM port.
 *
 * This function is a weak override for the standard _read syscall.
 *
 * \param file Unused file descriptor.
 * \param ptr Pointer to where read bytes should be stored.
 * \param len Maximum number of bytes to read.
 * \return The number of bytes actually read.
 */
int __attribute__((weak)) _read(int file, char* ptr, int len)
{

    /* Local variable */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellVcp_s* vcp = vcpStdIO;
    UShellOsal_s* const osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    int numberOfBytes = 0U;
    size_t readBytesQueue = 0U;

    /* Print string to the vcp object */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (len == 0U))
        {
            numberOfBytes = 0;
            break;
        }

        /* Lock */
        uShellVcpReadLock(vcp);

        do
        {
            /* Get the stream buffer */
            osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuff);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuff == NULL))
            {
                numberOfBytes = 0;
                break;
            }

            do
            {
                /* Read from the  stream buffer */
                readBytesQueue = UShellOsalStreamBuffReceiveBlocking(osal,
                                                                     streamBuff,
                                                                     (void*) &ptr [numberOfBytes++],
                                                                     1U);
                if (readBytesQueue == 0)
                {
                    numberOfBytes = 0;
                    break;
                }

                /* Check the end */
                if (numberOfBytes == len)
                {
                    break;
                }

            } while (1);

        } while (0);

        /* Unlock */
        uShellVcpReadUnlock(vcp);

    } while (0);

    return numberOfBytes;
}

/**
 * \brief Writes data to the virtual COM port.
 *
 * This function is a weak override for the standard _write syscall.
 *
 * \param file Unused file descriptor.
 * \param ptr Pointer to the data to be written.
 * \param len Number of bytes to write.
 * \return The number of bytes actually written.
 */
int __attribute__((weak)) _write(int file, char* ptr, int len)
{
    /* Local variable */
    UShellVcp_s* vcp = vcpStdIO;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    int bytesWritten = 0;

    do
    {
        /* Validate input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL) ||
            (len <= 0))
        {
            return 0;
        }

        /* Lock */
        uShellVcpPrintLock(vcp);

        status = uShellVcpPrintBytes(vcp, ptr, len);
        if (status != USHELL_VCP_NO_ERR)
        {
            bytesWritten = 0;
            break;
        }

        /* Send the message to the queue */
        uShellVcpPrintUnlock(vcp);

        bytesWritten = len;

    } while (0);

    return bytesWritten;
}

#endif