/**
 * \file        ushell_osal.c
 * \brief       UShell OSAL layer interface implementation.
 * \author      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning     A warning may be placed here...
 * \bug         Bug report may be placed here...
 */

//===============================================================================[ INCLUDE ]=======================================================================================

#include "ushell_osal.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

//=====================================================================[ INTERNAL MACRO DEFENITIONS ]==============================================================================

/**
 * \brief UShell OSAL ASSERT macro definition.
 *
 * This macro is used to perform assertions in the UShell Operating System Abstraction Layer (OSAL).
 * If `USHELL_ASSERT` is defined, `USHELL_OSAL_ASSERT` will use it to perform the assertion.
 * Otherwise, `USHELL_OSAL_ASSERT` will be defined as an empty macro.
 *
 * \param[in] cond The condition to be asserted.
 */
#ifndef USHELL_OSAL_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_OSAL_ASSERT(cond) USHELL_ASSERT(cond)
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
 * \param[in] portable - pointer to the portable table
 * \param[out] none
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalInit(UShellOsal_s* osal,
                               const char* name,
                               void* const parent,
                               const UShellOsalPortable_s* portable)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Initialize the UShell OSAL object */
    do
    {
        /* Check input parameter */
        if (osal == NULL)
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
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

    /* Return the status */
    return status;
}

/**
 * \brief Deinitialize UShell OSAL instance
 * \note  Call this function when all functionality has been stopped
 * \param[in] osal - pointer to OSAL instance
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s* osal)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Deinitialize the UShell OSAL object */
    do
    {
        /* Check input parameter */
        if (osal == NULL)
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Clear the OSAL object */
        memset(osal, 0, sizeof(UShellOsal_s));

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get pointer to a parent of the given OSAL object
 * \param[in]  osal      - pointer to osal instance which parent object will be returned
 * \param[out] parent    - pointer to an object into which the current osal parent pointer will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentGet(UShellOsal_s* const osal, void** const parent)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the parent object */
    do
    {
        /* Check input parameter */
        if ((osal == NULL) ||
            (parent == NULL))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the parent */
        *parent = osal->parent;

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Set the parent object for the given OSAL instance
 * \param[in] osal      - pointer to osal instance being modified
 * \param[in] parent    - pointer to parent object being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s* const osal,
                                    void* const parent)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Set the parent object */
    do
    {
        /* Check input parameter */
        if ((osal == NULL) ||
            (parent == NULL))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Set the parent */
        osal->parent = parent;

    } while (0);

    return status;
}

