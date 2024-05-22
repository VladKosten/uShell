/**
 * \file       ushell_osal_freertos.c
 * \brief      UShell OSAL FreeRTOS portable layer
 * \author     Vladislav Kosten (vladkosten@gmail.com)
 * \copyright  Copyright (c) 2024 Vlad Kosten. All rights reserved.
 * \warning    A warning may be placed here...
 * \bug        Bug report may be placed here...
 */


//===============================================================================[ INCLUDE ]=======================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Port includes */
#include "ushell_osal_freertos.h"
#include <task.h>
#include <semphr.h>
#include <queue.h>



//=====================================================================[ INTERNAL MACRO DEFENITIONS ]==============================================================================

/**
* \brief UShell OSAL ASSERT macro definition
*/
#ifndef USHELL_OSAL_FREERTOS_ASSERT
    #ifdef USHELL_OSAL_ASSERT
        #define USHELL_OSAL_FREERTOS_ASSERT(cond)  USHELL_OSAL_ASSERT(cond)
    #else
        #define USHELL_OSAL_FREERTOS_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]==========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]====================================================================

/**
* \brief Handle for critical section
 */
static SemaphoreHandle_t ushellOsalFreertosCriticalSectionHandle = NULL;

/**
 * \brief Create the queue
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueItemSize - the size of the queue item
 * \param[in]   queueDepth    - queue depth
 * \param[out]  queueHandle   - queue handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueCreate(void * const osalFreertos, const size_t queueItemSize,
                                                                                const size_t queueDepth,
                                                                                UShellOsalQueueHandle_t * const queueHandle);

/**
 * \brief Delete the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - queue handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueDelete(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Put the item to the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be put
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPut(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle,
                                                                                 const void *const queueItemPtr);

/**
 * \brief Put the item to the queue with specified waiting time
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be posted.
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPost(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle,
                                                                                  void * const queueItemPtr,
                                                                                  const uint32_t timeoutMs);

/**
 * \brief Get the item from the queue [NON-BLOCKING CALL]
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * \param[out]  queueItemPtr  - tointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemGet(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle,
                                                                                 void *const queueItemPtr);

/**
 * \brief Get the item from the queue [BLOCKING CALL]
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * \param[out]  queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemWait(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle,
                                                                                  void *const queueItemPtr);

/**
 * \brief Get the item from the queue with specified waiting time
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * \param[out]  queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \param[in]   timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPend(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle,
                                                                                  void * const queueItemPtr,
                                                                                  const uint32_t timeoutMs);

/**
 * \brief  Reset the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue to be reset
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueReset(void * const osalFreertos, const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Create the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[out] lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosLockObjCreate(void * const osalFreertos, UShellOsalLockObjHandle_t * const lockObjHandle);

/**
 * \brief Delete the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosLockObjDelete(void * const osalFreertos, const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Lock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosLock(void * const osalFreertos, const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Unlock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosUnlock(void * const osalFreertos, const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Create a counting semaphore
 * \param[in] osalFreertos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreCountMax - the maximum count of the semaphore
 * \param[in] semaphoreInitVal  - the initial value of the semaphore
 * \param[out] semaphoreHandle  - semaphore handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreCreate(void * const osalFreertos, const UShellOsalSemaphoreCount_t semaphoreCountMax,
                                                                                    const UShellOsalSemaphoreCount_t semaphoreInitVal,
                                                                                    UShellOsalSemaphoreHandle_t *const semaphoreHandle);

/**
 * \brief Delete a semaphore
 * \param[in] osalFreertos     - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle  - semaphore handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreDelete(void * const osalFreertos, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Acquire a semaphore
 * \param[in] osalFreertos     - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle  - semaphore handle to be acquired
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreAcquire(void * const osalFreertos, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Release a semaphore
 * \param[in] osalFreertos     - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle  - semaphore handle to be released
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreRelease(void * const osalFreertos, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Get the count of a semaphore
 * \param[in] osalFreertos     - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle  - semaphore handle to get the count
 * \param[out] semaphoreCount  - the count of the semaphore
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreCountGet(void * const osalFreertos, const UShellOsalSemaphoreHandle_t semaphoreHandle,
                                                                                      UShellOsalSemaphoreCount_t *const semaphoreCount);

/**
 * \brief Create the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[out] threadHandle - thread handle that was created
 * \param[in] threadCfg     - thread configuration structure
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadCreate(void * const osalFreertos, UShellOsalThreadHandle_t * const threadHandle,
                                                                                 UShellOsalThreadCfg_s threadCfg);

/**
 * \brief Delete the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelete(void * const osalFreertos, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Suspend the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle to be suspended
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadSuspend(void * const osalFreertos, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Resume the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle to be resumed
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadResume(void * const osalFreertos, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief delay the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] msDelay       - delay in milliseconds
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelay(const void * const osalFreertos, const uint32_t msDelay);

/**
 * \brief Find the queue handle in the queue handles table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] queueHandle  - queue handle to be found
 * \return uint16_t - index of the queue handle in the table
 */
