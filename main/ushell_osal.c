/**
 * \file    ushell_osal.c
 * \brief   UShell OSAL layer interface implementation.
 * \author     Vladislav Kosten (vladkosten@gmail.com)
 * \copyright  Copyright (c) 2024 Vlad Kosten. All rights reserved.
 * \warning   A warning may be placed here...
 * \bug       Bug report may be placed here...
 */


//===============================================================================[ INCLUDE ]=======================================================================================

#include "ushell_osal.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//=====================================================================[ INTERNAL MACRO DEFENITIONS ]==============================================================================

/**
* \brief UShell OSAL ASSERT macro definition
*/
#ifndef USHELL_OSAL_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_OSAL_ASSERT(cond)  USHELL_ASSERT(cond)
     #else
        #define USHELL_OSAL_ASSERT(cond)
     #endif
 #endif


//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]==========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]====================================================================

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]==============================================================================

/**
 * \brief Initialize UShell OSAL instance
 *        Sets name, parent, clears all internal objects etc.
 * \param[in] osal - pointer to OSAL instance
 * \param[in] name - pointer to the name of the OSAL instance
 * \param[in] parent - pointer to a parent object
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalInit(UShellOsal_s *osal, const char *name, void *const parent)
{
    if (NULL == osal)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Clear the OSAL object */
    memset(osal, 0, sizeof(UShellOsal_s));

    /* Init obj */
    osal->name   = name;
    osal->parent = parent;

    return USHELL_OSAL_NO_ERR;
}


/**
 * \brief Deinitialize UShell OSAL instance
 * \note  Call this function when all functionality has been stopped
 * \param[in] osal - pointer to OSAL instance
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s *osal)
{
    if ((NULL == osal))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Clear the OSAL object */
    memset(osal, 0, sizeof(UShellOsal_s));

    return USHELL_OSAL_NO_ERR;
}


/**
 * \brief Get pointer to a parent of the given OSAL object
 * \param[in]  osal      - pointer to osal instance which parent object will be returned
 * \param[out] parent    - pointer to an object into which the current osal parent pointer will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentGet(UShellOsal_s *const osal, void **const parent)
{
    if (NULL == osal)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    *parent = osal->parent;

    return USHELL_OSAL_NO_ERR;
}


/**
 * \brief Set the parent object for the given OSAL instance
 * \param[in] osal      - pointer to osal instance being modified
 * \param[in] parent    - pointer to parent object being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s *const osal, void *const parent)
{
    if ((NULL == osal) ||
        (NULL == parent))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    osal->parent = parent;

    return USHELL_OSAL_NO_ERR;
}


/**
 * \brief Get pointer to the name field of the given OSAL instance
 * \param[in] osal  - pointer to osal instance
 * \param[out] name  - pointer to an object into which the current osal name will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameGet(UShellOsal_s *const osal, const char **const name)
{
    if (NULL == osal)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    *name = osal->name;

    return USHELL_OSAL_NO_ERR;
}

/**
 * \brief Set name for the given OSAL instance
 * \param[in] osal  - pointer to osal instance being modified
 * \param[in] name  - pointer to name string being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s *const osal, char *const name)
{
    /* Checking of params */
    if((NULL == osal) ||
       (NULL == name))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Set name */
    osal->name = name;

    return USHELL_OSAL_NO_ERR;
}


/**
 * \brief Create the queue
 * \param[in]   osal          - OSAL descriptor;
 * \param[in]   queueItemSize - the size of the queue item
 * \param[in]   queueDepth    - the queue depth
 * \param[out]  queueHandle   - the queue handle was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueCreate(UShellOsal_s *const osal, const size_t queueItemSize,
                                                                const size_t queueDepth,
                                                                UShellOsalQueueHandle_t * const queueHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueCreate))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    UShellOsalErr_e retStatus = osal->portable->queueCreate(osal, queueItemSize, queueDepth, queueHandle);

    return retStatus;
}


/**
 * \brief Delete the queue
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueDelete(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueDelete))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    UShellOsalErr_e retStatus = osal->portable->queueDelete(osal, queueHandle);

    return retStatus;
}


/**
 * \brief Put item to the queue
 * \param[in] osal - OSAL descriptor;
 * \param[in] queueHandle   - the queue handle in which to put the item
 * \param[in] queueItemPtr  - pointer to the item source buffer
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemPut(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                 const void *const queueItemPtr)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle) ||
        (NULL == queueItemPtr))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueItemPut))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    UShellOsalErr_e retStatus = osal->portable->queueItemPut(osal, queueHandle, queueItemPtr);

    return retStatus;
}


/**
 * \brief Put item to the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * \param[in] osal - OSAL descriptor;
 * \param[in] queueHandle   - the queue handle in which to put the item
 * \param[in] queueItemPtr  - pointer to the item source buffer
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e ushellOsalQueueItemPost(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                  void *const queueItemPtr,
                                                                  const uint32_t timeoutMs)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle) ||
        (NULL == queueItemPtr))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if ((NULL == osal->portable->queueItemPost))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    UShellOsalErr_e retStatus = osal->portable->queueItemPost(osal, queueHandle, queueItemPtr, timeoutMs);

    return retStatus;
}


/**
 * \brief Get item from the queue
 * \note  (NON-BLOCKING CALL)
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle in which to put the item
 * \param[out]  queueItemPtr  - pointer to the destination buffer in which the item should be places
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemGet(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                 void *const queueItemPtr)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle) ||
        (NULL == queueItemPtr))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueItemGet))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Get item from the queue */
    UShellOsalErr_e retStatus = osal->portable->queueItemGet(osal, queueHandle, queueItemPtr);

    return retStatus;
}