/**
 * \brief Get pointer to the name field of the given OSAL instance
 * \param[in] osal  - pointer to osal instance
 * \param[out] name  - pointer to an object into which the current osal name will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameGet(UShellOsal_s* const osal, const char** const name)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the name */
    do
    {
        /* Check input parameter */
        if ((osal == NULL) ||
            (name == NULL))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the name */
        *name = osal->name;

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Set name for the given OSAL instance
 * \param[in] osal  - pointer to osal instance being modified
 * \param[in] name  - pointer to name string being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s* const osal, char* const name)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Set the name */
    do
    {
        /* Check input parameter */
        if ((osal == NULL) ||
            (name == NULL))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Set the name */
        osal->name = name;

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create the queue
 * \param[in]   osal          - OSAL descriptor;
 * \param[in]   queueItemSize - the size of the queue item
 * \param[in]   queueDepth    - the queue depth
 * \param[out]  queueHandle   - the queue handle was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueCreate(UShellOsal_s* const osal,
                                      const size_t queueItemSize,
                                      const size_t queueDepth,
                                      UShellOsalQueueHandle_t* const queueHandle)
{
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
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueCreate))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Call the port specific function */
        status = osal->portable->queueCreate(osal,
                                             queueItemSize,
                                             queueDepth,
                                             queueHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Delete the queue
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueDelete(UShellOsal_s* const osal,
                                      const UShellOsalQueueHandle_t queueHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the queue */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == queueHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueDelete))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Call the port specific function */
        status = osal->portable->queueDelete(osal, queueHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Put item to the queue
 * \param[in] osal - OSAL descriptor;
 * \param[in] queueHandle   - the queue handle in which to put the item
 * \param[in] queueItemPtr  - pointer to the item source buffer
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemPut(UShellOsal_s* const osal,
                                       const UShellOsalQueueHandle_t queueHandle,
                                       const void* const queueItemPtr)
{
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
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueItemPut))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Put item to the queue */
        status = osal->portable->queueItemPut(osal, queueHandle, queueItemPtr);

    } while (0);

    /* Return the status */
    return status;
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
UShellOsalErr_e ushellOsalQueueItemPost(UShellOsal_s* const osal,
                                        const UShellOsalQueueHandle_t queueHandle,
                                        void* const queueItemPtr,
                                        const UShellOsalTimeMs_t timeoutMs)
{
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
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueItemPost))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Put item to the queue */
        status = osal->portable->queueItemPost(osal,
                                               queueHandle,
                                               queueItemPtr,
                                               timeoutMs);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get item from the queue
 * \note  (NON-BLOCKING CALL)
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle in which to put the item
 * \param[out]  queueItemPtr  - pointer to the destination buffer in which the item should be places
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemGet(UShellOsal_s* const osal,
                                       const UShellOsalQueueHandle_t queueHandle,
                                       void* const queueItemPtr)
{
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
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueItemGet))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Get item from the queue */
        status = osal->portable->queueItemGet(osal, queueHandle, queueItemPtr);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get item from the queue
 * \note  (BLOCKING CALL WITH INFINITE WAIT)
 * \param[in]  osal - OSAL descriptor;
 * \param[in]  queueHandle   - the queue handle in which to put the item
 * \param[out] queueItemPtr  - pointer to the destination buffer in which the item should be places
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemWait(UShellOsal_s* const osal,
                                        const UShellOsalQueueHandle_t queueHandle,
                                        void* const queueItemPtr)
{
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
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueItemWait))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Get item from the queue */
        status = osal->portable->queueItemWait(osal,
                                               queueHandle,
                                               queueItemPtr);

    } while (0);

    /* Return the status */
    return status;
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
UShellOsalErr_e ushellOsalQueueItemPend(UShellOsal_s* const osal,
                                        const UShellOsalQueueHandle_t queueHandle,
                                        void* const queueItemPtr,
                                        const UShellOsalTimeMs_t timeoutMs)
{
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
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueItemPend))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Get item from the queue */
        status = osal->portable->queueItemPend(osal,
                                               queueHandle,
                                               queueItemPtr,
                                               timeoutMs);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Reset queue
 * \param[in] osal          - pointer to OSAL instance
 * \param[in] queueHandle   - the queue handle to reset
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueReset(UShellOsal_s* const osal,
                                     const UShellOsalQueueHandle_t queueHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Reset queue */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == queueHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->queueReset))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Reset queue */
        status = osal->portable->queueReset(osal, queueHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[out]  lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjCreate(UShellOsal_s* const osal,
                                        UShellOsalLockObjHandle_t* const lockObjHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the lock object */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == lockObjHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->lockObjCreate))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Create the lock object */
        status = osal->portable->lockObjCreate(osal, lockObjHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Delete the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjDelete(UShellOsal_s* const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the lock object */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == lockObjHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->lockObjDelete))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Delete the lock object */
        status = osal->portable->lockObjDelete(osal, lockObjHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Lock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLock(UShellOsal_s* const osal, const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Lock access to the resource */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == lockObjHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->lock))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Lock access to the resource */
        status = osal->portable->lock(osal, lockObjHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Unlock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s* const osal,
                                 const UShellOsalLockObjHandle_t lockObjHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Unlock access to the resource */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == lockObjHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->unlock))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Unlock access to the resource */
        status = osal->portable->unlock(osal, lockObjHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create the thread
 * \param[in]   osal - OSAL descriptor;
 * \param[out]  threadHandle  - thread handle by which created thread can be referenced
 * \param[in]   threadCfg     - thread configuration
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadCreate(UShellOsal_s* const osal,
                                       UShellOsalThreadHandle_t* const threadHandle,
                                       UShellOsalThreadCfg_s threadCfg)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the thread */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == threadHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->threadCreate))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Create the thread */
        status = osal->portable->threadCreate(osal,
                                              threadHandle,
                                              threadCfg);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Delete the thread
 * \note The operation must be stopped before deleting the thread
 *        to not to damage the system.
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   threadHandle  - the handle of the thread being deleted
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadDelete(UShellOsal_s* const osal,
                                       const UShellOsalThreadHandle_t threadHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the thread */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == threadHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->threadDelete))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Delete the thread */
        status = osal->portable->threadDelete(osal,
                                              threadHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Suspend the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being suspend
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadSuspend(UShellOsal_s* const osal,
                                        const UShellOsalThreadHandle_t threadHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Suspend the thread */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == threadHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->threadSuspend))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Suspend the thread */
        status = osal->portable->threadSuspend(osal, threadHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief       Perform some delay
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   msDelay - the delay timeout value expressed in ms;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalThreadDelay(const UShellOsal_s* const osal,
                                      const UShellOsalTimeMs_t msDelay)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Perform some delay */
    do
    {
        /* Checking of params */
        if (NULL == osal)
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->threadDelay))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Perform some delay */
        status = osal->portable->threadDelay(osal, msDelay);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Resume the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being resumed
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadResume(UShellOsal_s* const osal,
                                       const UShellOsalThreadHandle_t threadHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Resume the thread */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == threadHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->threadResume))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Resume the thread */
        status = osal->portable->threadResume(osal, threadHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create the stream buffer
 * \param osal              - pointer to OSAL instance
 * \param buffSizeBytes     - the size of the stream buffer in bytes
 * \param triggerLevelBytes - trigger level in bytes (watermark)
 * \param streamBuffHandle  - the stream buffer handle was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalStreamBuffCreate(UShellOsal_s* const osal,
                                           const size_t buffSizeBytes,
                                           const size_t triggerLevelBytes,
                                           UShellOsalStreamBuffHandle_t* const streamBuffHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (0 == buffSizeBytes) ||
            (0 == triggerLevelBytes) ||
            (triggerLevelBytes > buffSizeBytes))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffCreate))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Create the stream buffer */
        status = osal->portable->streamBuffCreate(osal,
                                                  buffSizeBytes,
                                                  triggerLevelBytes,
                                                  streamBuffHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Delete the stream buffer
 * \param osal              - pointer to OSAL instance
 * \param streamBuffHandle  - stream buffer handle being deleted
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalStreamBuffDelete(UShellOsal_s* const osal,
                                           const UShellOsalStreamBuffHandle_t streamBuffHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffDelete))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Delete the stream buffer */
        status = osal->portable->streamBuffDelete(osal, streamBuffHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Send data to the stream buffer
 * \param osal              - pointer to OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer to which a stream is being sent
 * \param txData            - pointer to the buffer that holds the bytes to be copied into the stream buffer
 * \param dataLengthBytes   - the size of the data in bytes
 * \param msToWait          - the maximum amount of time the task should remain in the blocked state to wait
 * \return the number of bytes written to the stream buffer.
 */
size_t UShellOsalStreamBuffSend(UShellOsal_s* const osal,
                                const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                const void* txData,
                                const size_t dataLengthBytes,
                                const uint32_t msToWait)
{
    /* Local variables */
    size_t bytesWritten = 0;

    /* Send data to the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (NULL == txData) ||
            (0 == dataLengthBytes))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesWritten = 0;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffSend))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesWritten = 0;
            break;
        }

        /* Send data to the stream buffer */
        bytesWritten = osal->portable->streamBuffSend(osal,
                                                      streamBuffHandle,
                                                      txData,
                                                      dataLengthBytes,
                                                      msToWait);

    } while (0);

    /* Return the number of bytes written */
    return bytesWritten;
}

