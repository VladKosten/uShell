/**
* \file         ushell.c
* \brief        The file contains the implementation of the UShell main module. The module is responsible for the initialization of the UShell
*               and the main loop of the UShell. The UShell is a simple command line interface that allows the user to interact with the system via
*               the serial port.
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

/* Project includes */
#include "ushell.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_ASSERT
    #define USHELL_ASSERT(cond)
#endif

typedef enum
{
    USHELL_ASCII_CHAR_CR = 0x0D,       ///< Carriage Return
    USHELL_ASCII_CHAR_LF = 0x0A,       ///< Line Feed
    USHELL_ASCII_CHAR_BS = 0x08,       ///< Backspace
    USHELL_ASCII_CHAR_DEL = 0x7F,      ///< Delete
    USHELL_ASCII_CHAR_SPACE = 0x20,    ///< Space
    USHELL_ASCII_CHAR_TAB = 0x09       ///< Tab

} UShellAsciiChar_e;

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

/**
 * \brief Enum of possible events
 */
typedef enum
{
    USHELL_MSG_NONE = 0,       // No message
    USHELL_MSG_RX_RECEIVED,    // RX received
    USHELL_MSG_TX_COMPLETE,    // Output changed
    USHELL_MSG_RX_TX_ERROR     // Error

} UShellMsg_e;

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShell thread worker
 * \param[in] uShell - uShell object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellWorker(void* const uShell);

/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
 */
static void uShellRxReceivedCb(const void* const hal);

/**
 * \brief Callback for the transmitted data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is transmitted to the serial port.
 */
static void uShellTxCpltCb(const void* const hal);

/**
 * \brief Callback for the error
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the error occurs in the serial port.
 */
static void uShellXferErrorCb(const void* const hal);

/**
 * \brief Print string
 * \param[in] uShell - uShell object
 * \param[in] str - string to be printed
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellPrint(const UShell_s* const uShell,
                               const char* const str);
/**
 * @brief Get char from the input
 * @param uShell - uShell object
 * @param ch - pointer to the char
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellScanChar(const UShell_s* const uShell,
                                  UShellItem_t* const ch);

/**
 * \brief Find command
 * \param[in] uShell - uShell object
 * \param[in] str - string to be found
 * \param[out] ind - index of the command
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellFindCmd(const UShell_s* const uShell, const char* const str, uint8_t* const ind);

/**
 * @brief Initialize the runtime environment
 * @param uShell - uShell object
 * @param osal - osal object
 * @param hal - hal object
 * @param cfg - configuration object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvInit(UShell_s* const uShell,
                                   UShellOsal_s* const osal,
                                   UShellHal_s* const hal,
                                   UShellCfg_s* const cfg);

/**
 * @brief Deinitialize the runtime environment
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvDeInit(UShell_s* const uShell);

/**
 * @brief Initialize the runtime environment HAL
 * @param uShell - uShell object
 * @param hal - hal object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvHalInit(UShell_s* const uShell,
                                      UShellHal_s* const hal);

/**
 * @brief Deinitialize the runtime environment HAL
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvHalDeInit(UShell_s* const uShell);

/**
 * @brief Initialize the runtime environment OSAL
 * @param[in] uShell - uShell object
 * @param[in] osal - osal object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellRtEnvOsalInit(UShell_s* const uShell,
                                       UShellOsal_s* const osal);

/**
 * @brief Deinitialize the runtime environment OSAL
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvOsalDeInit(UShell_s* const uShell);

/**
 * @brief Initialize the runtime environment authentication
 * @param[in] uShell - uShell object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellRtEnvFuncAuthInit(UShell_s* const uShell);

/**
 * @brief Deinitialize the runtime environment authentication
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncAuthDeInit(UShell_s* const uShell);

/**
 * @brief Initialize the runtime environment history
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryInit(UShell_s* const uShell);

/**
 * @brief Deinitialize the runtime environment history
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryDeInit(UShell_s* const uShell);

/**
 * @brief Send the message to the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgSend(UShell_s* const dio,
                                      const UShellMsg_e msg);

/**
 * @brief Flush the message queue
 * @param[in] dio - pointer to a UShell instance;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgFlush(UShell_s* const dio);

/**
 * @brief Pend for the message in the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @param timeMs - time in milliseconds;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgPend(UShell_s* const dio,
                                      UShellMsg_e* const msg,
                                      const uint32_t timeMs);

/**
 * @brief Wait for the message in the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgWait(UShell_s* const dio,
                                      UShellMsg_e* const msg);

/**
 * \brief Lock the dio monitor
 * \param[in] dio - pointer to a UShell_s instance;
 * \return no;
 */
