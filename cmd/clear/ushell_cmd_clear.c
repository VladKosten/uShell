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

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShellCmdClear object (base object)
 */
UShellCmdClear_s uShellCmdClear = {0};    ///< UShellCmdClear object (base object)

/**
 * @brief Execute the help command.
 * @param cmd - UShellCmd object
 * @param argc - number of arguments
 * @param argv - array of arguments
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdClearExec(void* const cmd,
                                         const int argc,
                                         char* const argv []);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShell  module.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdClearInit(void)
{

    /* Local variable */
    int status = 0;                                  // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {

        /* Initialize the UShellCmdClear object */
        memset(&uShellCmdClear, 0, sizeof(uShellCmdClear));

        /* Init the base class */
        cmdStatus = UShellCmdInit(&uShellCmdClear.cmd,
                                  USHELL_CMD_CLEAR_NAME,
                                  USHELL_CMD_CLEAR_HELP,
                                  uShellCmdClearExec);
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
            status = -1;    // Set status to error if command deinitialization fails
            break;          // Exit the loop
        }

        /* Remove the UShellCmdClear object from the list */
        memset(&uShellCmdClear, 0, sizeof(uShellCmdClear));    // Clear the UShellCmdClear object

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
static UShellCmdErr_e uShellCmdClearExec(void* const cmd,
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
            printf("clear: Invalid arguments\r\n ");    // Print error message for invalid arguments
            break;                                      // Exit the loop
        }

        /* Clear the screen */
        printf("\033[H\033[J");    // ANSI escape code to clear the screen

    } while (0);

    return status;    // Return success code
}