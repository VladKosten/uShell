/**
 * \file         ushell_cmd_help.c
 * \brief        The file contains the implementation of the UShell command help module.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_cmd_help.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================
/**
 * \brief Assert macro for the UShellFs module.
 */
#ifndef USHELL_CMD_HELP_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_CMD_HELP_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_CMD_FS_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShellCmdHelp object (base object)
 */
UShellCmdHelp_s uShellCmdHelp = {0};    ///< UShellCmdHelp object (base object)

/**
 * \brief Execute the help command.
 * \param cmd - UShellCmd object
 * \param readSocket - UShell socket object for reading
 * \param writeSocket - UShell socket object for writing
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdHelpExec(void* const cmd,
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
                                        const int argc,
                                        char* const argv []);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShell  module.
 * \param [in] rootCmd - The first cmd in the list of commands to be initialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdHelpInit(UShellCmd_s* rootCmd)
{

    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input parameter */
        if (rootCmd == NULL)
        {
            USHELL_CMD_HELP_ASSERT(0);    // Set status to error if rootCmd is NULL
            status = -1;                  // Set status to error if rootCmd is NULL
            break;                        // Exit the loop
        }

        /* Initialize the UShellCmdHelp object */
        memset(&uShellCmdHelp, 0, sizeof(uShellCmdHelp));

        /* Init the base class */
        cmdStatus = UShellCmdInit(&uShellCmdHelp.cmd,
                                  USHELL_CMD_HELP_NAME,
                                  USHELL_CMD_HELP_HELP,
                                  uShellCmdHelpExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_HELP_ASSERT(0);    // Set status to error if command initialization fails
            status = -2;                  // Set status to error if command initialization fails
            break;                        // Exit the loop
        }

        /* Set the attributes */
        uShellCmdHelp.rootCmd = rootCmd;    // Set the root command

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] cmd - UShellCmd obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdHelpDeinit()
{
    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Deinit the UShellCmdHelp object */
        cmdStatus = UShellCmdDeinit(&uShellCmdHelp.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_HELP_ASSERT(0);
            status = -1;    // Set status to error if command deinitialization fails
            break;          // Exit the loop
        }

        /* Remove the UShellCmdHelp object from the list */
        memset(&uShellCmdHelp, 0, sizeof(uShellCmdHelp));    // Clear the UShellCmdHelp object

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
static UShellCmdErr_e uShellCmdHelpExec(void* const cmd,
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    (void) socketStatus;                                      // Suppress unused variable warning

    do
    {
        /* Check input parameter */
        if ((cmd == NULL) ||
            (cmd != &uShellCmdHelp.cmd))
        {
            USHELL_CMD_HELP_ASSERT(0);               // Set status to error if command is NULL
            status = USHELL_CMD_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                   // Exit the loop
        }

        /* We dont need arg */
        if (argc > 0)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "help: Invalid arguments\n ");    // Print error message to socket
            USHELL_CMD_HELP_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);      // Assert if socket status is not OK
            break;                                                             // Exit the loop
        }

        /* Go to all commands */
        UShellCmd_s* currCmd = (UShellCmd_s*) uShellCmdHelp.rootCmd;    // Set current command to root command
        if (currCmd == NULL)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "help: No commands\n ");        // Print error message to socket
            USHELL_CMD_HELP_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Assert if socket status is not OK
            break;                                                           // Exit the loop
        }

        while (currCmd != NULL)
        {
            /* Print command name and help */
            socketStatus = UShellSocketPrint(writeSocket,
                                             "    %s: %s\n",
                                             currCmd->name,
                                             currCmd->help);                 // Print command name and help message
            USHELL_CMD_HELP_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Assert if socket status is not OK
            /* Move to the next command */
            currCmd = currCmd->next;    // Set current command to next command
        }

    } while (0);

    return status;    // Return success code
}