static inline uint16_t ushellOsalFreertosFindQueueHandle(UShellOsalFreertos_s * const osalFreeRtos, const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Find the lock object handle in the lock objects table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle to be found
 * \return uint16_t - index of the lock object handle in the table
 */
static inline uint16_t ushellOsalFreertosFindLockObjHandle(UShellOsalFreertos_s * const osalFreeRtos, const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Find the semaphore handle in the semaphores table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle - semaphore handle to be found
 * \return uint16_t - index of the semaphore handle in the table
 */
static inline uint16_t ushellOsalFreertosFindSemaphoreHandle(UShellOsalFreertos_s * const osalFreeRtos, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Find the thread handle in the thread objects table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] threadHandle - thread handle to be found
 * \return uint16_t - index of the thread handle in the table
 */
static inline uint16_t ushellOsalFreertosFindThreadHandle(UShellOsalFreertos_s * const osalFreeRtos, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Perform thread parameters validation procedure
 *        in terms of the requirements of the FreeRTOS
 * \param[in] threadCfg - pointer to the thread configuration structure
 * \return bool - true if the parameters are valid, false otherwise
 */
static bool ushellOsalFreertosCheckParam(const UShellOsalThreadCfg_s *const threadCfg);

/**
* \brief Enter critical section
* \param[in] osalFreertos - pointer to FreeRTOS osal instance
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
*/
static UShellOsalErr_e ushellOsalFreertosCriticalSectionEnter(void *const osal);

/**
* \brief Exit critical section
* \param[in] osalFreertos - pointer to FreeRTOS osal instance
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
*/
static UShellOsalErr_e ushellOsalFreertosCriticalSectionExit(void *const osal);

/**
 * \brief FreeRTOS priority levels hash-table
 */
static const UBaseType_t UShellOsalFreertosThreadPriority[USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE] =
{
    USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW,
    USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE,
    USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH,
    USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA
};

/**
 * \brief UShell FreeRTOS OSAL portable structure
 */
static const UShellOsalPortable_s FreeRtosPortable =
{
    .queueCreate                = ushellOsalFreertosQueueCreate,
    .queueDelete                = ushellOsalFreertosQueueDelete,
    .queueItemPut               = ushellOsalFreertosQueueItemPut,
    .queueItemPost              = ushellOsalFreertosQueueItemPost,
    .queueItemGet               = ushellOsalFreertosQueueItemGet,
    .queueItemWait              = ushellOsalFreertosQueueItemWait,
    .queueItemPend              = ushellOsalFreertosQueueItemPend,
    .queueReset                 = ushellOsalFreertosQueueReset,
    .lockObjCreate              = ushellOsalFreertosLockObjCreate,
    .lockObjDelete              = ushellOsalFreertosLockObjDelete,
    .lock                       = ushellOsalFreertosLock,
    .unlock                     = ushellOsalFreertosUnlock,
    .semaphoreCreate            = ushellOsalFreertosSemaphoreCreate,
    .semaphoreDelete            = ushellOsalFreertosSemaphoreDelete,
    .semaphoreAcquire           = ushellOsalFreertosSemaphoreAcquire,
    .semaphoreRelease           = ushellOsalFreertosSemaphoreRelease,
    .semaphoreCountGet          = ushellOsalFreertosSemaphoreCountGet,
    .threadCreate               = ushellOsalFreertosThreadCreate,
    .threadDelete               = ushellOsalFreertosThreadDelete,
    .threadSuspend              = ushellOsalFreertosThreadSuspend,
    .threadResume               = ushellOsalFreertosThreadResume,
    .threadDelay                = ushellOsalFreertosThreadDelay,
    .criticalSectionEnter       = ushellOsalFreertosCriticalSectionEnter,
    .criticalSectionExit        = ushellOsalFreertosCriticalSectionExit,
};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]==============================================================================

/**
 * \brief Initialize the UShell FreeRTOS osal instance
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] name         - pointer to the name of the OSAL instance [optional]
 * \param[in] parent       - pointer to a parent object [optional]
 * \param[in] param        - pointer to FreeRTOS param structure [optional]
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosInit(UShellOsalFreertos_s *const osalFreertos, const char *name, void *const parent,
                                                                                 const UShellOsalFreertosParam_s *const param)
{
    if (NULL == osalFreertos)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    // TODO: perform parameters validation
    // ... assign if OK
    osalFreertos->param = *param;

    UShellOsal_s *osal         = (UShellOsal_s *)osalFreertos;
    UShellOsalErr_e initStatus = UShellOsalInit(osal, name, parent);
    if (USHELL_OSAL_NO_ERR != initStatus)
    {
        return initStatus;  // Exit: error during init procedure
    }

    // Assign FreeRTOS specific portable structure
    osal->portable = &FreeRtosPortable;

    // Create an emulation of the critical section
    if(NULL == ushellOsalFreertosCriticalSectionHandle)
    {
        ushellOsalFreertosCriticalSectionHandle = xSemaphoreCreateRecursiveMutex();
        if(NULL == ushellOsalFreertosCriticalSectionHandle)
        {
            return USHELL_OSAL_SEMAPHORE_MEM_ALLOC_ERR;
        }
    }

    return USHELL_OSAL_NO_ERR;
}

/**
 * \brief Deinitialize UShell FreeRTOS OSAL instance
 * \param[in] osalFreertos - pointer to osal FreeRTOS instance
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosDeinit(void *const osalFreertos)
{
    if ((NULL == osalFreertos))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    // Cast to the corresponding type
    UShellOsalFreertos_s *osal = (UShellOsalFreertos_s *)osalFreertos;

    // Suspend all tasks
    for (int i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
    {
        if (NULL != osal->base.threadObj[i].threadHandle)
        {
            vTaskSuspend((TaskHandle_t)osal->base.threadObj[i].threadHandle);
        }
    }

    // Delete all queues
    for (int i = 0; i < USHELL_OSAL_QUEUE_SLOTS_NUM; i++)
    {
        if (NULL != osal->base.queueHandle[i])
        {
            vQueueDelete((QueueHandle_t)osal->base.queueHandle[i]);
            osal->base.queueHandle[i] = NULL;
        }
    }

    // Delete all lock objects
    for (int i = 0; i < USHELL_OSAL_LOCK_OBJS_NUM; i++)
    {
        if (NULL != osal->base.lockObjHandle[i])
        {
            vSemaphoreDelete(osal->base.lockObjHandle[i]);
            osal->base.lockObjHandle[i] = NULL;
        }
    }

    // Delete all semaphores
    for (int i = 0; i < USHELL_OSAL_SEMAPHORE_OBJS_NUM; i++)
    {
        if (NULL != osal->base.semaphoreHandle[i])
        {
            vSemaphoreDelete(osal->base.semaphoreHandle[i]);
            osal->base.semaphoreHandle[i] = NULL;
        }
    }

    // Delete all tasks
    for (int i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
    {
        if (NULL != osal->base.threadObj[i].threadHandle)
        {
            vTaskDelete((TaskHandle_t)osal->base.threadObj[i].threadHandle);
            osal->base.threadObj[i].threadHandle             = NULL;
            osal->base.threadObj[i].threadCfg.name           = NULL;
            osal->base.threadObj[i].threadCfg.threadWorker   = NULL;
            osal->base.threadObj[i].threadCfg.stackSize      = 0;
            osal->base.threadObj[i].threadCfg.threadParam    = NULL;
            osal->base.threadObj[i].threadCfg.threadPriority = 0;
        }
    }

    // Deinitialize the base instance
    UShellOsalErr_e deinitStatus = UShellOsalDeinit(&osal->base);

    return deinitStatus;
}

//============================================================================[PRIVATE FUNCTIONS]==================================================================================

/**
 * \brief Create the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueItemSize - the size of the queue item
 * \param[in] queueDepth    - queue depth
 * \param[out] queueHandle  - queue handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueCreate(void * const osalFreertos, const size_t queueItemSize,
                                                                                const size_t queueDepth,
                                                                                UShellOsalQueueHandle_t * const queueHandle)
{
    // Check income params with assertions because
    // they must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;
    *queueHandle = NULL;  // Clear stored value

    // Create the FreeRTOS queue:
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_QUEUE_SLOTS_NUM; i++)
    {
        if (NULL == osal->queueHandle[i])
        {
            // Congratulations!
            // The free slot was found, create the queue
            osal->queueHandle[i] = xQueueCreate(queueDepth, queueItemSize);
            if (NULL == osal->queueHandle[i])
            {
                // Queue was not created and must not be used
                // Exit: error - memory required to create the queue could not be allocated
                return USHELL_OSAL_QUEUE_MEM_ALLOCATION_ERR;
            }

            // We can only get there if the queue was created
            // No additional checks needed
            *queueHandle = osal->queueHandle[i];
            // So break the loop
            break;
        }
    }

    // 2. Check if the queue was created
    if (NULL == *queueHandle)
    {
        return USHELL_OSAL_QUEUE_CREATE_ERR;  // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}

/**
 * \brief Delete the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - queue handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueDelete(void * const osalFreertos,
                                                            const UShellOsalQueueHandle_t queueHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Finally delete the queue
    vQueueDelete((QueueHandle_t)queueHandle);
    // And clear the queue slot in the table
    osal->queueHandle[queueIndexNum - 1] = NULL;  // Subtract 1 because find function increases actual index by 1

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Put the item to the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be put
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPut(void * const osalFreertos,
                                                                  const UShellOsalQueueHandle_t queueHandle,
                                                                  const void *const queueItemPtr)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    BaseType_t sendStatus = pdFALSE;
    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Put the item to the queue from ISR
        sendStatus = xQueueSendFromISR((QueueHandle_t)queueHandle, (const void *)queueItemPtr,
                                       &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    // Put the item to the queue
    else
    {
        sendStatus = xQueueSend((QueueHandle_t)queueHandle, (const void *)queueItemPtr,
                                pdMS_TO_TICKS(0));   // without waiting time
    }

    if (pdTRUE != sendStatus)
    {
        return USHELL_OSAL_QUEUE_IS_EMPTY_ERR;  // Exit: error - the queue is empty
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Put the item to the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be posted.
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPost(void * const osalFreertos,
                                                                  const UShellOsalQueueHandle_t queueHandle,
                                                                  void * const queueItemPtr,
                                                                  const uint32_t timeoutMs)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Safely convert timeout in ms to FreeRTOS ticks
    TickType_t safeTimeoutInTicks = ushellOsalFreertosSafeTimeoutToTicks(timeoutMs);

    // Put the item to the queue
    BaseType_t sendStatus = xQueueSend((QueueHandle_t)queueHandle, (const void *)queueItemPtr,
                                       safeTimeoutInTicks);   // with waiting time specified
    if (sendStatus != pdTRUE)
    {
        return USHELL_OSAL_QUEUE_OVERFLOW_ERR;  // Exit: error - no free space in the queue
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Get the item from the queue [BLOCKING CALL]
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * \param[in] queueItemPtr  - tointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemGet(void * const osalFreertos,
                                                                   const UShellOsalQueueHandle_t queueHandle,
                                                                   void *const queueItemPtr)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    BaseType_t receiveStatus = pdFALSE;
    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Get the item from the queue from ISR level
        receiveStatus =  xQueueReceiveFromISR((QueueHandle_t)queueHandle, (void *)queueItemPtr,
                                                &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    // Get the item from the queue
    else
    {
    receiveStatus = xQueueReceive((QueueHandle_t)queueHandle, (void *)queueItemPtr,
                                    pdMS_TO_TICKS(0));  // non-blocking call
    }

    if (pdTRUE != receiveStatus)
    {
        return USHELL_OSAL_QUEUE_IS_EMPTY_ERR;  // Exit: error - the queue is empty
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}

/**
 * \brief Get the item from the queue [BLOCKING CALL]
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * \param[in] queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemWait(void * const osalFreertos,
                                                                   const UShellOsalQueueHandle_t queueHandle,
                                                                   void *const queueItemPtr)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Get the item from the FreeRTOS queue
    // The task will blocked indefinitely (without a timeout) until an item arrived
    BaseType_t receiveStatus = xQueueReceive((QueueHandle_t)queueHandle, (void *)queueItemPtr,
                                             portMAX_DELAY);
    if (receiveStatus != pdTRUE)
    {
        // The only way we can get there if INCLUDE_vTaskSuspend is set to '0'
        USHELL_OSAL_FREERTOS_ASSERT(0);

        return USHELL_OSAL_QUEUE_IS_EMPTY_ERR;  // Exit: error - the queue is empty
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Get the item from the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * \param[in] queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPend(void * const osalFreertos,
                                                                   const UShellOsalQueueHandle_t queueHandle,
                                                                   void * const queueItemPtr,
                                                                   const uint32_t timeoutMs)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Safely convert timeout in ms to FreeRTOS ticks
    TickType_t safeTimeoutInTicks = ushellOsalFreertosSafeTimeoutToTicks(timeoutMs);

    // Get the item from the FreeRTOS queue
    BaseType_t receiveStatus = xQueueReceive((QueueHandle_t)queueHandle, (void *)queueItemPtr,
                                             safeTimeoutInTicks); // with waiting time specified
    if (receiveStatus != pdTRUE)
    {
        return USHELL_OSAL_QUEUE_IS_EMPTY_ERR;  // Exit: error - the queue is empty
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Reset the queue
 * \param[in][in]  osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in][in]  queueHandle   - the handle the queue to reset .
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueReset(void * const osalFreertos,
                                                                const UShellOsalQueueHandle_t queueHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
    if (0 == queueIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Reset FreeRTOS queue
    BaseType_t receiveStatus = xQueueReset((QueueHandle_t)queueHandle);  // non-blocking call
    if (receiveStatus != pdTRUE)
    {
        return USHELL_OSAL_QUEUE_IS_EMPTY_ERR;  // Exit: error - the queue hasn't cleared.
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}

/**
* \brief Enter critical section
* \param[in][in] osalFreertos - pointer to FreeRTOS osal instance
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
*/
static UShellOsalErr_e ushellOsalFreertosCriticalSectionEnter(void *const osal)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    if(ushellOsalFreertosCriticalSectionHandle == NULL)
    {
        return USHELL_OSAL_SEMAPHORE_OBJ_CREATE_ERR;
    }

    BaseType_t status = pdFALSE;
    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    // Acquire the semaphore
    USHELL_OSAL_FREERTOS_ASSERT(NULL != ushellOsalFreertosCriticalSectionHandle);
    status = xSemaphoreTakeRecursive(ushellOsalFreertosCriticalSectionHandle, portMAX_DELAY);
    if (pdTRUE != status)
    {
        return USHELL_OSAL_SEMAPHORE_ACQUIRE_ERR;
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}

/**
* \brief Exit critical section
* \param[in][in] osalFreertos - pointer to FreeRTOS osal instance
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
*/
static UShellOsalErr_e ushellOsalFreertosCriticalSectionExit(void *const osal)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    if(ushellOsalFreertosCriticalSectionHandle == NULL)
    {
        return USHELL_OSAL_SEMAPHORE_OBJ_CREATE_ERR;
    }

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    BaseType_t status = pdFALSE;
    USHELL_OSAL_FREERTOS_ASSERT(NULL != ushellOsalFreertosCriticalSectionHandle);
    status = xSemaphoreGiveRecursive(ushellOsalFreertosCriticalSectionHandle);
    if (pdTRUE != status)
    {
        return USHELL_OSAL_SEMAPHORE_RELEASE_ERR;
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}

/**
 * \brief Create the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosLockObjCreate(void * const osalFreertos,
                                                                   UShellOsalLockObjHandle_t * const lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;
    *lockObjHandle = NULL;  // Clear stored value

    // Create the FreeRTOS lock object (recursive mutex):
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_LOCK_OBJS_NUM; i++)
    {
        if (NULL == osal->lockObjHandle[i])
        {
            // Congratulations!
            // The free slot was found, create the lock object
            osal->lockObjHandle[i] = xSemaphoreCreateRecursiveMutex();
            if (NULL == osal->lockObjHandle[i])
            {
                // Lock object was not created and must not be used
                // Exit: error - the memory required to hold the mutex could not be allocated
                return USHELL_OSAL_LOCK_OBJ_MEM_ALLOCATION_ERR;
            }

            // We can only get there if the lock object was created
            // No additional checks needed
            *lockObjHandle = osal->lockObjHandle[i];
            // So break the loop
            break;
        }
    }

    // 2. Check if the lock object was created
    if (NULL == *lockObjHandle)
    {
        return USHELL_OSAL_LOCK_OBJ_CREATE_ERR;  // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Delete the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object to deleted
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosLockObjDelete(void * const osalFreertos,
                                                                   const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;

    uint16_t lockObjIndex = ushellOsalFreertosFindLockObjHandle(osalFreertos, lockObjHandle);
    if (0 == lockObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_LOCK_OBJS_NUM < lockObjIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Delete the lock object
    vSemaphoreDelete((SemaphoreHandle_t)lockObjHandle);
    // Clear the lock object slot in the table
    osal->lockObjHandle[lockObjIndex - 1] = NULL;  // Subtract 1 because find function increasws actual index by 1

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Lock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - the lock object handle
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosLock(void * const osalFreertos,
                                                          const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t lockObjIndex = ushellOsalFreertosFindLockObjHandle(osalFreertos, lockObjHandle);
    if (0 == lockObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_LOCK_OBJS_NUM < lockObjIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Lock
    BaseType_t lockStatus = xSemaphoreTakeRecursive((SemaphoreHandle_t)lockObjHandle, portMAX_DELAY);
    if (pdTRUE != lockStatus)
    {
        USHELL_OSAL_FREERTOS_ASSERT(0);

        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Unlock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - the lock object handle
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosUnlock(void * const osalFreertos,
                                                            const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t lockObjIndex = ushellOsalFreertosFindLockObjHandle(osalFreertos, lockObjHandle);
    if (0 == lockObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_LOCK_OBJS_NUM < lockObjIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Unlock
    BaseType_t unlockStatus = xSemaphoreGiveRecursive((SemaphoreHandle_t)lockObjHandle);
    if (pdTRUE != unlockStatus)
    {
        USHELL_OSAL_FREERTOS_ASSERT(0);

        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Create a counting semaphore
 * \param[in] osalFreertos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreCountMax - The maximum count value that can be reached.
 *                            When the semaphore reaches this value it can no longer be 'given'.
 * \param[in] semaphoreHandle   - semaphore handle by which created semaphore can be referenced
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreCreate(void * const osalFreertos,
                                                                     const UShellOsalSemaphoreCount_t semaphoreCountMax,
                                                                     const UShellOsalSemaphoreCount_t semaphoreInitVal,
                                                                     UShellOsalSemaphoreHandle_t *const semaphoreHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreHandle);
    USHELL_OSAL_FREERTOS_ASSERT(0 != semaphoreCountMax);
    USHELL_OSAL_FREERTOS_ASSERT(semaphoreCountMax >= semaphoreInitVal);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;
    *semaphoreHandle = NULL;

    // Create the FreeRTOS semaphore object:
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_SEMAPHORE_OBJS_NUM; i++)
    {
        if (NULL == osal->semaphoreHandle[i])
        {
            // Congratulations!
            // The free slot was found, create the semaphore object
            // Initial semaphore count value is 0!
            osal->semaphoreHandle[i] = xSemaphoreCreateCounting(semaphoreCountMax, semaphoreInitVal);
            if (NULL == osal->semaphoreHandle[i])
            {
                // Semaphore was not created and must not be used
                // Exit: error - the RAM required to hold the semaphore cannot be allocated
                return USHELL_OSAL_SEMAPHORE_MEM_ALLOC_ERR;
            }

            // We can only get there if the semaphore was created
            // No additional checks needed
            *semaphoreHandle = osal->semaphoreHandle[i];
            // So break the loop
            break;
        }
    }

    // 2. Check if the semaphore object was created
    if (NULL == *semaphoreHandle)
    {
        return USHELL_OSAL_SEMAPHORE_OBJ_CREATE_ERR;  // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Delete a semaphore
 * \param[in] osalFreertos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle   - the handle of the semaphore being deleted
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreDelete(void * const osalFreertos,
                                                                     const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;

    uint16_t semaphoreIndex = ushellOsalFreertosFindSemaphoreHandle(osalFreertos, semaphoreHandle);
    if (0 == semaphoreIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_SEMAPHORE_OBJS_NUM < semaphoreIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Delete the semaphore
    vSemaphoreDelete((SemaphoreHandle_t)semaphoreHandle);
    // Clear the lock object slot in the table
    osal->semaphoreHandle[semaphoreIndex - 1] = NULL;  // Subtract 1 because find function increases actual index by 1

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Acquire a semaphore
 * \param[in] osalFreertos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle   - a handle to the semaphore being taken
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreAcquire(void * const osalFreertos,
                                                                      const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreHandle);

    uint16_t semaphoreIndex = ushellOsalFreertosFindSemaphoreHandle(osalFreertos, semaphoreHandle);
    if (0 == semaphoreIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_SEMAPHORE_OBJS_NUM < semaphoreIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    BaseType_t acquireStatus = pdFALSE;
    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Acquire the semaphore from ISR
        acquireStatus = xSemaphoreTakeFromISR(semaphoreHandle, &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    // Acquire the semaphore
    else
    {
        acquireStatus = xSemaphoreTake(semaphoreHandle, portMAX_DELAY); // TODO: to change to 0 ?
    }

    if (pdTRUE != acquireStatus)
    {
        return USHELL_OSAL_SEMAPHORE_ACQUIRE_ERR;
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Release a semaphore
 * \param[in] osalFreertos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle   - a handle to the semaphore being released
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreRelease(void * const osalFreertos,
                                                                      const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreHandle);

    uint16_t semaphoreIndex = ushellOsalFreertosFindSemaphoreHandle(osalFreertos, semaphoreHandle);
    if (0 == semaphoreIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_SEMAPHORE_OBJS_NUM < semaphoreIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    BaseType_t releaseStatus = pdFALSE;
    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Release the semaphore from ISR
        releaseStatus = xSemaphoreGiveFromISR(semaphoreHandle, &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    // Release the semaphore
    else
    {
        releaseStatus = xSemaphoreGive(semaphoreHandle);
    }

    if (pdTRUE != releaseStatus)
    {
        return USHELL_OSAL_SEMAPHORE_RELEASE_ERR;
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Get the count of a semaphore
 * \param[in] osalFreertos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle   - a handle of the semaphore being queried
 * \param[in] semaphoreCount    - pointer to a destination buff
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosSemaphoreCountGet(void * const osalFreertos,
                                                                       const UShellOsalSemaphoreHandle_t semaphoreHandle,
                                                                       UShellOsalSemaphoreCount_t *const semaphoreCount)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreCount);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t semaphoreIndex = ushellOsalFreertosFindSemaphoreHandle(osalFreertos, semaphoreHandle);
    if (0 == semaphoreIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_SEMAPHORE_OBJS_NUM < semaphoreIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Get the semaphore count
    UBaseType_t currSemaphoreCount = uxSemaphoreGetCount(semaphoreHandle);
    *semaphoreCount = (UShellOsalSemaphoreCount_t)currSemaphoreCount;

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Create the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle by which created thread can be referenced
 * \param[in] threadCfg     - thread configuration
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadCreate(void * const osalFreertos,
                                                                  UShellOsalThreadHandle_t * const threadHandle,
                                                                  UShellOsalThreadCfg_s threadCfg)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    // Check the given thread parameters
    if (!ushellOsalFreertosCheckParam(&threadCfg))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;
    *threadHandle = NULL;  // clear stored object

    // Create the FreeRTOS thread:
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
    {
        if (NULL == osal->threadObj[i].threadHandle)
        {
            // Congratulations!
            // The free slot was found, create the thread
            UBaseType_t threadPrio = UShellOsalFreertosThreadPriority[threadCfg.threadPriority];

            BaseType_t xReturned = xTaskCreate(threadCfg.threadWorker,
                                               threadCfg.name,
                                               (threadCfg.stackSize / sizeof(UBaseType_t)),   // convert byte-size in words
                                               threadCfg.threadParam,
                                               threadPrio,
                                               (TaskHandle_t *)&osal->threadObj[i].threadHandle);
            if (pdPASS != xReturned)
            {
                // Thread was not created and must not be used
                // Exit: error - memory required to create the thread could not be allocated
                return USHELL_OSAL_THREAD_MEM_ALLOCATION_ERR;
            }

            USHELL_OSAL_FREERTOS_ASSERT(NULL != osal->threadObj[i].threadHandle);

            // We can only get there if the thread was created
            // No additional checks needed
            osal->threadObj[i].threadCfg = threadCfg;
            *threadHandle                = osal->threadObj[i].threadHandle;
            // So break the loop
            break;
        }
    }

    // 2. Check if the thread was created
    if (NULL == *threadHandle)
    {
        return USHELL_OSAL_THREAD_CREATE_ERR;  // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Delete the thread
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] threadHandle - the handle of the thread being deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelete(void * const osalFreertos,
                                                                  const UShellOsalThreadHandle_t threadHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;

    uint16_t threadObjIndex = ushellOsalFreertosFindThreadHandle(osalFreertos, threadHandle);
    if (0 == threadObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_THREADS_NUM < threadObjIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Delete the task
    vTaskSuspend((TaskHandle_t)osal->threadObj[threadObjIndex - 1].threadHandle);
    vTaskDelete((TaskHandle_t)osal->threadObj[threadObjIndex - 1].threadHandle);

    // Clear the thread object slot in the table
    osal->threadObj[threadObjIndex - 1].threadHandle = NULL;  // Subtract 1 because find function incrises actual index by 1
    // Reset the parameters
    osal->threadObj[threadObjIndex - 1].threadCfg.name           = NULL;
    osal->threadObj[threadObjIndex - 1].threadCfg.threadWorker   = NULL;
    osal->threadObj[threadObjIndex - 1].threadCfg.stackSize      = 0;
    osal->threadObj[threadObjIndex - 1].threadCfg.threadParam    = NULL;
    osal->threadObj[threadObjIndex - 1].threadCfg.threadPriority = 0;

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Suspend the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadObj     - the handle of the thread being suspend
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadSuspend(void * const osalFreertos,
                                                                   const UShellOsalThreadHandle_t threadHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;

    uint16_t threadObjIndex = ushellOsalFreertosFindThreadHandle(osalFreertos, threadHandle);
    if (0 == threadObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_THREADS_NUM < threadObjIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Suspend the thread
    vTaskSuspend((TaskHandle_t)osal->threadObj[threadObjIndex - 1].threadHandle);

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}


/**
 * \brief Resume the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadObj     - the handle of the thread being resumed
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosThreadResume(void * const osalFreertos,
                                                                  const UShellOsalThreadHandle_t threadHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s *osal = (UShellOsal_s *)osalFreertos;

    uint16_t threadObjIndex = ushellOsalFreertosFindThreadHandle(osalFreertos, threadHandle);
    if (0 == threadObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_THREADS_NUM < threadObjIndex)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Resume the thread
    vTaskResume((TaskHandle_t)osal->threadObj[threadObjIndex - 1].threadHandle);

    return USHELL_OSAL_NO_ERR;  // Exit: no errors
}

/**
 * \brief       Perform some delay
 * \param[in][in]   osalFreertos - pointer to FreeRTOS osal instance;
 * \param[in][in]   msDelay - delay in ms;
 * \param[in][out]  no;
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelay(const void * const osalFreertos,
                                                                 const uint32_t msDelay)
{
    /* Checking of params */
    USHELL_OSAL_FREERTOS_ASSERT(osalFreertos);
    (void)osalFreertos;

    /* Check where the call is coming from */
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;  // Exit: Error: Call from isr
    }

    vTaskDelay(pdMS_TO_TICKS(msDelay));

    return USHELL_OSAL_NO_ERR;
}

