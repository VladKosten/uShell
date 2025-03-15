/**
 * \file         ushell_history.c
 * @brief        The file contains the implementation of the UShell history module.
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2021
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_history.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * @brief Assert macro for the UShellHal module.
 */
#ifndef USHELL_HISTORY_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_AUTH_ASSERT(cond) USHELL_HISTORY_ASSERT(cond)
    #else
        #define USHELL_HISTORY_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Initialize UShell auth object
 * @param[in] auth - uShell auth object to be initialized
 * @param[in] parent - parent object
 * @param[in] password - password of the object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthInit(UShellAuth_s* const auth,
                               const void* const parent,
                               const char* const password)
{
    /* Check input parameter */
    USHELL_AUTH_ASSERT(auth != NULL);
    USHELL_AUTH_ASSERT(password != NULL);
    USHELL_AUTH_ASSERT(parent != NULL);

    /* Local variable */
    UShellAuthErr_e status = USHELL_AUTH_NO_ERR;

    /* Initialize the object */
    do
    {
        /* Check input parameter */
        if ((auth == NULL) ||
            (password == NULL) ||
            (parent == NULL))
        {
            status = USHELL_AUTH_INVALID_ARGS_ERR;
            break;
        }

        /* Save the fields */
        auth->password = password;
        auth->parent = parent;

        /* Set the object as not authenticated */
        auth->isAuth = false;

    } while (0);

    return status;
}

/**
 * @brief Initialize UShell history object
 * @param[in] history - uShell history object to be initialized
 * @param[in] parent - parent object
 * @return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryInit(UShellHistory_s* const history,
                                     const void* const parent)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);
    USHELL_HISTORY_ASSERT(parent != NULL);

    /* Local variable */
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;

    /* Initialize the object */
    do
    {
        /* Check input parameter */
        if ((history == NULL) ||
            (parent == NULL))
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
        }

        /* Flush the object */
        memset(history, 0, sizeof(UShellHistory_s));

        /* Set the parent object */
        history->parent = parent;

    } while (0);

    return status;
}

/**
 * @brief Deinit UShell history object
 * @param[in] history - uShell history object to be deinitialized
 * @return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryDeInit(UShellHistory_s* const history)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);

    /* Local variable */
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;

    /* Deinitialize the object */
    do
    {
        /* Check input parameter */
        if (history == NULL)
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
            break;
        }

        /* Flush the object */
        memset(history, 0, sizeof(UShellHistory_s));

    } while (0);

    return status;
}

/**
 * @brief Add command string to the history buffer
 * @param history - uShell history object
 * @param str - command string to be added
 * @return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryAdd(UShellHistory_s* const history,
                                    const char* const str)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);

    /* Local variable */
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;

    /* Add the command string */
    do
    {
        /* Check input parameter */
        if ((history == NULL) ||
            (str == NULL))
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
            break;
        }

        /* Add the command string */
        strncpy(history->ringBuffer [history->headIndex], str, USHELL_HISTORY_MAX_STR_LEN);

        /* Update the head index */
        history->headIndex = (history->headIndex + 1) % USHELL_HISTORY_SIZE;

    } while (0);

    return status;
}

/**
 * @brief Get command string from the history buffer
 * @param[in] history - uShell history object
 * @param[out] str - buffer for the retrieved command string
 * @param[in] index - offset from the last command in the history
 * @return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryGetByIndex(UShellHistory_s* const history,
                                           char* const str,
                                           const size_t index)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);

    /* Local variable */
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;

    /* Get the command string */
    do
    {
        /* Check input parameter */
        if ((history == NULL) ||
            (str == NULL) ||
            (index >= USHELL_HISTORY_SIZE))
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
            break;
        }

        /* Сalculate the real index */
        size_t realIndex = (history->headIndex + USHELL_HISTORY_SIZE - 1 - index) % USHELL_HISTORY_SIZE;

        strncpy(str, history->ringBuffer [realIndex], USHELL_HISTORY_MAX_STR_LEN);

    } while (0);

    return status;
}

/**
 * @brief Find cmd in the history buffer
 * @param[in] history - uShell history object
 * @param[in] str - command string to be found
 * @param[in] index - index of the command string in the history buffer
 * @return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryFindCmd(UShellHistory_s* const history,
                                        const char* const str,
                                        size_t* const index)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);
    USHELL_HISTORY_ASSERT(str != NULL);
    USHELL_HISTORY_ASSERT(index != NULL);

    /* Local variable */
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;

    /* Find the command string */
    do
    {
        /* Check input parameter */
        if ((history == NULL) ||
            (str == NULL) ||
            (index == NULL))
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
            break;
        }

        /* Find the command string */
        for (size_t i = 0; i < USHELL_HISTORY_SIZE; i++)
        {
            if (strncmp(str, history->ringBuffer [i], USHELL_HISTORY_MAX_STR_LEN) == 0)
            {
                *index = i;
                break;
            }
        }

    } while (0);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
