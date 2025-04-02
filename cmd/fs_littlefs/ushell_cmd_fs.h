#ifndef USHELL_CMD_FS_H_
#define USHELL_CMD_FS_H_

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

#include "lfs.h"
#include "ushell_cfg.h"
#include "ushell_cmd.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

#ifndef USHELL_CMD_FS_START_PATH
    #define USHELL_CMD_FS_START_PATH "/"    ///< Start path for the file system commands
#endif

/**
 * \brief UShell command name for cd
 */
#ifndef USHELL_CMD_FS_CD_NAME
    #define USHELL_CMD_FS_CD_NAME "cd"    ///< UShell command name for cd
#endif

/**
 * \brief UShell command help for cd
 */
#ifndef USHELL_CMD_FS_CD_HELP
    #define USHELL_CMD_FS_CD_HELP "Change directory"    ///< UShell command help for cd
#endif

/**
 * \brief UShell command name for ls
 */
#ifndef USHELL_CMD_FS_LS_NAME
    #define USHELL_CMD_FS_LS_NAME "ls"    ///< UShell command name for ls
#endif

/**
 * \brief UShell command help for ls
 */
#ifndef USHELL_CMD_FS_LS_HELP
    #define USHELL_CMD_FS_LS_HELP "List files"    ///< UShell command help for ls
#endif

#ifndef USHELL_CMD_FS_RM_NAME
    #define USHELL_CMD_FS_RM_NAME "rm"    ///< UShell command name for rm
#endif

/**
 * \brief UShell command help for rm
 */
#ifndef USHELL_CMD_FS_RM_HELP
    #define USHELL_CMD_FS_RM_HELP "Remove file or dir "    ///< UShell command help for rm
#endif

/**
 * \brief UShell command name for mkdir
 */
#ifndef USHELL_CMD_FS_MKDIR_NAME
    #define USHELL_CMD_FS_MKDIR_NAME "mkdir"    ///< UShell command name for mkdir
#endif

/**
 * \brief UShell command help for mkdir
 */
#ifndef USHELL_CMD_FS_MKDIR_HELP
    #define USHELL_CMD_FS_MKDIR_HELP "Create directory"    ///< UShell command help for mkdir
#endif

/**
 * \brief UShell command name for cat
 */
#ifndef USHELL_CMD_FS_CAT_NAME
    #define USHELL_CMD_FS_CAT_NAME "cat"    ///< UShell command name for cat
#endif

/**
 * \brief UShell command help for cat
 */
#ifndef USHELL_CMD_FS_CAT_HELP
    #define USHELL_CMD_FS_CAT_HELP "Print file content"    ///< UShell command help for cat
#endif

/**
 * \brief UShell maximum path length
 */
#ifndef USHELL_CMD_FS_MAX_PATH
    #define USHELL_CMD_FS_MAX_PATH 64    ///< Maximum path length
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Describe UShellCmdLs.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellCmd object (base object)

} UShellCmdFsLs_s;

/**
 * \brief Describe UShellCmdCd.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellCmd object (base object)
} UShellCmdFsCd_s;

/**
 * \brief Describe UShellCmdFsMkdir.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellCmd object (base object)
} UShellCmdFsMkdir_s;

/**
 * \brief Describe UShellCmdFsRm.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellCmd object (base object)
} UShellCmdFsRm_s;

/**
 * \brief Describe UShellCmdFsCat.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< UShellCmd object (base object)
} UShellCmdFsCat_s;

typedef struct
{
    UShellCmdFsLs_s cmdLs;          ///< UShellCmd object (base object)
    UShellCmdFsCd_s cmdCd;          ///< UShellCmd object (base object)
    UShellCmdFsRm_s cmdRm;          ///< UShellCmd object (base object)
    UShellCmdFsMkdir_s cmdMkdir;    ///< UShellCmd object (base object)
    UShellCmdFsCat_s cmdCat;        ///< UShellCmd object (base object)

    lfs_t* lfs;                            ///< LittleFS object
    char path [USHELL_CMD_FS_MAX_PATH];    ///< Current path

} UShellCmdFs_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief UShellCmdFs object (base object)
 */
extern UShellCmdFs_s uShellCmdFs;    ///< UShellCmd object (base object)

/**
 * \brief Initialize the commands for file system operations.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdFsInit(lfs_t* lfs);

/**
 * \brief Deinitialize the UShell cmd
 * \param [in] cmd - UShellCmd obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdFsDeinit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_FS_H_ */