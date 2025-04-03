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
static uint16_t xModemServerCrc(uint16_t crc, uint8_t byte);

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xmodem - the xmodem server object
 * @param[in] byte - the buffer to receive the byte
 * @param[in] parent - pointer to the parent object (optional)
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerIsRxByte(XModemServer_s* xmodem,
                                              bool* isRxByte,
                                              void* parent);

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xdm - the xmodem server object
 * @param[in] byte - the byte to receive
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerRxByte(XModemServer_s* const xdm,
                                            uint8_t* const byte);

/**
 * @brief Transmit a byte to the xmodem client
 * @param[in] xdm - the xmodem server object
 * @param[in] byte - the byte to transmit
 * @return int - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e xModemServerTxByte(XModemServer_s* xdm,
                                            uint8_t byte);

/**
 * @brief Delay for a specified time
 * @param xdm - the xmodem server object
 * @param ms - the time in milliseconds to delay
 */
static XModemServerErr_e xModemServerDelayS(XModemServer_s* xdm,
                                            int ms);

/**
 * @brief Write data from the xmodem client
 * @param xdm - the xmodem server object
 * @param data - the data to write
 * @param size - the size of the data to write
 */
static XModemServerErr_e xModemServerWrite(XModemServer_s* xdm,
                                           uint8_t* data,
                                           int size);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Initialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @param[in] port - the port table for the xmodem server
 * @param[in] parent - pointer to the parent object (optional)
 * @return int - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerInit(XModemServer_s* xdm,
                                   XModemServerPort_s* port,
                                   void* parent)
{
    /* Local var */
    XModemServerErr_e status = 0;    // Variable to store the status of the operation

    /* Initialize the xmodem server object */
    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (port == NULL) ||
            (parent == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Flush the xmodem server object */
        memset(xdm, 0, sizeof(*xdm));

        /* Initialize the xmodem server object */
        xdm->port = port;
        xdm->parent = parent;

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Deinitialize the Xmodem server
 * @param[in] xdm - the xmodem server object
 * @return int - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerDeinit(XModemServer_s* xdm)
{
    /* Local var */
    XModemServerErr_e status = 0;    // Variable to store the status of the operation

    /* Deinitialize the xmodem server object */
    do
    {
        /* Check input parameter */
        if (xdm == NULL)
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Flush the xmodem server object */
        memset(xdm, 0, sizeof(*xdm));

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Reset the xmodem server
 * @param xdm - the xmodem server object
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerReset(XModemServer_s* xdm)
{
    /* Local var */
    XModemServerErr_e status = 0;    // Variable to store the status of the operation

    /* Reset the xmodem server object */
    do
    {
        /* Check input parameter */
        if (xdm == NULL)
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Reset the xmodem server object */
        xdm->errorCount = 0;                       // Reset the error count
        xdm->blockNum = 0;                         // Reset the block number
        xdm->packetPos = 0;                        // Reset the packet position
        xdm->crc = 0;                              // Reset the CRC value
        xdm->packetSize = 0;                       // Reset the packet size
        xdm->repeating = false;                    // Reset the repeating flag
        xdm->state = XMODEM_SERVER_STATE_START;    // Set the initial state to START
        xdm->lastEventTime = 0;                    // Reset the last event time

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Process the xmodem server
 * @param[in] xdm - the xmodem server object
 * @param[in] packet - the packet to process
 * @param[in] blockNum - the block number to process
 * @param[in] ms_time - the time in milliseconds since the last event
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
/**
 * @brief Process the XModem server state machine.
 *
 * This function reads bytes from the client, advances the state machine,
 * and when a complete packet is received and verified, sends an ACK and
 * returns the packet size. In case of error or incomplete packet reception,
 * an appropriate error code is returned.
 *
 * @param[in,out] xdm Pointer to the XModem server object.
 * @return int If a full packet has been processed, returns its size;
 *             otherwise, returns 0 on non-fatal conditions or an error code.
 */
XModemServerErr_e XModemServerProc(XModemServer_s* xdm)
{
    /* Local variables */
    XModemServerErr_e status = XMODEM_SERVER_SUC;
    uint8_t byte = 0;
    bool isRxByte = false;

    /* Process incoming bytes and update the state machine */
    while (1)
    {
        /* Check if a byte is available from the client */
        switch (xdm->state)
        {
            /* Start state: send initial character and wait for activity */
            case XMODEM_SERVER_STATE_START :
            {
                status = xModemServerTxByte(xdm, 'C');
                if (status != XMODEM_SERVER_SUC)
                    return status;

                status = xModemServerIsRxByte(xdm, &isRxByte, xdm->parent);
                if (status != XMODEM_SERVER_SUC)
                    return status;

                if (!isRxByte)
                {
                    status = xModemServerDelayS(xdm, 1000);    // Delay for 1 second
                    break;
                }

                xdm->state = XMODEM_SERVER_STATE_SOH;
                break;
            }

            /* Expecting SOH (start-of-header) or EOT */
            case XMODEM_SERVER_STATE_SOH :
            {
                status = xModemServerRxByte(xdm, &byte);
                if (status != XMODEM_SERVER_SUC)
                    return status;

                if (byte == XMODEM_SOH)
                {
                    xdm->state = XMODEM_SERVER_STATE_BLOCK_NUM;
                    xdm->packetSize = XMODEM_MAX_PACKET_SIZE;
                }
                else if (byte == XMODEM_EOT)
                {
                    xdm->state = XMODEM_SERVER_STATE_SUCCESS;
                    xModemServerTxByte(xdm, XMODEM_ACK);
                    return XMODEM_SERVER_SUC;
                }
                else
                {
                    /* Invalid byte received; reset state and increment error count */
                    xdm->state = XMODEM_SERVER_STATE_SOH;
                    xdm->errorCount++;
                    if (xdm->errorCount >= XMODEM_MAX_ERRORS)
                        return XMODEM_SERVER_UNEXPECTED_ERR;
                }
                break;
            }

            /* Receive the block number */
            case XMODEM_SERVER_STATE_BLOCK_NUM :
            {
                status = xModemServerRxByte(xdm, &byte);
                if (status != XMODEM_SERVER_SUC)
                    return status;

                if (byte == XMODEM_CAN)
                {
                    xdm->state = XMODEM_SERVER_STATE_FAILURE;
                    return XMODEM_SERVER_UNEXPECTED_ERR;
                }
                if (byte == ((xdm->blockNum + 1) & 0xFF))
                {
                    xdm->state = XMODEM_SERVER_STATE_BLOCK_NEG;
                    xdm->repeating = false;
                }
                else if (byte == (xdm->blockNum & 0xFF))
                {
                    xdm->state = XMODEM_SERVER_STATE_BLOCK_NEG;
                    xdm->repeating = true;
                }
                else if (byte == XMODEM_SOH || byte == XMODEM_STX)
                {
                    /* Restart reception of a new block */
                    xdm->state = XMODEM_SERVER_STATE_BLOCK_NUM;
                }
                else
                {
                    xdm->state = XMODEM_SERVER_STATE_SOH;
                }
                break;
            }

            /* Receive the block number negative */
            case XMODEM_SERVER_STATE_BLOCK_NEG :
            {
                status = xModemServerRxByte(xdm, &byte);
                if (status != XMODEM_SERVER_SUC)
                    return status;

                uint8_t expectedNeg = ~(xdm->blockNum + 1) & 0xFF;
                if (xdm->repeating)
                    expectedNeg = (~xdm->blockNum) & 0xFF;
                if (byte == expectedNeg)
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

            /* Receive data bytes */
            case XMODEM_SERVER_STATE_DATA :
            {
                status = xModemServerRxByte(xdm, &byte);
                if (status != XMODEM_SERVER_SUC)
                    return status;
                xdm->packet_data [xdm->packetPos++] = byte;
                if (xdm->packetPos >= xdm->packetSize)
                    xdm->state = XMODEM_SERVER_STATE_CRC0;
                break;
            }

            /* Receive first CRC byte */
            case XMODEM_SERVER_STATE_CRC0 :
            {
                status = xModemServerRxByte(xdm, &byte);
                if (status != XMODEM_SERVER_SUC)
                    return status;
                xdm->crc = ((uint16_t) byte) << 8;
                xdm->state = XMODEM_SERVER_STATE_CRC1;
                break;
            }

            /* Receive second CRC byte and verify */
            case XMODEM_SERVER_STATE_CRC1 :
            {
                status = xModemServerRxByte(xdm, &byte);
                if (status != XMODEM_SERVER_SUC)
                    return status;
                xdm->crc |= byte;
                {
                    uint16_t computedCrc = 0;
                    for (int i = 0; i < xdm->packetSize; i++)
                        computedCrc = xModemServerCrc(computedCrc, xdm->packet_data [i]);
                    if (computedCrc != xdm->crc)
                    {
                        xdm->errorCount++;
                        xdm->state = XMODEM_SERVER_STATE_SOH;
                        xModemServerTxByte(xdm, XMODEM_NACK);
                        continue;
                    }
                    else if (xdm->repeating)
                    {
                        /* Duplicate packet received; acknowledge and reset state */
                        xdm->state = XMODEM_SERVER_STATE_SOH;
                        continue;
                    }
                    else
                    {
                        xdm->state = XMODEM_SERVER_STATE_PROCESS_PACKET;
                    }
                }
                break;
            }

            /* In failure or success state, exit processing */
            case XMODEM_SERVER_STATE_FAILURE :
                return XMODEM_SERVER_UNEXPECTED_ERR;
            case XMODEM_SERVER_STATE_SUCCESS :
                return XMODEM_SERVER_SUC;

            /* Process the complete packet */
            case XMODEM_SERVER_STATE_PROCESS_PACKET :
            {
                /* Write the received data to the file */
                status = xModemServerWrite(xdm, xdm->packet_data, xdm->packetSize);
                if (status != XMODEM_SERVER_SUC)
                {
                    xdm->state = XMODEM_SERVER_STATE_FAILURE;
                    return status;
                }
                xModemServerTxByte(xdm, XMODEM_ACK);
                xdm->blockNum++;
                xdm->state = XMODEM_SERVER_STATE_SOH;
                xdm->packetPos = 0;
                xdm->crc = 0;
                xdm->packetSize = 0;
                xdm->repeating = false;
                xdm->lastEventTime = 0;
                xdm->errorCount = 0;    // Reset the error count after successful packet processing
            }

            default :
                break;
        }
    }
    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * @brief Calculate the CRC of a byte
 * @param[in] crc - the current CRC value
 * @param[in] byte - the byte to calculate the CRC for
 * @return uint16_t - error code. non-zero = an error has occurred;
 */
uint16_t xModemServerCrc(uint16_t crc, uint8_t byte)
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

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xmodem - the xmodem server object
 * @param[in] byte - the buffer to receive the byte
 * @param[in] parent - pointer to the parent object (optional)
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerIsRxByte(XModemServer_s* xmodem,
                                              bool* isRxByte,
                                              void* parent)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_SUC;    // Variable to store the status of the operation

    /* Check if the byte is valid */
    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (isRxByte == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Check if the byte is valid */
        if ((xmodem->port == NULL) ||
            (xmodem->port->isRxByte == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Receive the byte from the xmodem client */
        status = xmodem->port->isRxByte(xmodem, isRxByte, parent);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xdm - the xmodem server object
 * @param[in] byte - the buffer to receive the byte
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerRxByte(XModemServer_s* const xdm,
                                            uint8_t* const byte)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_SUC;    // Variable to store the status of the operation

    /* Receive byte */
    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (byte == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Check if the byte is valid */
        if ((xdm->port == NULL) ||
            (xdm->port->rxByte == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Receive the byte from the xmodem client */
        status = xdm->port->rxByte(xdm, byte, xdm->parent);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Transmit a byte to the xmodem client
 * @param[in] xdm - the xmodem server object
 * @param[in] byte - the byte to transmit
 * @return int - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e xModemServerTxByte(XModemServer_s* xdm, uint8_t byte)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_SUC;    // Variable to store the status of the operation

    /* Send byte */
    do
    {
        /* Check input parameter */
        if (xdm == NULL)
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Check if the byte is valid */
        if ((xdm->port == NULL) ||
            (xdm->port->txByte == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Transmit the byte to the xmodem client */
        status = xdm->port->txByte(xdm, byte, xdm->parent);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Delay for a specified time
 * @param xdm - the xmodem server object
 * @param ms - the time in milliseconds to delay
 */
static XModemServerErr_e xModemServerDelayS(XModemServer_s* xdm, int ms)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_SUC;    // Variable to store the status of the operation

    /* Delay */
    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (ms <= 0))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Check if the delay function is valid */
        if ((xdm->port == NULL) ||
            (xdm->port->delayMs == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Delay for the specified time */
        status = xdm->port->delayMs(xdm, ms);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Write data from the xmodem client
 * @param xdm - the xmodem server object
 * @param data - the data to write
 * @param size - the size of the data to write
 */
static XModemServerErr_e xModemServerWrite(XModemServer_s* xdm,
                                           uint8_t* data,
                                           int size)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_SUC;    // Variable to store the status of the operation

    /* Send data */
    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (data == NULL) ||
            (size <= 0))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Check if the write function is valid */
        if ((xdm->port == NULL) ||
            (xdm->port->write == NULL))
        {
            status = XMODEM_SERVER_ARG_ERR;    // Set status to error if command is NULL
            break;                             // Exit the loop
        }

        /* Write data from the xmodem client */
        status = xdm->port->write(xdm, data, size);

    } while (0);

    return status;    // Return success code
}