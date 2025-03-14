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

/*===========================================================[MACRO DEFINITIONS]============================================*/

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

} UShellCmdErr_e;

/**
 * \brief Describe a cmd help string.
 */
typedef char* UShellCmdHelp_t;

/**
 * @brief Describe size of one item in the UShell.
 */
typedef char UShellCmdItem_t;

/**
 * @brief Describe a cmd port table.
 */
typedef struct
{
    /**
     * @brief Execute cmd
     * \param[in] cmd - pointer to the cmd object
     * \param[in] arg - pointer to the arguments (Can be NULL and it means no arguments)
     * \param[in] out - pointer to the output buffer (Can be NULL and it means no output)
     */
    UShellCmdErr_e (*exec)(void* const cmd,
                           const UShellCmdItem_t* const arg,
                           UShellCmdItem_t* const out);
} UShellCmdPortTable_s;

/**
 * \brief Describe UShellCmd.
 *
 * This structure represents a UShell command.
 *
 * \param parent Pointer to the parent object (ushell object).
 * \param name Pointer to the name of the command.
 * \param help Pointer to the help string (must be a valid string).
 */
typedef struct
{
    /* Mandatory */
    const void* parent;                      ///< Pointer to the parent object
    const char* name;                        ///< Pointer to the name of the object (aka command)
    const UShellCmdHelp_t* help;             ///< Pointer to the help string (Must be string)
    const UShellCmdPortTable_s* portable;    ///< Pointer to the port table

} UShellCmd_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShell cmd  module.
 * \param [in] cmd - UShellCmd obj to be initialized
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [in] help - help string
 * \param [in] portable - pointer to the portable table
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdInit(UShellCmd_s* const cmd,
                             const char* const name,
                             const void* const parent,
                             const UShellCmdHelp_t* const help,
                             const UShellCmdPortTable_s* const portable);

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] cmd - UShellOsal obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdDeinit(UShellCmd_s* const cmd);

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_H_ */