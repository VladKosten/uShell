/**
 * \file         xmodem_client.c
 * \brief        The file contains the implementation of the Xmodem client.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "xmodem_client.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the xmodem client module.
 */
#ifndef XMODEM_CLIENT_ASSERT
    #ifdef USHELL_ASSERT
        #define XMODEM_CLIENT_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define XMODEM_CLIENT_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xmodem - the xmodem client object
 * @param[in] byte - the buffer to receive the byte
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemClientErr_e xModemClientIsRxByte(XModemClient_s* xmodem,
                                              bool* isRxByte);

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] client - the xmodem client object
 * @param[in] byte - the byte to receive
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemClientErr_e xModemClientRxByte(XModemClient_s* const client,
                                            uint8_t* const byte);

/**
 * @brief Transmit a byte to the xmodem client
 * @param[in] client - the xmodem client object
 * @param[in] byte - the byte to transmit
 * @return int - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e xModemClientTransmit(XModemClient_s* client,
                                              uint8_t* data,
                                              const size_t size);

/**
 * @brief Delay for a specified time
 * @param client - the xmodem client object
 * @param ms - the time in milliseconds to delay
 */
static XModemClientErr_e xModemClientDelayS(XModemClient_s* client,
                                            int ms);

/**
 * @brief Read data from the xmodem client
 * @param client - the xmodem client object
 * @param data - the buffer to read the data
 * @param size - the size of the data to read
 * @param usedSize - the size of the data read
 */
static XModemClientErr_e xModemClientReadFromMemory(XModemClient_s* client,
                                                    uint8_t* data,
                                                    const size_t size,
                                                    size_t* const usedSize,
                                                    const size_t offset);

/**
 * @brief Process the XModem client state machine.
 * @param client - the xmodem client object
 * @return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e xModemClientFSMProc(XModemClient_s* client);

/**
 * @brief Flush the io buffer
 * @param client - the xmodem client obj;
 */
static void xModemClientIoFlush(XModemClient_s* client);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the XModem client module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] port - pointer to the port table for the xmodem client
 * \param [in] parent - pointer to the parent object
 * \return XModemClientErr_e - error code
 */
