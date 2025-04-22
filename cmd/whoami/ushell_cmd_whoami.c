/**
 * \file         ushell_cmd_whoami.c
 * \brief        The file contains the implementation of the UShell command clear module.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_cmd_whoami.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================
/**
 * \brief Assert macro for the whoami module.
 */
#ifndef USHELL_CMD_WHOAMI_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_CMD_WHOAMI_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_CMD_FS_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShellCmdWhoAmI object (base object)
 */
UShellCmdWhoAmI_s uShellCmdWhoAmI = {0};    ///< UShellCmdWhoAmI object (base object)

/**
 * \brief Execute the whoami command.
 * \param cmd - UShellCmd object
 * \param readSocket - UShell socket object for reading
 * \param writeSocket - UShell socket object for writing
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdWhoAmIExec(void* const cmd,
                                          UShellSocket_s* const readSocket,
                                          UShellSocket_s* const writeSocket,
                                          const int argc,
                                          char* const argv []);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShell  whoami module.
 * \param [in] rootCmd - The first cmd in the list of commands to be initialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdWhoAmIInit(UShellCmd_s* rootCmd)
{
    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input */
        if (rootCmd == NULL)
        {
            USHELL_CMD_WHOAMI_ASSERT(0);    // Set status to error if root command is NULL
            status = -1;                    // Set status to error if root command is NULL
            break;                          // Exit the loop
        }

        /* Initialize the UShellCmdWhoAmI object */
        memset(&uShellCmdWhoAmI, 0, sizeof(uShellCmdWhoAmI));

        /* Init the base class */
        cmdStatus = UShellCmdInit(&uShellCmdWhoAmI.cmd,
                                  USHELL_CMD_WHOAMI_NAME,
                                  USHELL_CMD_WHOAMI_HELP,
                                  uShellCmdWhoAmIExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_WHOAMI_ASSERT(0);    // Set status to error if command initialization fails
            status = -2;                    // Set status to error if command initialization fails
            break;                          // Exit the loop
        }

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdWhoAmI.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_WHOAMI_ASSERT(0);    // Set status to error if command attachment fails
            status = -3;                    // Set status to error if command attachment fails
            break;                          // Exit the loop
        }

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Deinitialize the UShell cmd
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdWhoAmIDeinit()
{
    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Deinit the UShellCmdWhoAmI object */
        cmdStatus = UShellCmdDeinit(&uShellCmdWhoAmI.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_WHOAMI_ASSERT(0);    // Set status to error if command deinitialization fails
            status = -1;                    // Set status to error if command deinitialization fails
            break;                          // Exit the loop
        }

        /* Remove the UShellCmdWhoAmI object from the list */
        memset(&uShellCmdWhoAmI, 0, sizeof(uShellCmdWhoAmI));    // WhoAmI the UShellCmdWhoAmI object

    } while (0);

    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Execute the whoami command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdWhoAmIExec(void* const cmd,
                                          UShellSocket_s* const readSocket,
                                          UShellSocket_s* const writeSocket,
                                          const int argc,
                                          char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    (void) socketStatus;                                      // Unused variable

    do
    {
        /* Check input parameter */
        if ((cmd == NULL) ||
            (cmd != &uShellCmdWhoAmI.cmd))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                   // Exit the loop
        }

        /* We dont need arg */
        if (argc > 0)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "whoiam : Invalid arguments\n ");    // Print error message for invalid arguments
            USHELL_CMD_WHOAMI_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);       // Set status to error if socket write fails
            break;                                                                // Exit the loop
        }

        /* WhoAmI the screen */
        /* WhoAmI the screen */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "Device: %s\n"
                                         "Version: %s\n",
                                         USHELL_CMD_WHOAMI_DEVICE_NAME,
                                         USHELL_CMD_WHOAMI_DEVICE_VERSION);    // Print device name and version
        USHELL_CMD_WHOAMI_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);        // Set status to error if socket write fails

    } while (0);

    return status;    // Return success code
}