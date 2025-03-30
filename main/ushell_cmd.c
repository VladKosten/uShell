/**
 * \file         ushell_cmd.c
 * \brief        The file contains the implementation of the UShell command module.
 *               Object UShellCmd is a parent object for the command objects.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
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
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [in] help - help string
 * \param [in] portable - pointer to the portable table
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdInit(UShellCmd_s* const cmd,
                             const char* const name,
                             const void* const parent,
                             const UShellCmdHelp_t* const help,
                             const UShellCmdExec_f* const execFunc)
{
    /* Check input parameter */
    if ((NULL == cmd) ||
        (NULL == name) ||
        (NULL == parent) ||
        (NULL == execFunc))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Init the object */
    memset((void*) cmd, 0, sizeof(UShellCmd_s));

    /* Set the name */
    cmd->name = name;

    /* Set the parent */
    cmd->parent = parent;

    /* Set the help string */
    cmd->help = help;

    /* Set the portable structure */
    cmd->execFunc = execFunc;

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
    /* Check input parameter */
    if (NULL == cmd)
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Deinit the object */
    memset((void*) cmd, 0, sizeof(UShellCmd_s));

    return USHELL_CMD_NO_ERR;
}

/**
 * @brief Set the parent
 * @param[in] cmd - the cmd to be set
 * @param[in] parent - the parent to be set
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdParentSet(UShellCmd_s* const cmd,
                                  const void* const parent)
{
    /* Check input parameter */
    if ((NULL == cmd))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Set the parent */
    cmd->parent = parent;

    return USHELL_CMD_NO_ERR;
}

/**
 * @brief Execute the cmd
 * @param[in] cmd - UShellCmd obj to be executed
 * @param[in] arg - pointer to the arguments (Can be NULL and it means no arguments)
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdExec(UShellCmd_s* const cmd,
                             const UShellCmdItem_t* const arg)
{
    return USHELL_CMD_NO_ERR;
}

/**
 * \brief Get the name of the UShell  module.
 * \param [in] cmd - UShellOsal obj
 * \param [out] name - name of the object
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdNameGet(UShellCmd_s* const cmd,
                                char** const name)
{
    /* Check input parameter */
    if ((NULL == cmd) ||
        (NULL == name))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;
    }

    /* Get the name */
    *name = (char*) cmd->name;

    return USHELL_CMD_NO_ERR;
}

/**
 * \brief Get the help string of the cmd.
 * \param [in] cmd - UShellOsal obj
 * \param [out] help - help string
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdHelpGet(UShellCmd_s* const cmd, UShellCmdHelp_t** const help)
{
    /* Check input parameter */
    if ((NULL == cmd) ||
        (NULL == help))
    {
        return USHELL_CMD_INVALID_ARGS_ERR;    //< Invalid arguments
    }

    /* Get the help string */
    *help = (char*) cmd->help;

    return USHELL_CMD_NO_ERR;    //< Success
}

/**
 * @brief Add cmd to the list
 * @param cmdRoot - the root of the list
 * @param cmd - the cmd to be added
 * @return - error code
 */
UShellCmdErr_e UShellCmdListAdd(UShellCmd_s* const cmdRoot,
                                UShellCmd_s* const cmd)
{
    return USHELL_CMD_NO_ERR;
}

/**
 * @brief Remove cmd from the list
 * @param[in] cmdRoot - the root of the list
 * @param[in] cmd - the cmd to be removed
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListRemove(UShellCmd_s* const cmdRoot,
                                   UShellCmd_s* const cmd)
{
    return USHELL_CMD_NO_ERR;
}

/**
 * @brief Find cmd by name
 * @param[in] cmdRoot - the root of the list
 * @param[in] cmd - the cmd to be found
 * @param[in] isInList - true if the cmd is in the list, false otherwise
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdIsInList(UShellCmd_s* const cmdRoot,
                                 UShellCmd_s* const cmd,
                                 bool* const isInList)
{
    return USHELL_CMD_NO_ERR;
}

/**
 * @brief Get the next cmd in the list
 * @param[in] cmdRoot - the root of the list
 * @param[in] cmd - the cmd to be found
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListNextGet(UShellCmd_s* const cmdRoot,
                                    UShellCmd_s** const cmd)
{
    return USHELL_CMD_NO_ERR;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
