/**
* \file         ushell.c
* \brief        The file contains the implementation of the UShell main module. The module is responsible for the initialization of the UShell
*               and the main loop of the UShell. The UShell is a simple command line interface that allows the user to interact with the system via
*               the serial port.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright    MIT License (c) 2021
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...

*/
//===============================================================================[ INCLUDE ]========================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* Project includes */
#include "ushell.h"
#include "ushell_hal.h"
#include "ushell_osal.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_ASSERT
    #define USHELL_ASSERT(cond) assert(cond)
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

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShell thread worker
 * \param[in] uShell - uShell object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellThreadWorker(void* const uShell);

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
static UShellErr_e uShellPrint(const UShell_s* const uShell, const char* const str);

/**
 * \brief Find command
 * \param[in] uShell - uShell object
 * \param[in] str - string to be found
 * \param[out] ind - index of the command
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellFindCommand(const UShell_s* const uShell, const char* const str, uint8_t* const ind);

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
UShellErr_e UShellInit(UShell_s* const uShell, const UShellOsal_s* const osal, const UShellHal_s* const hal, const void* const parent, const char* const name)
{
    /* Check input parametrs */
    if ((uShell == NULL) || (osal == NULL) || (hal == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Set the parent object */
    uShell->parent = parent;

    /* Set the name of the object */
    uShell->name = name;

    /* Set the osal object */
    uShell->osal = osal;

    /* Set the hal object */
    uShell->hal = hal;

    /* Initialize the commands */
    for (uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        uShell->cmd [i] = NULL;
    }

    /* Attach worker */
    UShellOsalErr_e osalErr = UShellOsalWorkerAttach(uShell->osal, uShellThreadWorker);
    USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);

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
    /* Check input parametrs */
    if (uShell == NULL)
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Check RTE state */
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    if (uShell->osal->portTable != NULL)
    {
        /* Stop the thread */
        if (uShell->osal->portTable->threadStop != NULL)
        {
            osalErr = uShell->osal->portTable->threadStop(uShell);
            USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
        }

        /* Detach worker */
        if (uShell->osal->worker != NULL)
        {
            osalErr = UShellOsalWorkerDetach(uShell->osal);
            USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
        }
    }

    /* Deinitialize the commands */
    for (uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        if (uShell->cmd [i] != NULL)
        {
            UShellCmdDeInit(uShell->cmd [i]);
        }
    }

    /* Remove obj */
    uShell->parent = NULL;
    uShell->name = NULL;
    uShell->osal = NULL;
    uShell->hal = NULL;

    return USHELL_NO_ERR;
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
    /* Check input parametrs */
    if (uShell == NULL)
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Flush event */
    UShellOsalErr_e osalErr = UShellOsalEventFlush(uShell->osal);
    if (osalErr != USHELL_OSAL_NO_ERR)
    {
        return USHELL_PORT_ERR;
    }

    /* Start thread */
    osalErr = UShellOsalThreadStart(uShell->osal);
    if (osalErr != USHELL_OSAL_NO_ERR)
    {
        return USHELL_PORT_ERR;
    }

    return USHELL_NO_ERR;
}

