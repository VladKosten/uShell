#ifndef XMODEM_CLIENT_H_
#define XMODEM_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "xmodem.h"
#include "ushell_cfg.h"
/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief XModem client max err
 */
#ifndef XMODEM_CLIENT_MAX_ERR_COUNT
    #define XMODEM_CLIENT_MAX_ERR_COUNT 5U    ///< Maximum number of errors before aborting the transfer
#endif

/**
 * \brief XModem start timeout
 */
#ifndef XMODEM_CLIENT_START_TIMEOUT_MS
    #define XMODEM_CLIENT_START_TIMEOUT_MS 3000U    ///< Timeout for the start of the transfer
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes returned by the XModem module.
 */
typedef enum
{
    XMODEM_CLIENT_NO_ERR = 0,          ///< Exit: no errors (success)
    XMODEM_CLIENT_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    XMODEM_CLIENT_RUN_TIME_ERR,        ///< Exit: error - runtime error (e.g. invalid state)
    XMODEM_CLIENT_TRANSFER_ERR,        ///< Exit: error - transfer err
    XMODEM_CLIENT_PORT_ERR,            ///< Exit: error - port err
    XMODEM_CLIENT_TIMEOUT_ERR,         ///< Exit: error - timeout err

} XModemClientErr_e;

/**
 * \brief Define the packet index type
 */
typedef size_t XModemClientPacketInd_t;    ///< Packet index type

/**
 * \brief Define the error count type
 */
typedef size_t XModemClientErrCount_t;    ///< Error count type

/**
 * \brief Define the size
 */
typedef size_t XModemClientSize_t;    ///< Size type

/**
 * \brief States for the XModem client state machine.
 * \note The XMODEM_STATE_COUNT value represents the total number of states.
 */
typedef enum
{
    XMODEM_CLIENT_STATE_START,        /**< Initial state, sending NAK to the client and waiting symbol */
    XMODEM_CLIENT_STATE_PACKET_SEND,  /**< Receiving the packet from the client. */
    XMODEM_CLIENT_STATE_WAIT_RESP,    /**< Processing the received packet. */
    XMODEM_CLIENT_STATE_TRANSFER_END, /**< Transfer completed. */

} XModemClientState_e;

/**
 * \brief Port table for the xmodem client
 */
typedef struct
{
    XModemClientErr_e (*transmit)(void* client, uint8_t* data, const size_t size, size_t timeMs);
    XModemClientErr_e (*receive)(void* client, uint8_t* const data, const size_t size, size_t timeMs);
    XModemClientErr_e (*readFromMemory)(void* client,
                                        uint8_t* data,
                                        const size_t size,
                                        size_t* const usedSize,
                                        const size_t offset);    ///< Hooks to write data from the xmodem client

} XModemClientPort_s;

typedef struct
{
    uint8_t data [XMODEM_ADU_CRC16_SIZE];    ///< Data buffer
    size_t size;                             ///< Size of the data buffer

} XModemClientIo_s;

/**
 * \brief Describe XModem client
 */
typedef struct
{
    /* Dependencies */
    XModemClientPort_s* port;    ///< Pointer to the port table
    const void* parent;          ///< Pointer to the parent object

    /* Internal use */
    XModem_s xmodem;                             ///< XModem object
    XModemClientState_e state;                   ///< Current state of the xmodem client
    XModemClientIo_s io;                         ///< I/O buffer for the xmodem client
    XModemClientPacketInd_t currentPacketInd;    ///< Current packet size
    XModemClientErrCount_t currentErrCount;      ///< Current error count
    XModemClientSize_t currentDataSize;          ///< Current data size

} XModemClient_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the XModem client module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] port - pointer to the port table for the xmodem client
 * \param [in] parent - pointer to the parent object
 * \return XModemClientErr_e - error code
 */
XModemClientErr_e XModemClientInit(XModemClient_s* const client,
                                   XModemClientPort_s* const port,
                                   const void* const parent);

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemClientErr_e - error code
 */
XModemClientErr_e XModemClientDeinit(XModemClient_s* const client);

/**
 * \brief Process the XModem client state machine.
 * \param[in] xmodem - the xmodem client object
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
XModemClientErr_e XModemClientProc(XModemClient_s* const client);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XMODEM_H_ */