static void uShellLock(const UShell_s* const dio);

/**
 * \brief Unlock the dio monitor
 * \param[in] dio - pointer to a UShell_s instance;
 * \return no;
 */
static void uShellUnlock(const UShell_s* const dio);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Init uShell object
 * \param[in] uShell - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellInit(UShell_s* const uShell,
                       const UShellOsal_s* const osal,
                       const UShellHal_s* const hal,
                       const UShellCfg_s cfg,
                       void* const parent,
                       const char* const name)
{

    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(osal != NULL);
    USHELL_ASSERT(hal != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Initialize the runtime environment */
        if ((uShell == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            return USHELL_INVALID_ARGS_ERR;
        }

        /* Flush the objects */
        memset(uShell, 0, sizeof(UShell_s));

        /* Set the parent object */
        uShell->parent = parent;
        uShell->name = name;

        /* Initialize the runtime environment */
        status = uShellRtEnvInit(uShell,
                                 osal,
                                 hal,
                                 &cfg);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);

    return USHELL_NO_ERR;
}

/**
 * \brief DeInit uShell object
 * \param[in] uShell - uShell object to be deinitialized
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellDeInit(UShell_s* const uShell)
{

    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the runtime environment */
        uShellRtEnvDeInit(uShell);

        /* Clear the object */
        memset(uShell, 0, sizeof(UShell_s));

    } while (0);

    return status;
}

/**
 * \brief Run uShell object
 * \note Used after the initialization of the UShell object.
 * \param[in] uShell - uShell object to be run
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellRun(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Flush the queue msg */
        status = uShellQueueMsgFlush(uShell);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

        /* Find the thread handle */
        UShellOsalThread_s* thread = NULL;
        UShellOsalErr_e osalStatus = UShellOsalThreadHandleGet(uShell->osal, 0U, &thread);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Start the thread */
        osalStatus = UShellOsalThreadResume(uShell->osal, thread->threadHandle);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Stop uShell object
 * \param[in] uShell - uShell object to be stopped
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellStop(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Find the thread handle */
        UShellOsalThread_s* thread = NULL;
        UShellOsalErr_e osalStatus = UShellOsalThreadHandleGet(uShell->osal, 0U, &thread);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Start the thread */
        osalStatus = UShellOsalThreadResume(uShell->osal, thread->threadHandle);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Attach command to uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be attached
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellCmdAttach(UShell_s* const uShell, const UShellCmd_s* const cmd)
{
    /* Check input parameters */
    if ((uShell == NULL) || (cmd == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;    ///< Exit: Invalid arguments
    }

    /* Attach command */
    uint8_t attachFlag = 0;
    for (uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        if (uShell->cmd [i] == NULL)
        {
            uShell->cmd [i] = cmd;
            attachFlag = 1;
            break;
        }
    }

    if (attachFlag == 0)
    {
        return USHELL_CMD_ERR;    ///< Exit: Command not attached
    }

    return USHELL_NO_ERR;    ///< Exit: Success
}

/**
 * \brief Detach command from uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be detached
 * \param[out] none
 */
UShellErr_e UShellCmdDetach(UShell_s* const uShell, const UShellCmd_s* const cmd)
{
    /* Check input parameters */
    if ((uShell == NULL) ||
        (cmd == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;    ///< Exit: Invalid arguments
    }

    /* Detach command */
    uint8_t detachFlag = 0;
    for (uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        if (uShell->cmd [i] == cmd)
        {

            uShell->cmd [i] = NULL;
            detachFlag = 1;
            break;
        }
    }

    if (detachFlag == 0)
    {
        return USHELL_CMD_ERR;    ///< Exit: Command not detached
    }

    return USHELL_NO_ERR;    ///< Exit: Command not found
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief UShell thread worker
 * \param[in] uShell - uShell object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellWorker(void* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShell_s* const ushell = (UShell_s*) uShell;
    UShellErr_e status = USHELL_NO_ERR;
    UShellItem_t item [100];

    /* First msg */
    /* Open the hal */
    UShellHalErr_e halStatus = UShellHalOpen(ushell->hal);
    if (halStatus != USHELL_HAL_NO_ERR)
    {
        return;
    }

    /* Main loop */
    while (1)
    {

        while (1)
        {
            /* Print data */
            strcpy(item, "Enter command: ");
            status = uShellPrint(ushell, item);
            if (status != USHELL_NO_ERR)
            {
                continue;
            }
        }

        /* Get data*/
        status = uShellScanChar(ushell, &item);
        if (status != USHELL_NO_ERR)
        {
            continue;
        }

        /* Print data */
        status = uShellPrint(ushell, &item);
        if (status != USHELL_NO_ERR)
        {
            continue;
        }

        // /* Process the data */
        // switch (item)
        // {
        //     /* Carriage return (\r) */
        //     case USHELL_ASCII_CHAR_CR :
        //     case USHELL_ASCII_CHAR_LF :
        //     {

        //    /* Find command */
        //    uint8_t ind = 0;
        //    UShellErr_e ushellErr = uShellFindCmd(ushell, ushell->io.buffer, &ind);
        //    if (ushellErr != USHELL_NO_ERR)
        //    {
        //        break;
        //    }

        //    break;
        // }

        //    /* Backspace  */
        //    case USHELL_ASCII_CHAR_BS :
        //    case USHELL_ASCII_CHAR_DEL :
        //    {
        //        /* Process the command */
        //        break;
        //    }

        //    /* Horizontal tab */
        //    case USHELL_ASCII_CHAR_TAB :
        //    {
        //        /* Process the command */
        //        break;
        //    }

        //    /* Acknowledge */
        //    default :
        //    {
        //        /* Store the data */
        //        if (ushell->io.ind < USHELL_BUFFER_SIZE)
        //        {
        //            ushell->io.buffer [ushell->io.ind++] = item;
        //        }

        //    break;
        // }
        // }
    }
}

/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
 */
static void uShellRxReceivedCb(const void* const hal)
{
    /* Check input parameters */
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShell_s* const ushell = (UShell_s*) ushellHal->parent;
    UShellErr_e status = USHELL_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL))
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_RX_RECEIVED);
        USHELL_ASSERT(status == USHELL_NO_ERR);
        (void) status;

    } while (0);
}

