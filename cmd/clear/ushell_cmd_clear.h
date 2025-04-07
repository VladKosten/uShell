#ifndef USHELL_CMD_CLEAR_H_
#define USHELL_CMD_CLEAR_H_

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

#ifndef USHELL_CMD_CLEAR_NAME
    #define USHELL_CMD_CLEAR_NAME "clear"    ///< UShell command clear name
#endif

#ifndef USHELL_CMD_CLEAR_HELP
    #define USHELL_CMD_CLEAR_HELP "Clear the screen"    ///< UShell command clear description
#endif
/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Describe UShellCmdClear.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellCmd object (base object)

} UShellCmdClear_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief UShellCmdClear object (base object)
 */
extern UShellCmdClear_s uShellCmdClear;

/**
 * \brief Initialize the UShell command clear module.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdClearInit(void);

/**
 * \brief Deinitialize the UShell command clear module.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdClearDeinit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_CLEAR_H_ */