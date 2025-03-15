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
#include "ushell_auth.h"
#include "ushell_history.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * @brief Description of the maximum number of commands in the UShell
 */
#ifndef USHELL_MAX_CMD
    #define USHELL_MAX_CMD 10
#endif

/**
 * @brief Description of the maximum size of the buffer in the UShell
 */
#ifndef USHELL_BUFFER_SIZE
    #define USHELL_BUFFER_SIZE 64
#endif

/**
 * @brief Description of the default password in the UShell
 */
#ifndef USHELL_AUTH_PASSWORD
    #define USHELL_AUTH_PASSWORD "admin"
#endif

#ifndef USHELL_THREAD_NAME
    #define USHELL_THREAD_NAME "USHELL"
#endif

#ifndef USHELL_THREAD_STACK_SIZE
    #define USHELL_THREAD_STACK_SIZE 1024U
#endif

#ifndef USHELL_THREAD_PRIORITY
    #define USHELL_THREAD_PRIORITY USHELL_OSAL_THREAD_PRIORITY_LOW
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * @brief Describe size of one item in the UShell
 */
typedef char UShellItem_t;

/**
 * @brief Describe feature of the UShell
 */
typedef bool UShellFeature_b;

/**
 * @brief Enumeration of possible error codes returned by the UShell Hal module.
 */
typedef enum
{
    USHELL_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_NOT_INIT_ERR,        ///< Exit: error - not initialized
    USHELL_PORT_ERR,            ///< Exit: error - port error (e.g. port layer error)
    USHELL_CMD_ERR,             ///< Exit: error - no space for command
    USHELL_XFER_ERR,            ///< Exit: error - transfer error

} UShellErr_e;

/**
 * @brief Description of the uShell IO object
 * @note This object is used to store the buffer for input/output operations in the uShell
 */
typedef struct
{
    UShellItem_t buffer [USHELL_BUFFER_SIZE];    ///< Buffer for commands
    size_t ind;                                  ///< Size of the buffer

} UShellIo_s;

/**
 * @brief Description of the uShell object
 */
typedef enum
{
    USHELL_FEATURE_AUTH = 0,    ///< Enable authentication
    USHELL_FEATURE_ECHO,        ///< Enable echo feature
    USHELL_FEATURE_PROMPT,      ///< Enable prompt display
    USHELL_FEATURE_HISTORY,     ///< Enable command history

} UShellFeature_e;

/**
 * \brief Configuration settings for the uShell.
 *
 * This structure holds Boolean flags that enable or disable
 * various uShell features, such as authentication, echo, prompt,
 * and command history.
 */
typedef struct
{
    UShellFeature_b authIsEn;       ///< Enable authentication.
    UShellFeature_b echoIsEn;       ///< Enable echo feature.
    UShellFeature_b promptIsEn;     ///< Enable prompt display.
    UShellFeature_b historyIsEn;    ///< Enable command history.

} UShellCfg_s;

/**
 * @brief Description of the uShell object
 */
typedef struct
{
    /* Non-optional fields */
    const void* parent;    ///< Parent object
    const char* name;      ///< Name of the object

    /* Dependencies */
    const UShellOsal_s* osal;    ///< OSAL object
    const UShellHal_s* hal;      ///< HAL object

    /* Optional fields */
    UShellCfg_s cfg;                      ///< Configuration object
    UShellCmd_s* cmd [USHELL_MAX_CMD];    ///< Commands array
    UShellAuth_s auth;                    ///< Authentication object
    UShellHistory_s history;              ///< History object
    UShellIo_s io;                        ///< IO object

} UShell_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Init uShell object
 * \param[in] uShell - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellInit(UShell_s* const uShell,
                       const UShellOsal_s* const osal,
                       const UShellHal_s* const hal,
                       const UShellCfg_s cfg,
                       void* const parent,
                       const char* const name);

/**
 * @brief DeInit uShell object
 * \param[in] uShell - uShell object to be deinitialized
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellDeInit(UShell_s* const uShell);

/**
 * @brief Run uShell object
 * \param[in] uShell - uShell object to be run
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellRun(UShell_s* const uShell);

/**
 * @brief Stop uShell object
 * \param[in] uShell - uShell object to be stopped
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellStop(UShell_s* const uShell);

/**
 * @brief Attach command to uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be attached
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellErr_e UShellCmdAttach(UShell_s* const uShell,
                            const UShellCmd_s* const cmd);

/**
 * @brief Detach command from uShell object
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