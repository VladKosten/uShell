/**
 * \file         xmodem.c
 * \brief        The file contains the implementation of the Xmodem protocol.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "xmodem.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the xmodem module.
 */
#ifndef XMODEM_ASSERT
    #ifdef USHELL_ASSERT
        #define XMODEM_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define XMODEM_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Calculate the CRC16 of a byte
 * \param[in] crc - the current CRC value
 * \param[in] byte - the byte to calculate the CRC for
 * \return uint16_t - error code. non-zero = an error has occurred;
 */
static uint16_t xModemCrc16(uint8_t* data, size_t size);

/**
 * \brief Calculate the CRC8 of a byte
 * \param[in] crc - the current CRC value
 * \param[in] byte - the byte to calculate the CRC for
 * \return uint8_t - error code. non-zero = an error has occurred;
 */
static uint8_t xModemCrc8(uint8_t* data, size_t size);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the XModem module.
 * \param [in] xmodem - XModem obj to be initialized
 * \param [in] parent - pointer to the parent object (optional)
 * \return XModemErr_e - error code
 */
XModemErr_e XModemInit(XModem_s* const xmodem,
                       const void* const parent,
                       const XModemCrcType_e crc)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        if (!(crc == XMODEM_CRC_16) &&
            !(crc == XMODEM_CRC_8))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Flush the xmodem object */
        memset(xmodem, 0, sizeof(*xmodem));    // Clear the xmodem object

        /* Set the parent */
        xmodem->parent = parent;    // Set the parent object

        /* Set the crc type*/
        xmodem->crcType = crc;    // Set the CRC type

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Deinitialize the XModem module.
 * \param [in] xmodem - XModem obj to be deinitialized
 * \return XModemErr_e - error code
 */
XModemErr_e XModemDeinit(XModem_s* const xmodem)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Deinitialize the xmodem object */
        memset(xmodem, 0, sizeof(*xmodem));    // Clear the xmodem object

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Set the XModem CRC type.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] crc - CRC type to be set
 * \return XModemErr_e - error code
 */
XModemErr_e XModemCrcSet(XModem_s* const xmodem,
                         const XModemCrcType_e crc)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Check if the CRC type is valid */
        if (!(crc == XMODEM_CRC_16) &&
            !(crc == XMODEM_CRC_8))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Set the CRC type */
        xmodem->crcType = crc;    // Set the CRC type

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Flush the XModem module.
 * \param [in] xmodem - XModem obj to be processed
 * \return XModemErr_e - error code
 */
XModemErr_e XModemFlush(XModem_s* const xmodem)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Flush the xmodem object */
        memset(&xmodem->adu, 0, sizeof(XModemAdu_s));    // Clear the xmodem object
        xmodem->adu.crcHigh = 0;                         // Set the CRC to 0
        xmodem->adu.crcLow = 0;                          // Set the CRC to 0
        xmodem->adu.id = 0;                              // Set the ID to 0
        xmodem->adu.idComp = 0;                          // Set the ID complement to 0
        xmodem->adu.preamble = XMODEM_CONST_SOH;         // Set the preamble to SOH

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Encode the data using XModem protocol.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] data - pointer to the data to be encoded
 * \param [in] size - size of the data to be encoded
 * \return XModemErr_e - error code
 */
