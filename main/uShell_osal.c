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
 * \brief Initialize the UShell Hal module.
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
       (portTable->msgGet == NULL) || (portTable->threadStart == NULL) ||
       (portTable->threadStop == NULL) || (portTable->msgSend == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;
    }

    /* Initialize the object */
    osal->portTable = portTable;
    osal->parent = parent;
    osal->name = name;
    osal->worker = NULL;
    osal->msgHandle = NULL;
    osal->mutexHandle = NULL;
    osal->threadHandle = NULL;

    return USHELL_OSAL_NO_ERR;      // Exit: Success
}

/**
 * \brief Deinitialize the UShell Hal module.
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
    osal->msgHandle = NULL;
    osal->mutexHandle = NULL;
    osal->threadHandle = NULL;

    return USHELL_OSAL_NO_ERR;      // Exit: Success

}

/**
 * \brief Set the parent object of the UShellOsal module.
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
 * \brief Get the parent object of the UShellOsal module.
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
 * \brief Set the name of the UShellOsal module.
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
 * \brief Get the name of the UShellOsal module.
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
 * \brief Attach a worker function to the UShellOsal module.
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
 * \brief Detach a worker function from the UShellOsal module.
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
 * \brief Lock object
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
 * \brief Unlock object
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
 * \brief Send message
 * \param [in] osal - UShellOsal obj to send message
 * \param [in] msg - message to send
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalMsgSend(UShellOsal_s* const osal, const UShellOsalMsg_e msg)
{
    /* Check input parametrs */
    if(osal == NULL)
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    switch(msg)
    {
        case USHELL_OSAL_MSG_RX_COMPLETED:
        case USHELL_OSAL_MSG_TX_COMPLETE:
        case USHELL_OSAL_MSG_RX_TX_ERROR:
            break;
        default:
            return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->msgSend == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Send the message */
    UShellOsalErr_e status = osal->portTable->msgSend(osal, msg);

    return status;      // Exit: Success
}

/**
 * \brief Get message
 * \param [in] osal - UShellOsal obj to get message
 * \param [out] msg - pointer to store the message
 * \param [in] msWait - timeout in ms to wait for the message
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalMsgGet(UShellOsal_s* const osal, UShellOsalMsg_e* const msg, const UShellOsalTimeOut_t msWait)
{
    /* Check input parametrs */
    if((osal == NULL) || (msg == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS_ERR;   // Exit: error - invalid arguments
    }

    /* Check init state */
    if((osal->portTable == NULL) || (osal->portTable->msgGet == NULL))
    {
        return USHELL_OSAL_NOT_INIT_ERR;   // Exit: error - not initialized
    }

    /* Get the message */
    UShellOsalErr_e status = osal->portTable->msgGet(osal, msg, msWait);

    return status;      // Exit: Success
}

/**
 * \brief Start the thread
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
 * \brief Stop the thread
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
