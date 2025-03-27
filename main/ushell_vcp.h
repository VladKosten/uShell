#ifndef USHELL_VCP_H_
#define USHELL_VCP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Project includes */
#include "ushell_hal.h"
#include "ushell_osal.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * @brief Description of the maximum size of the buffer in the UShell
 *
 * This macro defines the maximum size of the buffer used in the UShell.
 */
#ifndef USHELL_VCP_BUFFER_SIZE
    #define USHELL_VCP_BUFFER_SIZE 64
#endif

/**
 * @brief Name of the uShell thread.
 *
 * This macro defines the name used when creating the uShell thread.
 */
#ifndef USHELL_VCP_THREAD_NAME
    #define USHELL_VCP_THREAD_NAME "USHELL_VCP"
#endif

/**
 * @brief Stack size for the uShell thread.
 *
 * This macro defines the stack size allocated for the uShell thread.
 */
#ifndef USHELL_VCP_THREAD_STACK_SIZE
    #define USHELL_VCP_THREAD_STACK_SIZE 512U
#endif

/**
 * @brief Priority of the uShell thread.
 *
 * This macro defines the scheduling priority used by the uShell thread.
 */
#ifndef USHELL_VCP_THREAD_PRIORITY
    #define USHELL_VCP_THREAD_PRIORITY USHELL_OSAL_THREAD_PRIORITY_LOW
#endif

/**
 * @brief Timeout for tx operation in the uShell VCP.
 *
 * This macro defines the timeout duration for the tx operation in the uShell VCP.
 */
#ifndef USHELL_VCP_TX_TIMEOUT_MS
    #define USHELL_VCP_TX_TIMEOUT_MS 3000U
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * @brief Describe size of one item in the UShell
 */
typedef char UShellVcpItem_t;

/**
 * @brief Enumeration of possible error codes returned by the UShell Hal module.
 */
typedef enum
{
    USHELL_VCP_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_VCP_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_VCP_NOT_INIT_ERR,        ///< Exit: error - not initialized
    USHELL_VCP_PORT_ERR,            ///< Exit: error - port error (e.g. port layer error)
    USHELL_VCP_XFER_ERR,            ///< Exit: error - transfer error
    USHELL_VCP_TIMEOUT_ERR,         ///< Exit: error - timeout error

} UShellVcpErr_e;

/**
 * @brief Description of the uShell IO object
 * @note This object is used to store the buffer for input/output operations in the uShell
 */
typedef struct
{
    UShellVcpItem_t buffer [USHELL_VCP_BUFFER_SIZE];    ///< Buffer for commands
    size_t ind;                                         ///< Size of the buffer

} UShellVcpIo_s;

/**
 * @brief Description of the uShell object
 * @note This object is used to store the uShell VCP object
 */
typedef struct
{
    /* Non-optional fields */
    const void* parent;    ///< Parent object
    const char* name;      ///< Name of the object

    /* Dependencies */
    const UShellOsal_s* osal;    ///< OSAL object
    const UShellHal_s* hal;      ///< HAL object

    /* Internal use  */
    UShellVcpIo_s io;    ///< IO object

} UShellVcp_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Init uShell vcp object
 * \param[in] vcp - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellVcpInit(UShellVcp_s* const vcp,
                             const UShellOsal_s* const osal,
                             const UShellHal_s* const hal,
                             void* const parent,
                             const char* const name);

/**
 * @brief DeInit uShell vcp  object
 * \param[in] vcp - uShell object to be deinitialized
 * \param[out] none
 * @return USHELL_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellVcpDeInit(UShellVcp_s* const vcp);

/**
 * @brief Print string to the uShell vcp object
 * @param[in] vcp - uShell object to be printed
 * @param[in] str - string to be printed
 * @param[out] none
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintStr(UShellVcp_s* const vcp, const char* const str);

/**
 * @brief Print char to the uShell vcp object
 * @param vcp - uShell object to be printed
 * @param ch - char to be printed
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintChar(UShellVcp_s* const vcp, const char ch);

/**
 * @brief Scan char from the uShell vcp object
 * @param[in] vcp - uShell object to be scanned
 * @param[in] ch - char to be scanned
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanChar(UShellVcp_s* const vcp,
                                 char* const ch);

/**
 * @brief Scan string from the uShell vcp object
 * @param[in] vcp - uShell object to be scanned
 * @param[in] str - string to be scanned
 * @param[in] size - size of the string to be scanned
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanStr(UShellVcp_s* const vcp,
                                char* const str,
                                const size_t maxSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_VCP_H_ */