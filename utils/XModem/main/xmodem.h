#ifndef XMODEM_H_
#define XMODEM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * @brief XModem ADU with CRC16 size
 */
#define XMODEM_ADU_CRC16_SIZE 133U    ///< XModem packet size + header + CRC

/**
 * @brief XModem ADU with CRC8 size
 */
#define XMODEM_ADU_CRC8_SIZE  132U    ///< XModem packet size + header + CRC

/**
 * @brief XModem PDU size
 */
#define XMODEM_PDU_SIZE       128U    ///< XModem packet size

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes returned by the XModem module.
 */
typedef enum
{
    XMODEM_NO_ERR = 0,          ///< Exit: no errors (success)
    XMODEM_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    XMODEM_ID_ERR,              ///< Exit: error - invalid ID
    XMODEM_CRC_ERR,             ///< Exit: error - invalid CRC
    XMODEM_TIMEOUT_ERR,         ///< Exit: error - timeout
    XMODEM_BUFFER_ERR,          ///< Exit: error - buffer overflow
    XMODEM_SOH_ERR,             ///< Exit: error - invalid SOH

} XModemErr_e;

/**
 * \brief Enumeration of XModem constants.
 */
typedef enum
{
    XMODEM_CONST_SOH = 0x01,      ///< Start of header
    XMODEM_CONST_STX = 0x02,      ///< Start of text
    XMODEM_CONST_EOT = 0x04,      ///< End of transmission
    XMODEM_CONST_ACK = 0x06,      ///< Acknowledge
    XMODEM_CONST_NACK = 0x15,     ///< Negative acknowledge
    XMODEM_CONST_CAN = 0x18,      ///< Cancel
    XMODEM_CONST_ETB = 0x17,      ///< End of transmission block
    XMODEM_CONST_C = 0x43,        ///< C character
    XMODEM_END_OF_FILE = 0x1A,    ///< End of file character

} XModemConst_e;

/**
 * \brief Packet structure for XModem protocol.
 */
typedef struct
{
    uint8_t data [XMODEM_PDU_SIZE];    ///< Data buffer

} XModemPdu_s;

/**
 * \brief Enumeration of XModem CRC types.
 */
typedef enum
{
    XMODEM_CRC_8,     ///< CRC-8 checksum type
    XMODEM_CRC_16,    ///< CRC-16 checksum type

} XModemCrcType_e;

/**
 * \brief Packet structure for XModem protocol.
 */
typedef struct
{
    uint8_t preamble;    ///< Preamble byte
    uint8_t id;          ///< Packet ID
    uint8_t idComp;      ///< Packet ID complement
    XModemPdu_s pdu;     ///< Packet data unit
    uint8_t crcHigh;     ///< CRC checksum high byte
    uint8_t crcLow;      ///< CRC checksum low byte

} XModemAdu_s;

/**
 * \brief Describe XModem.
 */
typedef struct
{
    /* Mandatory */
    const void* parent;         ///< Pointer to the parent object
    XModemCrcType_e crcType;    ///< CRC type
    XModemAdu_s adu;            ///< Packet structure
} XModem_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the XModem module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] parent - pointer to the parent object (optional)
 * \return XModemErr_e - error code
 */
XModemErr_e XModemInit(XModem_s* const xmodem,
                       const void* const parent,
                       const XModemCrcType_e crc);

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemErr_e - error code
 */
XModemErr_e XModemDeinit(XModem_s* const xmodem);

/**
 * \brief Set the XModem CRC type.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] crc - CRC type to be set
 * \return XModemErr_e - error code
 */
XModemErr_e XModemCrcSet(XModem_s* const xmodem,
                         const XModemCrcType_e crc);

/**
 * \brief Flush the XModem module.
 * \param [in] xmodem - XModem obj to be processed
 * \return XModemErr_e - error code
 */
XModemErr_e XModemFlush(XModem_s* const xmodem);

/**
 * \brief Encode the data using XModem protocol.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] data - pointer to the data to be encoded
 * \param [in] size - size of the data to be encoded
 * \return XModemErr_e - error code
 */
XModemErr_e XModemEncode(XModem_s* const xmodem,
                         const uint8_t* const data,
                         const size_t size);

/**
 * \brief Decode the data using XModem protocol.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] data - pointer to the data to be decoded
 * \param [in] buffSize - size of the data to be decoded
 * \return XModemErr_e - error code
 */
XModemErr_e XModemDecode(XModem_s* const xmodem,
                         uint8_t* const data,
                         const size_t buffSize);

/**
 * \brief Update the XModem packet ID.
 * \param [in] xmodem - XModem obj to be processed
 * \return XModemErr_e - error code
 */
XModemErr_e XModemAduIdUpdate(XModem_s* const xmodem);

/**
 * \brief Set the XModem packet ID.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] id - packet ID to be set
 * \return XModemErr_e - error code
 */
XModemErr_e XModemAduIdSet(XModem_s* const xmodem,
                           const uint8_t id);

/**
 * \brief Get the XModem packet ID.
 * \param [in] xmodem - XModem obj to be processed
 * \param [out] id - pointer to the packet ID to be retrieved
 * \return XModemErr_e - error code
 */
XModemErr_e XModemAduIdGet(XModem_s* const xmodem,
                           uint8_t* const id);

/**
 * \brief Set the XModem packet data unit.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] pdu - pointer to the packet data unit to be set
 * \return XModemErr_e - error code
 */
XModemErr_e XModemPduSet(XModem_s* const xmodem,
                         const XModemPdu_s* const pdu);

/**
 * \brief Get the XModem packet data unit.
 * \param [in] xmodem - XModem obj to be processed
 * \param [out] pdu - pointer to the packet data unit to be retrieved
 * \return XModemErr_e - error code
 */
XModemErr_e XModemPduGet(XModem_s* const xmodem,
                         XModemPdu_s* const pdu);

/**
 * \brief Get data from pdu
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] buff - buff to store data
 * \param [in] bufferSize - the size of buffer
 * \param [out] usedSize - the used size
 */
XModemErr_e XModemPduDataGet(XModem_s* const xmodem,
                             uint8_t* const buff,
                             const size_t bufferSize,
                             size_t* const usedSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XMODEM_H_ */