#ifndef USHELL_CMD_H_
#define USHELL_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief The maximum number of arguments in the UShell command.
 * This value is used to define the maximum number of arguments that can be
 * passed to a command in the UShell.
 */
#ifndef USHELL_CMD_MAX_ARGV
    #define USHELL_CMD_MAX_ARGV 5
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes returned by the UShellCmd module.
 */
typedef enum
{
    USHELL_CMD_NO_ERR = 0,               ///< Exit: no errors (success)
    USHELL_CMD_INVALID_ARGS_ERR,         ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_CMD_NOT_INIT_ERR,             ///< Exit: error - not initialized
    USHELL_CMD_PORT_ERR,                 ///< Exit: error - port error (e.g. port layer error)
    USHELL_CMD_EXECUTE_NUMB_ARGS_ERR,    ///< Exit: error - number of arguments error
    USHELL_CMD_ALREADY_EXISTS_ERR,       ///< Exit: error - command already exists

} UShellCmdErr_e;

/**
 * \brief Describe a cmd help string.
 */
typedef char* UShellCmdHelp_t;

/**
 * \brief Describe size of one item in the UShell.
 */
typedef char UShellCmdItem_t;

/**
 * \brief Describe a cmd exec function.
 */
typedef UShellCmdErr_e(UShellCmdExec_f)(void* const cmd,
                                        const int argc,
                                        char* const argv []);

/**
 * \brief Describe a cmd hook function.
 */
typedef struct
{
    void (*lock)(void* const cmd);      ///< Pointer to the lock function
    void (*unlock)(void* const cmd);    ///< Pointer to the unlock function

} UShellCmdHookTable_s;

/**
 * \brief Describe UShellCmd.
 */
typedef struct UShellCmd_t
{
    /* Mandatory */
    const void* parent;             ///< Pointer to the parent object
    const char* name;               ///< Pointer to the name of the object (aka command)
    const UShellCmdHelp_t* help;    ///< Pointer to the help string (Must be string)
    UShellCmdExec_f* execFunc;      ///< Pointer to the function to be executed
    struct UShellCmd_t* next;       ///< Pointer to the next command in the list
    UShellCmdHookTable_s* hook;     ///< Pointer to the hook table

} UShellCmd_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShell cmd  module.
 * \param [in] cmd - UShellCmd obj to be initialized
 * \param [in] name - name of the object
 * \param [in] help - help string
 * \param [in] portable - pointer to the portable table
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdInit(UShellCmd_s* const cmd,
                             char* const name,
                             UShellCmdHelp_t* const help,
                             UShellCmdExec_f* const execFunc);

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] cmd - UShellCmd obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdDeinit(UShellCmd_s* const cmd);

/**
 * \brief Set the parent
 * \param[in] cmd - the cmd to be set
 * \param[in] parent - the parent to be set
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdParentSet(UShellCmd_s* const cmd,
                                  const void* const parent);

/**
 * \brief Set the hook table
 * \param[in] cmd -  the cmd to be set
 * \param[in] hook -  the hook table to be set
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdHookTableSet(UShellCmd_s* const cmd,
                                     UShellCmdHookTable_s* const hook);

/**
 * \brief Execute the cmd
 * \param[in] cmd - UShellCmd obj to be executed
 * \param[in] argc - number of arguments
 * \param[in] argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdExec(UShellCmd_s* const cmd,
                             const int argc,
                             char* const argv []);

/**
 * \brief Get the name of the UShell  module.
 * \param [in] cmd - UShellOsal obj
 * \param [out] name - name of the object
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdNameGet(UShellCmd_s* const cmd,
                                char** const name);

/**
 * \brief Get the help string of the cmd.
 * \param [in] cmd - UShellOsal obj
 * \param [out] help - help string
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdHelpGet(UShellCmd_s* const cmd,
                                UShellCmdHelp_t** const help);

/**
 * \brief Add cmd to the list
 * \param cmdRoot - the root of the list
 * \param cmd - the cmd to be added
 * \return - error code
 */
UShellCmdErr_e UShellCmdListAdd(UShellCmd_s* const cmdRoot,
                                UShellCmd_s* const cmd);

/**
 * \brief Remove cmd from the list
 * \param[in] cmdRoot - the root of the list
 * \param[in] cmd - the cmd to be removed
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListRemove(UShellCmd_s** const cmdRoot,
                                   UShellCmd_s* const cmd);

/**
 * \brief Find cmd by name
 * \param[in] cmdRoot - the root of the list
 * \param[in] cmd - the cmd to be found
 * \param[in] isInList - true if the cmd is in the list, false otherwise
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdIsInList(UShellCmd_s* const cmdRoot,
                                 UShellCmd_s* const cmd,
                                 bool* const isInList);
/**
 * \brief Get the next cmd in the list
 * \param[in] cmdRoot - the root of the list
 * \param[in] cmd - the cmd to be found
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListNextGet(UShellCmd_s* const cmdRoot,
                                    UShellCmd_s** const cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_H_ */