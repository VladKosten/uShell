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
 * \brief Describe size of one item in the UShell.
 */
typedef char UShellCmdItem_t;

/**
 * \brief Describe a cmd exec function.
 */
typedef UShellCmdErr_e(UShellCmdExec_f)(void* const cmd,
                                        const UShellCmdItem_t* const arg);

/**
 * @brief Describe a cmd hook function.
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
                             const UShellCmdExec_f* const execFunc);

/**
 * \brief Deinitialize the UShell  module.
 * \param [in] cmd - UShellCmd obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
UShellCmdErr_e UShellCmdDeinit(UShellCmd_s* const cmd);

/**
 * @brief Set the parent
 * @param[in] cmd - the cmd to be set
 * @param[in] parent - the parent to be set
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdParentSet(UShellCmd_s* const cmd,
                                  const void* const parent);

/**
 * @brief Execute the cmd
 * @param[in] cmd - UShellCmd obj to be executed
 * @param[in] arg - pointer to the arguments (Can be NULL and it means no arguments)
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdExec(UShellCmd_s* const cmd,
                             const UShellCmdItem_t* const arg);

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
 * @brief Add cmd to the list
 * @param cmdRoot - the root of the list
 * @param cmd - the cmd to be added
 * @return - error code
 */
UShellCmdErr_e UShellCmdListAdd(UShellCmd_s* const cmdRoot,
                                UShellCmd_s* const cmd);

/**
 * @brief Remove cmd from the list
 * @param[in] cmdRoot - the root of the list
 * @param[in] cmd - the cmd to be removed
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListRemove(UShellCmd_s* const cmdRoot,
                                   UShellCmd_s* const cmd);

/**
 * @brief Find cmd by name
 * @param[in] cmdRoot - the root of the list
 * @param[in] cmd - the cmd to be found
 * @param[in] isInList - true if the cmd is in the list, false otherwise
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdIsInList(UShellCmd_s* const cmdRoot,
                                 UShellCmd_s* const cmd,
                                 bool* const isInList);
/**
 * @brief Get the next cmd in the list
 * @param[in] cmdRoot - the root of the list
 * @param[in] cmd - the cmd to be found
 * @return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
UShellCmdErr_e UShellCmdListNextGet(UShellCmd_s* const cmdRoot,
                                    UShellCmd_s** const cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_CMD_H_ */