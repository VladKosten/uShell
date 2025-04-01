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

/**
 * \brief Lock the cmd
 * \param cmd - cmd to be locked
 * \return none
 */
static void uShellCmdLock(UShellCmd_s* const cmd);

/**
 * \brief Unlock the cmd
 * \param cmd - cmd to be unlocked
 * \return none
 */
static void uShellCmdUnlock(UShellCmd_s* const cmd);

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
                             const UShellCmdHelp_t* const help,
                             const UShellCmdExec_f* const execFunc)
{

    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmd) ||
            (NULL == name) ||
            (NULL == execFunc) ||
            (NULL == help))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Init the object */
        memset((void*) cmd, 0, sizeof(UShellCmd_s));

        /* Set the name */
        cmd->name = name;

        /* Set the help string */
        cmd->help = help;

        /* Set the portable structure */
        cmd->execFunc = execFunc;

    } while (0);

    return status;
}

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] cmd - UShellOsal obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdDeinit(UShellCmd_s* const cmd)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if (NULL == cmd)
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Deinit the object */
        memset((void*) cmd, 0, sizeof(UShellCmd_s));

    } while (0);

    return status;
}

/**
 * \brief Set the parent
 * \param[in] cmd - the cmd to be set
 * \param[in] parent - the parent to be set
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdParentSet(UShellCmd_s* const cmd,
                                  const void* const parent)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmd))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Set the parent */
        cmd->parent = parent;
    } while (0);

    return status;
}

/**
 * \brief Set the hook table
 * \param[in] cmd -  the cmd to be set
 * \param[in] hook -  the hook table to be set
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdHookTableSet(UShellCmd_s* const cmd,
                                     UShellCmdHookTable_s* const hook)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmd))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Set the hook table */
        cmd->hook = hook;

    } while (0);

    return status;
}

/**
 * \brief Execute the cmd
 * \param[in] cmd - UShellCmd obj to be executed
 * \param[in] argc - number of arguments
 * \param[in] argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdExec(UShellCmd_s* const cmd,
                             const int argc,
                             char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmd) ||
            (cmd->execFunc == NULL))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* lock the cmd */
        uShellCmdLock(cmd);

        /* Thread safety */
        do
        {
            /* Execute the cmd */
            status = cmd->execFunc(cmd, argc, argv);
            if (status != USHELL_CMD_NO_ERR)
            {
                break;
            }

        } while (0);

        /* unlock the cmd */
        uShellCmdUnlock(cmd);

    } while (0);

    return status;
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
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmd) ||
            (NULL == name))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Get the name */
        *name = (char*) cmd->name;

    } while (0);

    return status;
}

/**
 * \brief Get the help string of the cmd.
 * \param [in] cmd - UShellOsal obj
 * \param [out] help - help string
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdHelpGet(UShellCmd_s* const cmd, UShellCmdHelp_t** const help)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmd) ||
            (NULL == help))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Get the help string */
        *help = (UShellCmdHelp_t*) cmd->help;

    } while (0);

    return status;
}

/**
 * \brief Add cmd to the list
 * \param cmdRoot - the root of the list
 * \param cmd - the cmd to be added
 * \return - error code
 */
UShellCmdErr_e UShellCmdListAdd(UShellCmd_s* const cmdRoot,
                                UShellCmd_s* const cmd)
{
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    UShellCmd_s* currCmd = cmdRoot;

    do
    {
        if ((NULL == cmdRoot) || (NULL == cmd))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* For safety */
        cmd->next = NULL;

        /* Lock the command list */
        uShellCmdLock(cmdRoot);

        /* Thread safe */
        while (currCmd != NULL)
        {
            /* Check if the cmd is already in the list */
            if (currCmd == cmd)
            {
                USHELL_CMD_ASSERT(0);
                status = USHELL_CMD_ALREADY_EXISTS_ERR;
                break;
            }

            /* Find the last cmd in the list */
            if (currCmd->next == NULL)
            {
                break;
            }

            /* Move to the next cmd */
            currCmd = currCmd->next;
        }

        /* Check if the cmd is already in the list */
        if (USHELL_CMD_NO_ERR == status)
        {
            currCmd->next = cmd;
        }

        /* Unlock the command list */
        uShellCmdUnlock(cmdRoot);

    } while (0);

    return status;
}