/**
 * \brief Get item from the queue
 * \note  (BLOCKING CALL WITH INFINITE WAIT)
 * \param[in]  osal - OSAL descriptor;
 * \param[in]  queueHandle   - the queue handle in which to put the item
 * \param[out] queueItemPtr  - pointer to the destination buffer in which the item should be places
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemWait(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                  void *const queueItemPtr)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle) ||
        (NULL == queueItemPtr))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueItemWait))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Get item from the queue */
    UShellOsalErr_e retStatus = osal->portable->queueItemWait(osal, queueHandle, queueItemPtr);

    return retStatus;
}


/**
 * \brief Get item from the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * \param[in]  UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]  queueHandle   - the queue handle from which to get the item
 * \param[in]  queueItemPtr  - pointer to the destination buffer in which the item should be placed
 * \param[out] timeoutMs     - timeout in milliseconds to wait for the item
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e  ushellOsalQueueItemPend(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                   void *const queueItemPtr,
                                                                   const uint32_t timeoutMs)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle) ||
        (NULL == queueItemPtr))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueItemPend))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Get item from the queue */
    UShellOsalErr_e retStatus = osal->portable->queueItemPend(osal, queueHandle, queueItemPtr, timeoutMs);

    return retStatus;
}


/**
 * @brief Reset queue
 * @param[in] osal          - pointer to OSAL instance
 * @param[in] queueHandle   - the queue handle to reset
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueReset(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == queueHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->queueReset))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Reset queue */
    UShellOsalErr_e retStatus = osal->portable->queueReset(osal, queueHandle);

    return retStatus;
}

/**
* \brief Enter critical section
* \param osal - pointer to osal instance being modified
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalCriticalSectionEnter(UShellOsal_s *const osal)
{
    /* Checking of params */
    if (NULL == osal)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->criticalSectionEnter))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Enter critical section */
    UShellOsalErr_e retStatus = osal->portable->criticalSectionEnter(osal);

    return retStatus;
}

/**
* \brief Exit critical section
* \param osal - pointer to osal instance being modified
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalCriticalSectionExit(UShellOsal_s *const osal)
{
    /* Checking of params */
    if (NULL == osal)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->criticalSectionExit))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Exit critical section */
    UShellOsalErr_e retStatus = osal->portable->criticalSectionExit(osal);

    return retStatus;
}

/**
 * \brief Create the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[out]  lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjCreate(UShellOsal_s *const osal, UShellOsalLockObjHandle_t * const lockObjHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == lockObjHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->lockObjCreate))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Create the lock object */
    UShellOsalErr_e retStatus = osal->portable->lockObjCreate(osal, lockObjHandle);

    return retStatus;
}

/**
 * \brief Delete the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjDelete(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == lockObjHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->lockObjDelete))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Delete the lock object */
    UShellOsalErr_e retStatus = osal->portable->lockObjDelete(osal, lockObjHandle);

    return retStatus;
}


/**
 * \brief Lock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLock(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == lockObjHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->lock))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Lock access to the resource */
    UShellOsalErr_e retStatus = osal->portable->lock(osal, lockObjHandle);

    return retStatus;
}



/**
 * \brief Unlock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == lockObjHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->unlock))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Unlock access to the resource */
    UShellOsalErr_e retStatus = osal->portable->unlock(osal, lockObjHandle);

    return retStatus;
}


