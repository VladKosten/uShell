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
    USHELL_ASCII_CHAR_ESC = 0x1B,      ///< Escape
} UShellAsciiChar_e;

/**
 * \brief ANSI escape codes for terminal control.
 *
 * This section defines several ANSI escape codes that are used to control
 * the terminal, such as clearing the screen, moving the cursor, etc.
 */
#define USHELL_CLEAR_SCREEN "\033[2J\033[1;1H"    ///< Clear screen command
#define USHELL_CLEAR_LINE   "\033[2K\r"           ///< Clear line command with carriage return
#define USHELL_DEL_CHAR     "\b \b"               ///< Delete character command

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
 * \note This function is the main loop of the UShell module.
 *       It is responsible for the processing of the commands and the interaction with the user.
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
 * @brief Find cmd by name
 * @param[in] uShell - uShell object
 * @param[in] cmdName - command name
 * @return UShellCmd_s* - error code. non-zero = an error has occurred;
 */
static UShellCmd_s* uShellCmdFindByName(UShell_s* const uShell,
                                        const char* const cmdName);

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
 * @brief Io flush function
 * @param[in] uShell - uShell object
 * @return none
 */
static void uShellIoFlush(UShell_s* const uShell);

/**
 * @brief Print the string
 * @param[in] uShell - the uShell object
 * @param[in] str - string to be printed
 * @return none
 */
static void uShellPrintStr(UShell_s* const uShell,
                           const char* const str);

/**
 * @brief Print the char
 * @param[in] uShell - the uShell object
 * @param[in] ch - char to be printed
 * @return none
 */
static void uShellPrintChar(UShell_s* const uShell,
                            const char ch);
/**
 * @brief Add cmd to history
 * @param[in] uShell - the uShell object
 * @return none
 */
static void uShellHistoryCmdAdd(UShell_s* const uShell);

/**
 * @brief Get previous cmd from history
 * @param[in] uShell - the uShell object
 * @return none
 */
static void uShellHistoryPrevCmdGet(UShell_s* const uShell);

/**
 * @brief Get next cmd from history
 * @param[in] uShell - the uShell object
 * @return none
 */
static void uShellHistoryNextCmdGet(UShell_s* const uShell);

/**
 * @brief Delay in milliseconds
 * @param[in] uShell - uShell object
 * @param[in] delayMs - delay in milliseconds
 */
static inline void uShellDelayMs(const UShell_s* const uShell,
                                 const uint32_t delayMs);

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
        uShell->fsmState = USHELL_STATE_INIT;

        /* Clear screen */
        uShellPrintStr(uShell, USHELL_CLEAR_SCREEN);

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
    UShellOsalThreadHandle_t thread = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) uShell->osal;
    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (osal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Find the thread handle */
        osalStatus = UShellOsalThreadHandleGet(osal, 0U, &thread);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Start the thread */
        osalStatus = UShellOsalThreadResume(osal, thread);
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
    UShellOsalThreadHandle_t thread = NULL;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) uShell->osal;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (osal == NULL))
        {
            status = USHELL_INVALID_ARGS_ERR;
            break;
        }

        /* Find the thread handle */
        osalStatus = UShellOsalThreadHandleGet(osal, 0U, &thread);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            status = USHELL_PORT_ERR;
            break;
        }

        /* Start the thread */
        osalStatus = UShellOsalThreadResume(osal, thread);
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
    /* Check input */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;
    uint8_t attachFlag = 0;

    do
    {
        /* Check input parameters */
        if ((uShell == NULL) ||
            (cmd == NULL))
        {
            return USHELL_INVALID_ARGS_ERR;    ///< Exit: Invalid arguments
        }

        /* lock */
        uShellLock(uShell);

        /* Attach command */

        for (uint8_t i = 0; i < USHELL_MAX_CMD; i++)
        {
            if (uShell->cmd [i] == NULL)
            {
                uShell->cmd [i] = cmd;
                attachFlag = 1;
                break;
            }
        }

        /* unlock */
        uShellUnlock(uShell);

        /* Check if command is attached */
        if (attachFlag == 0)
        {
            return USHELL_CMD_ERR;    ///< Exit: Command not attached
        }

    } while (0);

    return status;    ///< Exit: Success
}

