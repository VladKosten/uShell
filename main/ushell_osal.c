/**
 * \file        ushell_osal.c
 * @brief       UShell OSAL layer interface implementation.
 * \author      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright   Copyright (c) 2024 Vlad Kosten. All rights reserved.
 * \warning     A warning may be placed here...
 * \bug         Bug report may be placed here...
 */


//===============================================================================[ INCLUDE ]=======================================================================================

#include "ushell_osal.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//=====================================================================[ INTERNAL MACRO DEFENITIONS ]==============================================================================

/**
 * @brief UShell OSAL ASSERT macro definition.
 *
 * This macro is used to perform assertions in the UShell Operating System Abstraction Layer (OSAL).
 * If `USHELL_ASSERT` is defined, `USHELL_OSAL_ASSERT` will use it to perform the assertion.
 * Otherwise, `USHELL_OSAL_ASSERT` will be defined as an empty macro.
 *
 * @param[in] cond The condition to be asserted.
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
 * @brief Initialize UShell OSAL instance
 *        Sets name, parent, clears all internal objects etc.
 * @param[in] osal - pointer to OSAL instance
 * @param[in] name - pointer to the name of the OSAL instance
 * @param[in] parent - pointer to a parent object
 * @param[in] portable - pointer to the portable table
 * @param[out] none
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalInit(UShellOsal_s *osal,
                               const char *name,
                               void *const parent,
                               const UShellOsalPortable_s *portable)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Initialize the UShell OSAL object */
    do
    {
        /* Check input parameter */
        if(osal == NULL)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Clear the OSAL object */
        memset(osal, 0, sizeof(UShellOsal_s));

        /* Set the name */
        osal->name = name;

        /* Set the parent */
        osal->parent = parent;

        /* Set the portable structure */
        osal->portable = portable;

    } while (0);

    return status;
}


/**
 * @brief Deinitialize UShell OSAL instance
 * @note  Call this function when all functionality has been stopped
 * @param[in] osal - pointer to OSAL instance
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s *osal)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Deinitialize the UShell OSAL object */
    do
    {
        /* Check input parameter */
        if(osal == NULL)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Clear the OSAL object */
        memset(osal, 0, sizeof(UShellOsal_s));
    } while (0);

    return status;
}


/**
 * @brief Get pointer to a parent of the given OSAL object
 * @param[in]  osal      - pointer to osal instance which parent object will be returned
 * @param[out] parent    - pointer to an object into which the current osal parent pointer will be copied
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentGet(UShellOsal_s *const osal, void **const parent)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(parent != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the parent object */
    do
    {
        /* Check input parameter */
        if((osal == NULL) ||
           (parent == NULL))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the parent */
        *parent = osal->parent;

    } while (0);

    return status;
}


/**
 * @brief Set the parent object for the given OSAL instance
 * @param[in] osal      - pointer to osal instance being modified
 * @param[in] parent    - pointer to parent object being set
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s *const osal, void *const parent)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Set the parent object */
    do
    {
        /* Check input parameter */
        if((osal == NULL) ||
           (parent == NULL))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Set the parent */
        osal->parent = parent;

    } while (0);

    return status;
}


/**
 * @brief Get pointer to the name field of the given OSAL instance
 * @param[in] osal  - pointer to osal instance
 * @param[out] name  - pointer to an object into which the current osal name will be copied
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameGet(UShellOsal_s *const osal, const char **const name)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(name != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the name */
    do
    {
        /* Check input parameter */
        if((osal == NULL) ||
           (name == NULL))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the name */
        *name = osal->name;

    } while (0);

    return status;
}

/**
 * @brief Set name for the given OSAL instance
 * @param[in] osal  - pointer to osal instance being modified
 * @param[in] name  - pointer to name string being set
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s *const osal, char *const name)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Set the name */
    do
    {
        /* Check input parameter */
        if((osal == NULL) ||
           (name == NULL))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Set the name */
        osal->name = name;

    } while (0);

    return status;
}


