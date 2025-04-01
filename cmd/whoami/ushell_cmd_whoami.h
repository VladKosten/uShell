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

/*===========================================================[MACRO DEFINITIONS]============================================*/

#ifndef USHELL_CMD_WHOAMI_NAME
    #define USHELL_CMD_WHOAMI_NAME "whoami"    ///< UShell command whoami name
#endif

#ifndef USHELL_CMD_WHOAMI_HELP
    #define USHELL_CMD_WHOAMI_HELP "Display information about the system"    ///< UShell command whoami description
#endif

#ifndef USHELL_CMD_WHOAMI_SYSTEM_NAME
    #define USHELL_CMD_WHOAMI_SYSTEM_NAME "Xplained SamE54 \r\n"    ///< UShell command whoami system name
#endif

#ifndef USHELL_CMD_WHOAMI_SYSTEM_VERSION
    #define USHELL_CMD_WHOAMI_SYSTEM_VERSION " 0.0.1 ALFA \r\n"    ///< UShell command whoami system version
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
 * \brief UShellWhoAmIHelp object (base object)
 */
extern UShellCmdWhoAmI_s uShellCmdWhoAmI;

/**
 * \brief Initialize the UShell  module.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdWhoAmIInit();

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