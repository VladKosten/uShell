#ifndef USHELL_H_
#define USHELL_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Project includes */
#include "uShell_hal.h"
#include "uShell_osal.h"
#include "uShell_cmd.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

#ifndef USHELL_MAX_CMD
    #define USHELL_MAX_CMD 10
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/
/**
 * @brief Enumeration of possible error codes returned by the UShell Hal module.
 */
typedef enum
{
    USHELL_NO_ERR = 0,            ///< Exit: no errors (success)
    USHELL_INVALID_ARGS_ERR,      ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_NOT_INIT_ERR,          ///< Exit: error - not initialized
    USHELL_PORT_ERR,              ///< Exit: error - port error (e.g. port layer error)

}UShellErr_e;


/**
 * \brief Description of the uShell object
*/
typedef struct
{
    const void* parent;                         ///< Parent object
    const char* name;                           ///< Name of the object

    const UShellOsal_s* osal;                   ///< OSAL object
    const UShellHal_s* hal;                     ///< HAL object

    const UShellCmd_s* cmd[USHELL_MAX_CMD];     ///< Commands



}UShell_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init uShell object
 * \param[in] uShell - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
*/
UShellErr_e UShellInit(UShell_s* const uShell, const UShellOsal_s* const osal, const UShellHal_s* const hal, const void* const parent, const char* const name);

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
UShellErr_e UShellCmdAttach(UShell_s* const uShell, const UShellCmd_s* const cmd);

/**
 * \brief Detach command from uShell object
 * \param[in] uShell - uShell object
 * \param[in] cmd - command to be detached
 * \param[out] none
*/
UShellErr_e UShellCmdDetach(UShell_s* const uShell, const UShellCmd_s* const cmd);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_H_ */