/**
 * \brief Detach command from uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be detached
 * \param[out] none
 */
UShellErr_e UShellCmdDetach(UShell_s* const uShell, const UShellCmd_s* const cmd)
{

    /* Check input */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellErr_e status = USHELL_NO_ERR;
    uint8_t detachFlag = 0;

    do
    {
        /* Check input parameters */
        if ((uShell == NULL) ||
            (cmd == NULL))
        {
            return USHELL_INVALID_ARGS_ERR;    ///< Exit: Invalid arguments
        }

        /* LOCK */
        uShellLock(uShell);

        /* Detach command */
        for (uint8_t i = 0; i < USHELL_MAX_CMD; i++)
        {
            if (uShell->cmd [i] == cmd)
            {

                uShell->cmd [i] = NULL;
                detachFlag = 1;
                break;
            }
        }

        /* UNLOCK */
        uShellUnlock(uShell);

        /* Check if command is detached */
        if (detachFlag == 0)
        {
            return USHELL_CMD_ERR;    ///< Exit: Command not detached
        }

    } while (0);

    return status;    ///< Exit: Command not found
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
    UShell_s* ushell = (UShell_s*) uShell;
    UShellVcp_s* vcp = (UShellVcp_s*) ushell->vcp;
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;
    UShellItem_t item = 0;

    /* Main loop */
    while (1)
    {
        /* Little delay */
        uShellDelayMs(ushell, 100U);

        /* Check the state */
        switch (ushell->fsmState)
        {
            /* Initial state */
            case USHELL_STATE_INIT :
            {

                do
                {

                    /* Clear line */
                    uShellPrintStr(ushell, USHELL_CLEAR_LINE);

                    /* Print header */
                    uShellPrintStr(ushell, USHELL_HELLO_MSG);

                    /* Check we have any input symbol */
                    vcpStatus = UShellVcpScanCharNonBlock(vcp, &item);
                    if (vcpStatus != USHELL_VCP_NO_ERR)
                    {
                        break;
                    }

                    /* Check the input, if it is not CR or LF */
                    if (!(item == USHELL_ASCII_CHAR_CR) &&
                        !(item == USHELL_ASCII_CHAR_LF))
                    {
                        break;
                    }

                    /* Flush the io */
                    uShellIoFlush(ushell);

                    /* Change the state */
                    ushell->fsmState = (ushell->cfg.authIsEn == true)
                                           ? USHELL_STATE_AUTH
                                           : USHELL_STATE_PROC_INP;

                } while (1);

                break;
            }

            /* Authentication state */
            case USHELL_STATE_AUTH :
            {
                do
                {
                    /* Clear line */
                    uShellPrintStr(ushell, USHELL_CLEAR_LINE);

                    /* Print the password prompt */
                    uShellPrintStr(ushell, USHELL_PASSWORD_PROMPT);

                    /* Print the IO */
                    uShellPrintStr(ushell, ushell->io.buffer);

                    /* Check we have any input symbol */
                    vcpStatus = UShellVcpScanCharNonBlock(vcp, &item);
                    if (vcpStatus != USHELL_VCP_NO_ERR)
                    {
                        break;
                    }

                    /* Check the input */
                    switch (item)
                    {
                        /* Carriage return (\r) */
                        case USHELL_ASCII_CHAR_CR :
                        case USHELL_ASCII_CHAR_LF :
                        {
                            /* Check password */
                            int cmpRes = strcmp(ushell->io.buffer, USHELL_AUTH_PASSWORD);
                            if (cmpRes == 0)
                            {
                                /* Print succ msg */
                                uShellPrintStr(ushell, USHELL_AUTH_OK_MSG);

                                /* Clear io */
                                uShellIoFlush(ushell);

                                /* Change the state */
                                ushell->fsmState = USHELL_STATE_PROC_INP;
                            }
                            else
                            {
                                /* Print fail msg */
                                uShellPrintStr(ushell, USHELL_AUTH_FAIL_MSG);
                            }

                            break;
                        }

                        /* Backspace  */
                        case USHELL_ASCII_CHAR_BS :
                        case USHELL_ASCII_CHAR_DEL :
                        {
                            /* Process the command */
                            uShellPrintStr(ushell, USHELL_DEL_CHAR);

                            /* Remove the last char */
                            if (ushell->io.ind > 0)
                            {
                                ushell->io.buffer [--ushell->io.ind] = 0;
                            }

                            break;
                        }

                        /* Acknowledge */
                        default :
                        {
                            do
                            {
                                /* Store the data */
                                if (ushell->io.ind >= strlen(USHELL_AUTH_PASSWORD))
                                {
                                    break;
                                }

                                /* Only accept alphanumeric characters */
                                if (!(item >= '0' && item <= '9') &&
                                    !(item >= 'A' && item <= 'Z') &&
                                    !(item >= 'a' && item <= 'z'))
                                {
                                    break;
                                }

                                ushell->io.buffer [ushell->io.ind++] = item;

                            } while (0);

                            break;
                        }
                    }

                    /* Check current state */
                    if (ushell->fsmState != USHELL_STATE_AUTH)
                    {
                        break;
                    }

                } while (1);

                break;
            }

            /* Processing input state */
            case USHELL_STATE_PROC_INP :
            {

                do
                {
                    /* Clear line */
                    uShellPrintStr(ushell, USHELL_CLEAR_LINE);

                    /* Print the prompt */
                    uShellPrintStr(ushell, USHELL_USER_PROMPT);

                    /* Print IO */
                    uShellPrintStr(ushell, ushell->io.buffer);

                    /* Get data*/
                    vcpStatus = UShellVcpScanCharNonBlock(vcp, &item);
                    if (vcpStatus != USHELL_VCP_NO_ERR)
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
                            uShellHistoryCmdAdd(ushell);

                            /* Find cmd */
                            ushell->currCmd = uShellCmdFindByName(ushell, ushell->io.buffer);

                            /* Flush the io */
                            uShellIoFlush(ushell);

                            if (ushell->currCmd == NULL)
                            {
                                /* Print error msg */
                                uShellPrintStr(ushell, USHELL_CMD_NOT_FOUND_MSG);
                                break;
                            }

                            /* Change state to proc cmd */
                            ushell->fsmState = USHELL_STATE_PROC_CMD;

                            break;
                        }

                        case USHELL_ASCII_CHAR_ESC :
                        {
                            /* Change state to proc esc */
                            ushell->fsmState = USHELL_STATE_PROC_ESC_SEQ;
                            break;
                        }

                        /* Backspace  */
                        case USHELL_ASCII_CHAR_BS :
                        case USHELL_ASCII_CHAR_DEL :
                        {
                            /* Remove the last char */
                            if (ushell->io.ind > 0)
                            {
                                ushell->io.buffer [--ushell->io.ind] = 0;
                            }

                            /* Process the command */
                            uShellPrintStr(ushell, USHELL_DEL_CHAR);

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

                    /* Check current state */
                    if (ushell->fsmState != USHELL_STATE_PROC_INP)
                    {
                        break;
                    }
                } while (1);

                break;
            }

            /* Processing command state */
            case USHELL_STATE_PROC_CMD :
            {

                do
                {
                    /* Add to history */
                    uShellHistoryCmdAdd(ushell);

                    /* Flush the io */
                    uShellIoFlush(ushell);

                    /* Change state to input */
                    ushell->fsmState = USHELL_STATE_PROC_INP;

                } while (0);

                break;
            }

            /* Processing escape state */
            case USHELL_STATE_PROC_ESC_SEQ :
            {

                do
                {
                    /* Get data*/
                    vcpStatus = UShellVcpScanCharNonBlock(vcp, &item);
                    if (vcpStatus != USHELL_VCP_NO_ERR)
                    {
                        item = 0;
                    }

                    /* Process the data */
                    switch (item)
                    {
                        case '[' :
                        {
                            /* All ok go next*/
                            break;
                        }

                        /* Arrow down */
                        case 'A' :
                        {
                            /* Get prev cmd */
                            uShellHistoryPrevCmdGet(ushell);

                            /* Change state to proc input */
                            ushell->fsmState = USHELL_STATE_PROC_INP;
                            break;
                        }

                        case 'B' :
                        {
                            /* Get next cmd */
                            uShellHistoryNextCmdGet(ushell);

                            /* Change state to proc input */
                            ushell->fsmState = USHELL_STATE_PROC_INP;

                            break;
                        }

                        default :
                        {
                            /* Flush the io */
                            uShellIoFlush(ushell);

                            /* Clear screen */
                            uShellPrintStr(ushell, USHELL_CLEAR_SCREEN);

                            /* Change state to init */
                            ushell->fsmState = USHELL_STATE_INIT;

                            break;
                        }
                    }

                    /* Check if we are in the escape state */
                    if (ushell->fsmState != USHELL_STATE_PROC_ESC_SEQ)
                    {
                        break;
                    }

                } while (1);

                break;
            }

            /* Error state */
            case USHELL_STATE_ERROR :
            {
                /* Clear screen */
                uShellPrintStr(ushell, USHELL_CLEAR_SCREEN);

                /* Flush the io */
                uShellIoFlush(ushell);

                /* Change state to init */
                ushell->fsmState = USHELL_STATE_INIT;

                break;
            }
        }
    }
}

