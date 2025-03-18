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

/* Project includes */
#include "ushell_vcp.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_VCP_ASSERT
    #define USHELL_VCP_ASSERT(cond)
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

/**
 * \brief Enum of possible events
 */
typedef enum
{
    USHELL_MSG_NONE = 0,       // No message
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
static void uShellVcpWorker(void* const uShell);

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
 * \brief Print io buffer
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellPrintIo(const UShell_s* const uShell);

/**
 * \brief Get char from the input
 * \param uShell - uShell object
 * \param ch - pointer to the char
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellScanCharWait(const UShell_s* const uShell,
                                      UShellItem_t* const ch);

/**
 * \brief Get char from the input with timeout
 * \param uShell - uShell object
 * \param ch - pointer to the char
 * \param timeMs - timeout in milliseconds
 * \return
 */
static UShellErr_e uShellScanCharPend(const UShell_s* const uShell,
                                      UShellItem_t* const ch,
                                      const uint32_t timeMs);

/**
 * \brief Find command
 * \param[in] uShell - uShell object
 * \param[in] str - string to be found
 * \param[out] ind - index of the command
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellFindCmd(const UShell_s* const uShell,
                                 const char* const str,
                                 uint8_t* const ind);

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
 * @brief Set the semaphore for the RX event
 * @param[in] dio - pointer to a UShell instance;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellSemaphoreRxEventSet(UShell_s* const dio);

/**
 * @brief Wait for the semaphore for the RX event
 * @param[in] dio - pointer to a UShell instance;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellSemaphoreRxEventWait(UShell_s* const dio);

/**
 * \brief Event pend
 * \param dio - pointer to a UShell instance;
 * \param timeMs - time in milliseconds;
 * \return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellSemaphoreRxEventPend(UShell_s* const dio,
                                              const uint32_t timeMs);

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

/**
 * \brief Clear the screen
 * \param uShell - uShell object
 * \return none
 */
static inline void uShellTerminalClearScreen(const UShell_s* const uShell);

/**
 * \brief Clear the line
 * \param uShell - uShell object
 * \return none
 */
static inline void uShellTerminalClearLine(const UShell_s* const uShell);

/**
 * \brief Print the prompt
 * \param uShell - uShell object
 */
static inline void uShellPrintUserPrompt(const UShell_s* const uShell);

/**
 * \brief Delete the character in terminal
 * \param[in] uShell - uShell object
 * \return none
 */
static inline void uShellTerminalDelChar(const UShell_s* const uShell);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Init uShell object
 * \param[in] uShell - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellInit(UShell_s* const uShell,
                       const UShellOsal_s* const osal,
                       const UShellHal_s* const hal,
                       const UShellCfg_s cfg,
                       void* const parent,
                       const char* const name)
{

    /* Check input parameters */
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(osal != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

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
                                 (UShellOsal_s*) osal,
                                 (UShellHal_s*) hal,
                                 (UShellCfg_s*) &cfg);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

        /* Set init state */
        uShell->fsmState = USHELL_STATE_IDLE;

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
    USHELL_VCP_ASSERT(uShell != NULL);

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

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief UShell thread worker
 * \param[in] uShell - uShell object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellVcpWorker(void* const uShell)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(uShell != NULL);

    /* Local variables */
    UShell_s* const ushell = (UShell_s*) uShell;
    UShellErr_e status = USHELL_NO_ERR;
    UShellItem_t item = 0;

    /* Main loop */
    while (1)
    {
        /* Clear line */
        uShellTerminalClearLine(ushell);

        /* Print the prompt */
        uShellPrintUserPrompt(ushell);

        /* Print IO buffer */
        uShellPrintIo(ushell);

        do
        {
            /* Get data*/
            status = uShellScanCharPend(ushell, &item, USHELL_CLI_UPDATE_TIMEOUT_MS);
            if (status != USHELL_NO_ERR)
            {
                break;
            }

            /* Process the data */
            switch (item)
            {
                /* Carriage return (\r) */
                case USHELL_ASCII_CHAR_CR :
                case USHELL_ASCII_CHAR_LF :
                {

                    /* Add to history */
                    UShellHistoryErr_e historyStatus = UShellHistoryAdd(&ushell->history,
                                                                        ushell->io.buffer);
                    if (historyStatus != USHELL_HISTORY_NO_ERR)
                    {
                        USHELL_VCP_ASSERT(0);
                    }
                    break;
                }

                /* Backspace  */
                case USHELL_ASCII_CHAR_BS :
                case USHELL_ASCII_CHAR_DEL :
                {
                    /* Process the command */
                    uShellTerminalDelChar(ushell);

                    /* Remove the last char */
                    if (ushell->io.ind > 0)
                    {
                        ushell->io.buffer [--ushell->io.ind] = 0;
                    }

                    break;
                }

                /* Horizontal tab */
                case USHELL_ASCII_CHAR_TAB :
                {
                    /* Process the command */
                    break;
                }

                /* Acknowledge */
                default :
                {
                    /* Store the data */
                    if (ushell->io.ind < USHELL_BUFFER_SIZE)
                    {
                        ushell->io.buffer [ushell->io.ind++] = item;
                    }

                    break;
                }
            }
        } while (0);
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
    USHELL_VCP_ASSERT(hal != NULL);

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
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellSemaphoreRxEventSet(ushell);
        USHELL_VCP_ASSERT(status == USHELL_NO_ERR);
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
    USHELL_VCP_ASSERT(hal != NULL);

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
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_TX_COMPLETE);
        USHELL_VCP_ASSERT(status == USHELL_NO_ERR);
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
    USHELL_VCP_ASSERT(hal != NULL);

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
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_RX_TX_ERROR);
        USHELL_VCP_ASSERT(status == USHELL_NO_ERR);
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
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(str != NULL);

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

        /* Send the data */
        halStatus = UShellHalWrite(uShell->hal, (uint8_t*) str, len + 1);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait the message */
        UShellMsg_e msg = USHELL_MSG_NONE;
        status = uShellQueueMsgPend(uShell,
                                    &msg,
                                    USHELL_CLI_TX_TIMEOUT_MS);
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
 * \brief Print io buffer
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellPrintIo(const UShell_s* const uShell)
{
    /* Check input */
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(str != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellMsg_e msg = USHELL_MSG_NONE;

    /* Printf */
    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Check we have data */
        if (uShell->io.ind == 0)
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

        /* Send the data */
        halStatus = UShellHalWrite(uShell->hal,
                                   (uint8_t*) uShell->io.buffer,
                                   uShell->io.ind);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait the message */
        UShellMsg_e msg = USHELL_MSG_NONE;
        status = uShellQueueMsgPend(uShell,
                                    &msg,
                                    USHELL_CLI_TX_TIMEOUT_MS);
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
static UShellErr_e uShellScanCharWait(const UShell_s* const uShell,
                                      UShellItem_t* const ch)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(ch != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    UShellMsg_e msg = USHELL_MSG_NONE;
    uint8_t item = 0;

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

        /* Set the rx mode */
        halStatus = UShellHalSetRxMode(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait the message */
        status = uShellSemaphoreRxEventWait(uShell);
        if (status != USHELL_NO_ERR)
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
    }

    while (0);

    return status;
}

/**
 * @brief Get char from the input with timeout
 * @param uShell - uShell object
 * @param ch - pointer to the char
 * @param timeMs - timeout in milliseconds
 * @return
 */
static UShellErr_e uShellScanCharPend(const UShell_s* const uShell,
                                      UShellItem_t* const ch,
                                      const uint32_t timeMs)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(ch != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;
    uint8_t item = 0;

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

        /* Set the rx mode */
        halStatus = UShellHalSetRxMode(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait the message */
        status = uShellSemaphoreRxEventPend(uShell,
                                            timeMs);
        if ((status != USHELL_NO_ERR))
        {
            status = USHELL_XFER_ERR;
            break;
        }

        UShellOsalSemaphoreCount_t count = 0;
        status = UShellOsalSemaphoreCountGet(uShell->osal, uShell->osal->semaphoreHandle [0], &count);

        /* Get the char */
        halStatus = UShellHalRead(uShell->hal, &item, 1U);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Set the char */
        *ch = item;
    }

    while (0);

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
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(osal != NULL);
    USHELL_VCP_ASSERT(hal != NULL);
    USHELL_VCP_ASSERT(cfg != NULL);

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
    USHELL_VCP_ASSERT(uShell != NULL);

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
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

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

        /* Open the hal */
        halStatus = UShellHalOpen(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            return;
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
    USHELL_VCP_ASSERT(uShell != NULL);

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

        /* Close */
        halStatus = UShellHalClose(uShell->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            return;
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
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(osal != NULL);

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
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
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
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(queue != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create semaphore for rx event */
        UShellOsalSemaphoreHandle_t semaphoreHandle = NULL;
        osalStatus = UShellOsalSemaphoreCreate(thisOsal,
                                               USHELL_BUFFER_SIZE,
                                               0U,
                                               &semaphoreHandle);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(semaphoreHandle != NULL);
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
                .threadWorker = uShellVcpWorker};
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
    USHELL_VCP_ASSERT(uShell != NULL);
    USHELL_VCP_ASSERT(uShell->osal != NULL);

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
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

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

    /* : Delete the semaphore */
    do
    {
        /* : : Find the semaphore handle */
        UShellOsalSemaphoreHandle_t semaphoreHandle = NULL;
        osalStatus = UShellOsalSemaphoreHandleGet(thisOsal, 0U, &semaphoreHandle);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (semaphoreHandle == NULL))
        {
            break;
        }

        /* : : Delete the semaphore */
        osalStatus = UShellOsalSemaphoreDelete(thisOsal, semaphoreHandle);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);

    /* Set parent */
    osalStatus = UShellOsalParentSet(thisOsal, NULL);
    USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    /* Flush */
    uShell->osal = NULL;
}

/**
 * @brief Set the semaphore for the RX event
 * @param[in] dio - pointer to a UShell instance;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellSemaphoreRxEventSet(UShell_s* const dio)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(dio != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Set the semaphore for the RX event */
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

        /* Get the semaphore handle */
        UShellOsalSemaphoreHandle_t semaphoreHandle = NULL;
        UShellOsalErr_e osalStatus = UShellOsalSemaphoreHandleGet(osal, 0U, &semaphoreHandle);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (semaphoreHandle == NULL))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Set the semaphore for the RX event */
        osalStatus = UShellOsalSemaphoreRelease(osal, semaphoreHandle);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Wait for the semaphore for the RX event
 * @param[in] dio - pointer to a UShell instance;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellSemaphoreRxEventWait(UShell_s* const dio)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(dio != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    /* Wait for the semaphore for the RX event */
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

        /* Get the semaphore handle */
        UShellOsalSemaphoreHandle_t semaphoreHandle = NULL;
        UShellOsalErr_e osalStatus = UShellOsalSemaphoreHandleGet(osal, 0U, &semaphoreHandle);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (semaphoreHandle == NULL))
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Wait for the semaphore for the RX event */
        osalStatus = UShellOsalSemaphoreAcquire(osal, semaphoreHandle);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Event pend
 * \param dio - pointer to a UShell instance;
 * \param timeMs - time in milliseconds;
 * \return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellSemaphoreRxEventPend(UShell_s* const dio,
                                              const uint32_t timeMs)
{
    {
        /* Check input parameters */
        USHELL_VCP_ASSERT(dio != NULL);

        /* Local variable */
        UShellErr_e status = USHELL_NO_ERR;

        /* Wait for the semaphore for the RX event */
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

            /* Get the semaphore handle */
            UShellOsalSemaphoreHandle_t semaphoreHandle = NULL;
            UShellOsalErr_e osalStatus = UShellOsalSemaphoreHandleGet(osal, 0U, &semaphoreHandle);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (semaphoreHandle == NULL))
            {
                status = USHELL_PORT_ERR;
                break;
            }

            /* Wait for the semaphore for the RX event */
            osalStatus = UShellOsalSemaphoreAcquire(osal, semaphoreHandle);
            if (osalStatus != USHELL_OSAL_NO_ERR)
            {
                status = USHELL_PORT_ERR;
                break;
            }

        } while (0);

        return status;
    }
}

/**
 * @brief Initialize the runtime environment history
 * @param uShell - uShell object
 * @return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryInit(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(uShell != NULL);

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
    USHELL_VCP_ASSERT(uShell != NULL);

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
    USHELL_VCP_ASSERT(dio != NULL);

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
 * \brief Unlock the dio monitor
 * \param[in] dio - pointer to a UShell_s instance;
 * \return no;
 */
static void uShellUnlock(const UShell_s* const dio)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(dio != NULL);

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
 * @brief Send the message to the queue
 * @param[in] dio - pointer to a UShell instance;
 * @param msg - message;
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellQueueMsgSend(UShell_s* const dio,
                                      const UShellMsg_e msg)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(dio != NULL);

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

        if (!(msg == USHELL_MSG_TX_COMPLETE) &&
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
    USHELL_VCP_ASSERT(dio != NULL);

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
    USHELL_VCP_ASSERT(dio != NULL);

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
        if (!(localMsg == USHELL_MSG_TX_COMPLETE) &&
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
    USHELL_VCP_ASSERT(dio != NULL);

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
        if (!(localMsg == USHELL_MSG_TX_COMPLETE) &&
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