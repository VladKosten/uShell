/**
 * \file         ushell_history.c
 * \brief        The file contains the implementation of the UShell history module.
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_history.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShellHal module.
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
 * \brief Initialize UShell history object
 * \param[in] history - uShell history object to be initialized
 * \param[in] parent - parent object
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
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
 * \brief Deinit UShell history object
 * \param[in] history - uShell history object to be deinitialized
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
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
 * \brief Add command string to the history buffer
 * \param history - uShell history object
 * \param str - command string to be added
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
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

        /* Force null termination */
        history->ringBuffer [history->headIndex][USHELL_HISTORY_MAX_STR_LEN - 1] = '\0';

        /* Update the head index */
        history->headIndex = (history->headIndex + 1) % USHELL_HISTORY_SIZE;

        /* Set current index to the last command in the history */
        history->currentIndex = (history->headIndex + USHELL_HISTORY_SIZE - 1) % USHELL_HISTORY_SIZE;

    } while (0);

    return status;
}

/**
 * \brief Get the previous (older) command string from the history buffer.
 * \param[in] history - uShell history object.
 * \param[in] str - buffer for storing the command.
 * \return UShellHistoryErr_e - error code, non-zero value indicates an error.
 */
UShellHistoryErr_e UShellHistoryCmdPrevGet(UShellHistory_s* const history,
                                           char* const str,
                                           const size_t bufferSize)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;
    size_t tries = 0;

    /* Get prev cmd */
    do
    {
        /* Check input parameter */
        if ((history == NULL) ||
            (str == NULL) ||
            (bufferSize == 0))
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
            break;
        }

        /*  Find nearest non-empty command */
        while ((tries < USHELL_HISTORY_SIZE) &&
               (history->ringBuffer [history->currentIndex][0] == '\0'))
        {
            history->currentIndex = (history->currentIndex + USHELL_HISTORY_SIZE - 1) % USHELL_HISTORY_SIZE;
            tries++;
        }

        /* Check if there are no commands in the history */
        if (tries == USHELL_HISTORY_SIZE)
        {
            str [0] = '\0';
            break;
        }

        /* Check if the buffer is large enough */
        if (bufferSize < strlen(history->ringBuffer [history->currentIndex]) + 1)
        {
            status = USHELL_HISTORY_SIZE_ERR;
            break;
        }

        /* Copy the found non-empty command */
        strncpy(str, history->ringBuffer [history->currentIndex], USHELL_HISTORY_MAX_STR_LEN);

        /* Force null termination */
        history->currentIndex = (history->currentIndex + USHELL_HISTORY_SIZE - 1) % USHELL_HISTORY_SIZE;

    } while (0);

    return status;
}

/**
 * \brief Get the next (newer) command string from the history buffer.
 * \param[in] history - uShell history object.
 * \param[in] str - buffer for storing the command.
 * \return UShellHistoryErr_e - error code, non-zero value indicates an error.
 */
UShellHistoryErr_e UShellHistoryCmdNextGet(UShellHistory_s* const history,
                                           char* const str,
                                           const size_t bufferSize)
{
    /* Check input parameter */
    USHELL_HISTORY_ASSERT(history != NULL);

    /* Local variable */
    UShellHistoryErr_e status = USHELL_HISTORY_NO_ERR;
    size_t tries = 0;

    /* Get next cmd */
    do
    {
        /* Check input parameter */
        if ((history == NULL) ||
            (str == NULL) ||
            (bufferSize == 0))
        {
            status = USHELL_HISTORY_INVALID_ARGS_ERR;
            break;
        }

        /* Find nearest non-empty command */
        while ((tries < USHELL_HISTORY_SIZE) &&
               (history->ringBuffer [history->currentIndex][0] == '\0'))
        {
            history->currentIndex = (history->currentIndex + 1) % USHELL_HISTORY_SIZE;
            tries++;
        }

        /* Check if there are no commands in the history */
        if (tries == USHELL_HISTORY_SIZE)
        {
            str [0] = '\0';
            break;
        }

        /* Check if the buffer is large enough */
        if (bufferSize < strlen(history->ringBuffer [history->currentIndex]) + 1)
        {
            status = USHELL_HISTORY_SIZE_ERR;
            break;
        }

        /* Copy the found non-empty command */
        strncpy(str, history->ringBuffer [history->currentIndex], USHELL_HISTORY_MAX_STR_LEN);
        str [USHELL_HISTORY_MAX_STR_LEN - 1] = '\0';

        /* Update the current index */
        history->currentIndex = (history->currentIndex + 1) % USHELL_HISTORY_SIZE;

    } while (0);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
