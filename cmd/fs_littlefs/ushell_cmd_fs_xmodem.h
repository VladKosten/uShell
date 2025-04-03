
#ifndef XMODEM_SERVER_H
#define XMODEM_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include <stdint.h>
#include <stdbool.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief XModem packet size
 */
#ifndef XMODEM_MAX_PACKET_SIZE
    #define XMODEM_MAX_PACKET_SIZE 128    ///< Maximum packet size
#endif

/**
 * @brief XModem protocol constants
 */
#define XMODEM_SOH            0x01    ///< Start of header
#define XMODEM_STX            0x02    ///< Start of text
#define XMODEM_EOT            0x04    ///< End of transmission
#define XMODEM_ACK            0x06    ///< Acknowledge
#define XMODEM_NACK           0x15    ///< Negative acknowledge
#define XMODEM_CAN            0x18    ///< Cancel

/**
 * @brief XModem packet timeout
 */
#define XMODEM_PACKET_TIMEOUT 1000

/**
 * @brief XModem maximum number of errors
 */
#define XMODEM_MAX_ERRORS     10

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief XModem possible error codes
 */
typedef enum
{
    XMODEM_SERVER_SUC = 0,                ///< Success
    XMODEM_SERVER_ARG_ERR = -1,           ///< Argument error
    XMODEM_SERVER_TIMEOUT_ERR = -2,       ///< Timeout error
    XMODEM_SERVER_CRC_ERR = -3,           ///< CRC error
    XMODEM_SERVER_EOT_ERR = -4,           ///< EOT error
    XMODEM_SERVER_BLOCK_ERR = -5,         ///< Block error
    XMODEM_SERVER_UNEXPECTED_ERR = -6,    ///< Unexpected error

} XModemServerErr_e;

/**
 * @brief States for the XModem server state machine.
 * @note The XMODEM_STATE_COUNT value represents the total number of states.
 */
typedef enum
{
    XMODEM_SERVER_STATE_START,          /**< Initial state, waiting to begin packet reception. */
    XMODEM_SERVER_STATE_SOH,            /**< Detected Start Of Header (SOH) byte, beginning of packet. */
    XMODEM_SERVER_STATE_BLOCK_NUM,      /**< Receiving the block number of the current packet. */
    XMODEM_SERVER_STATE_BLOCK_NEG,      /**< Receiving the complement (negative) of the block number. */
    XMODEM_SERVER_STATE_DATA,           /**< Receiving the packet data bytes. */
    XMODEM_SERVER_STATE_CRC0,           /**< Receiving the first byte of the CRC checksum. */
    XMODEM_SERVER_STATE_CRC1,           /**< Receiving the second byte of the CRC checksum. */
    XMODEM_SERVER_STATE_PROCESS_PACKET, /**< Processing the complete packet after reception. */
    XMODEM_SERVER_STATE_PACKET_SUC,     /**< Packet processed successfully. */
    XMODEM_SERVER_STATE_SUCCESS,        /**< Successful packet reception and processing. */
    XMODEM_SERVER_STATE_FAILURE,        /**< An error occurred during packet processing. */

    XMODEM_STATE_COUNT, /**< Total number of states. */

} XModemServerState_e;

/**
 * @brief Port table for the xmodem server
 */
typedef struct
{
    XModemServerErr_e (*isRxByte)(void* xmodem, bool* isRx, void* parent);     ///< Callback function to check if a byte is available from the xmodem client
    XModemServerErr_e (*txByte)(void* xmodem, uint8_t byte, void* parent);     ///< Callback function to transmit a byte to the xmodem client
    XModemServerErr_e (*rxByte)(void* xmodem, uint8_t* byte, void* parent);    ///< Callback function to receive a byte from the xmodem client
    XModemServerErr_e (*delayMs)(void* xmodem, int ms);                        ///< Callback function to delay for a specified time
    XModemServerErr_e (*write)(void* xmodem, uint8_t* data, int size);         ///< Callback function to write data from the xmodem client

} XModemServerPort_s;

/**
 * * @brief XModem server object
 */
typedef struct
{
    void* parent;                                    ///< Pointer to the parent object (optional)
    XModemServerState_e state;                       ///< Current state of the xmodem server
    uint8_t packet_data [XMODEM_MAX_PACKET_SIZE];    ///< Buffer to store the packet data
    int packetPos;                                   ///< Current position in the packet data buffer
    uint16_t crc;                                    ///< CRC value for the packet
    uint16_t packetSize;                             ///< Size of the packet data
    bool repeating;                                  ///< Flag to indicate if the packet is a duplicate
    int64_t lastEventTime;                           ///< Timestamp of the last event
    uint32_t blockNum;                               ///< Current block number
    uint32_t errorCount;                             ///< Number of errors encountered
    XModemServerPort_s* port;                        ///< Pointer to the port table for the xmodem server

} XModemServer_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Initialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @param[in] port - the port table for the xmodem server
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerInit(XModemServer_s* xdm,
                                   XModemServerPort_s* port,
                                   void* parent);

/**
 * @brief Deinitialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerDeinit(XModemServer_s* xdm);

/**
 * @brief Process the xmodem server
 * @param[in] xdm - the xmodem server object
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerProc(XModemServer_s* xdm);

/**
 * @brief Reset the xmodem server
 * @param xdm - the xmodem server object
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerReset(XModemServer_s* xdm);

#ifdef __cplusplus
}
#endif

#endif