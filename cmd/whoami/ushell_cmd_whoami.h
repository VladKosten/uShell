#ifndef USHELL_CMD_WHOAMI_H_
#define USHELL_CMD_WHOAMI_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "ushell_cmd.h"
#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

#ifndef USHELL_CMD_WHOAMI_NAME
    #define USHELL_CMD_WHOAMI_NAME "whoami"    ///< UShell command whoami name
#endif

#ifndef USHELL_CMD_WHOAMI_HELP
    #define USHELL_CMD_WHOAMI_HELP "Display information about the system"    ///< UShell command whoami description
#endif

#ifndef USHELL_CMD_WHOAMI_DEVICE_NAME
    #define USHELL_CMD_WHOAMI_DEVICE_NAME "Xplained SamE54"    ///< UShell command whoami system name
#endif

#ifndef USHELL_CMD_WHOAMI_DEVICE_VERSION
    #define USHELL_CMD_WHOAMI_DEVICE_VERSION "1.0.1"    ///< UShell command whoami system version
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Describe UShellWhoAmI.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellWhoAmI object (base object)

} UShellCmdWhoAmI_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShell  whoami module.
 * \param [in] rootCmd - The first cmd in the list of commands to be initialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdWhoAmIInit(UShellCmd_s* rootCmd);

/**
 * \brief Deinitialize the UShell cmd
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdWhoAmIDeinit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_WHOAMI_H_ */