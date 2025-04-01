#ifndef USHELL_H_
#define USHELL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Project includes */
#include "ushell_cmd.h"
#include "ushell_hal.h"
#include "ushell_osal.h"
#include "ushell_history.h"
#include "ushell_vcp.h"
#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief The maximum number of commands in the UShell.
 */
#ifndef USHELL_MAX_CMD
    #define USHELL_MAX_CMD 15
#endif

/**
 * \brief The maximum number of arguments in the UShell.
 */
#ifndef USHELL_UPD_TIME_MS
    #define USHELL_UPD_TIME_MS 100U
#endif

/**
 * \brief The delay time in milliseconds before starting the UShell.
 */
#ifndef USHELL_OSAL_START_DELAY_MS
    #define USHELL_OSAL_START_DELAY_MS 1000U
#endif

/**
 * \brief The maximum size of the buffer in the UShell.
 */
#ifndef USHELL_BUFFER_SIZE
    #define USHELL_BUFFER_SIZE 256
#endif

/**
 * \brief The default password in the UShell.
 */
#ifndef USHELL_AUTH_PASSWORD
    #define USHELL_AUTH_PASSWORD "admin"
#endif

/**
 * \brief The name of the UShell thread.
 */
#ifndef USHELL_THREAD_NAME
    #define USHELL_THREAD_NAME "USHELL"
#endif

/**
 * \brief The stack size of the UShell thread in bytes.
 */
#ifndef USHELL_THREAD_STACK_SIZE_BYTE
    #define USHELL_THREAD_STACK_SIZE_BYTE 2048U
#endif

/**
 * \brief The OSAL thread priority for the UShell.
 */
#ifndef USHELL_THREAD_PRIORITY
    #define USHELL_THREAD_PRIORITY USHELL_OSAL_THREAD_PRIORITY_LOW
#endif

/**
 * \brief The default prompt displayed by the UShell.
 */
#ifndef USHELL_USER_PROMPT
    #define USHELL_USER_PROMPT "uShell> "
#endif

/**
 * \brief The default message displayed when the UShell is started.
 */
#ifndef USHELL_HELLO_MSG
    #define USHELL_HELLO_MSG "Press Enter to start ..."
#endif

/**
 * \brief The password prompt displayed by the UShell.
 */
#ifndef USHELL_AUTH_PROMPT
    #define USHELL_AUTH_PROMPT "Password: "
#endif

/**
 * \brief The message displayed when authentication is successful.
 */
#ifndef USHELL_AUTH_OK_MSG
    #define USHELL_AUTH_OK_MSG " \n\rAuthentication OK \n\r"
#endif

/**
 * \brief The message displayed when authentication fails.
 */
#ifndef USHELL_AUTH_FAIL_MSG
    #define USHELL_AUTH_FAIL_MSG "Authentication FAIL \n\r"
#endif

/**
 * \brief The message displayed when the command is not found.
 */
#ifndef USHELL_CMD_NOT_FOUND_MSG
    #define USHELL_CMD_NOT_FOUND_MSG "Command not found \n\r"
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Describe size of one item in the UShell
 */

typedef char UShellItem_t;

/**
 * \brief Describe feature of the UShell
 */
typedef bool UShellFeature_b;

/**
 * \brief Enumeration of possible error codes returned by the UShell Hal module.
 */
typedef enum
{
    USHELL_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_NOT_INIT_ERR,        ///< Exit: error - not initialized
    USHELL_PORT_ERR,            ///< Exit: error - port error (e.g. port layer error)
    USHELL_CMD_ERR,             ///< Exit: error - no space for command
    USHELL_XFER_ERR,            ///< Exit: error - transfer error
    USHELL_TIMEOUT_ERR,         ///< Exit: error - timeout error

} UShellErr_e;

/**
 * \brief Description of the uShell IO object
 * \note This object is used to store the buffer for input/output operations in the uShell
 */
typedef struct
{
    UShellItem_t buffer [USHELL_BUFFER_SIZE];    ///< Buffer for commands
    size_t ind;                                  ///< Size of the buffer

} UShellIo_s;

/**
 * \brief Enumeration of the uShell finite state machine states.
 *
 * This enumeration defines the different states in which the uShell can operate.
 */
typedef enum
{
    USHELL_STATE_INIT,            ///< uShell is in initialization state
    USHELL_STATE_AUTH,            ///< uShell is in authentication state
    USHELL_STATE_PROC_INP,        ///< uShell is processing input
    USHELL_STATE_PROC_CMD,        ///< uShell is processing command
    USHELL_STATE_PROC_ESC_SEQ,    ///< uShell is processing escape sequence
    USHELL_STATE_ERROR,           ///< uShell is in error state
} UShellFsmState_e;

/**
 * \brief Description of the uShell object
 */
typedef enum

{
    USHELL_FEATURE_AUTH = 0,    ///< Enable authentication
    USHELL_FEATURE_PROMPT,      ///< Enable prompt display
    USHELL_FEATURE_HISTORY,     ///< Enable command history

} UShellFeature_e;

/**
 * \brief Configuration settings for the uShell.
 *
 * This structure holds Boolean flags that enable or disable
 * various uShell features, such as authentication, prompt,
 * and command history.
 */
typedef struct
{
    UShellFeature_b authIsEn;       ///< Enable authentication.
    UShellFeature_b promptIsEn;     ///< Enable prompt display.
    UShellFeature_b historyIsEn;    ///< Enable command history.

} UShellCfg_s;

/**
 * \brief Description of the uShell object
 */
typedef struct
{
    /* Non-optional fields */
    const void* parent;    ///< Parent object
    const char* name;      ///< Name of the object

    /* Dependencies */
    const UShellOsal_s* osal;    ///< OSAL object
    const UShellHal_s* hal;      ///< HAL object
    const UShellVcp_s* vcp;      ///< VCP object

    /* Optional fields */
    UShellFsmState_e fsmState;    ///< Finite state machine state
    UShellCfg_s cfg;              ///< Configuration object
    UShellCmd_s* cmdRoot;         ///< Commands array
    UShellHistory_s history;      ///< History object
    UShellCmd_s* currCmd;         ///< Current command
    UShellIo_s io;                ///< IO object

} UShell_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init uShell object
 * \param[in] uShell - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] vcs - vcp object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[in] cmdRoot - root command
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellInit(UShell_s* const uShell,
                       const UShellOsal_s* const osal,
                       const UShellVcp_s* const vcp,
                       const UShellCfg_s* const cfg,
                       void* const parent,
                       const char* const name,
                       const UShellCmd_s* const cmdRoot);

/**
 * \brief DeInit uShell object
 * \param[in] uShell - uShell object to be deinitialized
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellDeInit(UShell_s* const uShell);

/**
 * \brief Run uShell object
 * \param[in] uShell - uShell object to be run
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellRun(UShell_s* const uShell);

/**
 * \brief Stop uShell object
 * \param[in] uShell - uShell object to be stopped
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellStop(UShell_s* const uShell);

/**
 * \brief Attach command to uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be attached
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellCmdAttach(UShell_s* const uShell,
                            const UShellCmd_s* const cmd);

/**
 * \brief Detach command from uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be detached
 * \param[out] none
 */
UShellErr_e UShellCmdDetach(UShell_s* const uShell,
                            const UShellCmd_s* const cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_H_ */