/**
 * \brief Create the semaphore object
 * \param[in] osal               - pointer to OSAL instance
 * \param[in] semaphoreCountMax  - the maximum count of the semaphore
 * \param[in] semaphoreInitValue - the initial value of the semaphore
 * \param[in] semaphoreHandle    - semaphore object handle that was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreCreate(UShellOsal_s *const osal, const UShellOsalSemaphoreCount_t semaphoreCountMax,
                                                                    const UShellOsalSemaphoreCount_t semaphoreInitValue,
                                                                    UShellOsalSemaphoreHandle_t *const semaphoreHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == semaphoreHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->semaphoreCreate))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Create the semaphore object */
    UShellOsalErr_e retStatus = osal->portable->semaphoreCreate(osal, semaphoreCountMax, semaphoreInitValue, semaphoreHandle);

    return retStatus;
}

/**
 * \brief Delete the semaphore object
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreDelete(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == semaphoreHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->semaphoreDelete))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Delete the semaphore object */
    UShellOsalErr_e retStatus = osal->portable->semaphoreDelete(osal, semaphoreHandle);

    return retStatus;
}

/**
 * \brief Acquire the semaphore
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle to acquire
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreAcquire(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == semaphoreHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->semaphoreAcquire))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Acquire the semaphore */
    UShellOsalErr_e retStatus = osal->portable->semaphoreAcquire(osal, semaphoreHandle);

    return retStatus;
}

/**
 * \brief Release the semaphore
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle to release
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreRelease(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == semaphoreHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->semaphoreRelease))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Release the semaphore */
    UShellOsalErr_e retStatus = osal->portable->semaphoreRelease(osal, semaphoreHandle);

    return retStatus;
}

/**
 * \brief Get the current count of the semaphore
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle
 * \param[in] semaphoreCount    - the current count of the semaphore
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreCountGet(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle, UShellOsalSemaphoreCount_t *const semaphoreCount)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == semaphoreHandle) ||
        (NULL == semaphoreCount))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->semaphoreCountGet))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Get the current count of the semaphore */
    UShellOsalErr_e retStatus = osal->portable->semaphoreCountGet(osal, semaphoreHandle, semaphoreCount);

    return retStatus;
}

/**
 * \brief Create the thread
 * \param[in]   osal - OSAL descriptor;
 * \param[out]  threadHandle  - thread handle by which created thread can be referenced
 * \param[in]   threadCfg     - thread configuration
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadCreate(UShellOsal_s *const osal, UShellOsalThreadHandle_t * const threadHandle, UShellOsalThreadCfg_s threadCfg)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == threadHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->threadCreate))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Create the thread */
    UShellOsalErr_e retStatus = osal->portable->threadCreate(osal, threadHandle, threadCfg);

    return retStatus;
}

/**
 * \brief Delete the thread
 * \note The operation must be stopped before deleting the thread
 *        to not to damage the system.
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   threadHandle  - the handle of the thread being deleted
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadDelete(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == threadHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->threadDelete))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Delete the thread */
    UShellOsalErr_e retStatus = osal->portable->threadDelete(osal, threadHandle);

    return retStatus;
}

/**
 * \brief Suspend the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being suspend
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadSuspend(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == threadHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->threadSuspend))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Suspend the thread */
    UShellOsalErr_e retStatus = osal->portable->threadSuspend(osal, threadHandle);

    return retStatus;
}

/**
 * \brief       Perform some delay
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   const uint32_t msDelay - the delay timeout value expressed in ms;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalThreadDelay(const UShellOsal_s* const osal, const uint32_t msDelay)
{
    /* Checking of params */
    if (NULL == osal)
    {
        return USHELL_OSAL_INVALID_ARGS;    // Exit: Error: Invalid args
    }

    /* Checking is init obj */
    if((NULL == osal->portable) ||
       (NULL == osal->portable->threadDelay))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;        // Exit: Error: init obj
    }

    /* Perform delay */
    UShellOsalErr_e osalStatus = osal->portable->threadDelay(osal,msDelay);

    return osalStatus;                  // Exit: no errors
}

/**
 * \brief Resume the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being resumed
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadResume(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle)
{
    /* Checking of params */
    if ((NULL == osal) ||
        (NULL == threadHandle))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Checking is init obj */
    if ((NULL == osal->portable) ||
        (NULL == osal->portable->threadResume))
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    /* Resume the thread */
    UShellOsalErr_e retStatus = osal->portable->threadResume(osal, threadHandle);

    return retStatus;
}
//============================================================================[PRIVATE FUNCTIONS]==================================================================================
