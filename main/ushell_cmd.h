#ifndef USHELL_CMD_H_
#define USHELL_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

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
    USHELL_CMD_NO_ERR = 0,            ///< Exit: no errors (success)
    USHELL_CMD_INVALID_ARGS_ERR,      ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_CMD_NOT_INIT_ERR,          ///< Exit: error - not initialized

} UShellCmdErr_e;

/**
 * \brief Prototype of the function to execute a command.
*/
typedef UShellCmdErr_e (*UShelCmdExecute_t)( const void* const cmd, const char* const args, const size_t argsLen);

/**
 * \brief Describe a cmd.
*/
typedef char* UShellCmdHelp_t;

/**
 * \brief Descriibe UShellCmd
*/
typedef struct
{
    const void* parent;         ///< Pointer to the parent object

    const char* name;           ///< Pointer to the name of the object
    size_t arg_max;             ///< Maximum number of arguments

    UShelCmdExecute_t execute;  ///< Pointer to the function to execute the command
    UShellCmdHelp_t help;       ///< Pointer to the help string
}UShellCmd_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShell Hal module.
 * \param [in] osal - UShellOsal obj to be initialized
 * \param [in] portTable - port table to be used
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdInit(UShellCmd_s* const osal, const UShellCmd_s* const cmd, const char* const name, const void* const parent);

/**
 * \brief Deinitialize the UShell Hal module.
 * \param [in] osal - UShellOsal obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdDeinit(UShellCmd_s* const osal);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_CMD_H_ */