/**
 * @brief Create the queue
 * @param[in]   osal          - OSAL descriptor;
 * @param[in]   queueItemSize - the size of the queue item
 * @param[in]   queueDepth    - the queue depth
 * @param[out]  queueHandle   - the queue handle was created
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueCreate(UShellOsal_s *const osal,
                                      const size_t queueItemSize,
                                      const size_t queueDepth,
                                      UShellOsalQueueHandle_t * const queueHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);
    USHELL_OSAL_ASSERT(queueItemSize > 0);
    USHELL_OSAL_ASSERT(queueDepth > 0);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the queue */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == queueHandle) ||
            (queueItemSize == 0) ||
            (queueDepth == 0))
        {
            return USHELL_OSAL_INVALID_ARGS;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueCreate))
        {
            return USHELL_OSAL_PORT_SPECIFIC_ERR;
        }

        /* Call the port specific function */
        status = osal->portable->queueCreate(osal, queueItemSize, queueDepth, queueHandle);

    } while (0);

    return status;

    return status;
}


/**
 * @brief Delete the queue
 * @param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * @param[in]   queueHandle   - the queue handle to delete
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueDelete(UShellOsal_s *const osal,
                                      const UShellOsalQueueHandle_t queueHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the queue */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == queueHandle))
        {
            return USHELL_OSAL_INVALID_ARGS;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueDelete))
        {
            return USHELL_OSAL_PORT_SPECIFIC_ERR;
        }

        /* Call the port specific function */
        status = osal->portable->queueDelete(osal, queueHandle);

    } while (0);

    return status;
}


/**
 * @brief Put item to the queue
 * @param[in] osal - OSAL descriptor;
 * @param[in] queueHandle   - the queue handle in which to put the item
 * @param[in] queueItemPtr  - pointer to the item source buffer
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemPut(UShellOsal_s *const osal,
                                       const UShellOsalQueueHandle_t queueHandle,
                                       const void *const queueItemPtr)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Put item to the queue */
    do
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

        /* Put item to the queue */
        status = osal->portable->queueItemPut(osal, queueHandle, queueItemPtr);

    }while (0);

    return status;
}


/**
 * @brief Put item to the queue
 * @note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * @param[in] osal - OSAL descriptor;
 * @param[in] queueHandle   - the queue handle in which to put the item
 * @param[in] queueItemPtr  - pointer to the item source buffer
 * @param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e ushellOsalQueueItemPost(UShellOsal_s *const osal,
                                        const UShellOsalQueueHandle_t queueHandle,
                                        void *const queueItemPtr,
                                        const UShellOsalTimeMs_t timeoutMs)
{
   /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);
    USHELL_OSAL_ASSERT(queueItemPtr != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Put item to the queue */
    do
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
            (NULL == osal->portable->queueItemPost))
        {
            return USHELL_OSAL_PORT_SPECIFIC_ERR;
        }

        /* Put item to the queue */
        status = osal->portable->queueItemPost(osal, queueHandle, queueItemPtr, timeoutMs);

    } while (0);

    return status;
}


/**
 * @brief Get item from the queue
 * @note  (NON-BLOCKING CALL)
 * @param[in]   osal - OSAL descriptor;
 * @param[in]   queueHandle   - the queue handle in which to put the item
 * @param[out]  queueItemPtr  - pointer to the destination buffer in which the item should be places
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemGet(UShellOsal_s *const osal,
                                       const UShellOsalQueueHandle_t queueHandle,
                                       void *const queueItemPtr)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);
    USHELL_OSAL_ASSERT(queueItemPtr != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get item from the queue */
    do
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
        status = osal->portable->queueItemGet(osal, queueHandle, queueItemPtr);

    } while (0);

    return status;
}


/**
 * @brief Get item from the queue
 * @note  (BLOCKING CALL WITH INFINITE WAIT)
 * @param[in]  osal - OSAL descriptor;
 * @param[in]  queueHandle   - the queue handle in which to put the item
 * @param[out] queueItemPtr  - pointer to the destination buffer in which the item should be places
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemWait(UShellOsal_s *const osal,
                                        const UShellOsalQueueHandle_t queueHandle,
                                        void *const queueItemPtr)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);
    USHELL_OSAL_ASSERT(queueItemPtr != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get item from the queue */
    do
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
        status = osal->portable->queueItemWait(osal, queueHandle, queueItemPtr);

    } while (0);

    return status;
}


