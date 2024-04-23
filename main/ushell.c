/**
* \file         UShell.c
* \brief        The file contains the implementation of the UShell main module. The module is responsible for the initialization of the UShell
*               and the main loop of the UShell. The UShell is a simple command line interface that allows the user to interact with the system via
*               the serial port.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright
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
    USHELL_ASCII_CONTROL_CHAR_NUL = 0x00,    ///< Null  (Null character)
    USHELL_ASCII_CONTROL_CHAR_SOH = 0x01,    ///< Start of Heading  ()
    USHELL_ASCII_CONTROL_CHAR_STX = 0x02,    ///< Start of Text
    USHELL_ASCII_CONTROL_CHAR_ETX = 0x03,    ///< End of Text
    USHELL_ASCII_CONTROL_CHAR_EOT = 0x04,    ///< End of Transmission
    USHELL_ASCII_CONTROL_CHAR_ENQ = 0x05,    ///< Enquiry
    USHELL_ASCII_CONTROL_CHAR_ACK = 0x06,    ///< Acknowledge
    USHELL_ASCII_CONTROL_CHAR_BEL = 0x07,    ///< Bell
    USHELL_ASCII_CONTROL_CHAR_BS = 0x08,     ///< Backspace
    USHELL_ASCII_CONTROL_CHAR_HT = 0x09,     ///< Horizontal Tab
    USHELL_ASCII_CONTROL_CHAR_LF = 0x0A,     ///< Line Feed
    USHELL_ASCII_CONTROL_CHAR_VT = 0x0B,     ///< Vertical Tab
    USHELL_ASCII_CONTROL_CHAR_FF = 0x0C,     ///< Form Feed
    USHELL_ASCII_CONTROL_CHAR_CR = 0x0D,     ///< Carriage Return
    USHELL_ASCII_CONTROL_CHAR_SO = 0x0E,     ///< Shift Out
    USHELL_ASCII_CONTROL_CHAR_SI = 0x0F,     ///< Shift In
    USHELL_ASCII_CONTROL_CHAR_DLE = 0x10,    ///< Data Link Escape
    USHELL_ASCII_CONTROL_CHAR_DC1 = 0x11,    ///< Device Control 1 (XON)
    USHELL_ASCII_CONTROL_CHAR_DC2 = 0x12,    ///< Device Control 2
    USHELL_ASCII_CONTROL_CHAR_DC3 = 0x13,    ///< Device Control 3 (XOFF)
    USHELL_ASCII_CONTROL_CHAR_DC4 = 0x14,    ///< Device Control 4
    USHELL_ASCII_CONTROL_CHAR_NAK = 0x15,    ///< Negative Acknowledge
    USHELL_ASCII_CONTROL_CHAR_SYN = 0x16,    ///< Synchronous Idle
    USHELL_ASCII_CONTROL_CHAR_ETB = 0x17,    ///< End of Transmission Block
    USHELL_ASCII_CONTROL_CHAR_CAN = 0x18,    ///< Cancel
    USHELL_ASCII_CONTROL_CHAR_EM = 0x19,     ///< End of Medium
    USHELL_ASCII_CONTROL_CHAR_SUB = 0x1A,    ///< Substitute
    USHELL_ASCII_CONTROL_CHAR_ESC = 0x1B,    ///< Escape
    USHELL_ASCII_CONTROL_CHAR_FS = 0x1C,     ///< File Separator
    USHELL_ASCII_CONTROL_CHAR_GS = 0x1D,     ///< Group Separator
    USHELL_ASCII_CONTROL_CHAR_RS = 0x1E,     ///< Record Separator
    USHELL_ASCII_CONTROL_CHAR_US = 0x1F      ///< Unit Separator

    USHELL_ASCII_CONTROL_CHAR_DEL = 0x7F     ///< Delete

}UShellAsciiControlChar_e;

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
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
*/
static void uShellRxCpltCallback(const void* const hal);

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
UShellErr_e UShellInit(UShell_s* const uShell, const UShellOsal_s* const osal,
                                               const UShellHal_s* const hal,
                                               const void* const parent,
                                               const char* const name)
{
    /* Check input parametrs */
    if((uShell == NULL) || (osal == NULL) || (hal == NULL))
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
    for(uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        uShell->cmd[i] = NULL;
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
    if(uShell == NULL)
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Check RTE state */
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    if(uShell->osal->portTable != NULL)
    {
        /* Stop the thread */
        if(uShell->osal->portTable->threadStop != NULL)
        {
            osalErr = uShell->osal->portTable->threadStop(uShell);
            USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
        }

        /* Detach worker */
        if(uShell->osal->worker != NULL)
        {
            osalErr = UShellOsalWorkerDetach(uShell->osal);
            USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
        }
    }

    /* Deinitialize the commands */
    for(uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        if(uShell->cmd[i] != NULL)
        {
            UShellCmdDeInit(uShell->cmd[i]);
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
    if(uShell == NULL)
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Flush event */
    UShellOsalErr_e osalErr = UShellOsalEventFlush(uShell->osal);
    if(osalErr != USHELL_OSAL_NO_ERR)
    {
        return USHELL_PORT_ERR;
    }

    /* Start thread */
    osalErr = UShellOsalThreadStart(uShell->osal);
    if(osalErr != USHELL_OSAL_NO_ERR)
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
    if(uShell == NULL)
    {
        return USHELL_INVALID_ARGS_ERR;
    }

    /* Check RTE state */
    UShellOsalErr_e osalErr = UShellOsalThreadStop(uShell->osal);
    if(osalErr != USHELL_OSAL_NO_ERR)
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
    if((uShell == NULL) || (cmd == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;     ///< Exit: Invalid arguments
    }

    /* Attach command */
    for(uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        if(uShell->cmd[i] == NULL)
        {
            uShell->cmd[i] = cmd;
            return USHELL_NO_ERR;           ///< Exit: Success
        }
    }

    return USHELL_CMD_SPACE_ERR;            ///< Exit: No space for the command
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
    if((uShell == NULL) || (cmd == NULL))
    {
        return USHELL_INVALID_ARGS_ERR;     ///< Exit: Invalid arguments
    }

    /* Detach command */
    for(uint8_t i = 0; i < USHELL_MAX_CMD; i++)
    {
        if(uShell->cmd[i] == cmd)
        {
            uShell->cmd[i] = NULL;
            return USHELL_NO_ERR;           ///< Exit: Success
        }
    }

    return USHELL_NO_ERR;       ///< Exit: Command not found
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
    UShell_s* const ushell = (UShell_s*)uShell;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    (void) osalErr;
    UShellHalErr_e halErr = USHELL_HAL_NO_ERR;
    (void) halErr;
    UShellOsalMsg_e msg = USHELL_OSAL_MSG_NONE;
    UShellItem_t item = 0;

    /* Check RTE state */
    USHELL_ASSERT(ushell->hal != NULL);
    USHELL_ASSERT(ushell->hal->rxReceivedCb != NULL);
    USHELL_ASSERT(ushell->hal->txCompleteCb != NULL);
    USHELL_ASSERT(ushell->hal->rxTxErrorCb != NULL);
    USHELL_ASSERT(ushell->osal != NULL);
    USHELL_ASSERT(ushell->osal->worker != NULL);

    /* Main loop */
    while(1)
    {
        /* Wait msg */
        do
        {
            osalErr = UShellOsalMsgGet(ushell->osal, &msg, USHELL_OSAL_WAIT_FOREVER);
            USHELL_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
        } while (msg != USHELL_OSAL_MSG_RX_RECEIVED);

        /* Get the data */
        halErr = ushell->hal->portTable->receieve(ushell->hal, &item);
        USHELL_ASSERT(halErr == USHELL_HAL_NO_ERR);

        /* Process the data */

    }
}


/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
*/
static void uShellRxCpltCallback(const void* const hal);

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