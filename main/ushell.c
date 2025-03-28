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
#include <stdio.h>

/* Project includes */
#include "ushell.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_ASSERT
    #define USHELL_ASSERT(cond)
#endif

/**
 * \brief Enumeration of ASCII control characters used in uShell.
 *
 * This enumeration defines several ASCII control characters that are used
 * to manage user input, such as carriage return, line feed, backspace, etc.
 */
typedef enum
{
    USHELL_ASCII_CHAR_CR = 0x0D,       ///< Carriage Return
    USHELL_ASCII_CHAR_LF = 0x0A,       ///< Line Feed
    USHELL_ASCII_CHAR_BS = 0x08,       ///< Backspace
    USHELL_ASCII_CHAR_DEL = 0x7F,      ///< Delete
    USHELL_ASCII_CHAR_SPACE = 0x20,    ///< Space
    USHELL_ASCII_CHAR_TAB = 0x09,      ///< Tab
    USHELL_ASCII_CHAR_ENTER = 0x0D,    ///< Enter
} UShellAsciiChar_e;

#define USHELL_CLEAR_SCREEN "\033[2J\033[1;1H"    ///< Clear screen command
#define USHELL_CLEAR_LINE   "\033[2K\r"           ///< Clear line command with carriage return
#define USHELL_DEL_CHAR     "\b \b"               ///< Delete character command
#define USHELL_ARROW_UP     "\033[A"              ///< Arrow up command
#define USHELL_ARROW_DOWN   "\033[B"              ///< Arrow down command

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
static void uShellWorker(void* const uShell);

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
 * \brief Initialize the runtime environment
 * \param uShell - uShell object
 * \param osal - osal object
 * \param hal - hal object
 * \param cfg - configuration object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvInit(UShell_s* const uShell,
                                   UShellOsal_s* const osal,
                                   UShellCfg_s* const cfg);

/**
 * \brief Deinitialize the runtime environment
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvDeInit(UShell_s* const uShell);

/**
 * \brief Initialize the runtime environment OSAL
 * \param[in] uShell - uShell object
 * \param[in] osal - osal object
 * \return UShellErr_e - error code. non-zero = an error has occurred;
 */
static UShellErr_e uShellRtEnvOsalInit(UShell_s* const uShell,
                                       UShellOsal_s* const osal);

/**
 * \brief Deinitialize the runtime environment OSAL
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvOsalDeInit(UShell_s* const uShell);

/**
 * \brief Initialize the runtime environment history
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryInit(UShell_s* const uShell);

/**
 * \brief Deinitialize the runtime environment history
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvFuncHistoryDeInit(UShell_s* const uShell);

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
 * \brief Print IO buffer
 * \param uShell - uShell object
 */
static inline void uShellPrintIo(UShell_s* const uShell);

/**
 * \brief Delete the character in terminal
 * \param[in] uShell - uShell object
 * \return none
 */