/**
 * \brief Find the queue handle in the queue handles table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] queueHandle  - queue handle to be found
 * \return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindQueueHandle(UShellOsalFreertos_s * const osalFreeRtos,
                                                               const UShellOsalQueueHandle_t queueHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);  // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);  // Must be validated by the caller

    UShellOsal_s *osal = (UShellOsal_s *)osalFreeRtos;
    uint16_t handleIndex     = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_QUEUE_SLOTS_NUM; i++)
    {
        if (queueHandle == osal->queueHandle[i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}


/**
 * \brief Find the stream buffer handle in the stream buffer handlers table
 * \param[in] osalFreeRtos      - pointer to FreeRTOS osal instance
 * \param[in] streamBuffHandle  - stream buffer handle to be found
 * \return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindStreamBuffHandle(UShellOsalFreertos_s * const osalFreeRtos,
                                                                    const UShellOsalStreamBuffHandle_t streamBuffHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);  // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);  // Must be validated by the caller

    UShellOsal_s *osal = (UShellOsal_s *)osalFreeRtos;
    uint16_t handleIndex     = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_STREAM_BUFF_SLOTS_NUM; i++)
    {
        if (streamBuffHandle == osal->streamBuffHandle[i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}


/**
 * \brief Find the lock object handle in the lock objects table
 * \param[in] osalFreeRtos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle to be found
 * \return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindLockObjHandle(UShellOsalFreertos_s * const osalFreeRtos,
                                                                 const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);  // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);  // Must be validated by the caller

    UShellOsal_s *osal = (UShellOsal_s *)osalFreeRtos;
    uint16_t handleIndex     = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_LOCK_OBJS_NUM; i++)
    {
        if (lockObjHandle == osal->lockObjHandle[i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}


/**
 * \brief Find the semaphore handle in the semaphores table
 * \param[in] osalFreeRtos      - pointer to FreeRTOS osal instance
 * \param[in] semaphoreHandle   - semaphore handle to be found
 * \return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindSemaphoreHandle(UShellOsalFreertos_s * const osalFreeRtos,
                                                                   const UShellOsalSemaphoreHandle_t semaphoreHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);  // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != semaphoreHandle);  // Must be validated by the caller

    UShellOsal_s *osal = (UShellOsal_s *)osalFreeRtos;
    uint16_t handleIndex     = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_SEMAPHORE_OBJS_NUM; i++)
    {
        if (semaphoreHandle == osal->semaphoreHandle[i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}


/**
 * \brief Find the thread handle in the thread objects table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] threadHandle - thread handle to be found
 * \return uint16_t handleIndex + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindThreadHandle(UShellOsalFreertos_s * const osalFreeRtos,
                                                                const UShellOsalThreadHandle_t threadHandle)
{
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);  // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);  // Must be validated by the caller

    UShellOsal_s *osal = (UShellOsal_s *)osalFreeRtos;
    uint16_t handleIndex     = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
    {
        if (threadHandle == osal->threadObj[i].threadHandle)
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}


/**
 * \brief Perform thread configuration parameters validation procedure
 *        in terms of the requirements of the FreeRTOS
 * \param[in] threadCfg - pointer to a thread configuration structure being checked
 * \return true - if configuration is OK\
 * \return false - if not
 */
static bool ushellOsalFreertosCheckParam(const UShellOsalThreadCfg_s *const threadCfg)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadCfg);

    // Check stack size:
    // Convert from byte-size to words number
    const configSTACK_DEPTH_TYPE stackSize = (threadCfg->stackSize / (sizeof(UBaseType_t)));
    if (configMINIMAL_STACK_SIZE > stackSize)
    {
        return false;  // Exit: error - stack size is less than the minimum allowed
    }

    // Check thread  priority:
    // Check lower border
    if (USHELL_OSAL_THREAD_PRIORITY_LOW > threadCfg->threadPriority)
    {
        return false;  // Exit: error - thread priority can't be less or equal to the idle task priority
    }

    // Check the higher border
    if (USHELL_OSAL_THREAD_PRIORITY_ULTRA < threadCfg->threadPriority)
    {
        return false;  // Exit: error - thread priority can't be higher or equal to the max priority level
    }

    // Check the worker function
    if (NULL == threadCfg->threadWorker)
    {
        return false;  // Exit: error - invalid worker function pointer
    }

    return true;  // Exit: no errors
}