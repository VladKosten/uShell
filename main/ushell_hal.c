/**
 * \file         UShell_hal.c
 * @brief        The file contains the implementation of the UShell hardware abstraction layer module.
 *               The module is interface between the UShell and the hardware.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_hal.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * @brief Assert macro for the UShellHal module.
 */
#ifndef USHELL_HAL_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_HAL_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_HAL_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Initialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShellHalStm32)
 * \param[in] name - pointer to the name of the object (e.g. "UShellHal")
 * \param[in] cbTable - pointer to the callback table to be used ()
 * \param[in] portTable Pointer to the port table
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success, otherwise error code
 */
UShellHalErr_e UShellHalInit(UShellHal_s* const hal,
                             const void* const parent,
                             const char* const name,
                             const UShellHalPortTable_s* const portTable)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(portTable != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Initialize the UShellHal object */
    do
    {
        /* Check input parameter */
        if ((hal == NULL) ||
            (portTable == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Initialize the UShellHal object */
        memset(hal, 0, sizeof(UShellHal_s));

        /* Set the parent object */
        hal->parent = parent;

        /* Set the name of the object */
        hal->name = name;

        /* Set the port table */
        hal->port = portTable;

    } while (0);

    return status;
}

/**
 * @brief Deinitialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be deinitialized
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success, otherwise error code
 */
UShellHalErr_e UShellHalDeinit(UShellHal_s* const hal)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Deinitialize the UShellHal object */
    do
    {
        /* Check input parameter */
        if (hal == NULL)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        memset(hal, 0, sizeof(UShellHal_s));

    } while (0);

    return status;
}

/**
 * @brief Set the parent of the UShellHal object
 * @param[in] hal - UShellHal object to set the parent
 * @param[in] parent - pointer to the parent object
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalParentSet(UShellHal_s* const hal,
                                  const void* const parent)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if ((hal == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Set the parent object */
        hal->parent = parent;

    } while (0);

    return status;
}

/**
 * @brief Get parent from the UShellHal object
 * \param[in] hal - UShellHal object to get the parent
 * \param[out] parent - pointer to store the parent
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalParentGet(UShellHal_s* const hal,
                                  void** const parent)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(parent != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if ((hal == NULL) ||
            (parent == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Get the parent object */
        *parent = (void*) hal->parent;

    } while (0);

    return status;
}

/**
 * @brief Get the name of the UShellHal object
 * \param[in] hal - UShellHal object to get the name
 * \param[out] name - pointer to store the name
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalNameGet(UShellHal_s* const hal,
                                const char** const name)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(name != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if ((hal == NULL) ||
            (name == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Get the name of the object */
        *name = hal->name;

    } while (0);

    return status;
}