static inline void
uShellTerminalDelChar(const UShell_s* const uShell);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Init uShell object
 * \param[in] uShell - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] vcs - vcp object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellInit(UShell_s* const uShell,
                       const UShellOsal_s* const osal,
                       const UShellVcp_s* const vcp,
                       const UShellCfg_s cfg,
                       void* const parent,
                       const char* const name)
{

    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(osal != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Initialize the runtime environment */
        if ((uShell == NULL) ||
            (osal == NULL) ||
            (vcp == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Flush the objects */
        memset(uShell, 0, sizeof(UShell_s));

        /* Set the attribute  */
        uShell->parent = parent;
        uShell->name = name;
        uShell->vcp = vcp;

        /* Initialize the runtime environment */
        status = uShellRtEnvInit(uShell,
                                 (UShellOsal_s*) osal,
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

        /* Find the thread handle */
        UShellOsalThread_s* thread = NULL;
        UShellOsalErr_e osalStatus = UShellOsalThreadHandleGet((UShellOsal_s*) uShell->osal, 0U, &thread);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Start the thread */
        osalStatus = UShellOsalThreadResume((UShellOsal_s*) uShell->osal, thread->threadHandle);
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
        UShellOsalErr_e osalStatus = UShellOsalThreadHandleGet((UShellOsal_s*) uShell->osal, 0U, &thread);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Start the thread */
        osalStatus = UShellOsalThreadResume((UShellOsal_s*) uShell->osal, thread->threadHandle);
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
    UShellItem_t item = 0;

    /* Main loop */
    while (1)
    {
        /* Clear line */
        uShellTerminalClearLine(ushell);

        /* Print the prompt */
        uShellPrintUserPrompt(ushell);

        /* Print IO */
        uShellPrintIo(ushell);

        do
        {
            /* Get data*/
            scanf("%c", &item);

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
                        USHELL_ASSERT(0);
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
 * \brief Initialize the runtime environment
 * \param uShell - uShell object
 * \param osal - osal object
 * \param hal - hal object
 * \param cfg - configuration object
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellRtEnvInit(UShell_s* const uShell,
                                   UShellOsal_s* const osal,
                                   UShellCfg_s* const cfg)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(osal != NULL);
    USHELL_ASSERT(cfg != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (osal == NULL) ||
            (cfg == NULL))
        {
            return USHELL_INVALID_ARGS_ERR;
        }

        /* Attach the configuration object */
        uShell->cfg = *cfg;

        /* Initialize the runtime environment history */
        if (uShell->cfg.historyIsEn == true)
        {
            status = uShellRtEnvFuncHistoryInit(uShell);
            if (status != USHELL_NO_ERR)
            {
                break;
            }
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
 * \brief Deinitialize the runtime environment
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
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

        /* Deinitialize the runtime environment authentication */
        uShellRtEnvFuncHistoryDeInit(uShell);

    } while (0);

    return status;
}

/**
 * \brief Initialize the runtime environment OSAL
 * \param[in] uShell - uShell object
 * \param[in] osal - osal object
 * \return UShellErr_e - error code. non-zero = an error has occurred;
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
 * \brief Deinitialize the runtime environment OSAL
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
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
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);

    /* Set parent */
    osalStatus = UShellOsalParentSet(thisOsal, NULL);
    USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    /* Flush */
    uShell->osal = NULL;
}

/**
 * \brief Initialize the runtime environment history
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
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
 * \brief Deinitialize the runtime environment history
 * \param uShell - uShell object
 * \return USHELL_NO_ERR if success, otherwise error code
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
 * \brief Clear the screen
 * \param uShell - uShell object
 * \return none
 */
static inline void uShellTerminalClearScreen(const UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    /* Clear the screen */
    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Print the command */
        status = uShellPrint(uShell, USHELL_CLEAR_SCREEN);
        USHELL_ASSERT(status == USHELL_NO_ERR);
        if (status != USHELL_NO_ERR)
        {
            break;
        }

    } while (0);
}

/**
 * \brief Clear the line
 * \param uShell - uShell object
 * \return none
 */
static inline void uShellTerminalClearLine(const UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Clear the line */
    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            USHELL_ASSERT(0);
            break;
        }

        printf("%s", USHELL_CLEAR_LINE);

    } while (0);
}

/**
 * \brief Print the prompt
 * \param uShell - uShell object
 * \return none
 */
static inline void uShellPrintUserPrompt(const UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    /* Print the prompt */
    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Print the prompt */
        printf("%s", USHELL_USER_PROMPT);

    } while (0);
}

/**
 * \brief Print IO buffer
 * \param uShell - uShell object
 */
static inline void uShellPrintIo(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    /* Print IO buffer */
    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Set symbol end of string */
        uShell->io.buffer [uShell->io.ind] = 0;

        /* Print the IO buffer */
        printf("%s", uShell->io.buffer);

    } while (0);
}

/**
 * \brief Delete the character in terminal
 * \param[in] uShell - uShell object
 * \return none
 */
static inline void uShellTerminalDelChar(const UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShellErr_e status = USHELL_NO_ERR;

    /* Delete the character */
    do
    {
        /* Check input parameters */
        if (uShell == NULL)
        {
            USHELL_ASSERT(0);
            break;
        }

        printf("%s", USHELL_DEL_CHAR);

    } while (0);
}