/**
 * \brief Send data to a stream buffer (blocking, no timeout).
 *
 * This function writes data to the specified stream buffer, blocking until
 * all data has been written or an error occurs.
 *
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] streamBuffHandle Handle of the stream buffer.
 * \param[in] txData Pointer to the data to be sent.
 * \param[in] dataLengthBytes Length of the data to be sent in bytes.
 * \return Number of bytes actually written to the stream buffer.
 */
size_t UShellOsalStreamBuffSendBlocking(UShellOsal_s* const osal,
                                        const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                        const void* txData,
                                        const size_t dataLengthBytes)
{
    /* Local variables */
    size_t bytesWritten = 0;

    /* Send data to the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (NULL == txData) ||
            (0 == dataLengthBytes))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesWritten = 0;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffSendBlocking))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesWritten = 0;
            break;
        }

        /* Send data to the stream buffer */
        bytesWritten = osal->portable->streamBuffSendBlocking(osal,
                                                              streamBuffHandle,
                                                              txData,
                                                              dataLengthBytes);

    } while (0);

    /* Return the number of bytes written */
    return bytesWritten;
}

/**
 * \brief Receive data from the stream buffer
 * \param osal              - pointer to OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer from which bytes are to be received
 * \param rxData            - pointer to the buffer into which the received bytes will be copied
 * \param dataLengthBytes   - the size of the data buffer pointed to by rxData parameter in bytes
 * \param msToWait          - the maximum amount of time the task should remain in the blocked state to wait
 *                            for data to become available if the stream buffer is empty
 * \return the number of bytes read from the stream buffer.
 */