/**
 * @brief Get item from the queue
 * @note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * @param[in]  UShellOsal_s* const osal - OSAL descriptor;
 * @param[in]  queueHandle   - the queue handle from which to get the item
 * @param[in]  queueItemPtr  - pointer to the destination buffer in which the item should be placed
 * @param[out] timeoutMs     - timeout in milliseconds to wait for the item
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e  ushellOsalQueueItemPend(UShellOsal_s *const osal,
                                         const UShellOsalQueueHandle_t queueHandle,
                                         void *const queueItemPtr,
                                         const UShellOsalTimeMs_t timeoutMs)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);
    USHELL_OSAL_ASSERT(queueItemPtr != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get item from the queue */
    do
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
        status = osal->portable->queueItemPend(osal, queueHandle, queueItemPtr, timeoutMs);

    } while (0);

    return status;
}


/**
 * @brief Reset queue
 * @param[in] osal          - pointer to OSAL instance
 * @param[in] queueHandle   - the queue handle to reset
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueReset(UShellOsal_s *const osal,
                                     const UShellOsalQueueHandle_t queueHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(queueHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Reset queue */
    do
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
        status = osal->portable->queueReset(osal, queueHandle);

    } while (0);

    return status;
}


/**
 * @brief Create the lock object
 * @param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * @param[out]  lockObjHandle - lock object handle that was created
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjCreate(UShellOsal_s *const osal, UShellOsalLockObjHandle_t * const lockObjHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(lockObjHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the lock object */
    do
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
        status = osal->portable->lockObjCreate(osal, lockObjHandle);

    } while (0);

    return status;
}

/**
 * @brief Delete the lock object
 * @param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * @param[in]   lockObjHandle - lock object handle to delete
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjDelete(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(lockObjHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the lock object */
    do
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
        status = osal->portable->lockObjDelete(osal, lockObjHandle);

    } while (0);

    return status;
}


/**
 * @brief Lock access to the resource for third-party collaborators
 * @param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * @param[in]   lockObjHandle - lock object handle
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLock(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(lockObjHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Lock access to the resource */
    do
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
        status = osal->portable->lock(osal, lockObjHandle);

    } while (0);

    return status;
}



/**
 * @brief Unlock access to the resource for third-party collaborators
 * @param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * @param[in]   lockObjHandle - lock object handle
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s *const osal,
                                 const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(lockObjHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Unlock access to the resource */
    do
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
        status = osal->portable->unlock(osal, lockObjHandle);

    } while (0);

    return status;
}


/**
 * @brief Create the semaphore object
 * @param[in] osal               - pointer to OSAL instance
 * @param[in] semaphoreCountMax  - the maximum count of the semaphore
 * @param[in] semaphoreInitValue - the initial value of the semaphore
 * @param[in] semaphoreHandle    - semaphore object handle that was created
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreCreate(UShellOsal_s *const osal,
                                          const UShellOsalSemaphoreCount_t semaphoreCountMax,
                                          const UShellOsalSemaphoreCount_t semaphoreInitValue,
                                          UShellOsalSemaphoreHandle_t *const semaphoreHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(semaphoreHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the semaphore object */
    do
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
        status = osal->portable->semaphoreCreate(osal, semaphoreCountMax, semaphoreInitValue, semaphoreHandle);

    } while (0);

    return status;
}


/**
 * @brief Delete the semaphore object
 * @param[in] osal              - pointer to OSAL instance
 * @param[in] semaphoreHandle   - semaphore object handle to delete
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreDelete(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(semaphoreHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the semaphore object */
    do
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
        status = osal->portable->semaphoreDelete(osal, semaphoreHandle);

    } while (0);

    return status;
}


