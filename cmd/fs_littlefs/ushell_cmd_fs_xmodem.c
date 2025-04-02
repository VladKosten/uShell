/**
 * \file         ushell_cmd_fs_xmodem.c
 * \brief        The file contains the implementation of the UShell command for xmodem file transfer.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include <string.h>
#include "ushell_cmd_fs_xmodem.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShellHal module.
 */
#ifndef USHELL_CMD_FS_XMODEM_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_CMD_FS_XMODEM_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_CMD_FS_XMODEM_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * @brief Calculate the CRC of a byte
 * @param[in] crc - the current CRC value
 * @param[in] byte - the byte to calculate the CRC for
 * @return uint16_t - error code. non-zero = an error has occurred;
 */
uint16_t xmodemServerCrc(uint16_t crc, uint8_t byte);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Initialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @param[in] txByte - callback function to transmit a byte to the xmodem client
 * @param[in] parent - pointer to the parent object (optional)
 * @return int - error code. non-zero = an error has occurred;
 */
int XmodemServerInit(XmodemServer_s* xdm,
                     XmodemTxByte_f txByte,
                     void* parent)
{
    if (!txByte)
        return -1;
    memset(xdm, 0, sizeof(*xdm));
    xdm->txByte = txByte;
    xdm->parent = parent;

    xdm->txByte(xdm, 'C', xdm->parent);

    return 0;
}

/**
 * @brief Deinitialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @return int - error code. non-zero = an error has occurred;
 */
int XmodemServerDeinit(XmodemServer_s* xdm)
{
    if (!xdm)
        return -1;
    memset(xdm, 0, sizeof(*xdm));
    return 0;
}

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xdm - the xmodem server object
 * @param[in] byte - the byte to receive
 * @return true - error code. non-zero = an error has occurred;
 * @return false - error code. non-zero = an error has occurred;
 */
bool XmodemServerRxByte(XmodemServer_s* xdm, uint8_t byte)
{
    switch (xdm->state)
    {
        case XMODEM_SERVER_STATE_START :
        case XMODEM_SERVER_STATE_SOH :
            if (byte == XMODEM_SOH)
            {
                xdm->state = XMODEM_SERVER_STATE_BLOCK_NUM;
                xdm->packetSize = XMODEM_MAX_PACKET_SIZE;
            }
            else if (byte == XMODEM_EOT)
            {
                xdm->state = XMODEM_SERVER_STATE_SUCCESSFUL;
                xdm->txByte(xdm, XMODEM_ACK, xdm->parent);
            }
            break;
        case XMODEM_SERVER_STATE_BLOCK_NUM :
            if (byte == ((xdm->blockNum + 1) & 0xff))
            {
                xdm->state = XMODEM_SERVER_STATE_BLOCK_NEG;
                xdm->repeating = false;
            }
            else if (byte == (xdm->blockNum & 0xff))
            {
                xdm->state = XMODEM_SERVER_STATE_BLOCK_NEG;
                xdm->repeating = true;
            }
            else if (byte == XMODEM_SOH || byte == XMODEM_STX)
            {
                xdm->state = XMODEM_SERVER_STATE_BLOCK_NUM;
            }
            else
            {
                xdm->state = XMODEM_SERVER_STATE_SOH;
            }
            break;

        case XMODEM_SERVER_STATE_BLOCK_NEG :
        {
            uint8_t neg_block = ~(xdm->blockNum + 1) & 0xff;
            if (xdm->repeating)
                neg_block = (~xdm->blockNum) & 0xff;
            if (byte == neg_block)
            {
                xdm->packetPos = 0;
                xdm->state = XMODEM_SERVER_STATE_DATA;
            }
            else if (byte == XMODEM_SOH || byte == XMODEM_STX)
            {
                xdm->state = XMODEM_SERVER_STATE_BLOCK_NUM;
            }
            else
            {
                xdm->state = XMODEM_SERVER_STATE_SOH;
            }
            break;
        }
        case XMODEM_SERVER_STATE_DATA :
            xdm->packet_data [xdm->packetPos++] = byte;
            if (xdm->packetPos >= xdm->packetSize)
                xdm->state = XMODEM_SERVER_STATE_CRC0;
            break;

        case XMODEM_SERVER_STATE_CRC0 :
            xdm->crc = ((uint16_t) byte) << 8;
            xdm->state = XMODEM_SERVER_STATE_CRC1;
            break;

        case XMODEM_SERVER_STATE_CRC1 :
        {
            uint16_t crc = 0;
            xdm->crc |= byte;
            for (int i = 0; i < xdm->packetSize; i++)
                crc = XmodemServerCrc(crc, xdm->packet_data [i]);
            if (crc != xdm->crc)
            {
                xdm->errorCount++;
                xdm->state = XMODEM_SERVER_STATE_SOH;
                xdm->txByte(xdm, XMODEM_NACK, xdm->parent);
            }
            else if (xdm->repeating)
            {
                // xdm->txByte(xdm, XMODEM_ACK, xdm->parent);
                xdm->state = XMODEM_SERVER_STATE_SOH;
            }
            else
            {
                xdm->state = XMODEM_SERVER_STATE_PROCESS_PACKET;
            }
            break;
        }

        default :
            break;
    }

    return (xdm->state == XMODEM_SERVER_STATE_PROCESS_PACKET);
}