/**
 * \brief Find command
 * \param[in] uShell - uShell object
 * \param[in] str - string to be found
 * \param[out] ind - index of the command
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellFindCmd(const UShell_s* const uShell,
                                 const char* const str,
                                 uint8_t* const ind)
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

    /* Clear the object */
    return USHELL_NO_ERR;
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
 * @brief Find cmd by name
 * @param[in] uShell - uShell object
 * @param[in] cmdName - command name
 * @return UShellCmd_s* - error code. non-zero = an error has occurred;
 */
static UShellCmd_s* uShellCmdFindByName(UShell_s* const uShell,
                                        const char* const cmdName)
{
    /* Check input parameters */
    return NULL;
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
 * @brief Io flush function
 * @param[in] uShell - uShell object
 * @return none
 */
static void uShellIoFlush(UShell_s* const uShell)
{
    /* Check input parameters */
    USHELL_ASSERT(uShell != NULL);

    memset(uShell->io.buffer, 0, USHELL_BUFFER_SIZE);
    uShell->io.ind = 0;
}

/**
 * @brief Print the string
 * @param[in] uShell - the uShell object
 * @param[in] str - string to be printed
 * @return none
 */
static void uShellPrintStr(UShell_s* const uShell,
                           const char* const str)
{
    /* Check inp */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(str != NULL);

    /* Local variable */
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;
    UShellVcp_s* vcp = (UShellVcp_s*) uShell->vcp;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (str == NULL) ||
            (vcp == NULL))
        {
            break;
        }

        /* Print */
        vcpStatus = UShellVcpPrintStr(vcp, str);
        USHELL_ASSERT(vcpStatus == USHELL_VCP_NO_ERR);
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            break;
        }

    } while (0);
}

