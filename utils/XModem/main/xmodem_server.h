#ifndef XMODEM_SERVER_H_
#define XMODEM_SERVER_H_

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
/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief XModem ADU size
 */
#define XMODEM_SERVER_MAX_ERR_COUNT 15U    ///< Maximum number of errors before aborting the transfer

/**
 * \brief XModem timeout
 */
#define XMODEM_START_TIMEOUT_MS     3000U    ///< Timeout for the start of the transfer

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes returned by the XModem module.
 */
typedef enum
{
    XMODEM_SERVER_NO_ERR = 0,          ///< Exit: no errors (success)
    XMODEM_SERVER_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    XMODEM_SERVER_RUN_TIME_ERR,        ///< Exit: error - runtime error (e.g. invalid state)
    XMODEM_SERVER_TRANSFER_ERR,        ///< Exit: error - transfer err
    XMODEM_SERVER_PORT_ERR,            ///< Exit: error - port err
    XMODEM_SERVER_TIMEOUT_ERR,         ///< Exit: error - timeout err

} XModemServerErr_e;

/**
 * \brief Enumeration of XModem constants.
 */
typedef size_t XModemServerPacketInd_t;    ///< Packet index type

/**
 * \brief Enumeration of XModem constants.
 */
typedef size_t XModemServerErrCount_t;    ///< Error count type

/**
 * \brief States for the XModem server state machine.
 * \note The XMODEM_STATE_COUNT value represents the total number of states.
 */
typedef enum
{
    XMODEM_SERVER_STATE_START,        /**< Initial state, sending NAK to the server and waiting symbol */
    XMODEM_SERVER_STATE_SOH,          /**< Waiting for the SOH character from the server. */
    XMODEM_SERVER_STATE_PACKET_GET,   /**< Receiving the packet from the server. */
    XMODEM_SERVER_STATE_PACKET_PROC,  /**< Processing the received packet. */
    XMODEM_SERVER_STATE_TRANSFER_END, /**< Transfer completed. */

} XModemServerState_e;

/**
 * \brief Port table for the xmodem server
 */
typedef struct
{
    XModemServerErr_e (*transmit)(void* server, uint8_t* data, const size_t size, size_t timeMs);    ///< Hooks to transmit a byte to the xmodem server
    XModemServerErr_e (*receive)(void* server, uint8_t* data, const size_t size, size_t timeMs);     ///< Hooks to receive a byte from the xmodem server
    XModemServerErr_e (*writeToMemory)(void* server, uint8_t* data, int size);                       ///< Hooks to write data from the xmodem server

} XModemServerPort_s;

typedef struct
{
    uint8_t data [XMODEM_ADU_CRC16_SIZE];    ///< Data buffer
    size_t size;                             ///< Size of the data buffer

} XModemServerIo_s;

/**
 * \brief Describe XModem server
 */
typedef struct
{
    /* Dependencies */
    XModemServerPort_s* port;    ///< Pointer to the port table
    const void* parent;          ///< Pointer to the parent object

    /* Internal use */
    XModem_s xmodem;                             ///< XModem object
    XModemServerState_e state;                   ///< Current state of the xmodem server
    XModemServerIo_s io;                         ///< I/O buffer for the xmodem server
    XModemServerPacketInd_t currentPacketInd;    ///< Current packet size
    XModemServerErrCount_t currentErrCount;      ///< Current error count

} XModemServer_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the XModem server module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] port - pointer to the port table for the xmodem server
 * \param [in] parent - pointer to the parent object
 * \return XModemServerErr_e - error code
 */
XModemServerErr_e XModemServerInit(XModemServer_s* const server,
                                   XModemServerPort_s* const port,
                                   const void* const parent);

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemServerErr_e - error code
 */
XModemServerErr_e XModemServerDeinit(XModemServer_s* const server);

/**
 * \brief Process the XModem server state machine.
 * \param[in] xmodem - the xmodem server object
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerProc(XModemServer_s* const server);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XMODEM_H_ */