/**
 * \brief Stop uShell object
 * \param[in] uShell - uShell object to be stopped
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellStop(UShell_s* const uShell)
{
    /* Check input parametrs */
    if (uShell == NULL)
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Check RTE state */
    UShellOsalErr_e osalErr = UShellOsalThreadStop(uShell->osal);
    if (osalErr != USHELL_OSAL_NO_ERR)
    {
        return USHELL_PORT_ERR;
    }

    return USHELL_NO_ERR;
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
    /* Check input parametrs */
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
    /* Check input parametrs */
    if ((uShell == NULL) || (cmd == NULL))
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
static void uShellThreadWorker(void* const uShell)
{
    /* Check input parametrs */
    USHELL_ASSERT(uShell != NULL);

    /* Local variables */
    UShell_s* const ushell = (UShell_s*) uShell;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    (void) osalErr;
    UShellHalErr_e halErr = USHELL_HAL_NO_ERR;
    (void) halErr;
    UShellItem_t item = 0;

    /* Check RTE state */
    USHELL_ASSERT(ushell->hal != NULL);
    USHELL_ASSERT(ushell->hal->rxReceivedCb != NULL);
    USHELL_ASSERT(ushell->hal->txCompleteCb != NULL);
    USHELL_ASSERT(ushell->hal->rxTxErrorCb != NULL);
    USHELL_ASSERT(ushell->osal != NULL);
    USHELL_ASSERT(ushell->osal->worker != NULL);

    /* First msg */

    /* Main loop */
    while (1)
    {

        /* Wait msg */
        do
        {
            osalErr = UShellOsalMsgGet(ushell->osal, &msg, USHELL_OSAL_WAIT_FOREVER);

        } while (msg != USHELL_OSAL_MSG_RX_RECEIVED && osalErr != USHELL_OSAL_NO_ERR);

        /* Get data*/
        halErr = UShellHalReceive(ushell->hal, &item);
        USHELL_ASSERT(halErr == USHELL_HAL_NO_ERR);

        /* Process the data */
        switch (item)
        {
            /* Carriage return (\r) */
            case USHELL_ASCII_CHAR_CR :
            case USHELL_ASCII_CHAR_LF :
            {
                /* Next row */
                vt100Err = UShellVt100CursorRowNext(&ushell->vt100);
                if (vt100Err != USHELL_VT100_NO_ERR)
                {
                    break;
                }

                /* Find command */
                uint8_t ind = 0;
                UShellErr_e ushellErr = uShellFindCommand(ushell, ushell->io.buffer, &ind);
                if (ushellErr != USHELL_NO_ERR)
                {
                    break;
                }

                /* Execute command */
                UShellCmdErr_e cmdErr = UShellCmdExecute(ushell->cmd [ind], 0, NULL);

                break;
            }

            /* Backspace  */
            case USHELL_ASCII_CHAR_BS :
            case USHELL_ASCII_CHAR_DEL :
            {
                /* Process the command */
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

                /* Maybe escape sequence */
                if ((ushell->io.ind >= USHELL_VT100_ESCAPE_SEQUENCE_SIZE_MIN) &&
                    (ushell->io.ind <= USHELL_VT100_ESCAPE_SEQUENCE_SIZE_MAX))
                {
                    vt100Err = UShellVt100ParseEscapeSequency(&ushell->vt100, ushell->io.buffer, ushell->io.ind);
                    USHELL_ASSERT(vt100Err == USHELL_VT100_NO_ERR);
                }

                break;
            }
        }
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
    /* Check input parametrs */
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShell_s* const ushell = (UShell_s*) ushellHal->parent;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    (void) osalErr;

    /* Send msg */
    osalErr = UShellOsalMsgSend(ushell->osal, USHELL_OSAL_MSG_RX_RECEIVED);
    USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
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
    /* Check input parametrs */
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShell_s* const ushell = (UShell_s*) ushellHal->parent;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    (void) osalErr;

    /* Send msg */
    osalErr = UShellOsalMsgSend(ushell->osal, USHELL_OSAL_MSG_TX_COMPLETE);
    USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
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
    /* Check input parametrs */
    USHELL_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShell_s* const ushell = (UShell_s*) ushellHal->parent;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    (void) osalErr;

    /* Send msg */
    osalErr = UShellOsalMsgSend(ushell->osal, USHELL_OSAL_MSG_RX_TX_ERROR);
    USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
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
    /* Check input parametrs */
    if ((uShell == NULL) || (str == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Check RTE state */
    if ((uShell->hal == NULL) ||
        (uShell->osal == NULL))
    {
        return USHELL_NOT_INIT_ERR;
    }

    UShellHalErr_e halErr = USHELL_HAL_NO_ERR;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    UShellVt100Err_e vt100Err = USHELL_VT100_NO_ERR;
    UShellOsalMsg_e msg = USHELL_OSAL_MSG_NONE;
    /* Transmit the data */
    uint8_t ind = 0;
    char symbol = str [ind++];
    do
    {
        if ((symbol == USHELL_ASCII_CHAR_CR) ||
            (symbol == USHELL_ASCII_CHAR_LF))
        {
            vt100Err = UShellVt100CursorRowNext(&uShell->vt100);
            if (vt100Err != USHELL_VT100_NO_ERR)
            {
                break;
            }
        }

        vt100Err = UShellVt100CursorColumnNext(&uShell->vt100);
        if (vt100Err != USHELL_VT100_NO_ERR)
        {
            break;
        }

        halErr = UShellHalTransmit(uShell->hal, symbol);
        if (halErr != USHELL_HAL_NO_ERR)
        {
            break;
        }

        osalErr = UShellOsalMsgGet(uShell->osal, &msg, 50U);
        if ((osalErr != USHELL_OSAL_NO_ERR) ||
            (msg != USHELL_OSAL_MSG_TX_COMPLETE))
        {
            break;
        }

        symbol = str [ind++];

    } while (symbol != '\0');
}

/**
 * \brief Find command
 * \param[in] uShell - uShell object
 * \param[in] str - string to be found
 * \param[out] ind - index of the command
 * \return USHELL_NO_ERR if success, otherwise error code
 */
static UShellErr_e uShellFindCommand(const UShell_s* const uShell, const char* const str, uint8_t* const ind)
{
    /* Check input parametrs */
    if ((uShell == NULL) ||
        (str == NULL) ||
        (ind == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Check RTE state */
    if (uShell->cmdCount != 0)
    {
        return USHELL_NOT_INIT_ERR;
    }

    /* Find command */
    uint8_t findFlag = 0;
    for (uint8_t i = 0; i < uShell->cmdCount; i++)
    {
        if (strncmp(str, uShell->cmd [i]->name, strlen(uShell->cmd [i]->name)) == 0)
        {
            *ind = i;
            findFlag = 1;
            break;
        }
    }

    if (findFlag == 0)
    {
        return USHELL_CMD_ERR;
    }

    return USHELL_NO_ERR;
}