/**
 * @brief Print the char
 * @param[in] uShell - the uShell object
 * @param[in] ch - char to be printed
 * @return none
 */
static void uShellPrintChar(UShell_s* const uShell,
                            const char ch)
{
    /* Check inp */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(ch != NULL);

    /* Local variable */
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;
    UShellVcp_s* vcp = (UShellVcp_s*) uShell->vcp;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (vcp == NULL))
        {
            break;
        }

        /* Print */
        vcpStatus = UShellVcpPrintChar(vcp, ch);
        USHELL_ASSERT(vcpStatus == USHELL_VCP_NO_ERR);
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            break;
        }

    } while (0);
}

/**
 * @brief Add cmd to history
 * @param[in] uShell - the uShell object
 * @return none
 */
static void uShellHistoryCmdAdd(UShell_s* const uShell)
{
    /* Check inp */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellHistoryErr_e hiStoryStatus = USHELL_HISTORY_NO_ERR;
    UShellHistory_s* history = (UShellHistory_s*) &uShell->history;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (history == NULL))
        {
            break;
        }

        /* Check history is enabled */
        if (uShell->cfg.historyIsEn == false)
        {
            break;
        }

        /* Add to history */
        hiStoryStatus = UShellHistoryAdd(history, uShell->io.buffer);
        USHELL_ASSERT(hiStoryStatus == USHELL_HISTORY_NO_ERR);
        if (hiStoryStatus != USHELL_HISTORY_NO_ERR)
        {
            break;
        }

    } while (0);
}

