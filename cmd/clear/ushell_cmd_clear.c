/**
 * \file         ushell_cmd_clear.c
 * \brief        The file contains the implementation of the UShell command clear module.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_cmd_clear.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShellFs clear module.
 */
#ifndef USHELL_CMD_CLEAR_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_CMD_CLEAR_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_CMD_FS_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShellCmdClear object (base object)
 */
UShellCmdClear_s uShellCmdClear = {0};    ///< UShellCmdClear object (base object)

/**
 * \brief Execute the help command.
 * \param cmd - UShellCmd object
 * \param readSocket - UShell socket object for reading
 * \param writeSocket - UShell socket object for writing
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdClearExec(void* const cmd,
                                         UShellSocket_s* const readSocket,
                                         UShellSocket_s* const writeSocket,
                                         const int argc,
                                         char* const argv []);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShell command clear module.
 * \param [in] rootCmd - The first cmd in the list of commands to be initialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdClearInit(UShellCmd_s* rootCmd)
{

    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input */
        if (rootCmd == NULL)
        {
            USHELL_CMD_CLEAR_ASSERT(0);    // Set status to error if root command is NULL
            status = -1;                   // Set status to error if root command is NULL
            break;                         // Exit the loop
        }

        /* Initialize the UShellCmdClear object */
        memset(&uShellCmdClear, 0, sizeof(uShellCmdClear));

        /* Init the base class */
        cmdStatus = UShellCmdInit(&uShellCmdClear.cmd,
                                  USHELL_CMD_CLEAR_NAME,
                                  USHELL_CMD_CLEAR_HELP,
                                  uShellCmdClearExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_CLEAR_ASSERT(0);    // Set status to error if command initialization fails
            status = -2;                   // Set status to error if command initialization fails
            break;                         // Exit the loop
        }

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdClear.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_CLEAR_ASSERT(0);    // Set status to error if command attachment fails
            status = -3;                   // Set status to error if command attachment fails
            break;                         // Exit the loop
        }

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdClearDeinit()
{
    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Deinit the UShellCmdClear object */
        cmdStatus = UShellCmdDeinit(&uShellCmdClear.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_CLEAR_ASSERT(0);    // Set status to error if command deinitialization fails
            status = -1;                   // Set status to error if command deinitialization fails
            break;                         // Exit the loop
        }

        /* Remove the UShellCmdClear object from the list */
        memset(&uShellCmdClear, 0, sizeof(uShellCmdClear));    // Clear the UShellCmdClear object

    } while (0);

    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Execute the help command.
 * \param cmd - UShellCmd object
 * \param readSocket - UShell socket object for reading
 * \param writeSocket - UShell socket object for writing
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdClearExec(void* const cmd,
                                         UShellSocket_s* const readSocket,
                                         UShellSocket_s* const writeSocket,
                                         const int argc,
                                         char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    (void) socketStatus;                                      // Variable to store socket status

    do
    {
        /* Check input parameter */
        if ((cmd == NULL))
        {
            USHELL_CMD_CLEAR_ASSERT(0);              // Set status to error if command is NULL
            status = USHELL_CMD_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                   // Exit the loop
        }

        /* We dont need arg */
        if (argc > 0)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "clear: Invalid arguments\n ");    // Print error message to socket
            USHELL_CMD_CLEAR_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);      // Set status to error if socket write fails

            break;    // Exit the loop
        }

        /* Clear the screen */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "\033[H\033[J");                 // Print clear screen command to socket
        USHELL_CMD_CLEAR_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Set status to error if socket write fails

    } while (0);

    return status;    // Return success code
}