/**
 * @brief Acquire the semaphore
 * @param[in] osal              - pointer to OSAL instance
 * @param[in] semaphoreHandle   - semaphore object handle to acquire
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreAcquire(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(semaphoreHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Acquire the semaphore */
    do
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
        status = osal->portable->semaphoreAcquire(osal, semaphoreHandle);

    } while (0);

    return status;
}


/**
 * @brief Release the semaphore
 * @param[in] osal              - pointer to OSAL instance
 * @param[in] semaphoreHandle   - semaphore object handle to release
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreRelease(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(semaphoreHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Release the semaphore */
    do
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
        status = osal->portable->semaphoreRelease(osal, semaphoreHandle);

    } while (0);

    return status;
}


/**
 * @brief Get the current count of the semaphore
 * @param[in] osal              - pointer to OSAL instance
 * @param[in] semaphoreHandle   - semaphore object handle
 * @param[in] semaphoreCount    - the current count of the semaphore
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreCountGet(UShellOsal_s *const osal,
                                            const UShellOsalSemaphoreHandle_t semaphoreHandle,
                                            UShellOsalSemaphoreCount_t *const semaphoreCount)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(semaphoreHandle != NULL);
    USHELL_OSAL_ASSERT(semaphoreCount != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the current count of the semaphore */
    do
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
        status = osal->portable->semaphoreCountGet(osal, semaphoreHandle, semaphoreCount);

    } while (0);

    return status;
}


/**
 * @brief Create the thread
 * @param[in]   osal - OSAL descriptor;
 * @param[out]  threadHandle  - thread handle by which created thread can be referenced
 * @param[in]   threadCfg     - thread configuration
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadCreate(UShellOsal_s *const osal,
                                       UShellOsalThreadHandle_t * const threadHandle,
                                       UShellOsalThreadCfg_s threadCfg)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(threadHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the thread */
    do
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
        status = osal->portable->threadCreate(osal, threadHandle, threadCfg);

    } while (0);

    return status;
}


/**
 * @brief Delete the thread
 * @note The operation must be stopped before deleting the thread
 *        to not to damage the system.
 * @param[in]   osal - OSAL descriptor;
 * @param[in]   threadHandle  - the handle of the thread being deleted
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadDelete(UShellOsal_s *const osal,
                                       const UShellOsalThreadHandle_t threadHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(threadHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the thread */
    do
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
        status = osal->portable->threadDelete(osal, threadHandle);

    } while (0);

    return status;
}

/**
 * @brief Suspend the thread
 * @param[in] osal - OSAL descriptor;
 * @param[in] threadHandle  - the handle of the thread being suspend
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadSuspend(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(threadHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Suspend the thread */
    do
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
        status = osal->portable->threadSuspend(osal, threadHandle);

    } while (0);

    return status;
}


/**
 * @brief       Perform some delay
 * @param[in]   osal - OSAL descriptor;
 * @param[in]   msDelay - the delay timeout value expressed in ms;
 * @param[out]  no;
 * @return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalThreadDelay(const UShellOsal_s* const osal, const UShellOsalTimeMs_t msDelay)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Perform some delay */
    do
    {
        /* Checking of params */
        if (NULL == osal)
        {
            return USHELL_OSAL_INVALID_ARGS;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->threadDelay))
        {
            return USHELL_OSAL_PORT_SPECIFIC_ERR;
        }

        /* Perform some delay */
        status = osal->portable->threadDelay(osal, msDelay);

    } while (0);

    return status;
}


/**
 * @brief Resume the thread
 * @param[in] osal - OSAL descriptor;
 * @param[in] threadHandle  - the handle of the thread being resumed
 * @return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadResume(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle)
{
    /* Checking of params */
    USHELL_OSAL_ASSERT(osal != NULL);
    USHELL_OSAL_ASSERT(threadHandle != NULL);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Resume the thread */
    do
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
        status = osal->portable->threadResume(osal, threadHandle);

    } while (0);

    return status;
}

//============================================================================[PRIVATE FUNCTIONS]==================================================================================
