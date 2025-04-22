/**
* \file         ushell_socket.c
* \brief        The file contains the implementation of the UShell socket module.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright    MIT License (c) 2025
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...

*/
//===============================================================================[ INCLUDE ]========================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Project includes */
#include "ushell_socket.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_SOCKET_ASSERT
    #define USHELL_SOCKET_ASSERT(cond)
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Print a block of data to the uShell socket object.
 * \param[in] socket   - uShell socket object.
 * \param[in] data  - pointer to the data to be printed.
 * \param[in] len   - length of the data to send.
 * \return UShellSocketErr_e - error code. non-zero means an error occurred.
 */
static UShellSocketErr_e uShellSocketWriteBytesBlock(UShellSocket_s* const socket,
                                                     const char* data,
                                                     size_t len);

/**
 * \brief Print a block of data to the uShell socket object in non-blocking mode
 * \note This function is non-blocking and will return the timeout period.
 * \param[in] socket - uShell object to be printed
 * \param[in] data - item to be printed
 * \param[in] len - size of the item to be printed
 * \param[in] timeout - timeout in milliseconds
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
static UShellSocketErr_e uShellSocketWriteBytesNonBlock(UShellSocket_s* const socket,
                                                        const char* data,
                                                        size_t len,
                                                        UShellSocketTimeMs_t timeout);

/**
 * \brief Read a block of data from the uShell socket object in non-blocking mode
 * \note This function is non-blocking and will return immediately if no data is available.
 * \param[in] socket - uShell object to be read
 * \param[out] data - buffer to store the read data
 * \param[in] len - size of the buffer
 * \param[in] timeout - timeout in milliseconds
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
static UShellSocketErr_e uShellSocketReadNonBlock(UShellSocket_s* const socket,
                                                  char* data,
                                                  size_t len,
                                                  UShellSocketTimeMs_t timeout);

/**
 * \brief Read a block of data from the uShell socket object in blocking mode
 * \note This function is blocking and will wait for the data to be read.
 * \param[in] socket - uShell object to be read
 * \param[out] data - buffer to store the read data
 * \param[in] len - size of the buffer
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e uShellSocketReadBlocking(UShellSocket_s* const socket,
                                           char* data,
                                           size_t len);
/**
 * \brief Process the format string and write to the uShell socket object
 * \param[in] socket - uShell object to be processed
 * \param[in] format - format string
 * \param[in] args - variable arguments list
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
static UShellSocketErr_e uShellSocketProcessFormat(UShellSocket_s* const socket,
                                                   const char* const format,
                                                   va_list args);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Init uShell socket object
 * \param[in] socket - uShell object to be initialized
 * \param[in] osal - OSAL object
 * \param[in] stream - stream object
 * \param[in] cfg - configuration of the socket
 * \param[in] cbTable - table of callback functions
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
                                   const char* const name)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    /* Initialize the socket object */
    do
    {
        /* Check inp parameter*/
        if ((socket == NULL) ||
            (stream == NULL) ||
            (parent == NULL) ||
            (cfg.size <= 0) ||
            (cbTable == NULL))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if ((cfg.type != USHELL_SOCKET_TYPE_READ) &&
            (cfg.type != USHELL_SOCKET_TYPE_WRITE))
        {
            /* Type is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the cbTable */
        if ((cbTable->readCb == NULL) ||
            (cbTable->writeCb == NULL))
        {
            /* Callback table is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the cfg */
        if (cfg.size <= 0)
        {
            /* Configuration is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Clear the object */
        memset(socket, 0, sizeof(UShellSocket_s));

        /* Set attribute */
        socket->parent = parent;
        socket->name = name;
        socket->cfg = cfg;
        socket->stream = (UShellOsalStreamBuffHandle_t*) stream;
        socket->osal = osal;
        socket->cbTable = (UShellSocketCbTable_s*) cbTable;

    } while (0);

    return status;
}

/**
 * \brief DeInit uShell socket  object
 * \param[in] socket - uShell object to be deinitialized
 * \param[out] none
 * \return USHELL_NO_ERR if success, otherwise error code
 */
UShellSocketErr_e UShellSocketDeInit(UShellSocket_s* const socket)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (socket == NULL)
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Clear the object */
        memset(socket, 0, sizeof(UShellSocket_s));

    } while (0);

    return status;
}

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
                                            const size_t size)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (item == NULL) ||
            (size == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Print data to the socket object */
        status = uShellSocketWriteBytesBlock(socket,
                                             item,
                                             size);

        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Print failed */
            USHELL_SOCKET_ASSERT(0);
            break;
        }

    } while (0);

    return status;
}

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
                                    const UShellSocketTimeMs_t timeout)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (item == NULL) ||
            (size == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (socket->cfg.type != USHELL_SOCKET_TYPE_WRITE)
        {
            /* Type is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Print data to the socket object */
        status = uShellSocketWriteBytesNonBlock(socket,
                                                item,
                                                size,
                                                timeout);

        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Print failed */
            USHELL_SOCKET_ASSERT(0);
            break;
        }

    } while (0);

    return status;
}

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
                                   const UShellSocketTimeMs_t timeout)
{

    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    /* Print string to the socket object */
    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (item == NULL) ||
            (size == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (socket->cfg.type != USHELL_SOCKET_TYPE_READ)
        {
            /* Type is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Read from the stream buffer */
        status = uShellSocketReadNonBlock(socket,
                                          item,
                                          size,
                                          timeout);
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Stream buffer is invalid */
            USHELL_SOCKET_ASSERT(0);
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Scan string from the uShell socket object
 * \param[in] socket - socket object to be scanned
 * \param[in] str - string to be scanned
 * \param[in] size - size of the string to be scanned
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketReadBlocking(UShellSocket_s* const socket,
                                           UShellSocketItem_t* const item,
                                           const size_t size)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    /* Print string to the socket object */
    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (item == NULL) ||
            (size == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (socket->cfg.type != USHELL_SOCKET_TYPE_READ)
        {
            /* Type is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Read from the stream buffer */
        status = uShellSocketReadBlocking(socket,
                                          item,
                                          size);
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Stream buffer is invalid */
            USHELL_SOCKET_ASSERT(0);
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Check if the socket is empty
 * \param[in] socket - socket object to be checked
 * \param[in] isEmpty - pointer to the variable that will hold the result
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketIsEmpty(UShellSocket_s* const socket,
                                      bool* const isEmpty)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* const osal = (UShellOsal_s*) socket->osal;
    UShellOsalStreamBuffHandle_t streamBuff = socket->stream;
    bool isEmptyBuff = false;

    /* Print string to the socket object */
    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (osal == NULL) ||
            (isEmpty == NULL))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if ((socket->cfg.type != USHELL_SOCKET_TYPE_READ) &&
            (socket->cfg.type != USHELL_SOCKET_TYPE_WRITE))
        {
            /* Type is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Read from the stream buffer */
        osalStatus = UShellOsalStreamBuffIsEmpty(osal, streamBuff, &isEmptyBuff);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Stream buffer is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Set the result */
        *isEmpty = isEmptyBuff;

    } while (0);

    return status;
}

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
                                    ...)
{
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    va_list args;
    va_start(args, format);

    do
    {
        /* Check input parameters */
        if ((socket == NULL) || (format == NULL))
        {
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (socket->cfg.type != USHELL_SOCKET_TYPE_WRITE)
        {
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Process the format string */
        status = uShellSocketProcessFormat(socket, format, args);

    } while (0);

    va_end(args);
    return status;
}

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
                                          va_list args)
{
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) || (format == NULL))
        {
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (socket->cfg.type != USHELL_SOCKET_TYPE_WRITE)
        {
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Process the format string */
        status = uShellSocketProcessFormat(socket, format, args);
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Print failed */
            USHELL_SOCKET_ASSERT(0);
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief Scan formatted string from the uShell socket object
 * \param[in] socket - uShell object to be scanned
 * \param[in] format - format string to be scanned
 * \param[out] none
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e UShellSocketScanf(UShellSocket_s* const socket,
                                    const char* const format,
                                    ...)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    va_list args;
    va_start(args, format);

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (format == NULL))
        {
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Check the type of the socket */
        if (socket->cfg.type != USHELL_SOCKET_TYPE_READ)
        {
            /* Type is invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_TYPE_ERR;
            break;
        }

        /* Iterate through the format string */
        for (const char* p = format; *p != '\0'; p++)
        {
            if (*p == '%')    // Handle format specifiers
            {
                p++;                           // Move to the next character
                if (*p == 'd' || *p == 'i')    // Decimal integer
                {
                    int* value = va_arg(args, int*);
                    char numStr [12];    // Enough for a 32-bit
                    snprintf(numStr, sizeof(numStr), "%d", *value);
                    status = uShellSocketWriteBytesBlock(socket, numStr, strlen(numStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == 'c')    // Character
                {
                    char* value = va_arg(args, char*);
                    status = uShellSocketWriteBytesBlock(socket, value, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == 's')    // String
                {
                    char* str = va_arg(args, char*);
                    status = uShellSocketWriteBytesBlock(socket, str, strlen(str));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == 'x' || *p == 'X')    // Hexadecimal
                {
                    unsigned int* value = va_arg(args, unsigned int*);
                    char hexStr [9];    // Enough for a 32-bit hex number
                    snprintf(hexStr, sizeof(hexStr), "%X", *value);
                    status = uShellSocketWriteBytesBlock(socket, hexStr, strlen(hexStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }

                else if (*p == 'f')    // Float
                {
                    double* value = va_arg(args, double*);
                    char floatStr [32];    // Enough for a double
                    snprintf(floatStr, sizeof(floatStr), "%f", *value);
                    status = uShellSocketWriteBytesBlock(socket, floatStr, strlen(floatStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == 'l')    // Long
                {
                    long* value = va_arg(args, long*);
                    char longStr [32];    // Enough for a long
                    snprintf(longStr, sizeof(longStr), "%ld", *value);
                    status = uShellSocketWriteBytesBlock(socket, longStr, strlen(longStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == 'u')    // Unsigned integer
                {
                    unsigned int* value = va_arg(args, unsigned int*);
                    char uintStr [12];    // Enough for a 32-bit unsigned int
                    snprintf(uintStr, sizeof(uintStr), "%u", *value);
                    status = uShellSocketWriteBytesBlock(socket, uintStr, strlen(uintStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == 'p')    // Pointer
                {
                    void** ptr = va_arg(args, void**);
                    char ptrStr [16];    // Enough for a pointer
                    snprintf(ptrStr, sizeof(ptrStr), "%p", *ptr);
                    status = uShellSocketWriteBytesBlock(socket, ptrStr, strlen(ptrStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else if (*p == '%')    // Literal '%'
                {
                    status = uShellSocketWriteBytesBlock(socket, p, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }

                else if (*p == 'b')    // Binary
                {
                    unsigned int* value = va_arg(args, unsigned int*);
                    char binStr [33];    // Enough for a 32-bit binary number
                    snprintf(binStr, sizeof(binStr), "%u", *value);
                    status = uShellSocketWriteBytesBlock(socket, binStr, strlen(binStr));
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        USHELL_SOCKET_ASSERT(0);
                        break;
                    }
                }
                else
                {
                    // Unsupported format specifier
                    USHELL_SOCKET_ASSERT(0);
                    break;
                }
            }

            else    // Regular character
            {
                status = uShellSocketWriteBytesBlock(socket, p, 1);
                if (status != USHELL_SOCKET_NO_ERR)
                {
                    USHELL_SOCKET_ASSERT(0);
                    break;
                }
            }

            /* Check if the write was successful */
            if (status != USHELL_SOCKET_NO_ERR)
            {
                /* Print failed */
                USHELL_SOCKET_ASSERT(0);
                break;
            }
        }

    } while (0);

    va_end(args);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Print a block of data to the uShell socket object.
 * \param[in] socket   - uShell socket object.
 * \param[in] data  - pointer to the data to be printed.
 * \param[in] len   - length of the data to send.
 * \return UShellSocketErr_e - error code. non-zero means an error occurred.
 */
static UShellSocketErr_e uShellSocketWriteBytesBlock(UShellSocket_s* const socket,
                                                     const char* data,
                                                     size_t len)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    UShellOsalStreamBuffHandle_t streamBuff = socket->stream;
    UShellOsal_s* const osal = (UShellOsal_s*) socket->osal;
    uint8_t* ptr = (uint8_t*) data;
    size_t totalSent = 0U;
    size_t remaining = len;
    size_t chunkSize = 0U;
    size_t chunkWriteCount = 0U;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (streamBuff == NULL) ||
            (data == NULL) ||
            (len == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Loop to send the data in chunks */
        while (remaining > 0U)
        {
            /* Calculate the chunk size */
            chunkSize = (remaining < socket->cfg.size)
                            ? remaining
                            : socket->cfg.size;

            /* Write to the stream buffer */
            chunkWriteCount = UShellOsalStreamBuffSendBlocking(osal,
                                                               streamBuff,
                                                               (void*) &ptr [totalSent],
                                                               chunkSize);
            /* Check if the write was successful */
            if (chunkWriteCount == 0U)
            {
                /* Send error */
                USHELL_SOCKET_ASSERT(0);
                status = USHELL_SOCKET_PORT_ERR;
                break;
            }

            /* Update the total sent bytes and remaining bytes */
            totalSent += chunkWriteCount;
            remaining -= chunkWriteCount;

            /* Call callback function if available */
            if (socket->cbTable->writeCb != NULL)
            {
                socket->cbTable->writeCb(socket, USHELL_SOCKET_CB_TYPE_WRITE, (void*) socket->parent);
            }
        }

    } while (0);

    return status;
}

/**
 * \brief Print a block of data to the uShell socket object in non-blocking mode
 * \note This function is non-blocking and will return the timeout period.
 * \param[in] socket - uShell object to be printed
 * \param[in] data - item to be printed
 * \param[in] len - size of the item to be printed
 * \param[in] timeout - timeout in milliseconds
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
static UShellSocketErr_e uShellSocketWriteBytesNonBlock(UShellSocket_s* const socket,
                                                        const char* data,
                                                        size_t len,
                                                        UShellSocketTimeMs_t timeout)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    UShellOsalStreamBuffHandle_t streamBuff = socket->stream;
    UShellOsal_s* const osal = (UShellOsal_s*) socket->osal;
    uint8_t* ptr = (uint8_t*) data;
    size_t totalSent = 0U;
    size_t remaining = len;
    size_t chunkSize = 0U;
    size_t chunkWriteCount = 0U;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (streamBuff == NULL) ||
            (data == NULL) ||
            (len == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Loop to send the data in chunks */
        while (remaining > 0U)
        {
            /* Calculate the chunk size */
            chunkSize = (remaining < socket->cfg.size)
                            ? remaining
                            : socket->cfg.size;

            /* Write to the stream buffer */
            chunkWriteCount = UShellOsalStreamBuffSend(osal,
                                                       streamBuff,
                                                       (void*) &ptr [totalSent],
                                                       chunkSize,
                                                       timeout);
            /* Check if the write was successful */
            if (chunkWriteCount == 0U)
            {
                /* Send error */
                USHELL_SOCKET_ASSERT(0);
                status = USHELL_SOCKET_TIMEOUT_ERR;
                break;
            }

            /* Update the total sent bytes and remaining bytes */
            totalSent += chunkWriteCount;
            remaining -= chunkWriteCount;

            /* Call callback function if available */
            if (socket->cbTable->writeCb != NULL)
            {
                socket->cbTable->writeCb(socket, USHELL_SOCKET_CB_TYPE_WRITE, (void*) socket->parent);
            }
        }

    } while (0);

    return status;
}

/**
 * \brief Read a block of data from the uShell socket object in non-blocking mode
 * \note This function is non-blocking and will return immediately if no data is available.
 * \param[in] socket - uShell object to be read
 * \param[out] data - buffer to store the read data
 * \param[in] len - size of the buffer
 * \param[in] timeout - timeout in milliseconds
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
static UShellSocketErr_e uShellSocketReadNonBlock(UShellSocket_s* const socket,
                                                  char* data,
                                                  size_t len,
                                                  UShellSocketTimeMs_t timeout)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    UShellOsalStreamBuffHandle_t streamBuff = socket->stream;
    UShellOsal_s* const osal = (UShellOsal_s*) socket->osal;
    uint8_t* ptr = (uint8_t*) data;
    size_t totalReceived = 0U;
    size_t remaining = len;
    size_t chunkSize = 0U;
    size_t chunkReadCount = 0U;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (streamBuff == NULL) ||
            (data == NULL) ||
            (len == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Loop to read the data in chunks */
        while (remaining > 0U)
        {
            /* Calculate the chunk size */
            chunkSize = (remaining < socket->cfg.size)
                            ? remaining
                            : socket->cfg.size;

            /* Read from the stream buffer */
            chunkReadCount = UShellOsalStreamBuffReceive(osal,
                                                         streamBuff,
                                                         (void*) &ptr [totalReceived],
                                                         chunkSize,
                                                         timeout);

            /* Check if the read was successful */
            if (chunkReadCount == 0U)
            {
                /* No data received or timeout occurred */
                status = USHELL_SOCKET_TIMEOUT_ERR;
                break;
            }

            /* Update the total received bytes and remaining bytes */
            totalReceived += chunkReadCount;
            remaining -= chunkReadCount;

            /* Call callback function if available */
            if (socket->cbTable->readCb != NULL)
            {
                socket->cbTable->readCb(socket, USHELL_SOCKET_CB_TYPE_READ, (void*) socket->parent);
            }
        }

    } while (0);

    return status;
}

/**
 * \brief Read a block of data from the uShell socket object in blocking mode
 * \note This function is blocking and will wait for the data to be read.
 * \param[in] socket - uShell object to be read
 * \param[out] data - buffer to store the read data
 * \param[in] len - size of the buffer
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
UShellSocketErr_e uShellSocketReadBlocking(UShellSocket_s* const socket,
                                           char* data,
                                           size_t len)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;
    UShellOsalStreamBuffHandle_t streamBuff = socket->stream;
    UShellOsal_s* const osal = (UShellOsal_s*) socket->osal;
    uint8_t* ptr = (uint8_t*) data;
    size_t totalReceived = 0U;
    size_t remaining = len;
    size_t chunkSize = 0U;
    size_t chunkReadCount = 0U;

    do
    {
        /* Check input parameters */
        if ((socket == NULL) ||
            (streamBuff == NULL) ||
            (data == NULL) ||
            (len == 0U))
        {
            /* Input parameters are invalid */
            USHELL_SOCKET_ASSERT(0);
            status = USHELL_SOCKET_INVALID_ARGS_ERR;
            break;
        }

        /* Loop to read the data in chunks */
        while (remaining > 0U)
        {
            /* Calculate the chunk size */
            chunkSize = (remaining < socket->cfg.size)
                            ? remaining
                            : socket->cfg.size;

            /* Read from the stream buffer */
            chunkReadCount = UShellOsalStreamBuffReceiveBlocking(osal,
                                                                 streamBuff,
                                                                 (void*) &ptr [totalReceived],
                                                                 chunkSize);

            /* Check if the read was successful */
            if (chunkReadCount == 0U)
            {
                /* No data received or timeout occurred */
                status = USHELL_SOCKET_PORT_ERR;
                break;
            }

            /* Update the total received bytes and remaining bytes */
            totalReceived += chunkReadCount;
            remaining -= chunkReadCount;

            /* Call callback function if available */
            if (socket->cbTable->readCb != NULL)
            {
                socket->cbTable->readCb(socket, USHELL_SOCKET_CB_TYPE_READ, (void*) socket->parent);
            }
        }

    } while (0);

    return status;
}

/**
 * \brief Process the format string and write to the uShell socket object
 * \param[in] socket - uShell object to be processed
 * \param[in] format - format string
 * \param[in] args - variable arguments list
 * \return UShellSocketErr_e - error code. non-zero = an error has occurred;
 */
static UShellSocketErr_e uShellSocketProcessFormat(UShellSocket_s* const socket,
                                                   const char* const format,
                                                   va_list args)
{
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;

    /* Iterate through the format string */
    for (const char* p = format; *p != '\0'; p++)
    {
        if (*p == '%')    // Handle format specifiers
        {
            p++;                           // Move to the next character
            if (*p == 'd' || *p == 'i')    // Decimal integer
            {
                int value = va_arg(args, int);
                char numStr [12];    // Enough for a 32-bit integer
                snprintf(numStr, sizeof(numStr), "%d", value);
                for (char* np = numStr; *np != '\0'; np++)
                {
                    status = uShellSocketWriteBytesBlock(socket, np, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        return status;
                    }
                }
            }
            else if (*p == 'u')    // Unsigned integer
            {
                unsigned int value = va_arg(args, unsigned int);
                char uintStr [12];    // Enough for a 32-bit unsigned integer
                snprintf(uintStr, sizeof(uintStr), "%u", value);
                for (char* up = uintStr; *up != '\0'; up++)
                {
                    status = uShellSocketWriteBytesBlock(socket, up, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        return status;
                    }
                }
            }
            else if (*p == 'c')    // Character
            {
                char value = (char) va_arg(args, int);
                status = uShellSocketWriteBytesBlock(socket, &value, 1);
                if (status != USHELL_SOCKET_NO_ERR)
                {
                    return status;
                }
            }
            else if (*p == 's')    // String
            {
                const char* str = va_arg(args, const char*);
                while (*str != '\0')
                {
                    status = uShellSocketWriteBytesBlock(socket, str, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        return status;
                    }
                    str++;
                }
            }
            else if (*p == 'x' || *p == 'X')    // Hexadecimal
            {
                unsigned int value = va_arg(args, unsigned int);
                char hexStr [9];    // Enough for a 32-bit hex number
                snprintf(hexStr, sizeof(hexStr), "%X", value);
                for (char* hp = hexStr; *hp != '\0'; hp++)
                {
                    status = uShellSocketWriteBytesBlock(socket, hp, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        return status;
                    }
                }
            }
            else if (*p == 'f')    // Float
            {
                double value = va_arg(args, double);
                char floatStr [32];    // Enough for a double
                snprintf(floatStr, sizeof(floatStr), "%f", value);
                for (char* fp = floatStr; *fp != '\0'; fp++)
                {
                    status = uShellSocketWriteBytesBlock(socket, fp, 1);
                    if (status != USHELL_SOCKET_NO_ERR)
                    {
                        return status;
                    }
                }
            }
            else if (*p == '%')    // Literal '%'
            {
                status = uShellSocketWriteBytesBlock(socket, p, 1);
                if (status != USHELL_SOCKET_NO_ERR)
                {
                    return status;
                }
            }
            else
            {
                // Unsupported format specifier
                USHELL_SOCKET_ASSERT(0);
                return USHELL_SOCKET_INVALID_ARGS_ERR;
            }
        }
        else    // Regular character
        {
            status = uShellSocketWriteBytesBlock(socket, p, 1);
            if (status != USHELL_SOCKET_NO_ERR)
            {
                return status;
            }
        }
    }

    return status;
}