
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

} XmodemServerErr_e;

/**
 * @brief The XModem server state machine
 */
typedef enum
{
    XMODEM_SERVER_STATE_START,
    XMODEM_SERVER_STATE_SOH,
    XMODEM_SERVER_STATE_BLOCK_NUM,
    XMODEM_SERVER_STATE_BLOCK_NEG,
    XMODEM_SERVER_STATE_DATA,
    XMODEM_SERVER_STATE_CRC0,
    XMODEM_SERVER_STATE_CRC1,
    XMODEM_SERVER_STATE_PROCESS_PACKET,
    XMODEM_SERVER_STATE_SUCCESSFUL,
    XMODEM_SERVER_STATE_FAILURE,

    XMODEM_STATE_COUNT,

} XmodemServerState_e;

/**
 * @brief Callback function to transmit a byte to the xmodem client
 */
typedef void (*XmodemTxByte_f)(void* xmodem,
                               uint8_t byte,
                               void* parent);

/**
 * This contains the state for the xmodem server.
 * None of its contents should be accessed directly, this structure
 * should be considered opaque
 */
typedef struct
{
    void* parent;
    XmodemServerState_e state;                       // What state are we in?
    uint8_t packet_data [XMODEM_MAX_PACKET_SIZE];    // Incoming packet data
    int packetPos;                                   // Where are we up to in this packet
    uint16_t crc;                                    // Whatis the expected CRC of the incoming packet
    uint16_t packetSize;                             // Are we receiving 128B or 1K packets?
    bool repeating;                                  // Are we receiving a packet that we've already processed?
    int64_t lastEventTime;                           // When did we last do something interesting?
    uint32_t blockNum;                               // What block are we up to?
    uint32_t errorCount;                             // How many errors have we seen?
    XmodemTxByte_f txByte;

} XmodemServer_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Initialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @param[in] txByte - callback function to transmit a byte to the xmodem client
 * @param[in] parent - pointer to the parent object (optional)
 * @return int - error code. non-zero = an error has occurred;
 */
int XmodemServerInit(XmodemServer_s* xdm,
                     XmodemTxByte_f txByte,
                     void* parent);

/**
 * @brief Deinitialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @return int - error code. non-zero = an error has occurred;
 */
int XmodemServerDeinit(XmodemServer_s* xdm);

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xdm - the xmodem server object
 * @param[in] byte - the byte to receive
 * @return true - error code. non-zero = an error has occurred;
 * @return false - error code. non-zero = an error has occurred;
 */
bool XmodemServerRxByte(XmodemServer_s* xdm, uint8_t byte);

/**
 * @brief Get the current state of the xmodem server
 * @param[in] xdm - the xmodem server object
 * @return XmodemServerState_e - error code. non-zero = an error has occurred;
 */
XmodemServerState_e XmodemServerGetState(XmodemServer_s* xdm);

/**
 * @brief Process the xmodem server
 * @param[in] xdm - the xmodem server object
 * @param[in] packet - the packet to process
 * @param[in] blockNum - the block number to process
 * @param[in] ms_time - the time in milliseconds since the last event
 * @return int - error code. non-zero = an error has occurred;
 */
int XmodemServerProc(XmodemServer_s* xdm,
                     uint8_t* packet,
                     uint32_t* blockNum,
                     int64_t ms_time);

/**
 * @brief Check if the xmodem server is done
 * @param[in] xdm - the xmodem server object
 * @return true - done.
 * @return false - not done.
 */
bool XmodemServerIsDone(XmodemServer_s* xdm);

#ifdef __cplusplus
}
#endif

#endif