size_t UShellOsalStreamBuffReceive(UShellOsal_s* const osal,
                                   const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                   void* const rxData,
                                   const size_t dataLengthBytes,
                                   const uint32_t msToWait)
{

    /* Local variables */
    size_t bytesRead = 0;

    /* Receive data from the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (NULL == rxData) ||
            (0 == dataLengthBytes))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesRead = 0;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffReceive))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesRead = 0;
            break;
        }

        /* Receive data from the stream buffer */
        bytesRead = osal->portable->streamBuffReceive(osal,
                                                      streamBuffHandle,
                                                      rxData,
                                                      dataLengthBytes,
                                                      msToWait);

    } while (0);

    /* Return the number of bytes read */
    return bytesRead;
}

/**
 * \brief Receive data from a stream buffer.
 *
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] streamBuffHandle Handle of the stream buffer.
 * \param[out] rxData Pointer to the buffer to store the received data.
 * \param[in] dataLengthBytes Length of the data to be received in bytes.
 * \param[in] msToWait Timeout in milliseconds.
 * \return Number of bytes received.
 *
 */
size_t UShellOsalStreamBuffReceiveBlocking(UShellOsal_s* const osal,
                                           const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                           void* const rxData,
                                           const size_t dataLengthBytes)
{
    /* Local variables */
    size_t bytesRead = 0;

    /* Receive data from the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (NULL == rxData) ||
            (0 == dataLengthBytes))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesRead = 0;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffReceiveBlocking))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            bytesRead = 0;
            break;
        }

        /* Receive data from the stream buffer */
        bytesRead = osal->portable->streamBuffReceiveBlocking(osal,
                                                              streamBuffHandle,
                                                              rxData,
                                                              dataLengthBytes);

    } while (0);

    /* Return the number of bytes read */
    return bytesRead;
}