XModemErr_e XModemEncode(XModem_s* const xmodem,
                         const uint8_t* const data,
                         const size_t size)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    /* Encode */
    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (data == NULL) ||
            (size == 0) ||
            (size > XMODEM_PDU_SIZE))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Preamble */
        xmodem->adu.preamble = XMODEM_CONST_SOH;    // Set the preamble to SOH

        /* Update the xmodem data */
        memcpy(xmodem->adu.pdu.data, data, size);    // Copy the data to the xmodem object

        /* Check size if not equal to the xmodem size */
        if (size != XMODEM_PDU_SIZE)
        {
            /* Fill the rest of the data with 0x00 */
            for (size_t i = size; i < XMODEM_PDU_SIZE; i++)
            {
                xmodem->adu.pdu.data [i] = 0x00;    // Fill the rest of the data
            }
        }

        /* Check the crc  */
        switch (xmodem->crcType)
        {
            /* CRC-8 */
            case XMODEM_CRC_8 :
            {
                /* Calculate the CRC */
                uint8_t crc = xModemCrc8(xmodem->adu.pdu.data, XMODEM_PDU_SIZE);    // Calculate the CRC for the xmodem object
                xmodem->adu.crcHigh = crc;                                          // Set the CRC high byte

                break;
            }

            /* CRC-16 */
            case XMODEM_CRC_16 :
            {
                /* Calculate the CRC */
                uint16_t crc = xModemCrc16(xmodem->adu.pdu.data, XMODEM_PDU_SIZE);
                xmodem->adu.crcHigh = (uint8_t) (crc >> 8);     // Set the CRC high byte
                xmodem->adu.crcLow = (uint8_t) (crc & 0xFF);    // Set the CRC low byte

                break;
            }

            /* Err */
            default :
            {
                status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
                break;
            }
        }

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Decode the data using XModem protocol.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] data - pointer to the data to be decoded
 * \param [in] buffSize - size of the data to be decoded
 * \return XModemErr_e - error code
 */
XModemErr_e XModemDecode(XModem_s* const xmodem,
                         uint8_t* const data,
                         const size_t buffSize)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation
    size_t i = 0;                          // Index variable

    uint8_t id = 0;         // Variable to store the ID value
    uint8_t idComp = 0;     // Variable to store the ID complement value
    uint8_t* pdu = NULL;    // Pointer to the PDU data

    /* Decode */
    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (data == NULL) ||
            (buffSize > XMODEM_ADU_CRC16_SIZE))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Check SOH */
        if (data [0] != XMODEM_CONST_SOH)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if SOH is invalid
            break;                               // Exit the loop
        }

        /* Check the ID */
        id = data [1];        // Get the ID from the data
        idComp = data [2];    // Get the ID complement from the data
        if (id != (uint8_t) (~idComp))
        {
            status = XMODEM_ID_ERR;
            break;
        }

        /* Check the crc  */
        switch (xmodem->crcType)
        {
            /* CRC-8 */
            case XMODEM_CRC_8 :
            {
                /* Calculate the CRC */
                uint8_t crc = 0;                                 // Variable to store the CRC value
                crc = xModemCrc8(&data [3], XMODEM_PDU_SIZE);    // Calculate the CRC for the xmodem object

                /* Check if the CRC is valid */
                if (crc != data [XMODEM_ADU_CRC8_SIZE - 1])
                {
                    status = XMODEM_CRC_ERR;    // Set status to error if CRC is invalid
                    break;                      // Exit the loop
                }

                break;
            }

            /* CRC-16 */
            case XMODEM_CRC_16 :
            {
                /* Calculate the CRC */
                uint16_t crc = 0;                                 // Variable to store the CRC value
                crc = xModemCrc16(&data [3], XMODEM_PDU_SIZE);    // Calculate the CRC for the xmodem object

                /* Check if the CRC is valid */
                if (crc != (((uint16_t) data [XMODEM_ADU_CRC16_SIZE - 2] << 8) | data [XMODEM_ADU_CRC16_SIZE - 1]))
                {
                    status = XMODEM_CRC_ERR;    // Set status to error if CRC is invalid
                    break;                      // Exit the loop
                }
                break;
            }

            /* Err */
            default :
            {
                status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
                break;
            }
        }

        /* Check status */
        if (status != XMODEM_NO_ERR)
        {
            break;    // Exit the loop
        }

        /* Copy the data to the xmodem object */
        memcpy(&xmodem->adu, data, sizeof(XModemAdu_s));    // Copy the data to the xmodem object

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Update the XModem packet ID.
 * \param [in] xmodem - XModem obj to be processed
 * \return XModemErr_e - error code
 */