/**
 * \brief Callback for the transmitted data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is transmitted to the serial port.
 */
static void uShellTxCpltCb(const void* const hal)
{
    /* Check input parameters */
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShell_s* const ushell = (UShell_s*) ushellHal->parent;
    UShellErr_e status = USHELL_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL))
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_TX_COMPLETE);
        USHELL_ASSERT(status == USHELL_NO_ERR);
        (void) status;

    } while (0);
}

/**
 * \brief Callback for the error
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the error occurs in the serial port.
 */
static void uShellXferErrorCb(const void* const hal)
{
    /* Check input parameters */
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShell_s* const ushell = (UShell_s*) ushellHal->parent;
    UShellErr_e status = USHELL_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL))
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_RX_TX_ERROR);
        USHELL_ASSERT(status == USHELL_NO_ERR);
        (void) status;

    } while (0);
}

/**
 * \brief Print string
 * \param[in] uShell - uShell object
 * \param[in] str - string to be printed
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellPrint(const UShell_s* const uShell, const char* const str)
{
    /* Check input */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(str != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    size_t len = 0;
    UShellMsg_e msg = USHELL_MSG_NONE;

    /* Printf */
    do
    {
        /* Check input parameters */
        if ((uShell == NULL) ||
            (str == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Get length */
        len = strlen(str);
        if (len == 0)
        {
            break;
        }

        /* Set the tx mode */
        halStatus = UShellHalSetTxMode(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Flush the queue */
        status = uShellQueueMsgFlush(uShell);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

        /* Flush the buffers */
        halStatus = UShellHalFlush(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Send the data */
        halStatus = UShellHalWrite(uShell->hal, (uint8_t*) str, len);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait the message */
        UShellMsg_e msg = USHELL_MSG_NONE;
        status = uShellQueueMsgPend(uShell,
                                    &msg,
                                    USHELL_SEND_TIMEOUT_MS);
        if ((status != USHELL_NO_ERR) ||
            (msg != USHELL_MSG_TX_COMPLETE))
        {
            status = USHELL_XFER_ERR;
            break;
        }

        /* Success */

    } while (0);

    return status;
}

/**
 * @brief Get char from the input
 * @param uShell - uShell object
 * @param ch - pointer to the char
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellScanChar(const UShell_s* const uShell,
                                  UShellItem_t* const ch)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(ch != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellMsg_e msg = USHELL_MSG_NONE;
    char item = 0;

    /* Get the char */
    do
    {
        /* Check input parameters */
        if ((uShell == NULL) ||
            (ch == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Flush the queue */
        status = uShellQueueMsgFlush(uShell);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

        /* Flush the buffers */
        halStatus = UShellHalFlush(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Set the rx mode */
        halStatus = UShellHalSetRxMode(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait the message */
        status = uShellQueueMsgWait(uShell,
                                    &msg);
        if ((status != USHELL_NO_ERR) ||
            (msg != USHELL_MSG_RX_RECEIVED))
        {
            status = USHELL_XFER_ERR;
            break;
        }

        /* Get the char */
        halStatus = UShellHalRead(uShell->hal, &item, 1U);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Set the char */
        *ch = item;

    } while (0);

    return status;
}

/**
 * \brief Find command
 * \param[in] uShell - uShell object
 * \param[in] str - string to be found
 * \param[out] ind - index of the command
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellFindCmd(const UShell_s* const uShell, const char* const str, uint8_t* const ind)
{
    return USHELL_NO_ERR;
}

/**
 * @brief Initialize the runtime environment
 * @param uShell - uShell object
 * @param osal - osal object
 * @param hal - hal object
 * @param cfg - configuration object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvInit(UShell_s* const uShell,
                                   UShellOsal_s* const osal,
                                   UShellHal_s* const hal,
                                   UShellCfg_s* const cfg)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(osal != NULL);
    USHELL_ASSERT(hal != NULL);
    USHELL_ASSERT(cfg != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (osal == NULL) ||
            (hal == NULL) ||
            (cfg == NULL))
        {
            return USHELL_INVALID_ARGS_ERR;
        }

        /* Attach the configuration object */
        uShell->cfg = *cfg;

        /* Initialize the runtime environment authentication */
        if (uShell->cfg.authIsEn == true)
        {
            status = uShellRtEnvFuncAuthInit(uShell);
            if (status != USHELL_NO_ERR)
            {
                break;
            }
        }

        /* Initialize the runtime environment history */
        if (uShell->cfg.historyIsEn == true)
        {
            status = uShellRtEnvFuncHistoryInit(uShell);
            if (status != USHELL_NO_ERR)
            {
                break;
            }
        }

        /* Initialize the runtime environment HAL */
        status = uShellRtEnvHalInit(uShell, hal);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

        /* Initialize the runtime environment OSAL */
        status = uShellRtEnvOsalInit(uShell, osal);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);

    /* Check status */
    if (status != USHELL_NO_ERR)
    {
        /* Deinitialize the runtime environment */
        uShellRtEnvDeInit(uShell);
    }

    return status;
}

/**
 * @brief Deinitialize the runtime environment
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvDeInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameter */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the runtime environment OSAL */
        uShellRtEnvOsalDeInit(uShell);

        /* Deinitialize the runtime environment HAL */
        uShellRtEnvHalDeInit(uShell);

        /* Deinitialize the runtime environment authentication */
        uShellRtEnvFuncHistoryDeInit(uShell);

        /* Deinitialize the runtime environment history */
        uShellRtEnvFuncAuthDeInit(uShell);

    } while (0);

    return status;
}

/**
 * @brief Initialize the runtime environment HAL
 * @param uShell - uShell object
 * @param hal - hal object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvHalInit(UShell_s* const uShell,
                                      UShellHal_s* const hal)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (hal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Save the hal object */
        uShell->hal = hal;

        /* Attach the parent object */
        halStatus = UShellHalParentSet(hal, uShell);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Attach callback for the received data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_RECEIVED,
                                      uShellRxReceivedCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Attach callback for the transmitted data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_TX_COMPLETE,
                                      uShellTxCpltCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Attach callback for the error */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_TX_ERROR,
                                      uShellXferErrorCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Deinitialize the runtime environment HAL
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvHalDeInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (uShell->hal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Detach callback for the received data */
        halStatus = UShellHalCbDetach(uShell->hal,
                                      USHELL_HAL_CB_RX_RECEIVED);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Detach callback for the transmitted data */
        halStatus = UShellHalCbDetach(uShell->hal,
                                      USHELL_HAL_CB_TX_COMPLETE);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Detach callback for the error */
        halStatus = UShellHalCbDetach(uShell->hal,
                                      USHELL_HAL_CB_RX_TX_ERROR);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Detach the parent object */
        halStatus = UShellHalParentSet(uShell->hal, NULL);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Remove the hal object */
        uShell->hal = NULL;

    } while (0);

    return status;
}

/**
 * @brief Initialize the runtime environment OSAL
 * @param[in] uShell - uShell object
 * @param[in] osal - osal object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellRtEnvOsalInit(UShell_s* const uShell,
                                       UShellOsal_s* const osal)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(osal != NULL);

    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* thisOsal = (UShellOsal_s*) osal;

    /* Attach osal object */
    uShell->osal = thisOsal;

    /* Create osal objects */
    do
    {
        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, uShell);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create mutex */
        UShellOsalLockObjHandle_t lockObj = NULL;
        osalStatus = UShellOsalLockObjCreate(thisOsal, &lockObj);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_ASSERT(lockObj != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create queue for send/receive msg */
        UShellOsalQueueHandle_t queue = NULL;
        osalStatus = UShellOsalQueueCreate(thisOsal,
                                           sizeof(UShellMsg_e),
                                           8U,
                                           &queue);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_ASSERT(queue != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create task */
        UShellOsalThreadHandle_t thread = NULL;
        UShellOsalThreadCfg_s threadCfg =
            {
                .name = USHELL_THREAD_NAME,
                .stackSize = USHELL_THREAD_STACK_SIZE,
                .threadParam = uShell,
                .threadPriority = USHELL_THREAD_PRIORITY,
                .threadWorker = uShellWorker};
        osalStatus = UShellOsalThreadCreate(thisOsal, &thread, threadCfg);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_ASSERT(thread != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

    } while (0);

    if (osalStatus != USHELL_OSAL_NO_ERR)
    {
        uShellRtEnvOsalDeInit(uShell);
        return USHELL_PORT_ERR;
    }

    return USHELL_NO_ERR;
}

/**
 * @brief Deinitialize the runtime environment OSAL
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvOsalDeInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(uShell->osal != NULL);

    /* Status obj */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    (void) osalStatus;
    UShellOsal_s* thisOsal = (UShellOsal_s*) uShell->osal;

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
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);

    /* : Delete the queue */
    do
    {
        /* : : Find the queue handle */
        UShellOsalQueueHandle_t queue = NULL;
        osalStatus = UShellOsalQueueHandleGet(thisOsal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            break;
        }

        /* : : Delete the queue */
        osalStatus = UShellOsalQueueDelete(thisOsal, queue);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
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
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
    } while (0);

    /* Set parent */
    osalStatus = UShellOsalParentSet(thisOsal, NULL);
    USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    /* Flush */
    uShell->osal = NULL;
}

/**
 * @brief Initialize the runtime environment authentication
 * @param[in] uShell - uShell object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellRtEnvFuncAuthInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Initialize the authentication */
        status = UShellAuthInit(&uShell->auth,
                                uShell,
                                USHELL_AUTH_PASSWORD);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Deinitialize the runtime environment authentication
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncAuthDeInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the authentication */
        status = UShellAuthDeInit(&uShell->auth);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Initialize the runtime environment history
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Initialize the history */
        status = UShellHistoryInit(&uShell->history,
                                   uShell);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Deinitialize the runtime environment history
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryDeInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the history */
        status = UShellHistoryDeInit(&uShell->history);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Lock the dio monitor
 * \param[in] dio - pointer to a UShell_s instance;
 * \return no;
 */
static void uShellLock(const UShell_s* const dio)
{
    /* Check input parameters */
    USHELL_ASSERT(dio != NULL);

    do
    {
        /* Check input parameters */
        if ((dio == NULL) ||
            (dio->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) dio->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                0U,
                                                                &lockObj);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Lock */
        osalStatus = UShellOsalLock(osal, lockObj);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * \brief Unlock the dio monitor
 * \param[in] dio - pointer to a UShell_s instance;
 * \return no;
 */
static void uShellUnlock(const UShell_s* const dio)
{
    /* Check input parameters */
    USHELL_ASSERT(dio != NULL);

    do
    {
        /* Check input parameters */
        if ((dio == NULL) ||
            (dio->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) dio->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                0U,
                                                                &lockObj);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Unlock */
        osalStatus = UShellOsalUnlock(osal, lockObj);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * @brief Send the message to the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgSend(UShell_s* const dio,
                                      const UShellMsg_e msg)
{
    /* Check input parameters */
    USHELL_ASSERT(dio != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Send the message to the queue */
    do
    {
        /* Check input parameters */
        if ((dio == NULL) ||
            (dio->osal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        if (!(msg == USHELL_MSG_RX_RECEIVED) &&
            !(msg == USHELL_MSG_TX_COMPLETE) &&
            !(msg == USHELL_MSG_NONE) &&
            !(msg == USHELL_MSG_RX_TX_ERROR))

        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        UShellOsal_s* osal = (UShellOsal_s*) dio->osal;

        /* Get the queue handle */
        UShellOsalQueueHandle_t queue = NULL;
        UShellOsalErr_e osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Send the message to the queue */
        osalStatus = UShellOsalQueueItemPut(osal, queue, &msg);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Flush the message queue
 * @param[in] dio - pointer to a UShell instance;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgFlush(UShell_s* const dio)
{
    /* Check input parameters */
    USHELL_ASSERT(dio != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Flush the message queue */
    do
    {
        /* Check input parameters */
        if ((dio == NULL) ||
            (dio->osal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) dio->osal;

        /* Get the queue handle */
        UShellOsalQueueHandle_t queue = NULL;
        UShellOsalErr_e osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Flush the message queue */
        osalStatus = UShellOsalQueueReset(osal, queue);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Wait for the message in the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgPend(UShell_s* const dio,
                                      UShellMsg_e* const msg,
                                      const uint32_t timeMs)
{
    /* Check input parameters */
    USHELL_ASSERT(dio != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Wait for the message in the queue */
    do
    {
        /* Check input parameters */
        if ((dio == NULL) ||
            (dio->osal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) dio->osal;

        /* Get the queue handle */
        UShellOsalQueueHandle_t queue = NULL;
        UShellOsalErr_e osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait for the message in the queue */
        UShellMsg_e localMsg = USHELL_MSG_NONE;
        osalStatus = ushellOsalQueueItemPend(osal, queue, &localMsg, timeMs);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Check msg */
        if (!(localMsg == USHELL_MSG_RX_RECEIVED) &&
            !(localMsg == USHELL_MSG_TX_COMPLETE) &&
            !(localMsg == USHELL_MSG_NONE) &&
            !(localMsg == USHELL_MSG_RX_TX_ERROR))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Set the message */
        *msg = localMsg;

    } while (0);

    return status;
}

/**
 * @brief Wait for the message in the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgWait(UShell_s* const dio,
                                      UShellMsg_e* const msg)
{
    /* Check input parameters */
    USHELL_ASSERT(dio != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Wait for the message in the queue */
    do
    {
        /* Check input parameters */
        if ((dio == NULL) ||
            (dio->osal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) dio->osal;

        /* Get the queue handle */
        UShellOsalQueueHandle_t queue = NULL;
        UShellOsalErr_e osalStatus = UShellOsalQueueHandleGet(osal, 0U, &queue);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (queue == NULL))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait for the message in the queue */
        UShellMsg_e localMsg = USHELL_MSG_NONE;
        osalStatus = UShellOsalQueueItemWait(osal, queue, &localMsg);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Check msg */
        if (!(localMsg == USHELL_MSG_RX_RECEIVED) &&
            !(localMsg == USHELL_MSG_TX_COMPLETE) &&
            !(localMsg == USHELL_MSG_NONE) &&
            !(localMsg == USHELL_MSG_RX_TX_ERROR))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Set the message */
        *msg = localMsg;

    } while (0);

    return status;
}
