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

/**
 * \brief Description of the maximum number of arguments in the UShellCmd
*/
#ifndef USHELL_CMD_BUFFER_SIZE
    #define USHELL_CMD_BUFFER_SIZE 100
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes returned by the UShellCmd module.
 */
typedef enum
{
    USHELL_CMD_NO_ERR = 0,            ///< Exit: no errors (success)
    USHELL_CMD_INVALID_ARGS_ERR,      ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_CMD_NOT_INIT_ERR,          ///< Exit: error - not initialized
    USHELL_CMD_PORT_ERR,              ///< Exit: error - port error (e.g. port layer error)
    USHELL_CMD_EXECUTE_NUMB_ARGS_ERR, ///< Exit: error - number of arguments error
} UShellCmdErr_e;


/**
 * \brief Descriibe size of one item in the UShellCmd
*/
typedef char UShellCmdItem_c;

/**
 * \brief Descriibe size of one item in the UShellCmd
*/
typedef size_t UShellCmdSize_t;

/**
 * \brief Prototype of the function to execute a command.
*/
typedef UShellCmdErr_e (*UShelCmdExecute_t)( const void* const cmd, const int argc, const char* const argv[]);

/**
 * \brief Portable structure for the command
*/
typedef struct
{
    UShellCmdErr_e (*execute)( const void* const cmd, const int argc, const char* const argv[]);    ///< Pointer to the function to execute the command
    char* help;                                                                                     ///< Pointer to the help string
}UShellCmdPortable_s;

/**
 * \brief Prototype of the function to print a string.
*/
typedef UShellCmdErr_e (*UShellCmdPrintCb_t)(const void* const cmd, const char* const str);

/**
 * \brief Describe a cmd help string
*/
typedef char* UShellCmdHelp_t;

typedef struct
{
    UShellCmdItem_c buff[USHELL_CMD_BUFFER_SIZE];   ///< Pointer to the buffer
    UShellCmdSize_t size;                           ///< Size of the buffer
}UShellCmdIo_s;


/**
 * \brief Descriibe UShellCmd
*/
typedef struct
{
    const void* parent;             ///< Pointer to the parent object
    const char* name;               ///< Pointer to the name of the object

    size_t argMax;                  ///< Maximum number of arguments
    UShellCmdPrintCb_t print;       ///< Pointer to the function to print a string
    const UShellCmdPortable_s* portable;   ///< Portable structure for the command

    UShellCmdIo_s io;               ///< UShellCmd IO structure

}UShellCmd_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShell cmd  module.
 * \param [in] cmd - UShellCmd obj to be initialized
 * \param [in] argMax - maximum number of arguments of the cmd
 * \param [in] print - pointer to the function to print a string
 * \param [in] portable - portable structure for the command
 * \param [in] portTable - port table to be used
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdInit(UShellCmd_s* const cmd, size_t argMax,
                                                     const UShellCmdPrintCb_t print,
                                                     const UShellCmdPortable_s* const portable,
                                                     const char* const name,
                                                     const void* const parent);

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
UShellCmdErr_e UShellCmdNameGet(UShellCmd_s* const cmd, char** const name);

/**
 * \brief Get the parent of the UShell  module.
 * \param [in] cmd - UShellOsal obj
 * \param [out] parent - pointer to the parent object
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdArgMaxGet(UShellCmd_s* const cmd, size_t* const argMax);

/**
 * \brief Execute the UShell module.
 * \param [in] cmd - UShellOsal obj
 * \param [in] argc - number of arguments
 * \param [in] argv - arguments
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdExecute(UShellCmd_s* const cmd, const int argc, const char* const argv[]);

/**
 * \brief Get the help string of the cmd.
 * \param [in] cmd - UShellOsal obj
 * \param [out] help - help string
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellCmdHelpGet(UShellCmd_s* const cmd, UShellCmdHelp_t** const help);

/**
 * \brief Get the IO of the cmd.
 * \note This function called after the UShellCmdExecute
 * \param [in] cmd - UShellOsal obj
 * \param [out] buff - pointer to the buffer
 * \param [out] size - size of the buffer
 * \return UShellOsalErr_e - error code
*/
UShellCmdErr_e UShellIoGet(UShellCmd_s* const cmd, UShellCmdItem_c* const buff, UShellCmdSize_t* const size);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_CMD_H_ */