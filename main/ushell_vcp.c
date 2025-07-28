/**
* \file         ushell_vcp.c
* \brief        The file contains the implementation of the UShell vcp module.
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

} UShellVcpMsgXfer_e;

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShell thread worker
 * \param[in] arg - argument for the thread(aka vcp object)
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
 * \brief Lock the vcp object
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpLock(const UShellVcp_s* const vcp);

/**
 * \brief Unlock the vcp object
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpUnlock(const UShellVcp_s* const vcp);

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
static UShellVcpErr_e uShellVcpMsgXferSend(UShellVcp_s* const vcp,
                                           UShellVcpMsgXfer_e msgTransfer);

/**
 * \brief Flush message transfer from the vcp object
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgXferFlush(UShellVcp_s* const vcp);

/**
 * \brief Wait for message transfer from the vcp object (blocked )
 * \param vcp - vcp object
 * \param msgTransfer - message transfer to be waited
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgXferPend(UShellVcp_s* const vcp,
                                           UShellVcpMsgXfer_e* const msgTransfer,
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
 * \brief UShell VCP socket read callback function
 * \param[in] socket - pointer to the socket object
 * \param[in] type - type of the callback (read or write)
 * \param[in] parent - parent object
 * \return none
 */
static void uShellVcpSocketReadCb(void* socket,
                                  UShellSocketCbType_e type,
                                  void* parent);

/**
 * \brief UShell VCP socket write callback function
 * \param[in] socket - pointer to the socket object
 * \param[in] type - type of the callback (read or write)
 * \param[in] parent - parent object
 * \return none
 */
static void uShellVcpSocketWriteCb(void* socket,
                                   UShellSocketCbType_e type,
                                   void* parent);

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)
/**
 * \brief Used for stdio redirection
 */
static UShellSocket_s* uShellVcpStdSocketRead = NULL;
static UShellSocket_s* uShellVcpStdSocketWrite = NULL;
#endif