/**
 * @brief Attach callback to the UShellHal object
 * \param[in] hal - UShellHal object to attach the callback
 * \param[in] cbType - type of the callback to attach
 * \param[in] cb - callback to attach
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalCbAttach(UShellHal_s* const hal,
                                 const UShellHalCallback_e cbType,
                                 const UShellHalCb cb)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(cb != NULL);
    USHELL_HAL_ASSERT(cbType & USHELL_HAL_CB_ALL != 0);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;
    bool cbAttached = false;

    /* Process */
    do
    {
        /* Check input parameter */
        if ((hal == NULL) ||
            (cb == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is rx/tx error callback attached */
        if (cbType & USHELL_HAL_CB_RX_TX_ERROR)
        {
            hal->rxTxErrorCb = cb;
            cbAttached = true;
        }

        /* Check is tx complete callback attached */
        if (cbType & USHELL_HAL_CB_TX_COMPLETE)
        {
            hal->txCompleteCb = cb;
            cbAttached = true;
        }

        /* Check is rx received callback attached */
        if (cbType & USHELL_HAL_CB_RX_RECEIVED)
        {
            hal->rxReceivedCb = cb;
            cbAttached = true;
        }

        /* Check if callback was attached */
        if (!cbAttached)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Detach callback from the UShellHal object
 * \param[in] hal - UShellHal object to detach the callback
 * \param[in] cbType - type of the callback to detach
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalCbDetach(UShellHal_s* const hal,
                                 const UShellHalCallback_e cbType)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(cbType & USHELL_HAL_CB_ALL != 0);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;
    bool cbDetached = false;

    /* Process */
    do
    {
        /* Check input parameter */
        if (hal == NULL)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is rx/tx error callback attached */
        if (cbType & USHELL_HAL_CB_RX_TX_ERROR)
        {
            hal->rxTxErrorCb = NULL;
            cbDetached = true;
        }

        /* Check is tx complete callback attached */
        if (cbType & USHELL_HAL_CB_TX_COMPLETE)
        {
            hal->txCompleteCb = NULL;
            cbDetached = true;
        }

        /* Check is rx received callback attached */
        if (cbType & USHELL_HAL_CB_RX_RECEIVED)
        {
            hal->rxReceivedCb = NULL;
            cbDetached = true;
        }

        /* Check if callback was detached */
        if (!cbDetached)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief  Open the UShellHal object
 * @param[in] hal - UShellHal object to open
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalOpen(UShellHal_s* const hal)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if (hal == NULL)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is init */
        if ((hal->port == NULL) ||
            (hal->port->open == NULL))
        {
            status = USHELL_HAL_PORT_ERR;
            break;
        }

        /* Open the port */
        status = hal->port->open(hal);

    } while (0);

    return status;
}

/**
 * @brief Close the UShellHal object
 * @param[in] hal - UShellHal object to close
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalClose(UShellHal_s* const hal)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if (hal == NULL)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is init */
        if ((hal->port == NULL) ||
            (hal->port->close == NULL))
        {
            status = USHELL_HAL_PORT_ERR;
            break;
        }

        /* Close the port */
        status = hal->port->close(hal);

    } while (0);

    return status;
}

/**
 * @brief Write data to the UShellHal object
 * @param[in] hal - UShellHal object to write
 * @param[in] data - pointer to the data to write
 * @param[in] size - size of the data to write
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalWrite(UShellHal_s* const hal,
                              const UShellHalItem_t* const data,
                              const size_t size)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(data != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if ((hal == NULL) ||
            (data == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is init */
        if ((hal->port == NULL) ||
            (hal->port->write == NULL))
        {
            status = USHELL_HAL_PORT_ERR;
            break;
        }

        /* Close the port */
        status = hal->port->write(hal, data, size);

    } while (0);

    return status;
}

/**
 * @brief Read data from the UShellHal object
 * @param[in] hal - UShellHal object to read
 * @param[in] data - pointer to the data to read
 * @param[in] size - size of the data to read
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalRead(UShellHal_s* const hal,
                             UShellHalItem_t* const data,
                             const size_t size)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);
    USHELL_HAL_ASSERT(data != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if ((hal == NULL) ||
            (data == NULL))
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is init */
        if ((hal->port == NULL) ||
            (hal->port->read == NULL))
        {
            status = USHELL_HAL_PORT_ERR;
            break;
        }

        /* Close the port */
        status = hal->port->read(hal, data, size);

    } while (0);

    return status;
}

/**
 * @brief Set the tx mode of the UShellHal object
 * @param[in] hal - UShellHal object to set the tx mode
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalSetTxMode(UShellHal_s* const hal)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if (hal == NULL)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
            break;
        }

        /* Check is init */
        if ((hal->port == NULL) ||
            (hal->port->setTxMode == NULL))
        {
            status = USHELL_HAL_PORT_ERR;
            break;
        }

        /* Set the tx mode */
        status = hal->port->setTxMode(hal);

    } while (0);

    return status;
}

/**
 * @brief Set the rx mode of the UShellHal object
 * @param[in] hal - UShellHal object to set the rx mode
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalSetRxMode(UShellHal_s* const hal)
{
    /* Check input parameter */
    USHELL_HAL_ASSERT(hal != NULL);

    /* Local variable */
    UShellHalErr_e status = USHELL_HAL_NO_ERR;

    /* Process */
    do
    {
        /* Check input parameter */
        if (hal == NULL)
        {
            status = USHELL_HAL_INVALID_ARGS_ERR;
        }

        /* Check is init */
        if ((hal->port == NULL) ||
            (hal->port->setRxMode == NULL))
        {
            status = USHELL_HAL_PORT_ERR;
            break;
        }

        /* Set the rx mode */
        status = hal->port->setRxMode(hal);

    } while (0);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
