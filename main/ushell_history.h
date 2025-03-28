#ifndef USHELL_HISTORY_H_
#define USHELL_HISTORY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \def USHELL_HISTORY_SIZE
 * \brief The number of command strings stored in the history buffer.
 */
#ifndef USHELL_HISTORY_SIZE
    #define USHELL_HISTORY_SIZE 3
#endif

/**
 * \def USHELL_HISTORY_MAX_STR_LEN
 * \brief The maximum length of each command string in the history buffer.
 */
#ifndef USHELL_HISTORY_MAX_STR_LEN
    #define USHELL_HISTORY_MAX_STR_LEN 20
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes
 */
typedef enum
{
    USHELL_HISTORY_NO_ERR = 0,           ///< Exit: no errors (success)
    USHELL_HISTORY_INVALID_ARGS_ERR,     ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_HISTORY_NOT_INIT_ERR,         ///< Exit: error - not initialized
    USHELL_HISTORY_CMD_NOT_FOUND_ERR,    ///< Exit: error - command not found

} UShellHistoryErr_e;

/**
 * \brief Description of the uShell history object
 */
typedef struct
{
    /**
     * \brief Parent object
     */
    const void* parent;

    /**
     * \brief Pointer to the ring buffer array of strings
     */
    char ringBuffer [USHELL_HISTORY_SIZE][USHELL_HISTORY_MAX_STR_LEN];

    /**
     * \brief Index for the newest entry in the ring buffer
     */
    size_t headIndex;

    /**
     * \brief Index for the oldest entry in the ring buffer
     */
    size_t tailIndex;

} UShellHistory_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize UShell history object
 * @param[in] history - uShell history object to be initialized
 * @param[in] parent - parent object
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryInit(UShellHistory_s* const history,
                                     const void* const parent);

/**
 * \brief Deinit UShell history object
 * @param[in] history - uShell history object to be deinitialized
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryDeInit(UShellHistory_s* const history);

/**
 * \brief Add command string to the history buffer
 * @param history - uShell history object
 * @param str - command string to be added
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryAdd(UShellHistory_s* const history,
                                    const char* const str);

/**
 * \brief Get command string from the history buffer
 * @param[in] history - uShell history object
 * @param[in] str - command string to be added
 * @param[in] index - index of the command string in the history buffer
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryGetByIndex(UShellHistory_s* const history,
                                           char* const str,
                                           const size_t index);

/**
 * \brief Find cmd in the history buffer
 * @param[in] history - uShell history object
 * @param[in] str - command string to be found
 * @param[in] index - index of the command string in the history buffer
 * \return UShellHistoryErr_e - error code. non-zero = an error has occurred;
 */
UShellHistoryErr_e UShellHistoryFindCmd(UShellHistory_s* const history,
                                        const char* const str,
                                        size_t* const index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_AUTH_H_ */