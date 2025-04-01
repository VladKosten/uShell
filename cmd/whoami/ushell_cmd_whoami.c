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

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShellCmdWhoAmI object (base object)
 */
UShellCmdWhoAmI_s uShellCmdWhoAmI = {0};    ///< UShellCmdWhoAmI object (base object)

/**
 * @brief Execute the whoami command.
 * @param cmd - UShellCmd object
 * @param argc - number of arguments
 * @param argv - array of arguments
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdWhoAmIExec(void* const cmd,
                                          const int argc,
                                          char* const argv []);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShell  module.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdWhoAmIInit(void)
{

    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {

        /* Initialize the UShellCmdWhoAmI object */
        memset(&uShellCmdWhoAmI, 0, sizeof(uShellCmdWhoAmI));

        /* Init the base class */
        cmdStatus = UShellCmdInit(&uShellCmdWhoAmI.cmd,
                                  USHELL_CMD_WHOAMI_NAME,
                                  USHELL_CMD_WHOAMI_HELP,
                                  uShellCmdWhoAmIExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            status = -2;    // Set status to error if command initialization fails
            break;          // Exit the loop
        }

    } while (0);

    return 0;    // Return success code
}

/**
 * \brief Deinitialize the UShell  module.
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
            status = -1;    // Set status to error if command deinitialization fails
            break;          // Exit the loop
        }

        /* Remove the UShellCmdWhoAmI object from the list */
        memset(&uShellCmdWhoAmI, 0, sizeof(uShellCmdWhoAmI));    // WhoAmI the UShellCmdWhoAmI object

    } while (0);

    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * @brief Execute the help command.
 * @param cmd - UShellCmd object
 * @param argc - number of arguments
 * @param argv - array of arguments
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdWhoAmIExec(void* const cmd,
                                          const int argc,
                                          char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input parameter */
        if ((cmd == NULL))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                   // Exit the loop
        }

        /* We dont need arg */
        if (argc > 0)
        {
            printf("whoiam : Invalid arguments\r\n ");    // Print error message for invalid arguments
            break;                                        // Exit the loop
        }

        /* WhoAmI the screen */
        printf("System Name: %s", USHELL_CMD_WHOAMI_SYSTEM_NAME);          // Print system name
        printf("System Version: %s", USHELL_CMD_WHOAMI_SYSTEM_VERSION);    // Print system version

    } while (0);

    return status;    // Return success code
}