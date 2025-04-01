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
#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief Description of the maximum size of the buffer in the UShell VCP.
 */
#ifndef USHELL_VCP_BUFFER_SIZE
    #define USHELL_VCP_BUFFER_SIZE 128U
#endif

/**
 * \brief Name of the uShell VCP thread.
 */
#ifndef USHELL_VCP_THREAD_NAME
    #define USHELL_VCP_THREAD_NAME "USHELL_VCP"
#endif

/**
 * \brief Stack size for the uShell VCP  thread.
 */
#ifndef USHELL_VCP_THREAD_STACK_SIZE_BYTE
    #define USHELL_VCP_THREAD_STACK_SIZE_BYTE 512U
#endif

/**
 * \brief Priority of the uShell VCP thread.
 */
#ifndef USHELL_VCP_THREAD_PRIORITY
    #define USHELL_VCP_THREAD_PRIORITY USHELL_OSAL_THREAD_PRIORITY_LOW
#endif

/**
 * \brief Timeout for tx operation in the uShell VCP.
 */
#ifndef USHELL_VCP_TX_TIMEOUT_MS
    #define USHELL_VCP_TX_TIMEOUT_MS 3000U
#endif

/**
 * \brief Redirect standard input/output to the uShell VCP.
 *
 * This macro enables or disables the redirection of standard input/output to the uShell VCP.
 * getc() and putc() functions are redirected to the uShell VCP. (NOW only for gcc compiler)
 */
#ifndef USHELL_VCP_REDIRECT_STDIO
    #define USHELL_VCP_REDIRECT_STDIO TRUE
#endif

/**
 * \brief Timer period for the uShell VCP.
 *
 * This macro defines the timer period used in the uShell VCP.
 */
#ifndef USHELL_VCP_TIMER_INSPECT_NAME
    #define USHELL_VCP_TIMER_INSPECT_NAME "USHELL_VCP_TIMER"
#endif

/**
 * \brief Timer period for the uShell VCP.
 *
 * This macro defines the timer period used in the uShell VCP for inspection.
 */
#ifndef USHELL_VCP_TIMER_INSPECT_PERIOD_MS
    #define USHELL_VCP_TIMER_INSPECT_PERIOD_MS 1000U
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Describe size of one item in the UShell VCP
 */
typedef char UShellVcpItem_t;

/**
 * \brief Enumeration of possible error codes
 */
typedef enum
{
    USHELL_VCP_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_VCP_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_VCP_NOT_INIT_ERR,        ///< Exit: error - not initialized
    USHELL_VCP_PORT_ERR,            ///< Exit: error - port error (e.g. port layer error)
    USHELL_VCP_XFER_ERR,            ///< Exit: error - transfer error
    USHELL_VCP_TIMEOUT_ERR,         ///< Exit: error - timeout error
    USHELL_VCP_EMPTY_ERR,           ///< Exit: error - empty buffer error

} UShellVcpErr_e;

/**
 * \brief Description of the uShell VCP IO object
 * \note This object is used to store the buffer for input/output operations in the uShell
 */
typedef struct
{
    UShellVcpItem_t buffer [USHELL_VCP_BUFFER_SIZE];    ///< Buffer for commands
    size_t ind;                                         ///< Size of the buffer

} UShellVcpIo_s;

/**
 * \brief Description of the uShell VCP object
 * \note This object is used to store the uShell VCP object
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
    UShellVcpIo_s io;     ///< IO object
    bool usedForStdIO;    ///< Flag to indicate if the object is used for stdio (ONLY ONE INSTANCE CAN BE USED FOR STDIO)

} UShellVcp_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init uShell vcp object
 * \param[in] vcp - uShell object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] usedForStdIO - flag to indicate if the object is used for stdio (ONLY ONE INSTANCE CAN BE USED FOR STDIO)
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellVcpInit(UShellVcp_s* const vcp,
                             const UShellOsal_s* const osal,
                             const UShellHal_s* const hal,
                             const bool usedForStdIO,
                             void* const parent,
                             const char* const name);

/**
 * \brief DeInit uShell vcp  object
 * \param[in] vcp - uShell object to be deinitialized
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellVcpDeInit(UShellVcp_s* const vcp);

/**
 * \brief Print string to the uShell vcp object
 * \note This function is blocking and will wait for the string to be received.
 * \param[in] vcp - uShell object to be printed
 * \param[in] str - string to be printed
 * \param[out] none
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintStr(UShellVcp_s* const vcp,
                                 const char* const str);

/**
 * \brief Print char to the uShell vcp object
 * \note This function is blocking and will wait for the string to be received.
 * \param vcp - uShell object to be printed
 * \param ch - char to be printed
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintChar(UShellVcp_s* const vcp,
                                  const char ch);

/**
 * \brief Scan char from the uShell vcp object
 * \note: This function is blocking and will wait for the character to be received.
 * \param[in] vcp - uShell object to be scanned
 * \param[in] ch - char to be scanned
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanChar(UShellVcp_s* const vcp,
                                 char* const ch);

/**
 * \brief Scan character from the uShell vcp object in non-blocking mode
 * \note: This function is non-blocking and will return immediately.
 * \note: This function will return USHELL_VCP_EMPTY_ERR if no character is available.
 * \param[in] vcp - uShell object to be scanned
 * \param[in] ch - character to be scanned
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanCharNonBlock(UShellVcp_s* const vcp,
                                         char* const ch);

/**
 * \brief Scan string from the uShell vcp object
 * \note: This function is blocking and will wait for the string to be received.
 * \param[in] vcp - uShell object to be scanned
 * \param[in] str - string to be scanned
 * \param[in] size - size of the string to be scanned
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanStr(UShellVcp_s* const vcp,
                                char* const str,
                                const size_t maxSize);

/**
 * \brief Check if the uShell vcp object is empty
 * \param vcp - uShell object to be checked
 * \param isEmpty - pointer to store the result indicating if the object is empty
 * \return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanIsEmpty(UShellVcp_s* const vcp,
                                    bool* const isEmpty);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_VCP_H_ */