const static UShellSocketCbTable_s vcpSocketCbTable = {
    .readCb = uShellVcpSocketReadCb,
    .writeCb = uShellVcpSocketWriteCb,
};

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
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Initialize the runtime environment */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Flush the objects */
        memset(vcp, 0, sizeof(UShellVcp_s));

        /* Set the parent object */
        vcp->parent = parent;
        vcp->name = name;

        /* Initialize the runtime environment */
        status = uShellVcpRtEnvInit(vcp,
                                    (UShellOsal_s*) osal,
                                    (UShellHal_s*) hal);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* Runtime environment initialization failed */
            USHELL_VCP_ASSERT(0);
            break;
        }

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)
        /* This a temporary solution to redirect stdio to the vcp object */
        /* I now is not the best solution, but it works for now */

        /* Set buffer for stdio redirection */
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);

        /* For scanf() */
        UShellVcpSessionParam_s uShellVcpStdSocketParam =
            {
                .owner = vcp,
                .type = USHELL_VCP_DIR_READ,
            };
        status = UShellVcpSessionOpen(vcp,
                                      uShellVcpStdSocketParam,
                                      &uShellVcpStdSocketRead);
        if ((status != USHELL_VCP_NO_ERR) ||
            (uShellVcpStdSocketRead == NULL) ||
            (uShellVcpStdSocketRead->stream == NULL))
        {
            /* Session open failed */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* For printf() */
        uShellVcpStdSocketParam.type = USHELL_VCP_DIR_WRITE;
        status = UShellVcpSessionOpen(vcp,
                                      uShellVcpStdSocketParam,
                                      &uShellVcpStdSocketWrite);
        if ((status != USHELL_VCP_NO_ERR) ||
            (uShellVcpStdSocketWrite == NULL) ||
            (uShellVcpStdSocketWrite->stream == NULL))
        {
            /* Session open failed */
            USHELL_VCP_ASSERT(0);
            break;
        }
#endif

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
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
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
 * \brief Open a session for the uShell vcp object
 * \param[in] vcp - uShell object to be opened
 * \param[in] param - session parameter object
 * \param[in] socket - socket to read/write data
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpSessionOpen(UShellVcp_s* const vcp,
                                    const UShellVcpSessionParam_s param,
                                    UShellSocket_s** const socket)
{
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalStreamBuffHandle_t streamBuff = NULL;
    UShellSocketCfg_s socketCfg = {0};
    size_t freeSessionSlot = 0;
    bool slotFind = false;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (socket == NULL) ||
            (param.owner == NULL) ||
            (osal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (!(param.type == USHELL_VCP_DIR_READ) &&
            !(param.type == USHELL_VCP_DIR_WRITE))
        {
            /* Type is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* lock */
        uShellVcpLock(vcp);

        /* Thread safe code */
        do
        {
            /* Find free session slot */
            for (freeSessionSlot = 0; freeSessionSlot < USHELL_VCP_ACTIVE_SESSION_MAX; freeSessionSlot++)
            {
                if ((vcp->session [freeSessionSlot].param.owner == NULL) &&
                    (vcp->session [freeSessionSlot].used == false))
                {
                    slotFind = true;
                    break;
                }
            }

            /* Check we find slot */
            if (slotFind == false)
            {
                /* No free session slot */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_SESSION_SLOT_ERR;
                break;
            }

            /* Save param to the session slot */
            vcp->session [freeSessionSlot].param = param;

            /* Create stream buffer handle */
            osalStatus = UShellOsalStreamBuffCreate(osal,
                                                    USHELL_VCP_BUFFER_SIZE,
                                                    1U,
                                                    &streamBuff);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                /* Stream buffer creation failed */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Save the stream buffer handle to the session slot */
            vcp->session [freeSessionSlot].stream = streamBuff;

            /* Prepare socket cfg */
            socketCfg.size = USHELL_VCP_BUFFER_SIZE;
            socketCfg.type = (param.type == USHELL_VCP_DIR_READ)
                                 ? USHELL_SOCKET_TYPE_READ
                                 : USHELL_SOCKET_TYPE_WRITE;

            /* Init the socket object */
            socketStatus = UShellSocketInit(&vcp->session [freeSessionSlot].socket,
                                            osal,
                                            streamBuff,
                                            socketCfg,
                                            &vcpSocketCbTable,
                                            vcp,
                                            NULL);
            if (socketStatus != USHELL_SOCKET_NO_ERR)
            {
                /* Socket initialization failed */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Return the socket object */
            *socket = &vcp->session [freeSessionSlot].socket;

            /* Set the session slot as used */
            vcp->session [freeSessionSlot].used = true;

        } while (0);

        /* unlock */
        uShellVcpUnlock(vcp);

    } while (0);

    if (status != USHELL_VCP_NO_ERR)
    {
        /* Deinitialize the session slot */
        if (vcp->session [freeSessionSlot].stream != NULL)
        {
            osalStatus = UShellOsalStreamBuffDelete(osal,
                                                    vcp->session [freeSessionSlot].stream);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                /* Stream buffer deletion failed */
                USHELL_VCP_ASSERT(0);
            }
        }

        /* Deinitialize the socket object */
        socketStatus = UShellSocketDeInit(&vcp->session [freeSessionSlot].socket);
        USHELL_VCP_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);

        /* Clear the session slot */
        memset(&vcp->session [freeSessionSlot], 0, sizeof(UShellVcpSession_s));
    }

    return status;
}

/**
 * @brief Close a session for the uShell vcp object
 * @param[in] vcp - uShell object to be closed
 * @param[in] param - session parameter object
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpSessionClose(UShellVcp_s* const vcp,
                                     const UShellVcpSessionParam_s param)
{
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    size_t sessionSlot = 0;
    bool slotFind = false;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (param.owner == NULL) ||
            (osal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock the vcp object */
        uShellVcpLock(vcp);

        /* Thread safe code */
        do
        {
            /* Find session slot */
            for (sessionSlot = 0; sessionSlot < USHELL_VCP_ACTIVE_SESSION_MAX; sessionSlot++)
            {
                if ((vcp->session [sessionSlot].param.owner == param.owner) &&
                    (vcp->session [sessionSlot].used == true))
                {
                    slotFind = true;
                    break;
                }
            }

            /* Check we find slot */
            if (slotFind == false)
            {
                /* No free session slot */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_SESSION_SLOT_ERR;
                break;
            }

            /* Deinitialize the socket object */
            socketStatus = UShellSocketDeInit(&vcp->session [sessionSlot].socket);
            if (socketStatus != USHELL_SOCKET_NO_ERR)
            {
                /* Socket deinitialization failed */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Remove stream buffer handle */
            osalStatus = UShellOsalStreamBuffDelete(osal,
                                                    vcp->session [sessionSlot].stream);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                /* Stream buffer deletion failed */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Clear the session slot */
            memset(&vcp->session [sessionSlot], 0, sizeof(UShellVcpSession_s));
            vcp->session [sessionSlot].used = false;

        } while (0);

        /* Unlock the vcp object */
        uShellVcpUnlock(vcp);

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
                status = uShellVcpMsgXferFlush(vcp);
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
    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Process the received callback */
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL) ||
            (cbType != USHELL_HAL_CB_RX_RECEIVED))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        status = uShellVcpEventSend(ushell, USHELL_VCP_EVENT_RX_EVENT);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* Send error msg */
            USHELL_VCP_ASSERT(0);
            break;
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
    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Process the transmitted callback */
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL) ||
            (cbType != USHELL_HAL_CB_TX_COMPLETE))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        status = uShellVcpMsgXferSend(ushell, USHELL_VCP_MSG_TX_COMPLETE);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* Send error msg */
            USHELL_VCP_ASSERT(0);
            break;
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
    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Process the error callback */
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL) ||
            (cbType != USHELL_HAL_CB_RX_TX_ERROR))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        status = uShellVcpMsgXferSend(ushell, USHELL_VCP_MSG_TX_RX_ERR);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* Send error msg */
            USHELL_VCP_ASSERT(0);
            break;
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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Initialize the runtime environment HAL */
        status = uShellVcpRtEnvHalInit(vcp, hal);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* HAL initialization failed */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Initialize the runtime environment OSAL */
        status = uShellVcpRtEnvOsalInit(vcp, osal);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* OSAL initialization failed */
            USHELL_VCP_ASSERT(0);
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
    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Deinit the runtime environment */
    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
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
    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    /* Init the runtime environment HAL */
    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (hal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Save the hal object */
        vcp->hal = hal;

        /* Attach the parent object */
        halStatus = UShellHalParentSet(hal, vcp);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Parent object is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the received data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_RECEIVED,
                                      uShellVcpRxReceivedCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Callback is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the transmitted data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_TX_COMPLETE,
                                      uShellVcpTxCompleteCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Callback is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the error */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_TX_ERROR,
                                      uShellVcpXferErrorCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Callback is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Open the hal */
        halStatus = UShellHalOpen(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Open failed */
            USHELL_VCP_ASSERT(0);
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
    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;

    /* Deinit the runtime environment HAL */
    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (vcp->hal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Detach callback for the received data */
        halStatus = UShellHalCbDetach(hal,
                                      USHELL_HAL_CB_RX_RECEIVED);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Callback is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach callback for the transmitted data */
        halStatus = UShellHalCbDetach(hal,
                                      USHELL_HAL_CB_TX_COMPLETE);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Callback is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach callback for the error */
        halStatus = UShellHalCbDetach(hal,
                                      USHELL_HAL_CB_RX_TX_ERROR);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Callback is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach the parent object */
        halStatus = UShellHalParentSet(hal, NULL);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Parent object is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Close */
        halStatus = UShellHalClose(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Close failed */
            USHELL_VCP_ASSERT(0);
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
    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* thisOsal = (UShellOsal_s*) osal;

    /* Attach osal object */
    vcp->osal = thisOsal;

    /* Create osal objects */
    do
    {
        /* Check input */
        if ((vcp == NULL) ||
            (thisOsal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, vcp);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Parent object is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* : Create mutex for vcp */
        UShellOsalLockObjHandle_t lockObj = NULL;
        osalStatus = UShellOsalLockObjCreate(thisOsal, &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Mutex is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Create the events */
        UShellOsalEventGroupHandle_t event = NULL;
        osalStatus = UShellEventGroupCreate(thisOsal, &event);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (event == NULL))
        {
            /* Event is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Create the queue for transfer to read from VCP */
        UShellOsalQueueHandle_t queueRx = NULL;
        osalStatus = UShellOsalQueueCreate(thisOsal,
                                           sizeof(UShellVcpMsgXfer_e),
                                           4U,
                                           &queueRx);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queueRx == NULL))
        {
            /* Queue is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)

        /* Create stream buffer for transfer to write in VCP */
        UShellOsalStreamBuffHandle_t streamBufferTx = NULL;
        osalStatus = UShellOsalStreamBuffCreate(thisOsal,
                                                USHELL_VCP_BUFFER_SIZE,
                                                1U,
                                                &streamBufferTx);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBufferTx == NULL))
        {
            /* Stream buffer is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
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
            /* Stream buffer is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }
#endif

        /* Create timer for inspect the state*/
        UShellOsalTimerHandle_t timer = NULL;
        UShellOsalTimerCfg_s timerCfg =
            {
                .name = USHELL_VCP_TIMER_INSPECT_NAME,
                .timerParam = vcp,
                .periodMs = USHELL_VCP_TIMER_INSPECT_PERIOD_MS,
                .autoReloadState = true,
                .timerExpiredCb = uShellVcpTimerExpiredCb};

        osalStatus = UShellOsalTimerCreate(thisOsal, &timer, timerCfg);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (timer == NULL))
        {
            /* Timer is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Timer start */
        osalStatus = UShellOsalTimerStart(thisOsal, timer);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Timer start failed */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* : Create task */
        UShellOsalThreadHandle_t thread = NULL;
        UShellOsalThreadCfg_s threadCfg =
            {
                .name = USHELL_VCP_THREAD_NAME,
                .stackSize = USHELL_VCP_THREAD_STACK_SIZE_BYTE,
                .threadParam = vcp,
                .threadPriority = USHELL_VCP_THREAD_PRIORITY,
                .threadWorker = uShellWorker};
        osalStatus = UShellOsalThreadCreate(thisOsal, &thread, threadCfg);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (thread == NULL))
        {
            /* Thread is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

    } while (0);

    if (osalStatus != USHELL_OSAL_NO_ERR)
    {
        uShellVcpRtEnvOsalDeInit(vcp);
        return USHELL_VCP_PORT_ERR;
    }

    return status;
}

/**
 * \brief Deinitialize the runtime environment OSAL
 * \param vcp - vcp object
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvOsalDeInit(UShellVcp_s* const vcp)
{
    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* thisOsal = (UShellOsal_s*) vcp->osal;

    /* Deinit the runtime environment OSAL */
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

#if (USHELL_VCP_REDIRECT_STDIO == TRUE)
        /* : Delete the stream buffer for transfer to write in VCP */
        do
        {
            /* : : Find the stream buffer handle */
            UShellOsalStreamBuffHandle_t streamBuffer = NULL;
            osalStatus = UShellOsalStreamBuffHandleGet(thisOsal, 0U, &streamBuffer);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuffer == NULL))
            {
                break;
            }

            /* : : Delete the stream buffer */
            osalStatus = UShellOsalStreamBuffDelete(thisOsal, streamBuffer);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* : Delete the stream buffer for transfer to read from VCP */
        do
        {
            /* : : Find the stream buffer handle */
            UShellOsalStreamBuffHandle_t streamBuffer = NULL;
            osalStatus = UShellOsalStreamBuffHandleGet(thisOsal, 1U, &streamBuffer);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (streamBuffer == NULL))
            {
                break;
            }

            /* : : Delete the stream buffer */
            osalStatus = UShellOsalStreamBuffDelete(thisOsal, streamBuffer);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

#endif

        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, NULL);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        /* Flush */
        vcp->osal = NULL;

    } while (0);

    return status;
}

/**
 * \brief Lock the vcp object
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpLock(const UShellVcp_s* const vcp)
{
    /* Local variables */
    UShellOsalLockObjHandle_t lockObj = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Get the lock object */
        osalStatus = UShellOsalLockObjHandleGet(osal,
                                                0U,
                                                &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Lock object is invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Lock */
        osalStatus = UShellOsalLock(osal, lockObj);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Lock failed */
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief Unlock the vcp object
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpUnlock(const UShellVcp_s* const vcp)
{
    /* Local variables */
    UShellOsalLockObjHandle_t lockObj = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Get the lock object */
        osalStatus = UShellOsalLockObjHandleGet(osal,
                                                0U,
                                                &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Lock object is invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Unlock */
        osalStatus = UShellOsalUnlock(osal, lockObj);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Unlock failed */
            USHELL_VCP_ASSERT(0);
            break;
        }

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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
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
            /* Event is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Send the message to the queue */
        osalStatus = UShellEventGroupSetBits(osal, event, msgEvent);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Send error msg */
            USHELL_VCP_ASSERT(0);
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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalEventGroupHandleGet(osal, 0U, &eventHandle);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (eventHandle == NULL))
        {
            /* Event is invalid */
            USHELL_VCP_ASSERT(0);
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
            /* Wait error */
            USHELL_VCP_ASSERT(0);
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
            /* No event */
            USHELL_VCP_ASSERT(0);
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
static UShellVcpErr_e uShellVcpMsgXferSend(UShellVcp_s* const vcp,
                                           UShellVcpMsgXfer_e msgTransfer)

{
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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Check msg event */
        if (!(msgTransfer == USHELL_VCP_MSG_TX_NONE) &&
            !(msgTransfer == USHELL_VCP_MSG_TX_COMPLETE) &&
            !(msgTransfer == USHELL_VCP_MSG_TX_RX_ERR))

        {
            /* Invalid message transfer */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            /* Queue is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Send the message to the queue */
        osalStatus = UShellOsalQueueItemPut(osal, queue, &msgTransfer);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Send error msg */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Flush message transfer from the vcp object
 * \param vcp - vcp object
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgXferFlush(UShellVcp_s* const vcp)

{
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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            /* Queue is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Flush the queue */
        osalStatus = UShellOsalQueueReset(osal, queue);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Flush error msg */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Wait for message transfer from the vcp object (blocked )
 * \param vcp - vcp object
 * \param msgTransfer - message transfer to be waited
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpMsgXferPend(UShellVcp_s* const vcp,
                                           UShellVcpMsgXfer_e* const msgTransfer,
                                           const uint32_t timeout)
{
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellOsalQueueHandle_t queue = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellVcpMsgXfer_e msgTxLocal = USHELL_VCP_EVENT_NONE;

    /* Send the message to the queue */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (msgTransfer == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Get the queue handle */
        osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            /* Queue is invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Get the message from the queue */
        osalStatus = ushellOsalQueueItemPend(osal, queue, &msgTxLocal, timeout);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Wait error */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_PORT_ERR;
            *msgTransfer = USHELL_VCP_MSG_TX_RX_ERR;
            break;
        }

        /* Check msg event */
        if (!(msgTxLocal == USHELL_VCP_MSG_TX_NONE) &&
            !(msgTxLocal == USHELL_VCP_MSG_TX_COMPLETE) &&
            !(msgTxLocal == USHELL_VCP_MSG_TX_RX_ERR))

        {
            /* Invalid message transfer */
            USHELL_VCP_ASSERT(0);
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
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    size_t sendByte = 0U;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;

    /* Read from the port */
    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellVcpLock(vcp);

        /* Process send */
        do
        {
            /* Flush the buffer */
            uShellVcpIoBuffFlush(vcp);

            /* Read from hal */
            halStatus = UShellHalRead(hal, vcp->io.buffer, USHELL_VCP_BUFFER_SIZE, &vcp->io.ind);
            if (halStatus != USHELL_HAL_NO_ERR)
            {
                /* Read error */
                USHELL_VCP_ASSERT(0);
                status = USHELL_VCP_PORT_ERR;
                break;
            }

            /* Check we have to send */
            if (vcp->io.ind <= 0U)
            {
                break;
            }

            /* Go over the session */
            for (size_t session = 0; session < USHELL_VCP_ACTIVE_SESSION_MAX; session++)
            {
                /* Check if the session is active */
                if ((vcp->session [session].used == false) ||
                    (vcp->session [session].param.type != USHELL_VCP_DIR_READ) ||
                    (vcp->session [session].stream == NULL))
                {
                    continue;
                }

                /* Send to stream buffer */
                sendByte = UShellOsalStreamBuffSend(osal,
                                                    vcp->session [session].stream,
                                                    vcp->io.buffer,
                                                    vcp->io.ind,
                                                    0U);

                if (sendByte != vcp->io.ind)
                {
                    /* Send error */
                    continue;
                }
            }

        } while (1);

        /* Unlock */
        uShellVcpUnlock(vcp);

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
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellVcpMsgXfer_e msgTxLocal = USHELL_VCP_MSG_TX_NONE;
    bool streamIsEmpty = false;

    /* Read from the port */

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellVcpLock(vcp);

        /* Open tx mode */
        uShellVcpDirectTxSet(vcp);

        /* Process send */
        do
        {
            /* Go over all the session */
            for (size_t session = 0; session < USHELL_VCP_ACTIVE_SESSION_MAX; session++)
            {
                /* Check if the session is active */
                if ((vcp->session [session].used == false) ||
                    (vcp->session [session].param.type != USHELL_VCP_DIR_WRITE) ||
                    (vcp->session [session].stream == NULL))
                {
                    continue;
                }

                /* Check we have data in the stream buffer tx */
                osalStatus = UShellOsalStreamBuffIsEmpty(osal,
                                                         vcp->session [session].stream,
                                                         &streamIsEmpty);
                if (osalStatus != USHELL_OSAL_NO_ERR)
                {
                    /* Stream buffer is invalid */
                    USHELL_VCP_ASSERT(0);
                    status = USHELL_VCP_PORT_ERR;
                    break;
                }

                /* Check we have data in the stream buffer tx */
                if (streamIsEmpty == true)
                {
                    continue;
                }

                do
                {
                    /* Flush the buffer */
                    uShellVcpIoBuffFlush(vcp);

                    /* Read from stream buffer */
                    vcp->io.ind = UShellOsalStreamBuffReceive(osal,
                                                              vcp->session [session].stream,
                                                              vcp->io.buffer,
                                                              USHELL_VCP_BUFFER_SIZE,
                                                              0U);

                    /* Check we have to send */
                    if (vcp->io.ind <= 0U)
                    {
                        break;
                    }

                    /* Flush the tx queue */
                    status = uShellVcpMsgXferFlush(vcp);
                    if (status != USHELL_VCP_NO_ERR)
                    {
                        /* Flush error */
                        USHELL_VCP_ASSERT(0);
                        status = USHELL_VCP_PORT_ERR;
                        break;
                    }

                    /* Send to hal */
                    halStatus = UShellHalWrite(hal, vcp->io.buffer, vcp->io.ind);
                    if (halStatus != USHELL_HAL_NO_ERR)
                    {
                        /* Send error */
                        USHELL_VCP_ASSERT(0);
                        status = USHELL_VCP_PORT_ERR;
                        break;
                    }

                    /* Wait for tx complete */
                    status = uShellVcpMsgXferPend(vcp, &msgTxLocal, USHELL_VCP_TX_TIMEOUT_MS);
                    if ((msgTxLocal != USHELL_VCP_MSG_TX_COMPLETE) ||
                        (status != USHELL_VCP_NO_ERR))
                    {
                        /* Wait error */
                        USHELL_VCP_ASSERT(0);
                        status = USHELL_VCP_PORT_ERR;
                        break;
                    }

                } while (1);

                /* Check if we have an error */
                if (status != USHELL_VCP_NO_ERR)
                {
                    break;
                }
            }

        } while (0);

        /* Set rx mode */
        uShellVcpDirectRxSet(vcp);

        /* Unlock */
        uShellVcpUnlock(vcp);

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
    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

    bool isRxDataAvailable = false;

    do
    {

        /* Check input parameters */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Check we have data in hal */
        do
        {

            /* Get handle of rx stream */
            halStatus = UShellHalIsReadDataAvailable(hal, &isRxDataAvailable);
            USHELL_VCP_ASSERT(halStatus == USHELL_HAL_NO_ERR);
            if ((halStatus != USHELL_HAL_NO_ERR))
            {
                /* Hal error */
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
                /* Send error */
                USHELL_VCP_ASSERT(0);
                break;
            }

        } while (0);

        /* Check we have data in the stream buffer tx */
        do
        {
            /* Go over all the session */
            for (size_t session = 0; session < USHELL_VCP_ACTIVE_SESSION_MAX; session++)
            {
                /* Check if the session is active */
                if ((vcp->session [session].used == false) ||
                    (vcp->session [session].param.type != USHELL_VCP_DIR_WRITE) ||
                    (vcp->session [session].stream == NULL))
                {
                    continue;
                }

                /* Check we have data in the stream buffer tx */
                bool streamIsEmpty = false;
                osalStatus = UShellOsalStreamBuffIsEmpty(osal,
                                                         vcp->session [session].stream,
                                                         &streamIsEmpty);
                if (osalStatus != USHELL_OSAL_NO_ERR)
                {
                    /* Stream buffer is invalid */
                    USHELL_VCP_ASSERT(0);
                    status = USHELL_VCP_PORT_ERR;
                    break;
                }

                /* Check we have data in the stream buffer tx */
                if (streamIsEmpty == true)
                {
                    continue;
                }

                /* Set the tx event */
                status = uShellVcpEventSend(vcp, USHELL_VCP_EVENT_TX_EVENT);
                if (status != USHELL_VCP_NO_ERR)
                {
                    /* Send error */
                    USHELL_VCP_ASSERT(0);
                    break;
                }

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
    /* Flush the buffer */
    do
    {
        /* Check input parameters */
        if (vcp == NULL)
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Get the stream buffer handle */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 1U, &streamBuffer);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuffer == NULL))
        {
            /* Stream buffer is invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Flush the stream buffer */
        osalStatus = UShellOsalStreamBuffReset(osal, streamBuffer);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Flush error msg */
            USHELL_VCP_ASSERT(0);
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
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Get the stream buffer handle */
        osalStatus = UShellOsalStreamBuffHandleGet(osal, 0U, &streamBuffer);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBuffer == NULL))
        {
            /* Stream buffer is invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Flush the stream buffer */
        osalStatus = UShellOsalStreamBuffReset(osal, streamBuffer);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Flush error msg */
            USHELL_VCP_ASSERT(0);
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
    /* Local variable */
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (hal == NULL)
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set tx direction */
        halStatus = UShellHalSetTxMode(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Set tx direction error */
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
    /* Local variable */
    UShellHal_s* hal = (UShellHal_s*) vcp->hal;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (hal == NULL)
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set tx direction */
        halStatus = UShellHalSetRxMode(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            /* Set tx direction error */
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
    /* Local variables */
    UShellVcp_s* const ushell = (UShellVcp_s*) timerParam;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if (ushell == NULL)
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send the message to the queue */
        status = uShellVcpEventSend(ushell, USHELL_VCP_EVENT_INSPECT);
        if (status != USHELL_VCP_NO_ERR)
        {
            /* Send error msg */
            USHELL_VCP_ASSERT(0);
        }

    } while (0);
}

/**
 * \brief UShell VCP socket read callback function
 * \param[in] socket - pointer to the socket object
 * \param[in] type - type of the callback (read or write)
 * \param[in] parent - parent object
 * \return none
 */
static void uShellVcpSocketReadCb(void* socket,
                                  UShellSocketCbType_e type,
                                  void* parent)
{
    /* Local variable */
    UShellVcp_s* const vcp = (UShellVcp_s*) parent;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (vcp == NULL) ||
            (type != USHELL_SOCKET_CB_TYPE_READ))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief UShell VCP socket write callback function
 * \param[in] socket - pointer to the socket object
 * \param[in] type - type of the callback (read or write)
 * \param[in] parent - parent object
 * \return none
 */
static void uShellVcpSocketWriteCb(void* socket,
                                   UShellSocketCbType_e type,
                                   void* parent)
{
    {
        /* Local variable */
        UShellVcp_s* const vcp = (UShellVcp_s*) parent;
        UShellVcpErr_e status = USHELL_VCP_NO_ERR;

        do
        {
            /* Check input parameters */
            if ((socket == NULL) ||
                (vcp == NULL) ||
                (type != USHELL_SOCKET_CB_TYPE_WRITE))
            {
                /* Input parameters are invalid */
                USHELL_VCP_ASSERT(0);
                break;
            }

            /* Send the event to the queue */
            status = uShellVcpEventSend(vcp, USHELL_VCP_EVENT_TX_EVENT);
            if (status != USHELL_VCP_NO_ERR)
            {
                /* Send error msg */
                USHELL_VCP_ASSERT(0);
                break;
            }

        } while (0);
    }
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
int _read(int file, char* ptr, int len)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_VCP_NO_ERR;
    UShellSocket_s* socket = uShellVcpStdSocketRead;
    int numberOfBytes = 0;

    /* Print string to the vcp object */
    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (len == 0U))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            numberOfBytes = 0;
            break;
        }

        /* Read from the socket */
        status = UShellSocketReadBlocking(socket,
                                          (UShellSocketItem_t*) ptr,
                                          len);
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Read error */
            USHELL_VCP_ASSERT(0);
            numberOfBytes = 0;
            break;
        }

        /* Set the number of bytes read */
        numberOfBytes = len;

    } while (0);

    /* Return the number of bytes read */
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
int _write(int file, char* ptr, int len)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_VCP_NO_ERR;
    UShellSocket_s* socket = uShellVcpStdSocketWrite;
    int bytesWritten = 0;
    do
    {
        /* Validate input parameters */
        if ((socket == NULL) ||
            (len <= 0))
        {
            /* Input parameters are invalid */
            USHELL_VCP_ASSERT(0);
            bytesWritten = 0;
            break;
        }

        /* Send the data to the socket */
        status = UShellSocketWriteBlocking(socket,
                                           (UShellSocketItem_t*) ptr,
                                           len);
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Send error */
            USHELL_VCP_ASSERT(0);
            bytesWritten = 0;
            break;
        }

        /* Set the number of bytes written */
        bytesWritten = len;

    } while (0);

    /* Return the number of bytes written */
    return bytesWritten;
}

#endif