XModemErr_e XModemAduIdUpdate(XModem_s* const xmodem)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Update the xmodem id */
        xmodem->adu.id = (xmodem->adu.id + 1) % 256;    // Increment the id

        /* Fill the xmodem id complement */
        xmodem->adu.idComp = ~xmodem->adu.id;    // Set the id complement

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Set the XModem packet ID.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] id - packet ID to be set
 * \return XModemErr_e - error code
 */
XModemErr_e XModemAduIdSet(XModem_s* const xmodem,
                           const uint8_t id)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if (xmodem == NULL)
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Set the xmodem id */
        xmodem->adu.id = id % 256;    // Set the id

        /* Set the xmodem id complement */
        xmodem->adu.idComp = ~xmodem->adu.id;    // Set the id complement

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Get the XModem packet ID.
 * \param [in] xmodem - XModem obj to be processed
 * \param [out] id - pointer to the packet ID to be retrieved
 * \return XModemErr_e - error code
 */
XModemErr_e XModemAduIdGet(XModem_s* const xmodem,
                           uint8_t* const id)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (id == NULL))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Get the xmodem id */
        *id = xmodem->adu.id;    // Get the id

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Set the XModem packet data unit.
 * \param [in] xmodem - XModem obj to be processed
 * \param [in] pdu - pointer to the packet data unit to be set
 * \return XModemErr_e - error code
 */
XModemErr_e XModemPduSet(XModem_s* const xmodem,
                         const XModemPdu_s* const pdu)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (pdu == NULL))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Set the xmodem pdu */
        memcpy(&xmodem->adu.pdu, pdu, sizeof(XModemPdu_s));    // Copy the data to the xmodem object

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Get the XModem packet data unit.
 * \param [in] xmodem - XModem obj to be processed
 * \param [out] pdu - pointer to the packet data unit to be retrieved
 * \return XModemErr_e - error code
 */
XModemErr_e XModemPduGet(XModem_s* const xmodem,
                         XModemPdu_s* const pdu)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (pdu == NULL))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Get the xmodem pdu */
        memcpy(pdu, &xmodem->adu.pdu, sizeof(XModemPdu_s));    // Copy the data to the xmodem object

    } while (0);

    return status;    // Return success code
}

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
                             size_t* const usedSize)
{
    /* Local variable */
    XModemErr_e status = XMODEM_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((xmodem == NULL) ||
            (buff == NULL) ||
            (usedSize == NULL) ||
            (bufferSize < XMODEM_PDU_SIZE))
        {
            status = XMODEM_INVALID_ARGS_ERR;    // Set status to error if command is NULL
            break;                               // Exit the loop
        }

        /* Get the xmodem pdu */
        memcpy(buff, &xmodem->adu.pdu, XMODEM_PDU_SIZE);    // Copy the data to the xmodem object

        /* Return size */
        *usedSize = XMODEM_PDU_SIZE;

    } while (0);

    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Calculate the CRC checksum for the given byte.
 * \param crc - current CRC value
 * \param byte - byte to be processed
 * \return uint16_t - updated CRC value
 */
static uint16_t xModemCrc16(uint8_t* data, size_t size)
{
    uint16_t crc = 0;
    for (size_t i = 0; i < size; i++)
    {
        uint8_t d = data [i];
        crc ^= (uint16_t) d << 8;
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/**
 * \brief Calculate the CRC8 of a byte
 * \param[in] crc - the current CRC value
 * \param[in] byte - the byte to calculate the CRC for
 * \return uint8_t - error code. non-zero = an error has occurred;
 */
static uint8_t xModemCrc8(uint8_t* data, size_t size)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < size; i++)
    {
        checksum += data [i];
    }
    return checksum;
}