/**
 * @brief Get previous cmd from history
 * @param[in] uShell - the uShell object
 * @return none
 */
static void uShellHistoryPrevCmdGet(UShell_s* const uShell)
{
    /* Check inp */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellHistoryErr_e hiStoryStatus = USHELL_HISTORY_NO_ERR;
    UShellHistory_s* history = (UShellHistory_s*) &uShell->history;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (history == NULL))
        {
            break;
        }

        /* Check history is enabled */
        if (uShell->cfg.historyIsEn == false)
        {
            break;
        }

        /* Flush io */
        uShellIoFlush(uShell);

        /* Get previous cmd */
        hiStoryStatus = UShellHistoryCmdPrevGet(history,
                                                uShell->io.buffer,
                                                USHELL_BUFFER_SIZE);
        USHELL_ASSERT(hiStoryStatus == USHELL_HISTORY_NO_ERR);
        if (hiStoryStatus != USHELL_HISTORY_NO_ERR)
        {
            break;
        }

        /* Update the index */
        uShell->io.ind = strlen(uShell->io.buffer);

    } while (0);
}

/**
 * @brief Get next cmd from history
 * @param[in] uShell - the uShell object
 * @return none
 */
static void uShellHistoryNextCmdGet(UShell_s* const uShell)
{
    /* Check inp */
    USHELL_ASSERT(uShell != NULL);

    /* Local variable */
    UShellHistoryErr_e hiStoryStatus = USHELL_HISTORY_NO_ERR;
    UShellHistory_s* history = (UShellHistory_s*) &uShell->history;

    do
    {
        /* Check input parameter */
        if ((uShell == NULL) ||
            (history == NULL))
        {
            break;
        }

        /* Check history is enabled */
        if (uShell->cfg.historyIsEn == false)
        {
            break;
        }

        /* Flush io */
        uShellIoFlush(uShell);

        /* Get next cmd */
        hiStoryStatus = UShellHistoryCmdNextGet(history,
                                                uShell->io.buffer,
                                                USHELL_BUFFER_SIZE);
        USHELL_ASSERT(hiStoryStatus == USHELL_HISTORY_NO_ERR);
        if (hiStoryStatus != USHELL_HISTORY_NO_ERR)
        {
            break;
        }

        /* Update the index */
        uShell->io.ind = strlen(uShell->io.buffer);

    } while (0);
}

/**
 * @brief Delay in milliseconds
 * @param[in] uShell - uShell object
 * @param[in] delayMs - delay in milliseconds
 */
static inline void uShellDelayMs(const UShell_s* const uShell,
                                 const uint32_t delayMs)
{
    /* Check input */
    USHELL_ASSERT(uShell != NULL);
    USHELL_ASSERT(delayMs > 0U);

    /* Local variables */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) uShell->osal;

    do
    {
        /* Check input */
        if ((uShell == NULL) ||
            (osal == NULL))
        {
            USHELL_ASSERT(0);
            break;
        }

        /* Delay */
        osalStatus = UShellOsalThreadDelay(osal, delayMs);
        USHELL_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

    } while (0);
}