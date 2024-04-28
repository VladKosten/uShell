/**
* \file         UShell_hal.c
* \brief        The file contains the implementation of the UShell hardware abstraction layer module.
*               The module is interface between the UShell and the hardware.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_hal.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief Assert macro for the UShellHal module.
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
 * \brief Initialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShellHalStm32)
 * \param[in] name - pointer to the name of the object (e.g. "UShellHal")
 * \param[in] cbTable - pointer to the callback table to be used ()
 * \param[in] portTable Pointer to the port table
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success, otherwise error code
*/
UShellHalErr_e UShellHalInit(UShellHal_s* const hal, const void* const parent, const char* const name,
                                                  const UShellHalPortableTable_s* const portTable)
{
    /* Check input param */
    if((NULL == hal)    ||
       (NULL == parent) ||
       (NULL == portTable))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Initialize the UShellHal object */
    hal->parent = parent;
    hal->name = name;
    hal->portTable = portTable;
    hal->rxReceivedCb = NULL;
    hal->txCompleteCb = NULL;
    hal->rxTxErrorCb = NULL;

    return USHELL_HAL_NO_ERR;                   // Exit: success
}

/**
 * \brief Deinitialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be deinitialized
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success, otherwise error code
*/
UShellHalErr_e UShellHalDeinit(UShellHal_s* const hal)
{
    /* Check input param */
    if(NULL == hal)
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Deinitialize the UShellHal object */
    hal->parent = NULL;
    hal->name = NULL;
    hal->portTable = NULL;
    hal->rxReceivedCb = NULL;
    hal->txCompleteCb = NULL;
    hal->rxTxErrorCb = NULL;

    return USHELL_HAL_NO_ERR;                   // Exit: success
}

/**
 * \brief Send data to the port
 * \param[in] hal - UShellHal object to set parent
 * \param[in] parent - parent to be set to the hal object
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalParentSet(UShellHal_s* const hal, const void* const parent)
{
    /* Check input parameter */
    if((hal == NULL) || (parent == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Set parent to the UShellHal object */
    hal->parent = parent;

    return USHELL_HAL_NO_ERR;                   // Exit: success
}

/**
 * \brief Get parent from the UShellHal object
 * \param[in] hal - UShellHal object to get the parent
 * \param[out] parent - pointer to store the parent
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalParentGet(UShellHal_s* const hal, void** const parent)
{
    /* Check input parameter */
    if((hal == NULL) || (parent == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Get parent from the UShellHal object */
    *parent = hal->parent;

    return USHELL_HAL_NO_ERR;                   // Exit: success
}

/**
 * \brief Set the name of the UShellHal object
 * \param[in] hal - UShellHal object to set the name
 * \param[in] name - name to be set
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalNameSet(UShellHal_s* const hal, const char* const name)
{
    /* Check input parameter */
    if((hal == NULL) || (name == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Set the name of the UShellHal object */
    hal->name = name;

    return USHELL_HAL_NO_ERR;                   // Exit: success

}

/**
 * \brief Get the name of the UShellHal object
 * \param[in] hal - UShellHal object to get the name
 * \param[out] name - pointer to store the name
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalNameGet(UShellHal_s* const hal, const char** const name)
{
    /* Check input parameter */
    if((hal == NULL) || (name == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Get the name of the UShellHal object */
    *name = hal->name;

    return USHELL_HAL_NO_ERR;                   // Exit: success

}

/**
 * \brief Attach callback to the UShellHal object
 * \param[in] hal - UShellHal object to attach the callback
 * \param[in] cbType - type of the callback to attach
 * \param[in] cb - callback to attach
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalCbAttach(UShellHal_s* const hal, const UShellHalCallback_e cbType, const UShellHalCb cb)
{
    /* Check input parameter */
    if((hal == NULL) || (cb == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Attach callback to the UShellHal object */
    switch(cbType )
    {
        case USHELL_HAL_CB_RX_RECEIVED:
            hal->rxReceivedCb = cb;
            break;
        case USHELL_HAL_CB_TX_COMPLETE:
            hal->txCompleteCb = cb;
            break;
        case USHELL_HAL_CB_RX_TX_ERROR:
            hal->rxTxErrorCb = cb;
            break;
        case USHELL_HAL_CB_ALL:
            hal->rxReceivedCb = cb;
            hal->txCompleteCb = cb;
            hal->rxTxErrorCb = cb;
            break;
        default:
            return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    return USHELL_HAL_NO_ERR;                   // Exit: success
}
/**
 * \brief Detach callback from the UShellHal object
 * \param[in] hal - UShellHal object to detach the callback
 * \param[in] cbType - type of the callback to detach
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalCbDetach(UShellHal_s* const hal, const UShellHalCallback_e cbType)
{
    /* Check input parameter */
    if(hal == NULL)
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Detach callback from the UShellHal object */
    switch(cbType )
    {
        case USHELL_HAL_CB_RX_RECEIVED:
            hal->rxReceivedCb = NULL;
            break;
        case USHELL_HAL_CB_TX_COMPLETE:
            hal->txCompleteCb = NULL;
            break;
        case USHELL_HAL_CB_RX_TX_ERROR:
            hal->rxTxErrorCb = NULL;
            break;
        case USHELL_HAL_CB_ALL:
            hal->rxReceivedCb = NULL;
            hal->txCompleteCb = NULL;
            hal->rxTxErrorCb = NULL;
            break;
        default:
            return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    return USHELL_HAL_NO_ERR;                   // Exit: success
}

/**
 * \brief Send data
 * \param[in] hal - UShellHal object to send data
 * \param[in] item - pointer to the data to be sent
 * \param[out] none
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalCharSend(UShellHal_s* const hal, const UShellHalItem_t* const item)
{
    /* Check input parameter */
    if((hal == NULL) ||
       (item == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Check init state */
    if((hal->portTable == NULL) || (hal->portTable->charSend == NULL))
    {
        return USHELL_HAL_NOT_INIT_ERR;         // Exit: not initialized
    }

    /* Send data */
    UShellHalErr_e status = hal->portTable->charSend(hal, item);

    return status;                              // Exit: success
}

/**
 * \brief Receive data
 * \param[in] hal - UShellHal object to receive data
 * \param[out] item - pointer to the data to be received
 * \return UShellHalErr_e Error code. USHELL_HAL_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalCharReceive(UShellHal_s* const hal, const UShellHalItem_t* const symbol)
{
    /* Check input parameter */
    if((hal == NULL) ||
       (symbol == NULL))
    {
        return USHELL_HAL_INVALID_ARGS_ERR;     // Exit: error in input arguments
    }

    /* Check init state */
    if((hal->portTable == NULL) || (hal->portTable->charReceive == NULL))
    {
        return USHELL_HAL_NOT_INIT_ERR;         // Exit: not initialized
    }

    /* Receive data */
    UShellHalErr_e status = hal->portTable->charReceive(hal, symbol);

    return status;                              // Exit: success
}


//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
