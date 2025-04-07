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
 * @brief XModem ADU size
 */
#define XMODEM_CLIENT_MAX_ERR_COUNT 15U    ///< Maximum number of errors before aborting the transfer

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

} XModemClientErr_e;

/**
 * \brief Enumeration of XModem constants.
 */
typedef size_t XModemClientPacketInd_t;    ///< Packet index type

/**
 * \brief Enumeration of XModem constants.
 */
typedef size_t XModemClientErrCount_t;    ///< Error count type

/**
 * @brief States for the XModem server state machine.
 * @note The XMODEM_STATE_COUNT value represents the total number of states.
 */
typedef enum
{
    XMODEM_CLIENT_STATE_START,        /**< Initial state, sending NAK to the client and waiting symbol */
    XMODEM_CLIENT_STATE_PRE
    XMODEM_CLIENT_STATE_PACKET_GET,   /**< Receiving the packet from the client. */
    XMODEM_CLIENT_STATE_PACKET_PROC,  /**< Processing the received packet. */
    XMODEM_CLIENT_STATE_TRANSFER_END, /**< Transfer completed. */

} XModemClientState_e;

/**
 * @brief Port table for the xmodem server
 */
typedef struct
{
    XModemClientErr_e (*isReceivedByte)(void* xmodem, bool* isRx, void* parent);      ///< Hooks to check if a byte is available from the xmodem client
    XModemClientErr_e (*transmit)(void* xmodem, uint8_t* data, const size_t size);    ///< Hooks to transmit a byte to the xmodem client
    XModemClientErr_e (*receiveByte)(void* xmodem, uint8_t* byte, void* parent);      ///< Hooks to receive a byte from the xmodem client
    XModemClientErr_e (*delayMs)(void* xmodem, int ms);                               ///< Hooks to delay for a specified time
    XModemClientErr_e (*ReadFromMemory)(void* xmodem, uint8_t* data, int size);       ///< Hooks to write data from the xmodem client

} XModemClientPort_s;

typedef struct
{
    uint8_t data [XMODEM_ADU_SIZE];    ///< Data buffer
    size_t size;                       ///< Size of the data buffer

} XModemClientIo_s;

/**
 * \brief Describe XModem server
 */
typedef struct
{
    /* Dependencies */
    XModemClientPort_s* port;    ///< Pointer to the port table
    const void* parent;          ///< Pointer to the parent object

    /* Internal use */
    XModem_s xmodem;                             ///< XModem object
    XModemClientState_e state;                   ///< Current state of the xmodem server
    XModemClientIo_s io;                         ///< I/O buffer for the xmodem server
    XModemClientPacketInd_t currentPacketInd;    ///< Current packet size
    XModemClientErrCount_t currentErrCount;      ///< Current error count

} XModemClient_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the XModem server module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] port - pointer to the port table for the xmodem server
 * \param [in] parent - pointer to the parent object
 * \return XModemClientErr_e - error code
 */
XModemClientErr_e XModemClientInit(XModemClient_s* const xmodem,
                                   XModemClientPort_s* const port,
                                   const void* const parent);

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemClientErr_e - error code
 */
XModemClientErr_e XModemClientDeinit(XModemClient_s* const xmodem);

/**
 * @brief Process the XModem server state machine.
 * @param[in] xmodem - the xmodem server object
 * @return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
XModemClientErr_e XModemClientProc(XModemClient_s* const xmodem);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XMODEM_H_ */