/**
* \file         uShell_osal.c
* \brief        The file contains the implementation of the uShell OS abstraction layer.
*               The OSAL is used to provide the OS specific functionality to the uShell module.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include "uShell_osal.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief Assert macro for the UShell module.
*/
#ifndef USHELL_OSAL_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_OSAL_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_OSAL_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Initialize the UShell Hal module.
 * \param [in] osal - UShellOsal obj to be initialized
 * \param [in] portTable - port table to be used
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalInit(UShellOsal_s* const osal, const UShellOsalPortableTable_s* const portTable, const char* const name, const void* const parent)
{
    /* Check input parametrs */
    if((osal == NULL) || (portTable == NULL) || (parent == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check if the port table is valid */
    if((portTable->lock == NULL) || (portTable->unlock == NULL) ||
       (portTable->eventOcurred == NULL) || (portTable->eventGetQty == NULL) ||
       (portTable->eventFlush == NULL) || (portTable->threadStart == NULL) ||
       (portTable->threadStop == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;
    }

    /* Initialize the object */
    osal->portTable = portTable;
    osal->parent = parent;
    osal->name = name;
    osal->worker = NULL;
    osal->eventHandle = NULL;
    osal->mutexHandle = NULL;
    osal->threadHandle = NULL;

    return USHELL_OSAL_NO_ERR;      // Exit: Success
}

/**
 * @brief Deinitialize the UShell Hal module.
 * \param [in] osal - UShellOsal obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Deinitialize the object */
    osal->portTable = NULL;
    osal->parent = NULL;
    osal->name = NULL;
    osal->worker = NULL;
    osal->eventHandle = NULL;
    osal->mutexHandle = NULL;
    osal->threadHandle = NULL;

    return USHELL_OSAL_NO_ERR;      // Exit: Success

}

/**
 * @brief Set the parent object of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to set parent
 * \param [in] parent - pointer to the parent object
 * \param [out] none
*/
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s* const osal, const void* const parent)
{
    /* Check input parametrs */
    if((osal == NULL) || (parent == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Set the parent object */
    osal->parent = parent;

    return USHELL_OSAL_NO_ERR;      // Exit: Success
}

/**
 * @brief Get the parent object of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to get parent
 * \param [out] parent - pointer to the parent object
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalParentGet(const UShellOsal_s* const osal, void** const parent)
{
    /* Check input parametrs */
    if((osal == NULL) || (parent == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Get the parent object */
    *parent = osal->parent;

    return USHELL_OSAL_NO_ERR;      // Exit: Success

}
/**
 * @brief Set the name of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to set name
 * \param [in] name - name of the object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s* const osal, const char* const name)
{
    /* Check input parametrs */
    if((osal == NULL) || (name == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Set the name of the object */
    osal->name = name;

    return USHELL_OSAL_NO_ERR;      // Exit: Success
}

/**
 * @brief Get the name of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to get name
 * \param [out] name - name of the object
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalNameGet(const UShellOsal_s* const osal, const char** const name)
{
    /* Check input parametrs */
    if((osal == NULL) || (name == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Get the name of the object */
    *name = osal->name;

    return USHELL_OSAL_NO_ERR;      // Exit: Success

}

/**
 * @brief Attach a worker function to the UShellOsal module.
 * \param [in] osal - UShellOsal obj to attach worker
 * \param [in] worker - worker function
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalWorkerAttach(UShellOsal_s* const osal, UShellOsalWorker_t worker)
{
    /* Check input parametrs */
    if((osal == NULL) || (worker == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Attach the worker function */
    osal->worker = worker;

    return USHELL_OSAL_NO_ERR;      // Exit: Success

}

/**
 * @brief Detach a worker function from the UShellOsal module.
 * \param [in] osal - UShellOsal obj to detach worker
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalWorkerDetach(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Detach the worker function */
    osal->worker = NULL;

    return USHELL_OSAL_NO_ERR;      // Exit: Success

}

/**
 * @brief Lock object
 * \param [in] osal - UShellOsal obj to lock
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalLock(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->lock == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Lock the object */
    UShellOsalErr_e status = osal->portTable->lock(osal);

    return status;      // Exit: Success
}

/**
 * @brief Unlock object
 * \param [in] osal - UShellOsal obj to unlock
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->unlock == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Unlock the object */
    UShellOsalErr_e status = osal->portTable->unlock(osal);

    return status;      // Exit: Success

}

/**
 * @brief Event ocurred
 * \param [in] osal - UShellOsal obj to check event
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalEventOcurred(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->eventOcurred == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Check if event ocurred */
    UShellOsalErr_e status = osal->portTable->eventOcurred(osal);

    return status;      // Exit: Success
}

/**
 * @brief Get quantity of events ocurred
 * \param [in] osal - UShellOsal obj to wait for event
 * \param [out] qt - quantity of events
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalEventQtyGet(UShellOsal_s* const osal, uint8_t* const qty)
{
    /* Check input parametrs */
    if((osal == NULL) || (qty == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->eventQtyGet == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Get quantity of events ocurred */
    UShellOsalErr_e status = osal->portTable->eventQtyGet(osal, qty);

    return status;      // Exit: Success

}

/**
 * @brief Flush the event queue
 * \param [in] osal - UShellOsal obj to flush event queue
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalEventFlush(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->eventFlush == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Flush the event queue */
    UShellOsalErr_e status = osal->portTable->eventFlush(osal);

    return status;      // Exit: Success

}

/**
 * @brief Start the thread
 * \param [in] osal - UShellOsal obj to start thread
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalThreadStart(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->threadStart == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Start the thread */
    UShellOsalErr_e status = osal->portTable->threadStart(osal);

    return status;      // Exit: Success
}

/**
 * @brief Stop the thread
 * \param [in] osal - UShellOsal obj to stop thread
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalThreadStop(UShellOsal_s* const osal)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->threadStop == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Stop the thread */
    UShellOsalErr_e status = osal->portTable->threadStop(osal);

    return status;      // Exit: Success
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
