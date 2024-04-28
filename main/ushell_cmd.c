/**
* \file         UShell_cmd.c
* \brief        The file contains the implementation of the UShell command module.
*               Object UShellCmd is a parent object for the command objects.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_cmd.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief Assert macro for the UShellHal module.
*/
#ifndef USHELL_CMD_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_CMD_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_CMD_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShell cmd  module.
 * \param [in] cmd - UShellCmd obj to be initialized
 * \param [in] argMax - maximum number of arguments of the cmd
 * \param [in] portable - portable structure for the command
 * \param [in] portTable - port table to be used
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdInit(UShellCmd_s* const cmd, size_t argMax,
                                                     const UShellCmdPortable_s* const  portable,
                                                     const char* const name,
                                                     const void* const parent)
{
    /* Check input parametr */
    if((NULL == cmd) ||
       (NULL == name) ||
       (NULL == parent) ||
       (NULL == portable))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Init the object */
    memset((void*)cmd, 0, sizeof(UShellCmd_s));

    /* Set the name */
    cmd->name = name;

    /* Set the parent */
    cmd->parent = parent;

    /* Set the maximum number of arguments */
    cmd->argMax = argMax;

    /* Set the portable structure */
    cmd->portable = portable;

    return USHELL_CMD_NO_ERR;
}

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] cmd - UShellOsal obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdDeinit(UShellCmd_s* const cmd)
{
    /* Check input parametr */
    if(NULL == cmd)
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Deinit the object */
    memset((void*)cmd, 0, sizeof(UShellCmd_s));

    return USHELL_CMD_NO_ERR;
}

/**
 * \brief Attach the print function to the UShellCmd
 * \param [in] cmd - UShellCmd obj
 * \param [in] print - pointer to the function to print a string
 * \param [out] none
 * \return UShellCmdErr_e - error code
*/
UShellCmdErr_e UShellCmdPrintCbAttach(UShellCmd_s* const cmd, const UShellCmdPrintCb_t print)
{
    /* Check input parametr */
    if((NULL == cmd) ||
       (NULL == print))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Attach the print function */
    cmd->print = print;

    return USHELL_CMD_NO_ERR;

}

/**
 * \brief Detach the print function from the UShellCmd
 * \param [in] cmd - UShellCmd obj
 * \param [out] none
 * \return UShellCmdErr_e - error code
*/
UShellCmdErr_e UShellCmdPrintCbDetach(UShellCmd_s* const cmd)
{
    /* Check input parametr */
    if(NULL == cmd)
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Detach the print function */
    cmd->print = NULL;

    return USHELL_CMD_NO_ERR;

}

/**
 * \brief Get the name of the UShell  module.
 * \param [in] cmd - UShellOsal obj
 * \param [out] name - name of the object
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdNameGet(UShellCmd_s* const cmd, char** const name)
{
    /* Check input parametr */
    if((NULL == cmd) ||
       (NULL == name))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Get the name */
    *name = cmd->name;

    return USHELL_CMD_NO_ERR;
}

/**
 * \brief Get the parent of the UShell  module.
 * \param [in] cmd - UShellOsal obj
 * \param [out] parent - pointer to the parent object
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdArgMaxGet(UShellCmd_s* const cmd, size_t* const argMax)
{
    /* Check input parametr */
    if((NULL == cmd) ||
       (NULL == argMax))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Get the parent */
    *argMax = cmd->argMax;

    return USHELL_CMD_NO_ERR;

}

/**
 * \brief Execute the UShell module.
 * \param [in] cmd - UShellOsal obj
 * \param [in] argc - number of arguments
 * \param [in] argv - arguments
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdExecute(UShellCmd_s* const cmd, const int argc, const char* const argv[])
{
    /* Check input parametr */
    if((NULL == cmd) ||
       (NULL == argv))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((cmd->portable == NULL) ||
       (cmd->portable->execute == NULL))
    {
        return USHELL_CMD_NOT_INIT_ERR;
    }

    /* Check the number of arguments */
    if(argc > cmd->argMax)
    {
        return USHELL_CMD_EXECUTE_NUMB_ARGS_ERR;
    }

    /* Execute the command */
    UShellCmdErr_e err = cmd->portable->execute(cmd, argc, argv);

    return err;
}

/**
 * \brief Get the help string of the cmd.
 * \param [in] cmd - UShellOsal obj
 * \param [out] help - help string
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdHelpGet(UShellCmd_s* const cmd, UShellCmdHelp_t** const help)
{
    /* Check input parametr */
    if((NULL == cmd) ||
       (NULL == help))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;      //< Invalid arguments
    }

    /* Check init state */
    if((cmd->portable == NULL) ||
       (cmd->portable->help == NULL))
    {
        return USHELL_CMD_NOT_INIT_ERR;
    }

    /* Get the help string */

    *help = cmd->portable->help;

    return USHELL_CMD_NO_ERR;       //< Success
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