/**
 * @brief Get the current state of the xmodem server
 * @param[in] xdm - the xmodem server object
 * @return XmodemServerState_e - error code. non-zero = an error has occurred;
 */
XmodemServerState_e XmodemServerGetState(XmodemServer_s* xdm)
{
    return xdm->state;
}

/**
 * @brief Check if the xmodem server is done
 * @param[in] xdm - the xmodem server object
 * @return true - done.
 * @return false - not done.
 */
bool XmodemServerIsDone(XmodemServer_s* xdm)
{
    return xdm->state == XMODEM_SERVER_STATE_SUCCESSFUL || xdm->state == XMODEM_SERVER_STATE_FAILURE;
}

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
                     int64_t ms_time)
{
    if (XmodemServerIsDone(xdm))
        return 0;
    // Avoid confusion with 0 default value
    if (ms_time == 0)
        ms_time = 1;
    // Initialise our timer
    if (xdm->lastEventTime == 0)
        xdm->lastEventTime = ms_time;
    if (xdm->state == XMODEM_SERVER_STATE_START && ms_time - xdm->lastEventTime > 500)
    {
        xdm->txByte(xdm, 'C', xdm->parent);
        xdm->lastEventTime = ms_time;
    }
    if (ms_time - xdm->lastEventTime > XMODEM_PACKET_TIMEOUT)
    {
        xdm->errorCount++;
        xdm->state = XMODEM_SERVER_STATE_SOH;
        xdm->txByte(xdm, XMODEM_NACK, xdm->parent);
        xdm->lastEventTime = ms_time;
    }
    if (xdm->errorCount >= XMODEM_MAX_ERRORS)
    {
        xdm->state = XMODEM_SERVER_STATE_FAILURE;
        xdm->txByte(xdm, XMODEM_CAN, xdm->parent);
        xdm->lastEventTime = ms_time;
    }
    if (xdm->state != XMODEM_SERVER_STATE_PROCESS_PACKET)
        return 0;
    xdm->lastEventTime = ms_time;
    memcpy(packet, xdm->packet_data, xdm->packetSize);
    *blockNum = xdm->blockNum;
    xdm->blockNum++;
    xdm->state = XMODEM_SERVER_STATE_SOH;
    xdm->txByte(xdm, XMODEM_ACK, xdm->parent);
    return xdm->packetSize;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * @brief Calculate the CRC of a byte
 * @param[in] crc - the current CRC value
 * @param[in] byte - the byte to calculate the CRC for
 * @return uint16_t - error code. non-zero = an error has occurred;
 */
uint16_t xmodemServerCrc(uint16_t crc, uint8_t byte)
{
    crc = crc ^ ((uint16_t) byte) << 8;
    for (int i = 8; i > 0; i--)
    {
        if (crc & 0x8000)
            crc = crc << 1 ^ 0x1021;
        else
            crc = crc << 1;
    }
    return crc;
}