/**
 * \brief Remove a command from the list.
 *
 * \param[in,out] cmdRoot Pointer to the pointer to the root of the list.
 *                        This parameter is updated if the head command is removed.
 * \param[in]     cmd     Pointer to the command to be removed.
 * \return UShellCmdErr_e Error code; USHELL_CMD_NO_ERR if successful,
 *         or an error code (e.g. USHELL_CMD_INVALID_ARGS_ERR or USHELL_CMD_NOT_FOUND_ERR).
 */
UShellCmdErr_e UShellCmdListRemove(UShellCmd_s** const cmdRoot, UShellCmd_s* const cmd)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    UShellCmd_s* current;
    UShellCmd_s* previous = NULL;

    do
    {
        /* Check input parameter */
        if ((cmdRoot == NULL) ||
            (*cmdRoot == NULL) ||
            (cmd == NULL))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Lock the command list */
        uShellCmdLock(*cmdRoot);

        /* Thread safe removal */
        current = *cmdRoot;
        while (current != NULL)
        {
            if (current == cmd)
            {
                /* Remove the node */
                if (previous == NULL)
                {
                    *cmdRoot = current->next;
                }
                else
                {
                    previous->next = current->next;
                }
                break;
            }
            previous = current;
            current = current->next;
        }

        /* Unlock the command list */
        uShellCmdUnlock(*cmdRoot);

    } while (0);

    return status;
}

/**
 * \brief Find cmd by name
 * \param[in] cmdRoot - the root of the list
 * \param[in] cmd - the cmd to be found
 * \param[in] isInList - true if the cmd is in the list, false otherwise
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdIsInList(UShellCmd_s* const cmdRoot,
                                 UShellCmd_s* const cmd,
                                 bool* const isInList)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    UShellCmd_s* currCmd = cmdRoot;
    bool found = false;

    do
    {
        /* Check input parameter */
        if ((NULL == cmdRoot) ||
            (NULL == cmd) ||
            (NULL == isInList))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellCmdLock(cmdRoot);

        /* Thread safe */
        do
        {

            /* Find the last cmd in the list */
            while (currCmd->next != NULL)
            {
                /* Check if the cmd is already in the list */
                if (currCmd == cmd)
                {
                    found = true;
                    break;
                }

                /* Move to the next cmd */
                currCmd = currCmd->next;
            }

            /* Set the result */
            *isInList = found;

        } while (0);

        /* Unlock */
        uShellCmdUnlock(cmdRoot);

    } while (0);

    return status;
}

/**
 * \brief Get the next cmd in the list
 * \param[in] cmdRoot - the root of the list
 * \param[in] cmd - the cmd to be found
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListNextGet(UShellCmd_s* const cmdRoot,
                                    UShellCmd_s** const cmd)
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((NULL == cmdRoot) ||
            (NULL == cmd))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;
            break;
        }

        /* Lock */
        uShellCmdLock(cmdRoot);

        /* Thread safe */
        *cmd = cmdRoot->next;

        /* Unlock */
        uShellCmdUnlock(cmdRoot);

    } while (0);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Lock the cmd
 * \param cmd - cmd to be locked
 * \return none
 */
static void uShellCmdLock(UShellCmd_s* const cmd)
{
    /* Check input parameter */
    USHELL_CMD_ASSERT(cmd != NULL);

    do
    {
        /* Check  input parameter */
        if (cmd == NULL)
        {
            break;
        }

        /* Check hook table */
        if ((cmd->hook == NULL) ||
            (cmd->hook->lock == NULL))
        {
            break;
        }

        /* Lock the cmd */
        cmd->hook->lock(cmd);

    } while (0);
}

/**
 * \brief Unlock the cmd
 * \param cmd - cmd to be unlocked
 * \return none
 */
static void uShellCmdUnlock(UShellCmd_s* const cmd)
{
    /* Check input parameter */
    USHELL_CMD_ASSERT(cmd != NULL);

    do
    {
        /* Check  input parameter */
        if (cmd == NULL)
        {
            break;
        }

        /* Check hook table */
        if ((cmd->hook == NULL) ||
            (cmd->hook->unlock == NULL))
        {
            break;
        }

        /* Lock the cmd */
        cmd->hook->unlock(cmd);

    } while (0);
}