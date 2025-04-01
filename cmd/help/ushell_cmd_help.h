#ifndef USHELL_CMD_HELP_H_
#define USHELL_CMD_HELP_H_

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

#ifndef USHELL_CMD_HELP_NAME
    #define USHELL_CMD_HELP_NAME "help"    ///< UShell command help name
#endif

#ifndef USHELL_CMD_HELP_HELP
    #define USHELL_CMD_HELP_HELP "Display help information for commands"    ///< UShell command help description
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Describe UShellCmdHelp.
 */
typedef struct
{
    UShellCmd_s cmd;         ///< UShellCmd object (base object)
    UShellCmd_s* rootCmd;    ///< Pointer to the root command (head of the list)

} UShellCmdHelp_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief UShellCmdHelp object (base object)
 */
extern UShellCmdHelp_s uShellCmdHelp;

/**
 * \brief Initialize the UShell  module.
 * \param [in] rootCmd - The first cmd in the list of commands to be initialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdHelpInit(UShellCmd_s* rootCmd);

/**
 * \brief Deinitialize the UShell cmd
 * \param [in] cmd - UShellCmd obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdHelpDeinit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_HELP_H_ */