XModemClientErr_e XModemClientInit(XModemClient_s* const client,
                                   XModemClientPort_s* const port,
                                   const void* const parent)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation
    XModemErr_e xmodemStatus = XMODEM_NO_ERR;           // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((client == NULL) ||
            (port == NULL) ||
            (parent == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the port table is valid */
        if ((port->isReceivedByte == NULL) ||
            (port->transmit == NULL) ||
            (port->receiveByte == NULL) ||
            (port->delayMs == NULL) ||
            (port->readFromMemory == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Initialize the xmodem client object */
        memset(client, 0, sizeof(XModemClient_s));    // Flush the xmodem client object

        /* Initialize the xmodem object */
        xmodemStatus = XModemInit(&client->xmodem, client, XMODEM_CRC_16);
        if (xmodemStatus != XMODEM_NO_ERR)    // Check if the xmodem object is in error state
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Set the port table and parent object */
        client->port = port;        // Set the port table
        client->parent = parent;    // Set the parent object

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemClientErr_e - error code
 */
XModemClientErr_e XModemClientDeinit(XModemClient_s* const client)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    /* Deinitialize the xmodem client object */
    do
    {
        /* Check input parameter */
        if (client == NULL)
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Flush the xmodem client object */
        memset(client, 0, sizeof(XModemClient_s));    // Flush the xmodem client object

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Process the XModem client state machine.
 * @param[in] xmodem - the xmodem client object
 * @return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
XModemClientErr_e XModemClientProc(XModemClient_s* const client)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (client == NULL)
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        status = xModemClientFSMProc(client);    // Process the state machine
        if (status != XMODEM_CLIENT_NO_ERR)      // Check if the state machine is in error state
        {
            break;    // Exit the loop
        }

    } while (0);

    /* Process incoming bytes and update the state machine */

    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] xmodem - the xmodem client object
 * @param[in] byte - the buffer to receive the byte
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemClientErr_e xModemClientIsRxByte(XModemClient_s* xmodem,
                                              bool* isRxByte)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    /* Check if the byte is valid */
    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (isRxByte == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the byte is valid */
        if ((xmodem->port == NULL) ||
            (xmodem->port->isReceivedByte == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Receive the byte from the xmodem client */
        status = xmodem->port->isReceivedByte(xmodem, isRxByte);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Receive a byte from the xmodem client
 * @param[in] client - the xmodem client object
 * @param[in] byte - the buffer to receive the byte
 * @return true -  byte received successfully.
 * @return false - byte not received successfully.
 */
static XModemClientErr_e xModemClientRxByte(XModemClient_s* const client,
                                            uint8_t* const byte)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    /* Receive byte */
    do
    {
        /* Check input parameter */
        if ((client == NULL) ||
            (byte == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the byte is valid */
        if ((client->port == NULL) ||
            (client->port->receiveByte == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Receive the byte from the xmodem client */
        status = client->port->receiveByte(client, byte);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Transmit a byte to the xmodem client
 * @param[in] client - the xmodem client object
 * @param[in] data - the data to transmit
 * @param[in] size - the size of the data to transmit
 * @return int - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e xModemClientTransmit(XModemClient_s* client,
                                              uint8_t* data,
                                              const size_t size)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    /* Send byte */
    do
    {
        /* Check input parameter */
        if (client == NULL)
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the byte is valid */
        if ((client->port == NULL) ||
            (client->port->transmit == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Transmit the byte to the xmodem client */
        status = client->port->transmit(client, data, size);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Delay for a specified time
 * @param client - the xmodem client object
 * @param ms - the time in milliseconds to delay
 */
static XModemClientErr_e xModemClientDelayS(XModemClient_s* client, int ms)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    /* Delay */
    do
    {
        /* Check input parameter */
        if ((client == NULL) ||
            (ms <= 0))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the delay function is valid */
        if ((client->port == NULL) ||
            (client->port->delayMs == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Delay for the specified time */
        status = client->port->delayMs(client, ms);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Read data from the xmodem client
 * @param client - the xmodem client object
 * @param data - the buffer to read the data
 * @param size - the size of the data to read
 * @param usedSize - the size of the data read
 */
static XModemClientErr_e xModemClientReadFromMemory(XModemClient_s* client,
                                                    uint8_t* data,
                                                    const size_t size,
                                                    size_t* const usedSize,
                                                    const size_t offset)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation

    /* Send data */
    do
    {
        /* Check input parameter */
        if ((client == NULL) ||
            (data == NULL) ||
            (size <= 0) ||
            (usedSize == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the write function is valid */
        if ((client->port == NULL) ||
            (client->port->readFromMemory == NULL))
        {
            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Read data from the xmodem client */
        status = client->port->readFromMemory(client, data, size, usedSize, offset);

    } while (0);

    return status;    // Return success code
}

/**
 * @brief Flush the io buffer
 * @param client - the xmodem client obj;
 */
static void xModemClientIoFlush(XModemClient_s* client)
{
    do
    {
        /* Check input */
        if (client == NULL)
        {
            XMODEM_CLIENT_ASSERT(0);
            break;
        }

        /* Flush the buffer */
        memset(client->io.data, 0, XMODEM_ADU_CRC16_SIZE);
        client->io.size = 0;

    } while (0);
}

/**
 * @brief Process the XModem client state machine.
 * @param xmodem - the xmodem client object
 * @return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e xModemClientFSMProc(XModemClient_s* client)
{
    /* Local var */
    XModemClientErr_e status = XMODEM_CLIENT_NO_ERR;    // Variable to store the status of the operation
    XModemErr_e xmodemStatus = XMODEM_NO_ERR;           // Variable to store the status of the xmodem operation
    uint8_t byte = 0;                                   // Variable to store the received byte
    bool isRxByte = false;                              // Variable to store the received byte flag
    bool txIsEnd = false;

    /* Flush the state machine */
    client->state = XMODEM_CLIENT_STATE_START;    // Set the state to start
    client->currentPacketInd = 0;                 // Set the current packet index to 0
    client->currentErrCount = 0;                  // Set the current error count to 0

    /* Process the state machine */
    do
    {
        switch (client->state)
        {
            /* Wait for the SOH character */
            case XMODEM_CLIENT_STATE_START :
            {
                do
                {
                    /* Delay */
                    status = xModemClientDelayS(client, XMODEM_CLIENT_START_TIMEOUT_MS);
                    if (status != XMODEM_CLIENT_NO_ERR)    // Check if the byte is sent successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Wait for the SOH character */
                    status = xModemClientIsRxByte(client, &isRxByte);    // Check if the byte is received
                    if (status != XMODEM_CLIENT_NO_ERR)                  // Check if the byte is received successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Check if the byte is received */
                    if (isRxByte == false)    // Check if the byte is received
                    {
                        break;    // Exit the loop
                    }

                    /* Receive the packet */
                    status = xModemClientRxByte(client, &byte);    // Receive the packet
                    if (status != XMODEM_CLIENT_NO_ERR)            // Check if the byte is received successfully
                    {
                        break;    // Exit the loop
                    }

                    if ((byte == XMODEM_CONST_C))
                    {
                        /* Set the crc16 type */
                        xmodemStatus = XModemCrcSet(&client->xmodem,
                                                    XMODEM_CRC_16);
                        if (xmodemStatus != XMODEM_NO_ERR)    // Check if the xmodem object is in error state
                        {
                            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error
                            break;                                      // Exit the loop
                        }

                        /* Update packet ind */
                        client->currentPacketInd = 1;    // Increment the packet index

                        client->currentDataSize = 0;    // Set the current data size to 0

                        /* Change state to send packet */
                        client->state = XMODEM_CLIENT_STATE_PACKET_SEND;    // Change state to send packet

                        break;    // Exit the loop
                    }

                    else if (byte == XMODEM_CONST_NACK)
                    {
                        /* Set the crc16 type */
                        xmodemStatus = XModemCrcSet(&client->xmodem,
                                                    XMODEM_CRC_8);
                        if (xmodemStatus != XMODEM_NO_ERR)    // Check if the xmodem object is in error state
                        {
                            status = XMODEM_CLIENT_INVALID_ARGS_ERR;    // Set status to error
                            break;                                      // Exit the loop
                        }

                        /* Update packet ind */
                        client->currentPacketInd = 1;    // Increment the packet index

                        /* Update data size */
                        client->currentDataSize = 0;    // Set the current data size to 0

                        /* Change state to send packet */
                        client->state = XMODEM_CLIENT_STATE_PACKET_SEND;    // Change state to send packet

                        break;    // Exit the loop
                    }

                    else
                    {
                        /* Update err */
                        client->currentErrCount++;

                        break;    // Exit the loop
                    }

                } while (0);

                break;
            }

            /* Receive the packet */
            case XMODEM_CLIENT_STATE_PACKET_SEND :
            {
                do
                {
                    /* Read data from the memory */
                    status = xModemClientReadFromMemory(client,
                                                        client->io.data,
                                                        XMODEM_PDU_SIZE,
                                                        &client->io.size,
                                                        client->currentDataSize);

                    if (status != XMODEM_CLIENT_NO_ERR)    // Check if the byte is sent successfully
                    {
                        break;    // Exit the loop
                    }

                    if (client->io.size == 0)    // Check if the byte is sent successfully
                    {
                        /* Change state to end transfer */
                        client->state = XMODEM_CLIENT_STATE_TRANSFER_END;    // Change state to end transfer

                        break;    // Exit the loop
                    }

                    /* Encode */
                    xmodemStatus = XModemEncode(&client->xmodem,
                                                client->io.data,
                                                client->io.size);
                    if (xmodemStatus != XMODEM_NO_ERR)    // Check if the byte is sent successfully
                    {
                        status = XMODEM_CLIENT_RUN_TIME_ERR;    // Set status to error if command is NULL
                        break;                                  // Exit the loop
                    }

                    /* Set packet id */
                    xmodemStatus = XModemAduIdSet(&client->xmodem,
                                                  client->currentPacketInd % 256);
                    if (xmodemStatus != XMODEM_NO_ERR)    // Check if the byte is sent successfully
                    {
                        status = XMODEM_CLIENT_RUN_TIME_ERR;    // Set status to error if command is NULL
                        break;                                  // Exit the loop
                    }

                    /* Send data */
                    size_t sizeToSend = (client->xmodem.crcType == XMODEM_CRC_16)
                                            ? XMODEM_ADU_CRC16_SIZE
                                            : XMODEM_ADU_CRC8_SIZE;    // Set the size to send
                    status = xModemClientTransmit(client,
                                                  &client->xmodem.adu,
                                                  sizeToSend);
                    if (status != XMODEM_CLIENT_NO_ERR)    // Check if the byte is sent successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Change state to receive ack */
                    client->state = XMODEM_CLIENT_STATE_WAIT_RESP;    // Change state to receive ack

                } while (0);

                break;
            }

            case XMODEM_CLIENT_STATE_WAIT_RESP :
            {
                /* Receive the packet */
                status = xModemClientRxByte(client, &byte);    // Receive the packet
                if (status != XMODEM_CLIENT_NO_ERR)            // Check if the byte is received successfully
                {
                    break;    // Exit the loop
                }

                if (byte == XMODEM_CONST_ACK)
                {
                    /* Update packet ind */
                    client->currentPacketInd = (client->currentPacketInd + 1) % 256;    // Increment the packet index

                    /* Update data size */
                    client->currentDataSize += client->io.size;    // Increment the current data size

                    /* Flush the io buffer */
                    xModemClientIoFlush(client);    // Flush the io buffer

                    /* Change state to send packet */
                    client->state = XMODEM_CLIENT_STATE_PACKET_SEND;    // Change state to send packet
                }
                else if (byte == XMODEM_CONST_NACK)
                {
                    /* Update error count */
                    client->currentErrCount++;    // Increment the error count

                    /* Flush the io buffer */
                    xModemClientIoFlush(client);    // Flush the io buffer

                    /* Change state to send packet */
                    client->state = XMODEM_CLIENT_STATE_PACKET_SEND;    // Change state to send packet
                }
                else
                {
                    break;    // Exit the loop
                }

                break;
            }

            /* End transfer */
            case XMODEM_CLIENT_STATE_TRANSFER_END :
            {

                /* Send EOT */
                uint8_t eot = XMODEM_CONST_EOT;
                status = xModemClientTransmit(client, &eot, 1U);
                if (status != XMODEM_CLIENT_NO_ERR)
                {
                    break;    // Exit the loop on error
                }

                /* Delay */
                status = xModemClientDelayS(client, 100U);
                if (status != XMODEM_CLIENT_NO_ERR)
                {
                    break;    // Exit the loop on error
                }

                /* Wait for ACK after EOT */
                status = xModemClientRxByte(client, &byte);
                if ((status != XMODEM_CLIENT_NO_ERR) || (byte != XMODEM_CONST_ACK))
                {
                    // Если ACK не получен, можно либо повторить отправку, либо выйти с ошибкой
                    status = XMODEM_CLIENT_TRANSFER_ERR;
                    break;
                }

                txIsEnd = true;
                break;
            }

            /* Send err */
            default :
            {
                status = XMODEM_CLIENT_TRANSFER_ERR;
            }
        }

        /* Check status */
        if (status != XMODEM_CLIENT_NO_ERR ||
            txIsEnd == true)    // Check if the state machine is in error state
        {
            break;    // Exit the loop
        }

    } while (1);

    return status;    // Return success code
}