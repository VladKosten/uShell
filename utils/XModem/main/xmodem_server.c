/**
 * \file         xmodem_server.c
 * \brief        The file contains the implementation of the Xmodem server.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "xmodem_server.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the xmodem server module.
 */
#ifndef XMODEM_SERVER_ASSERT
    #ifdef USHELL_ASSERT
        #define XMODEM_SERVER_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define XMODEM_SERVER_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Receive a byte from the xmodem server
 * \param[in] xmodem - the xmodem server object
 * \param[in] byte - the buffer to receive the byte
 * \param[in] parent - pointer to the parent object (optional)
 * \return true -  byte received successfully.
 * \return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerIsRxByte(XModemServer_s* server,
                                              bool* isRxByte);

/**
 * \brief Receive a byte from the xmodem server
 * \param[in] server - the xmodem server object
 * \param[in] byte - the byte to receive
 * \return true -  byte received successfully.
 * \return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerRxByte(XModemServer_s* const server,
                                            uint8_t* const byte);

/**
 * \brief Transmit a byte to the xmodem server
 * \param[in] server - the xmodem server object
 * \param[in] byte - the byte to transmit
 * \return int - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e xModemServerTxByte(XModemServer_s* server,
                                            uint8_t byte);

/**
 * \brief Delay for a specified time
 * \param server - the xmodem server object
 * \param ms - the time in milliseconds to delay
 */
static XModemServerErr_e xModemServerDelayS(XModemServer_s* server,
                                            int ms);

/**
 * \brief Write data from the xmodem server
 * \param server - the xmodem server object
 * \param data - the data to write
 * \param size - the size of the data to write
 */
static XModemServerErr_e xModemServerWrite(XModemServer_s* server,
                                           uint8_t* data,
                                           int size);

/**
 * \brief Process the XModem server state machine.
 * \param server - the xmodem server object
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e xModemServerFSMProc(XModemServer_s* server);

/**
 * \brief Flush the io buffer
 * \param server - the xmodem server obj;
 */
static void xModemServerIoFlush(XModemServer_s* server);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the XModem server module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] port - pointer to the port table for the xmodem server
 * \param [in] parent - pointer to the parent object
 * \return XModemServerErr_e - error code
 */