/**
 * \brief Reset a stream buffer to its initial empty state
 * \param osal              - pointer to OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer being reset
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalStreamBuffReset(UShellOsal_s* const osal,
                                          const UShellOsalStreamBuffHandle_t streamBuffHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Reset the stream buffer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffReset))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Reset the stream buffer */
        status = osal->portable->streamBuffReset(osal, streamBuffHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Stream buffer is empty.
 *
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] streamBuffHandle Handle of the stream buffer.
 * \param[out] isEmpty Pointer to store the result indicating if the stream buffer is empty.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellOsalStreamBuffIsEmpty(UShellOsal_s* const osal,
                                            const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                            bool* const isEmpty)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Check if the stream buffer is empty */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (NULL == isEmpty))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->streamBuffIsEmpty))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check if the stream buffer is empty */
        status = osal->portable->streamBuffIsEmpty(osal,
                                                   streamBuffHandle,
                                                   isEmpty);
    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - timer handle by which created timer can be referenced
 * \param[in] timerCfg - timer configuration
 * \param[out] UShellOsalErr_e - error code. non-zero = an error has occurred;
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerCreate(UShellOsal_s* const osal,
                                      UShellOsalTimerHandle_t* const timerHandle,
                                      UShellOsalTimerCfg_s timerCfg)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the timer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle) ||
            (timerCfg.timerExpiredCb == NULL))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timerCreate))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Create the timer */
        status = osal->portable->timerCreate(osal,
                                             timerHandle,
                                             timerCfg);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Delete the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - the handle of the timer being deleted
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerDelete(UShellOsal_s* const osal,
                                      const UShellOsalTimerHandle_t timerHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the timer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timerDelete))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Delete the timer */
        status = osal->portable->timerDelete(osal, timerHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Start the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to start
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerStart(UShellOsal_s* const osal,
                                     const UShellOsalTimerHandle_t timerHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Start the timer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timerStart))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Start the timer */
        status = osal->portable->timerStart(osal, timerHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Stop the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to stop
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerStop(UShellOsal_s* const osal, const UShellOsalTimerHandle_t timerHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Stop the timer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timerStop))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Stop the timer */
        status = osal->portable->timerStop(osal, timerHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Reset the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to reset
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerReset(UShellOsal_s* const osal, const UShellOsalTimerHandle_t timerHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Reset the timer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timerReset))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Reset the timer */
        status = osal->portable->timerReset(osal, timerHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get a timer handle of the given OSAL object
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerSlotInd - index of timer slots
 * \param[in] timerHandle - pointer to an object into which the timer handle will be copied
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerHandleGet(UShellOsal_s* const osal,
                                         const size_t timerSlotInd,
                                         UShellOsalTimerHandle_t* const timerHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the timer handle */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle) ||
            (USHELL_OSAL_TIMER_NUM <= timerSlotInd))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the timer handle */
        *timerHandle = osal->timerObj [timerSlotInd].timerHandle;

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Change the period of the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to change the period
 * \param[in] periodMs - the new period in milliseconds
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerPeriodChange(UShellOsal_s* const osal,
                                            const UShellOsalTimerHandle_t timerHandle,
                                            const UShellOsalTimeMs_t periodMs)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Change the period of the timer */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timerHandle) ||
            (0 == periodMs))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timerPeriodChange))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Change the period of the timer */
        status = osal->portable->timerPeriodChange(osal, timerHandle, periodMs);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create an event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupCreate(UShellOsal_s* const osal,
                                       UShellOsalEventGroupHandle_t* const eventGroupHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Create the event group */
    do
    {
        /* Check input parameter */
        if ((NULL == osal) ||
            (NULL == eventGroupHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the OSAL object is initialized */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->eventGroupCreate))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Create the event group */
        status = osal->portable->eventGroupCreate(osal, eventGroupHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Create an event group with a specific name.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \param[in] name Name of the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupDelete(UShellOsal_s* const osal,
                                       const UShellOsalEventGroupHandle_t eventGroupHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Delete the event group */
    do
    {
        /* Check input parameter */
        if (NULL == osal)
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the OSAL object is initialized */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->eventGroupDelete))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Delete the event group */
        status = osal->portable->eventGroupDelete(osal, eventGroupHandle);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Set a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToSet Bits to set in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupSetBits(UShellOsal_s* const osal,
                                        const UShellOsalEventGroupHandle_t eventGroupHandle,
                                        const UShellOsalEventGroupBits_e bitsToSet)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Set the bits in the event group */
    do
    {
        /* Check input parameter */
        if ((NULL == osal) ||
            (NULL == eventGroupHandle))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the OSAL object is initialized */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->eventGroupSetBits))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Set the bits in the event group */
        status = osal->portable->eventGroupSetBits(osal, eventGroupHandle, bitsToSet);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Wait for bits in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToWait Bits to wait for in the event group.
 * \param[out] bitsReceived Pointer to store the received bits.
 * \param[in] clearOnExit Flag indicating whether to clear the bits on exit.
 * \param[in] waitAllBits Flag indicating whether to wait for all bits or any bit.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupBitsWait(UShellOsal_s* const osal,
                                         const UShellOsalEventGroupHandle_t eventGroupHandle,
                                         const UShellOsalEventGroupBits_e bitsToWait,
                                         UShellOsalEventGroupBits_e* const bitsReceived,
                                         const bool clearOnExit,
                                         const bool waitAllBits)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Wait for bits in the event group */
    do
    {
        /* Check input parameter */
        if ((NULL == osal) ||
            (NULL == eventGroupHandle) ||
            (NULL == bitsReceived))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the OSAL object is initialized */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->eventGroupBitsWait))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Wait for bits in the event group */
        status = osal->portable->eventGroupBitsWait(osal,
                                                    eventGroupHandle,
                                                    bitsToWait,
                                                    bitsReceived,
                                                    clearOnExit,
                                                    waitAllBits);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get the active bits in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[out] bitsActive Pointer to store the active bits.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupBitsActiveGet(UShellOsal_s* const osal,
                                              const UShellOsalEventGroupHandle_t eventGroupHandle,
                                              UShellOsalEventGroupBits_e* const bitsActive)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the active bits in the event group */
    do
    {
        /* Check input parameter */
        if ((NULL == osal) ||
            (NULL == eventGroupHandle) ||
            (NULL == bitsActive))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the OSAL object is initialized */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->eventGroupBitsActiveGet))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Get the active bits in the event group */
        status = osal->portable->eventGroupBitsActiveGet(osal, eventGroupHandle, bitsActive);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * @brief Get time in milliseconds
 * @param[in] osal - pointer to OSAL instance
 * @param[in] timeMs - pointer to an object into which the current time in milliseconds will be copied
 * @return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimeMsGet(UShellOsal_s* const osal,
                                    UShellOsalTimeMs_t* const timeMs)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the time in milliseconds */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == timeMs))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Checking is init obj */
        if ((NULL == osal->portable) ||
            (NULL == osal->portable->timeMsGet))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Get the time in milliseconds */
        status = osal->portable->timeMsGet(osal, timeMs);

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get an event group handle of the given OSAL object
 * \param[in] osal - pointer to OSAL instance
 * \param[in] eventGroupSlotInd - index of event group slots
 * \param[in] eventGroupHandle - pointer to an object into which the event group handle will be copied
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalEventGroupHandleGet(UShellOsal_s* const osal,
                                              const size_t eventGroupSlotInd,
                                              UShellOsalEventGroupHandle_t* const eventGroupHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the event group handle */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == eventGroupHandle) ||
            (USHELL_OSAL_EVENT_GROUPS_NUM <= eventGroupSlotInd))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the event group handle */
        *eventGroupHandle = osal->eventGroupHandle [eventGroupSlotInd];

    } while (0);

    return status;
}

