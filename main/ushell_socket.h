#ifndef USHELL_SOCKET_H_
#define USHELL_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

/* Project includes */
#include "ushell_osal.h"
#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes
 */
typedef enum
{
    USHELL_SOCKET_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_SOCKET_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_SOCKET_INVALID_TYPE_ERR,    ///< Exit: error - invalid type (e.g. invalid type of socket)
    USHELL_SOCKET_NOT_INIT_ERR,        ///< Exit: error - not initialized
    USHELL_SOCKET_PORT_ERR,            ///< Exit: error - port error (e.g. port layer error)
    USHELL_SOCKET_TIMEOUT_ERR,         ///< Exit: error - timeout error
    USHELL_SOCKET_EMPTY_ERR,           ///< Exit: error - empty buffer error

} UShellSocketErr_e;

/**
 * \brief Describe size of one item in the UShell VCP
 */
typedef char UShellSocketItem_t;

/**
 * \brief Describe the time in milliseconds
 */
typedef size_t UShellSocketTimeMs_t;

/**
 * \brief Enumeration of possible socket types
 * \note This is used to identify the type of socket (e.g. scan or print)
 */
typedef enum
{
    USHELL_SOCKET_TYPE_READ = 0,    ///< Input type(Scan type)
    USHELL_SOCKET_TYPE_WRITE,       ///< Output type(Print type)

} UShellSocketType_e;

/**
 * \brief Describe the configuration of the socket
 * \note This is used to identify the size and length of the item in the socket
 */
typedef struct
{
    UShellSocketType_e type;    ///< Type of the socket (input or output)
    size_t size;                ///< Size of the item in the socket

} UShellSocketCfg_s;

/**
 * \brief Enumeration of possible callback types
 * \note This is used to identify the type of callback (e.g. read or write)
 */
typedef enum
{
    USHELL_SOCKET_CB_TYPE_READ,     ///< Read callback type
    USHELL_SOCKET_CB_TYPE_WRITE,    ///< Write callback type
} UShellSocketCbType_e;

/**
 * \brief Callback function type for the socket
 * \note This is used to identify the type of callback (e.g. read or write)
 */
typedef void (*UShellSocketCb_f)(void* socket,
                                 UShellSocketCbType_e type,
                                 void* parent);

/**
 * \brief Callback table for the socket
 * \note This is used to identify the type of callback (e.g. read or write)
 */
typedef struct
{
    void (*readCb)(void* socket, UShellSocketCbType_e type, void* parent);     ///< Read callback function
    void (*writeCb)(void* socket, UShellSocketCbType_e type, void* parent);    ///< Write callback function

} UShellSocketCbTable_s;

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
    UShellOsal_s* osal;                      ///< OSAL object
    UShellOsalStreamBuffHandle_t* stream;    ///< Stream object for the uShell VCP object

    /* Internal use */
    UShellSocketCfg_s cfg;             ///< Type of the socket (input or output)
    UShellSocketCbTable_s* cbTable;    ///< Callback table for the socket

} UShellSocket_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Init uShell socket object
 * \param[in] socket - uShell object to be initialized
 * \param[in] osal - OSAL object
 * \param[in] stream - stream object
 * \param[in] cfg - configuration of the socket
 * \param[in] cbTable - table of callback functions
 * \param[in] hookTable - table of hook functions
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketInit(UShellSocket_s* const socket,
                                   UShellOsal_s* const osal,
                                   const UShellOsalStreamBuffHandle_t* const stream,
                                   const UShellSocketCfg_s cfg,
                                   const UShellSocketCbTable_s* const cbTable,
                                   void* const parent,
                                   const char* const name);

/**
 * \brief DeInit uShell socket  object
 * \param[in] socket - uShell object to be deinitialized
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellSocketErr_e UShellSocketDeInit(UShellSocket_s* const socket);

/**
 * \brief Attach a callback function to the uShell socket object
 * \param[in] socket - uShell object to be attached
 * \param[in] type - type of the callback (read or write)
 * \param[in] cbFunc - callback function to be attached
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketCbAttach(UShellSocket_s* const socket,
                                       UShellSocketCbType_e type,
                                       UShellSocketCb_f cbFunc);

/**
 * \brief Detach a callback function from the uShell socket object
 * \param[in] socket - uShell object to be detached
 * \param[in] type - type of the callback (read or write)
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketCbDetach(UShellSocket_s* const socket,
                                       UShellSocketCbType_e type);

/**
 * \brief Write data to the uShell socket object
 * \note This function is blocking and will wait for the string to be write.
 * \param[in] socket - uShell object to be printed
 * \param[in] item - item to be printed
 * \param[in] size - size of the item to be printed
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketWriteBlocking(UShellSocket_s* const socket,
                                            const UShellSocketItem_t* const item,
                                            const size_t size);

/**
 * \brief Write data to the uShell socket object in non-blocking mode
 * \note This function is non-blocking and will return the timeout period.
 * \param[in] socket - uShell object to be printed
 * \param[in] item - item to be printed
 * \param[in] size - size of the item to be printed
 * \param[in] timeout - timeout in milliseconds
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketWrite(UShellSocket_s* const socket,
                                    const UShellSocketItem_t* const item,
                                    const size_t size,
                                    const UShellSocketTimeMs_t timeout);

/**
 * \brief Scan character from the uShell socket object in non-blocking mode
 * \note This function is non-blocking and will return immediately.
 * \note This function will return USHELL_SOCKET_EMPTY_ERR if no character is available.
 * \param[in] socket - uShell object to be scanned
 * \param[in] ch - character to be scanned
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketRead(UShellSocket_s* const socket,
                                   UShellSocketItem_t* const item,
                                   const size_t size,
                                   const UShellSocketTimeMs_t timeout);

/**
 * \brief Scan string from the uShell socket object
 * \param[in] socket - socket object to be scanned
 * \param[in] str - string to be scanned
 * \param[in] size - size of the string to be scanned
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketReadBlocking(UShellSocket_s* const socket,
                                           UShellSocketItem_t* const item,
                                           const size_t size);

/**
 * \brief Check if the socket is empty
 * \param[in] socket - socket object to be checked
 * \param[in] isEmpty - pointer to the variable that will hold the result
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketIsEmpty(UShellSocket_s* const socket,
                                      bool* const isEmpty);

/**
 * \brief Print formatted string to the uShell socket object
 * \note This function is blocking and will wait for the string to be written.
 * \param[in] socket - uShell object to be printed
 * \param[in] format - format string to be printed
 * \param[out] none
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketPrint(UShellSocket_s* const socket,
                                    const char* const format,
                                    ...);

/**
 * \brief Print formatted string to the uShell socket object using va_list
 * \note This function is blocking and will wait for the string to be written.
 * \param[in] socket - uShell object to be printed
 * \param[in] format - format string to be printed
 * \param[in] args - va_list of arguments to be printed
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketPrintVaList(UShellSocket_s* const socket,
                                          const char* const format,
                                          va_list args);

/**
 * \brief Scan formatted string from the uShell socket object
 * \param[in] socket - uShell object to be scanned
 * \param[in] format - format string to be scanned
 * \param[out] none
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketScanf(UShellSocket_s* const socket,
                                    const char* const format,
                                    ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_SOCKET_H_ */