XModemServerErr_e XModemServerInit(XModemServer_s* const server,
                                   XModemServerPort_s* const port,
                                   const void* const parent)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation
    XModemErr_e xmodemStatus = XMODEM_NO_ERR;           // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((server == NULL) ||
            (port == NULL) ||
            (parent == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the port table is valid */
        if ((port->isReceivedByte == NULL) ||
            (port->transmitByte == NULL) ||
            (port->receiveByte == NULL) ||
            (port->delayMs == NULL) ||
            (port->writeToMemory == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Initialize the xmodem server object */
        memset(server, 0, sizeof(XModemServer_s));    // Flush the xmodem server object

        /* Initialize the xmodem object */
        xmodemStatus = XModemInit(&server->xmodem, server, XMODEM_CRC_16);
        if (xmodemStatus != XMODEM_NO_ERR)    // Check if the xmodem object is in error state
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Set the port table and parent object */
        server->port = port;        // Set the port table
        server->parent = parent;    // Set the parent object

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemServerErr_e - error code
 */
XModemServerErr_e XModemServerDeinit(XModemServer_s* const server)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    /* Deinitialize the xmodem server object */
    do
    {
        /* Check input parameter */
        if (server == NULL)
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Flush the xmodem server object */
        memset(server, 0, sizeof(XModemServer_s));    // Flush the xmodem server object

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Process the XModem server state machine.
 * \param[in] xmodem - the xmodem server object
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
XModemServerErr_e XModemServerProc(XModemServer_s* const xmodem)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        status = xModemServerFSMProc(xmodem);    // Process the state machine
        if (status != XMODEM_SERVER_NO_ERR)      // Check if the state machine is in error state
        {
            break;    // Exit the loop
        }

    } while (0);

    /* Process incoming bytes and update the state machine */

    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Receive a byte from the xmodem server
 * \param[in] xmodem - the xmodem server object
 * \param[in] byte - the buffer to receive the byte
 * \param[in] parent - pointer to the parent object (optional)
 * \return true -  byte received successfully.
 * \return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerIsRxByte(XModemServer_s* xmodem,
                                              bool* isRxByte)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    /* Check if the byte is valid */
    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (isRxByte == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the byte is valid */
        if ((xmodem->port == NULL) ||
            (xmodem->port->isReceivedByte == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Receive the byte from the xmodem server */
        status = xmodem->port->isReceivedByte(xmodem, isRxByte);

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Receive a byte from the xmodem server
 * \param[in] server - the xmodem server object
 * \param[in] byte - the buffer to receive the byte
 * \return true -  byte received successfully.
 * \return false - byte not received successfully.
 */
static XModemServerErr_e xModemServerRxByte(XModemServer_s* const server,
                                            uint8_t* const byte)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    /* Receive byte */
    do
    {
        /* Check input parameter */
        if ((server == NULL) ||
            (byte == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the byte is valid */
        if ((server->port == NULL) ||
            (server->port->receiveByte == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Receive the byte from the xmodem server */
        status = server->port->receiveByte(server, byte);

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Transmit a byte to the xmodem server
 * \param[in] server - the xmodem server object
 * \param[in] byte - the byte to transmit
 * \return int - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e xModemServerTxByte(XModemServer_s* server, uint8_t byte)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    /* Send byte */
    do
    {
        /* Check input parameter */
        if (server == NULL)
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the byte is valid */
        if ((server->port == NULL) ||
            (server->port->transmitByte == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Transmit the byte to the xmodem server */
        status = server->port->transmitByte(server, byte);

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Delay for a specified time
 * \param server - the xmodem server object
 * \param ms - the time in milliseconds to delay
 */
static XModemServerErr_e xModemServerDelayS(XModemServer_s* server, int ms)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    /* Delay */
    do
    {
        /* Check input parameter */
        if ((server == NULL) ||
            (ms <= 0))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the delay function is valid */
        if ((server->port == NULL) ||
            (server->port->delayMs == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Delay for the specified time */
        status = server->port->delayMs(server, ms);

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Write data from the xmodem server
 * \param server - the xmodem server object
 * \param data - the data to write
 * \param size - the size of the data to write
 */
static XModemServerErr_e xModemServerWrite(XModemServer_s* server,
                                           uint8_t* data,
                                           int size)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation

    /* Send data */
    do
    {
        /* Check input parameter */
        if ((server == NULL) ||
            (data == NULL) ||
            (size <= 0))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Check if the write function is valid */
        if ((server->port == NULL) ||
            (server->port->writeToMemory == NULL))
        {
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                                      // Exit the loop
        }

        /* Write data from the xmodem server */
        status = server->port->writeToMemory(server, data, size);

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Flush the io buffer
 * \param server - the xmodem server obj;
 */
static void xModemServerIoFlush(XModemServer_s* server)
{
    do
    {
        /* Check input */
        if (server == NULL)
        {
            XMODEM_SERVER_ASSERT(0);
            break;
        }

        /* Flush the buffer */
        memset(server->io.data, 0, XMODEM_ADU_CRC16_SIZE);
        server->io.size = 0;

    } while (0);
}

/**
 * \brief Process the XModem server state machine.
 * \param xmodem - the xmodem server object
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e xModemServerFSMProc(XModemServer_s* server)
{
    /* Local var */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;    // Variable to store the status of the operation
    XModemErr_e xmodemStatus = XMODEM_NO_ERR;           // Variable to store the status of the xmodem operation
    uint8_t byte = 0;                                   // Variable to store the received byte
    bool isRxByte = false;                              // Variable to store the received byte flag
    bool txIsEnd = false;

    /* Reset the state machine */
    server->state = XMODEM_SERVER_STATE_START;    // Set the state to start
    server->currentPacketInd = 0;                 // Set the current packet index to 0
    server->currentErrCount = 0;                  // Set the current error count to 0

    /* Process the state machine */
    do
    {
        switch (server->state)
        {
            /* Wait for the SOH character */
            case XMODEM_SERVER_STATE_START :
            {
                do
                {

                    /* Delay */
                    status = xModemServerDelayS(server, XMODEM_START_TIMEOUT_MS);
                    if (status != XMODEM_SERVER_NO_ERR)    // Check if the byte is sent successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Wait for the SOH character */
                    status = xModemServerIsRxByte(server, &isRxByte);    // Check if the byte is received
                    if (status != XMODEM_SERVER_NO_ERR)                  // Check if the byte is received successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Check if the byte is received */
                    if (isRxByte == false)    // Check if the byte is received
                    {
                        uint8_t byte = (server->xmodem.crcType == XMODEM_CRC_16) ? XMODEM_CONST_C : XMODEM_CONST_NACK;

                        /* Send the SOH character */
                        status = xModemServerTxByte(server, byte);    // Send the SOH character
                        if (status != XMODEM_SERVER_NO_ERR)           // Check if the byte is sent successfully
                        {
                            break;    // Exit the loop
                        }

                        break;    // Exit the loop
                    }

                    /* Receive the packet */
                    status = xModemServerRxByte(server, &byte);    // Receive the packet
                    if (status != XMODEM_SERVER_NO_ERR)            // Check if the byte is received successfully
                    {
                        break;    // Exit the loop
                    }

                    if (byte != XMODEM_SERVER_STATE_SOH)
                    {
                        /* Upd err count */
                        server->currentErrCount++;    // Increment the error count

                        /* Send the SOH character */
                        status = xModemServerTxByte(server, XMODEM_CONST_C);    // Send the SOH character
                        if (status != XMODEM_SERVER_NO_ERR)                     // Check if the byte is sent successfully
                        {
                            break;    // Exit the loop
                        }

                        break;    // Exit the loop
                    }

                    /* Save the received byte to the I/O buffer */
                    server->io.data [server->io.size++] = byte;    // Save the received byte to the I/O buffer

                    /* Change the state to receive the packet */
                    server->state = XMODEM_SERVER_STATE_PACKET_GET;    // Change the state to receive the packet

                } while (0);

                break;
            }

            /* Wait for the SOH character */
            case XMODEM_SERVER_STATE_SOH :
            {
                /* Flush xmodem object */
                xmodemStatus = XModemFlush(&server->xmodem);    // Flush the xmodem object
                if (xmodemStatus != XMODEM_NO_ERR)              // Check if the xmodem object is flushed successfully
                {
                    break;    // Exit the loop
                }

                /* Flush the I/O buffer */
                xModemServerIoFlush(server);

                /* Receive the packet */
                status = xModemServerRxByte(server, &byte);    // Receive the packet
                if (status != XMODEM_SERVER_NO_ERR)            // Check if the byte is received successfully
                {
                    break;    // Exit the loop
                }

                /* Check if the byte is received */
                switch (byte)
                {
                    case XMODEM_CONST_SOH :

                        /* Save the received byte to the I/O buffer */
                        server->io.data [server->io.size++] = byte;    // Save the received byte to the I/O buffer

                        /* Change State */
                        server->state = XMODEM_SERVER_STATE_PACKET_GET;    // Change the state to receive the packet
                        break;                                             // Exit the loop

                    case XMODEM_CONST_EOT :

                        /* Change State */
                        status = xModemServerTxByte(server, XMODEM_CONST_ACK);
                        if (status != XMODEM_SERVER_NO_ERR)    // Check if the byte is sent successfully
                        {
                            break;    // Exit the loop
                        }

                        /* Change state */
                        server->state = XMODEM_SERVER_STATE_TRANSFER_END;    // Change the state to transfer end
                        break;

                    default :

                        /* Err upd */
                        server->currentErrCount++;    // Increment the error count

                        break;    // Exit the loop
                }

                break;
            }

            /* Receive the packet */
            case XMODEM_SERVER_STATE_PACKET_GET :
            {
                do
                {
                    /* Receive the packet */
                    status = xModemServerRxByte(server, &byte);    // Receive the packet
                    if (status != XMODEM_SERVER_NO_ERR)            // Check if the byte is received successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Save the received byte to the I/O buffer */
                    server->io.data [server->io.size++] = byte;    // Save the received byte to the I/O buffer

                    size_t sizeToReceive = (server->xmodem.crcType == XMODEM_CRC_16)
                                               ? XMODEM_ADU_CRC16_SIZE
                                               : XMODEM_ADU_CRC8_SIZE;    // Set the size to send

                    /* Check if the I/O buffer is full */
                    if (server->io.size >= sizeToReceive)    // Check if the I/O buffer is full
                    {
                        server->state = XMODEM_SERVER_STATE_PACKET_PROC;    // Change the state to process the packet
                        break;                                              // Exit the loop
                    }

                } while (0);

                break;
            }

            /* Process the packet */
            case XMODEM_SERVER_STATE_PACKET_PROC :
            {
                /* Process the packet */
                do
                {
                    /* Decode the packet */
                    xmodemStatus = XModemDecode(&server->xmodem,
                                                server->io.data,
                                                server->io.size);    // Decode the packet
                    if (xmodemStatus != XMODEM_NO_ERR)               // Check if the packet is decoded successfully
                    {
                        /* Update the error count */
                        server->currentErrCount++;    // Increment the error count

                        /* Send NAK*/
                        status = xModemServerTxByte(server, XMODEM_CONST_NACK);    // Send NAK to the server
                        if (status != XMODEM_SERVER_NO_ERR)                        // Check if the byte is sent successfully
                        {
                            break;    // Exit the loop
                        }

                        server->state = XMODEM_SERVER_STATE_SOH;    // Change the state to receive the packet
                        break;                                      // Exit the loop
                    }

                    /* Get id of packet */
                    uint8_t idPacket = 0;
                    xmodemStatus = XModemAduIdGet(&server->xmodem, &idPacket);    // Get the id of the packet
                    if (xmodemStatus != XMODEM_NO_ERR)                            // Check if the id of the packet is received successfully
                    {

                        status = XMODEM_SERVER_RUN_TIME_ERR;    // Set status to error if command is NULL
                        break;
                    }

                    /* Check if the packet is valid */
                    if (idPacket == server->currentPacketInd)    // Check if the packet is valid
                    {
                        /* Update the error count */
                        server->currentErrCount++;    // Increment the error count

                        /* Send NAK*/
                        status = xModemServerTxByte(server, XMODEM_CONST_NACK);    // Send NAK to the server
                        if (status != XMODEM_SERVER_NO_ERR)                        // Check if the byte is sent successfully
                        {
                            break;    // Exit the loop
                        }

                        server->state = XMODEM_SERVER_STATE_SOH;    // Change the state to receive the packet
                        break;                                      // Exit the loop
                    }

                    /* Get data from pdu */
                    xmodemStatus = XModemPduDataGet(&server->xmodem,
                                                    server->io.data,
                                                    XMODEM_ADU_CRC16_SIZE,
                                                    &server->io.size);
                    if (xmodemStatus != XMODEM_NO_ERR)    // Check if the id of the packet is received successfully
                    {

                        status = XMODEM_SERVER_RUN_TIME_ERR;    // Set status to error if command is NULL
                        break;
                    }

                    /* Write data to memory */
                    status = xModemServerWrite(server, server->io.data, server->io.size);
                    if (status != XMODEM_SERVER_NO_ERR)    // Check if the byte is sent successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Update id */
                    server->currentPacketInd = idPacket;

                    /* Flush the io */
                    xModemServerIoFlush(server);

                    /* Send ack */
                    status = xModemServerTxByte(server, XMODEM_CONST_ACK);
                    if (status != XMODEM_SERVER_NO_ERR)    // Check if the byte is sent successfully
                    {
                        break;    // Exit the loop
                    }

                    /* Change state */
                    server->state = XMODEM_SERVER_STATE_SOH;    // Change the state to receive the packet

                } while (0);

                break;
            }

            /* End transfer */
            case XMODEM_SERVER_STATE_TRANSFER_END :
            {
                txIsEnd = true;

                break;
            }

            /* Send err */
            default :
            {
                status = XMODEM_SERVER_TRANSFER_ERR;
            }
        }

        /* Check the error count */
        if (server->currentErrCount >= XMODEM_SERVER_MAX_ERR_COUNT)    // Check if the error count is exceeded
        {
            status = XMODEM_SERVER_TRANSFER_ERR;    // Set status to error if command is NULL
            break;                                  // Exit the loop
        }

        /* Check status */
        if (status != XMODEM_SERVER_NO_ERR ||
            txIsEnd == true)    // Check if the state machine is in error state
        {
            break;    // Exit the loop
        }

    } while (1);

    return status;    // Return success code
}