/**
 * \brief Get a queue handle of the given OSAL object
 * \param[in] osal          - pointer to OSAL instance
 * \param[in] queueSlotInd  - index of queue slot
 * \param[out] queuehandle  - pointer to an object into which the current queue handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalQueueHandleGet(UShellOsal_s* const osal,
                                         const size_t queueSlotInd,
                                         UShellOsalQueueHandle_t* const queueHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the queue handle */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == queueHandle) ||
            (USHELL_OSAL_QUEUE_SLOTS_NUM <= queueSlotInd))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the queue handle */
        *queueHandle = osal->queueHandle [queueSlotInd];

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get a lockobj handle of the given OSAL object
 * \param[in]   osal            - pointer to OSAL instance
 * \param[in]   lockObjSlotInd  - index of lockobj slots
 * \param[out]  lockObjhandle   - pointer to an object into which the lockobj handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalLockObjHandleGet(UShellOsal_s* const osal,
                                           const size_t lockObjSlotInd,
                                           UShellOsalLockObjHandle_t* const lockObjHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the lock object handle */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == lockObjHandle) ||
            (USHELL_OSAL_LOCK_OBJS_NUM <= lockObjSlotInd))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the lock object handle */
        *lockObjHandle = osal->lockObjHandle [lockObjSlotInd];

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get a thread handle of the given OSAL object
 * \param[in]   osal            - pointer to OSAL instance
 * \param[in]   threadSlotInd   - index of thread slots
 * \param[out]  threadHandle	- pointer to an object into which the thread handle handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadHandleGet(UShellOsal_s* const osal,
                                          const size_t threadSlotInd,
                                          UShellOsalThreadHandle_t* const threadHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the thread handle */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == threadHandle) ||
            (USHELL_OSAL_THREADS_NUM <= threadSlotInd))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the thread handle */
        *threadHandle = osal->threadObj [threadSlotInd].threadHandle;

    } while (0);

    /* Return the status */
    return status;
}

/**
 * \brief Get a streambuff handle of the given OSAL object
 * \param[in]   osal          	    - pointer to OSAL instance
 * \param[in]   streamBuffSlotInd   - index of streambuff slots
 * \param[out]  streamBuffHandle    - pointer to an object into which the streambuff handle handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalStreamBuffHandleGet(UShellOsal_s* const osal,
                                              const size_t streamBuffSlotInd,
                                              UShellOsalStreamBuffHandle_t* const streamBuffHandle)
{
    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Get the stream buffer handle */
    do
    {
        /* Checking of params */
        if ((NULL == osal) ||
            (NULL == streamBuffHandle) ||
            (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM <= streamBuffSlotInd))
        {
            /* Invalid input parameter */
            USHELL_OSAL_ASSERT(0);
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Get the stream buffer handle */
        *streamBuffHandle = osal->streamBuffHandle [streamBuffSlotInd];

    } while (0);

    /* Return the status */
    return status;
}

//============================================================================[PRIVATE FUNCTIONS]==================================================================================
