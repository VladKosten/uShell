/**
 * \file       ushell_osal_freertos.c
 * @brief      UShell OSAL FreeRTOS portable layer
 * \author     Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
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

//=====================================================================[ INTERNAL MACRO DEFENITIONS ]==============================================================================

/**
 * @brief UShell OSAL ASSERT macro definition for FreeRTOS.
 *
 * This macro is used to perform assertions in the UShell Operating System Abstraction Layer (OSAL)
 * specific to FreeRTOS. If `USHELL_OSAL_ASSERT` is defined, `USHELL_OSAL_FREERTOS_ASSERT` will use it
 * to perform the assertion. Otherwise, `USHELL_OSAL_FREERTOS_ASSERT` will be defined as an empty macro.
 *
 * @param[in] cond The condition to be asserted.
 */
#ifndef USHELL_OSAL_FREERTOS_ASSERT
    #ifdef USHELL_OSAL_ASSERT
        #define USHELL_OSAL_FREERTOS_ASSERT(cond) USHELL_OSAL_ASSERT(cond)
    #else
        #define USHELL_OSAL_FREERTOS_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]==========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]====================================================================

/**
 * @brief Create the queue
 * @param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in]   queueItemSize - the size of the queue item
 * @param[in]   queueDepth    - queue depth
 * @param[out]  queueHandle   - queue handle that was created
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueCreate(void* const osalFreertos,
                                                     const size_t queueItemSize,
                                                     const size_t queueDepth,
                                                     UShellOsalQueueHandle_t* const queueHandle);

/**
 * @brief Delete the queue
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - queue handle to be deleted
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueDelete(void* const osalFreertos,
                                                     const UShellOsalQueueHandle_t queueHandle);

/**
 * @brief Put the item to the queue
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue on which the item is to be put
 * @param[in] queueItemPtr  - pointer to the item source buff
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPut(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      const void* const queueItemPtr);

/**
 * @brief Put the item to the queue with specified waiting time
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue on which the item is to be posted.
 * @param[in] queueItemPtr  - pointer to the item source buff
 * @param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPost(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr,
                                                       const uint32_t timeoutMs);

/**
 * @brief Get the item from the queue [NON-BLOCKING CALL]
 * @param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * @param[out]  queueItemPtr  - tointer to the buffer into which the received item will be copied
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemGet(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      void* const queueItemPtr);

/**
 * @brief Get the item from the queue [BLOCKING CALL]
 * @param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * @param[out]  queueItemPtr  - pointer to the buffer into which the received item will be copied
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemWait(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr);

/**
 * @brief Get the item from the queue with specified waiting time
 * @param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * @param[out]  queueItemPtr  - pointer to the buffer into which the received item will be copied
 * @param[in]   timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPend(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr,
                                                       const uint32_t timeoutMs);

/**
 * @brief  Reset the queue
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue to be reset
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueReset(void* const osalFreertos,
                                                    const UShellOsalQueueHandle_t queueHandle);

/**
 * @brief Create the lock object
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[out] lockObjHandle - lock object handle that was created
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosLockObjCreate(void* const osalFreertos,
                                                       UShellOsalLockObjHandle_t* const lockObjHandle);

/**
 * @brief Delete the lock object
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object handle to be deleted
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosLockObjDelete(void* const osalFreertos,
                                                       const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * @brief Lock the operation
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object handle
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosLock(void* const osalFreertos,
                                              const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * @brief Unlock the operation
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object handle
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosUnlock(void* const osalFreertos,
                                                const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * @brief Create the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[out] threadHandle - thread handle that was created
 * @param[in] threadCfg     - thread configuration structure
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadCreate(void* const osalFreertos,
                                                      UShellOsalThreadHandle_t* const threadHandle,
                                                      UShellOsalThreadCfg_s threadCfg);

/**
 * @brief Delete the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] threadHandle  - thread handle to be deleted
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelete(void* const osalFreertos,
                                                      const UShellOsalThreadHandle_t threadHandle);

/**
 * @brief Suspend the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] threadHandle  - thread handle to be suspended
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadSuspend(void* const osalFreertos,
                                                       const UShellOsalThreadHandle_t threadHandle);

/**
 * @brief Resume the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] threadHandle  - thread handle to be resumed
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadResume(void* const osalFreertos,
                                                      const UShellOsalThreadHandle_t threadHandle);

/**
 * @brief delay the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] msDelay       - delay in milliseconds
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelay(const void* const osalFreertos,
                                                     const uint32_t msDelay);

/**
 * @brief Find the queue handle in the queue handles table
 * @param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * @param[in] queueHandle  - queue handle to be found
 * @return uint16_t - index of the queue handle in the table
 */
static inline uint16_t ushellOsalFreertosFindQueueHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                         const UShellOsalQueueHandle_t queueHandle);

/**
 * @brief Find the lock object handle in the lock objects table
 * @param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object handle to be found
 * @return uint16_t - index of the lock object handle in the table
 */
static inline uint16_t ushellOsalFreertosFindLockObjHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                           const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * @brief Find the thread handle in the thread objects table
 * @param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * @param[in] threadHandle - thread handle to be found
 * @return uint16_t - index of the thread handle in the table
 */
static inline uint16_t ushellOsalFreertosFindThreadHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                          const UShellOsalThreadHandle_t threadHandle);

/**
 * @brief Perform thread parameters validation procedure
 *        in terms of the requirements of the FreeRTOS
 * @param[in] threadCfg - pointer to the thread configuration structure
 * @return bool - true if the parameters are valid, false otherwise
 */
static bool ushellOsalFreertosCheckParam(const UShellOsalThreadCfg_s* const threadCfg);

/**
 * @brief FreeRTOS priority levels hash-table.
 *
 * This static constant array defines the mapping of UShell OSAL thread priority levels
 * to FreeRTOS-specific priority levels. The array size is determined by the enumeration
 * value `USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE`.
 */
static const UBaseType_t UShellOsalFreertosThreadPriority [USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE] =
    {
        USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW,
        USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE,
        USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH,
        USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA};

/**
 * @brief UShell FreeRTOS OSAL portable structure.
 *
 * This static constant structure defines the function pointers for operations needed to port
 * the UShell Operating System Abstraction Layer (OSAL) to FreeRTOS. It includes functions for
 * queue management, lock management, semaphore management, thread management, and critical section handling.
 */
static const UShellOsalPortable_s FreeRtosPortable =
    {
        .queueCreate = ushellOsalFreertosQueueCreate,
        .queueDelete = ushellOsalFreertosQueueDelete,
        .queueItemPut = ushellOsalFreertosQueueItemPut,
        .queueItemPost = ushellOsalFreertosQueueItemPost,
        .queueItemGet = ushellOsalFreertosQueueItemGet,
        .queueItemWait = ushellOsalFreertosQueueItemWait,
        .queueItemPend = ushellOsalFreertosQueueItemPend,
        .queueReset = ushellOsalFreertosQueueReset,
        .lockObjCreate = ushellOsalFreertosLockObjCreate,
        .lockObjDelete = ushellOsalFreertosLockObjDelete,
        .lock = ushellOsalFreertosLock,
        .unlock = ushellOsalFreertosUnlock,
        .threadCreate = ushellOsalFreertosThreadCreate,
        .threadDelete = ushellOsalFreertosThreadDelete,
        .threadSuspend = ushellOsalFreertosThreadSuspend,
        .threadResume = ushellOsalFreertosThreadResume,
        .threadDelay = ushellOsalFreertosThreadDelay,
};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]==============================================================================

/**
 * @brief Initialize the UShell FreeRTOS osal instance
 * @param[in] osalFreertos - pointer to FreeRTOS osal instance
 * @param[in] name         - pointer to the name of the OSAL instance [optional]
 * @param[in] parent       - pointer to a parent object [optional]
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosInit(UShellOsalFreertos_s* const osalFreertos,
                                       const char* name,
                                       void* const parent)
{
    /* Check the input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    /* Initialize the UShell OSAL object */
    do
    {
        /* Check input parameter */
        if (NULL == osalFreertos)
        {
            return USHELL_OSAL_INVALID_ARGS;
        }

        /* Clear the OSAL object */
        memset(osalFreertos, 0, sizeof(UShellOsalFreertos_s));

        /* Initialize the base OSAL object */
        status = UShellOsalInit(osal, name, parent, &FreeRtosPortable);
        if (USHELL_OSAL_NO_ERR != status)
        {
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Deinitialize UShell FreeRTOS OSAL instance
 * @param[in] osalFreertos - pointer to osal FreeRTOS instance
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosDeinit(UShellOsalFreertos_s* const osalFreertos)
{

    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    do
    {
        /* Check input parameter */
        if ((NULL == osalFreertos))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Suspend all threads */
        for (int i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
        {
            if (NULL != osal->threadObj [i].threadHandle)
            {
                vTaskSuspend((TaskHandle_t) osal->threadObj [i].threadHandle);
            }
        }

        /* Delete all queues */
        for (int i = 0; i < USHELL_OSAL_QUEUE_SLOTS_NUM; i++)
        {
            if (NULL != osal->queueHandle [i])
            {
                vQueueDelete((QueueHandle_t) osal->queueHandle [i]);
                osal->queueHandle [i] = NULL;
            }
        }

        /* Delete all lock objects */
        for (int i = 0; i < USHELL_OSAL_LOCK_OBJS_NUM; i++)
        {
            if (NULL != osal->lockObjHandle [i])
            {
                vSemaphoreDelete(osal->lockObjHandle [i]);
                osal->lockObjHandle [i] = NULL;
            }
        }

        /* Delete all threads */
        for (int i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
        {
            if (NULL != osal->threadObj [i].threadHandle)
            {
                vTaskDelete((TaskHandle_t) osal->threadObj [i].threadHandle);
                osal->threadObj [i].threadHandle = NULL;
                osal->threadObj [i].threadCfg.name = NULL;
                osal->threadObj [i].threadCfg.threadWorker = NULL;
                osal->threadObj [i].threadCfg.stackSize = 0;
                osal->threadObj [i].threadCfg.threadParam = NULL;
                osal->threadObj [i].threadCfg.threadPriority = 0;
            }
        }

        /* Deinitialize the base OSAL object */
        status = UShellOsalDeinit(osal);
        if (USHELL_OSAL_NO_ERR != status)
        {
            break;
        }

        /* Clear the FreeRTOS OSAL object */
        memset(osalFreertos, 0, sizeof(UShellOsalFreertos_s));

    } while (0);

    return status;
}

//============================================================================[PRIVATE FUNCTIONS]==================================================================================

/**
 * @brief Create the queue
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueItemSize - the size of the queue item
 * @param[in] queueDepth    - queue depth
 * @param[out] queueHandle  - queue handle that was created
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueCreate(void* const osalFreertos,
                                                     const size_t queueItemSize,
                                                     const size_t queueDepth,
                                                     UShellOsalQueueHandle_t* const queueHandle)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(0 != queueItemSize);
    USHELL_OSAL_FREERTOS_ASSERT(0 != queueDepth);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    size_t queueIndexNum = 0;
    bool slotFound = false;

    /* Create the FreeRTOS queue */
    do
    {
        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Create the FreeRTOS queue: */
        /* : 1. check if there is a free slot in the queue handles table */
        for (int i = 0; i < USHELL_OSAL_QUEUE_SLOTS_NUM; i++)
        {
            if (NULL == osal->queueHandle [i])
            {
                slotFound = true;
                queueIndexNum = i;
                break;
            }
        }

        /* : 2. check if the we have a free slot */
        if (false == slotFound)
        {
            status = USHELL_OSAL_QUEUE_MEM_ALLOCATION_ERR;
            break;
        }

        /* : 3. create the FreeRTOS queue */
        osal->queueHandle [queueIndexNum] = xQueueCreate(queueDepth, queueItemSize);
        if (NULL == osal->queueHandle [queueIndexNum])
        {
            status = USHELL_OSAL_QUEUE_CREATE_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Delete the queue
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - queue handle to be deleted
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueDelete(void* const osalFreertos,
                                                     const UShellOsalQueueHandle_t queueHandle)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;

    do
    {
        /* Check input parameter */
        if ((NULL == osalFreertos) ||
            (NULL == queueHandle))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            return USHELL_OSAL_PORT_SPECIFIC_ERR;
        }

        /* Delete the FreeRTOS queue */
        vQueueDelete((QueueHandle_t) queueHandle);

        /* Clear the queue handle */
        osal->queueHandle [queueIndexNum - 1] = NULL;

    } while (0);

    return status;
}

/**
 * @brief Put the item to the queue
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue on which the item is to be put
 * @param[in] queueItemPtr  - pointer to the item source buff
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPut(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      const void* const queueItemPtr)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;
    BaseType_t sendStatus = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Put the item to the queue */
    do
    {
        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            /* Put the item to the queue from ISR level */
            sendStatus = xQueueSendFromISR((QueueHandle_t) queueHandle,
                                           (const void*) queueItemPtr,
                                           &xHigherPriorityTaskWoken);
            /* Switch context */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else
        {
            sendStatus = xQueueSend((QueueHandle_t) queueHandle,
                                    (const void*) queueItemPtr,
                                    pdMS_TO_TICKS(0));    // without waiting time
        }

        /* Check the status of the operation */
        if (pdTRUE != sendStatus)
        {
            status = USHELL_OSAL_QUEUE_IS_EMPTY_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Put the item to the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue on which the item is to be posted.
 * @param[in] queueItemPtr  - pointer to the item source buff
 * @param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPost(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr,
                                                       const uint32_t timeoutMs)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;
    TickType_t safeTimeoutInTicks = 0;
    BaseType_t sendStatus = pdFALSE;

    /* Put the item to the queue */
    do
    {
        /* Check the input parameters */
        if ((NULL == osalFreertos) ||
            (NULL == queueHandle) ||
            (NULL == queueItemPtr))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Convert the timeout in milliseconds to the ticks */
        safeTimeoutInTicks = ushellOsalFreertosSafeTimeoutToTicks(timeoutMs);

        /* Put the item to the queue */
        sendStatus = xQueueSend((QueueHandle_t) queueHandle,
                                (const void*) queueItemPtr,
                                safeTimeoutInTicks);    // with waiting time specified
        if (sendStatus != pdTRUE)
        {
            status = USHELL_OSAL_QUEUE_OVERFLOW_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Get the item from the queue [BLOCKING CALL]
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * @param[in] queueItemPtr  - tointer to the buffer into which the received item will be copied
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemGet(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      void* const queueItemPtr)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;
    BaseType_t receiveStatus = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Get the item from the queue */
    do
    {
        /* Check input parameters */
        if ((NULL == osalFreertos) ||
            (NULL == queueHandle) ||
            (NULL == queueItemPtr))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            /* Get the item from the queue from ISR level */
            receiveStatus = xQueueReceiveFromISR((QueueHandle_t) queueHandle,
                                                 (void*) queueItemPtr,
                                                 &xHigherPriorityTaskWoken);
            /* Switch context */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else
        {
            /* Get the item from the FreeRTOS queue */
            receiveStatus = xQueueReceive((QueueHandle_t) queueHandle,
                                          (void*) queueItemPtr,
                                          pdMS_TO_TICKS(0));
        }

        /* Check the status of the operation */
        if (pdTRUE != receiveStatus)
        {
            status = USHELL_OSAL_QUEUE_IS_EMPTY_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Get the item from the queue [BLOCKING CALL]
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * @param[in] queueItemPtr  - pointer to the buffer into which the received item will be copied
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemWait(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr)
{
    /* Must be validated by the caller */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;
    BaseType_t receiveStatus = pdFALSE;

    /* Get the item from the queue */
    do
    {
        /* Check input parameters */
        if ((NULL == osalFreertos) ||
            (NULL == queueHandle) ||
            (NULL == queueItemPtr))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /*  Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Get the item from the FreeRTOS queue */
        receiveStatus = xQueueReceive((QueueHandle_t) queueHandle,
                                      (void*) queueItemPtr,
                                      portMAX_DELAY);
        if (receiveStatus != pdTRUE)
        {
            status = USHELL_OSAL_QUEUE_IS_EMPTY_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Get the item from the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * @param[in] queueItemPtr  - pointer to the buffer into which the received item will be copied
 * @param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueItemPend(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr,
                                                       const uint32_t timeoutMs)
{
    /*  Must be validated by the caller */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;
    TickType_t safeTimeoutInTicks = 0;
    BaseType_t receiveStatus = pdFALSE;

    /* Get the item from the queue */
    do
    {
        /* Check input parameters */
        if ((NULL == osalFreertos) ||
            (NULL == queueHandle) ||
            (NULL == queueItemPtr))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Safely convert timeout in ms to FreeRTOS ticks */
        safeTimeoutInTicks = ushellOsalFreertosSafeTimeoutToTicks(timeoutMs);

        /* Get the item from the FreeRTOS queue */
        receiveStatus = xQueueReceive((QueueHandle_t) queueHandle,
                                      (void*) queueItemPtr,
                                      safeTimeoutInTicks);    // with waiting time specified
        if (receiveStatus != pdTRUE)
        {
            status = USHELL_OSAL_QUEUE_IS_EMPTY_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * @brief Reset the queue
 * @param[in][in]  osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in][in]  queueHandle   - the handle the queue to reset .
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosQueueReset(void* const osalFreertos,
                                                    const UShellOsalQueueHandle_t queueHandle)
{
    /* Must be validated by the caller */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    uint16_t queueIndexNum = 0;
    BaseType_t receiveStatus = pdFALSE;

    /* Reset the FreeRTOS queue */
    do
    {
        /* Check the input parameters */
        if ((NULL == osalFreertos) ||
            (NULL == queueHandle))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = ushellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
        if (0 == queueIndexNum)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the queue index is within the range */
        if (USHELL_OSAL_QUEUE_SLOTS_NUM < queueIndexNum)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Reset the FreeRTOS queue */
        receiveStatus = xQueueReset((QueueHandle_t) queueHandle);
        if (receiveStatus != pdTRUE)
        {
            status = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Create the lock object
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object handle that was created
 * @return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosLockObjCreate(void* const osalFreertos,
                                                       UShellOsalLockObjHandle_t* const lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    *lockObjHandle = NULL;    // Clear stored value

    // Create the FreeRTOS lock object (recursive mutex):
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_LOCK_OBJS_NUM; i++)
    {
        if (NULL == osal->lockObjHandle [i])
        {
            // Congratulations!
            // The free slot was found, create the lock object
            osal->lockObjHandle [i] = xSemaphoreCreateRecursiveMutex();
            if (NULL == osal->lockObjHandle [i])
            {
                // Lock object was not created and must not be used
                // Exit: error - the memory required to hold the mutex could not be allocated
                return USHELL_OSAL_LOCK_OBJ_MEM_ALLOCATION_ERR;
            }

            // We can only get there if the lock object was created
            // No additional checks needed
            *lockObjHandle = osal->lockObjHandle [i];
            // So break the loop
            break;
        }
    }

    // 2. Check if the lock object was created
    if (NULL == *lockObjHandle)
    {
        return USHELL_OSAL_LOCK_OBJ_CREATE_ERR;    // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Delete the lock object
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object to deleted
 * @return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosLockObjDelete(void* const osalFreertos,
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

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

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
    vSemaphoreDelete((SemaphoreHandle_t) lockObjHandle);
    // Clear the lock object slot in the table
    osal->lockObjHandle [lockObjIndex - 1] = NULL;    // Subtract 1 because find function increasws actual index by 1

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Lock the operation
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - the lock object handle
 * @return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosLock(void* const osalFreertos,
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
    BaseType_t lockStatus = xSemaphoreTakeRecursive((SemaphoreHandle_t) lockObjHandle, portMAX_DELAY);
    if (pdTRUE != lockStatus)
    {
        USHELL_OSAL_FREERTOS_ASSERT(0);

        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Unlock the operation
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - the lock object handle
 * @return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosUnlock(void* const osalFreertos,
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
    BaseType_t unlockStatus = xSemaphoreGiveRecursive((SemaphoreHandle_t) lockObjHandle);
    if (pdTRUE != unlockStatus)
    {
        USHELL_OSAL_FREERTOS_ASSERT(0);

        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Create the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] threadHandle  - thread handle by which created thread can be referenced
 * @param[in] threadCfg     - thread configuration
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadCreate(void* const osalFreertos,
                                                      UShellOsalThreadHandle_t* const threadHandle,
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

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    *threadHandle = NULL;    // clear stored object

    // Create the FreeRTOS thread:
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
    {
        if (NULL == osal->threadObj [i].threadHandle)
        {
            // Congratulations!
            // The free slot was found, create the thread
            UBaseType_t threadPrio = UShellOsalFreertosThreadPriority [threadCfg.threadPriority];

            BaseType_t xReturned = xTaskCreate(threadCfg.threadWorker,
                                               threadCfg.name,
                                               (threadCfg.stackSize / sizeof(UBaseType_t)),    // convert byte-size in words
                                               threadCfg.threadParam,
                                               threadPrio,
                                               (TaskHandle_t*) &osal->threadObj [i].threadHandle);
            if (pdPASS != xReturned)
            {
                // Thread was not created and must not be used
                // Exit: error - memory required to create the thread could not be allocated
                return USHELL_OSAL_THREAD_MEM_ALLOCATION_ERR;
            }

            USHELL_OSAL_FREERTOS_ASSERT(NULL != osal->threadObj [i].threadHandle);

            // We can only get there if the thread was created
            // No additional checks needed
            osal->threadObj [i].threadCfg = threadCfg;
            *threadHandle = osal->threadObj [i].threadHandle;
            // So break the loop
            break;
        }
    }

    // 2. Check if the thread was created
    if (NULL == *threadHandle)
    {
        return USHELL_OSAL_THREAD_CREATE_ERR;    // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Delete the thread
 * @param[in] osalFreertos - pointer to FreeRTOS osal instance
 * @param[in] threadHandle - the handle of the thread being deleted
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelete(void* const osalFreertos,
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

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

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
    vTaskSuspend((TaskHandle_t) osal->threadObj [threadObjIndex - 1].threadHandle);
    vTaskDelete((TaskHandle_t) osal->threadObj [threadObjIndex - 1].threadHandle);

    // Clear the thread object slot in the table
    osal->threadObj [threadObjIndex - 1].threadHandle = NULL;    // Subtract 1 because find function incrises actual index by 1
    // Reset the parameters
    osal->threadObj [threadObjIndex - 1].threadCfg.name = NULL;
    osal->threadObj [threadObjIndex - 1].threadCfg.threadWorker = NULL;
    osal->threadObj [threadObjIndex - 1].threadCfg.stackSize = 0;
    osal->threadObj [threadObjIndex - 1].threadCfg.threadParam = NULL;
    osal->threadObj [threadObjIndex - 1].threadCfg.threadPriority = 0;

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Suspend the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] threadObj     - the handle of the thread being suspend
 * @return UShellOsalErr_e error code
 */
static UShellOsalErr_e ushellOsalFreertosThreadSuspend(void* const osalFreertos,
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

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

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
    vTaskSuspend((TaskHandle_t) osal->threadObj [threadObjIndex - 1].threadHandle);

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief Resume the thread
 * @param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * @param[in] threadObj     - the handle of the thread being resumed
 * @return UShellOsalErr_e error code.
 */
static UShellOsalErr_e ushellOsalFreertosThreadResume(void* const osalFreertos,
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

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

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
    vTaskResume((TaskHandle_t) osal->threadObj [threadObjIndex - 1].threadHandle);

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * @brief       Perform some delay
 * @param[in][in]   osalFreertos - pointer to FreeRTOS osal instance;
 * @param[in][in]   msDelay - delay in ms;
 * @param[in][out]  no;
 * @return      UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e ushellOsalFreertosThreadDelay(const void* const osalFreertos,
                                                     const uint32_t msDelay)
{
    /* Checking of params */
    USHELL_OSAL_FREERTOS_ASSERT(osalFreertos);
    (void) osalFreertos;

    /* Check where the call is coming from */
    if (xPortInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;    // Exit: Error: Call from isr
    }

    vTaskDelay(pdMS_TO_TICKS(msDelay));

    return USHELL_OSAL_NO_ERR;
}

/**
 * @brief Find the queue handle in the queue handles table
 * @param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * @param[in] queueHandle  - queue handle to be found
 * @return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindQueueHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                         const UShellOsalQueueHandle_t queueHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);     // Must be validated by the caller

    UShellOsal_s* osal = (UShellOsal_s*) osalFreeRtos;
    uint16_t handleIndex = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_QUEUE_SLOTS_NUM; i++)
    {
        if (queueHandle == osal->queueHandle [i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}

/**
 * @brief Find the lock object handle in the lock objects table
 * @param[in] osalFreeRtos  - pointer to FreeRTOS osal instance
 * @param[in] lockObjHandle - lock object handle to be found
 * @return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindLockObjHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                           const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);     // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);    // Must be validated by the caller

    UShellOsal_s* osal = (UShellOsal_s*) osalFreeRtos;
    uint16_t handleIndex = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_LOCK_OBJS_NUM; i++)
    {
        if (lockObjHandle == osal->lockObjHandle [i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}

/**
 * @brief Find the thread handle in the thread objects table
 * @param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * @param[in] threadHandle - thread handle to be found
 * @return uint16_t handleIndex + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t ushellOsalFreertosFindThreadHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                          const UShellOsalThreadHandle_t threadHandle)
{
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);    // Must be validated by the caller

    UShellOsal_s* osal = (UShellOsal_s*) osalFreeRtos;
    uint16_t handleIndex = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_THREADS_NUM; i++)
    {
        if (threadHandle == osal->threadObj [i].threadHandle)
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}

/**
 * @brief Perform thread configuration parameters validation procedure
 *        in terms of the requirements of the FreeRTOS
 * @param[in] threadCfg - pointer to a thread configuration structure being checked
 * @return true - if configuration is OK\
 * @return false - if not
 */
static bool ushellOsalFreertosCheckParam(const UShellOsalThreadCfg_s* const threadCfg)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadCfg);

    // Check stack size:
    // Convert from byte-size to words number
    const configSTACK_DEPTH_TYPE stackSize = (threadCfg->stackSize / (sizeof(UBaseType_t)));
    if (configMINIMAL_STACK_SIZE > stackSize)
    {
        return false;    // Exit: error - stack size is less than the minimum allowed
    }

    // Check thread  priority:
    // Check lower border
    if (USHELL_OSAL_THREAD_PRIORITY_LOW > threadCfg->threadPriority)
    {
        return false;    // Exit: error - thread priority can't be less or equal to the idle task priority
    }

    // Check the higher border
    if (USHELL_OSAL_THREAD_PRIORITY_ULTRA < threadCfg->threadPriority)
    {
        return false;    // Exit: error - thread priority can't be higher or equal to the max priority level
    }

    // Check the worker function
    if (NULL == threadCfg->threadWorker)
    {
        return false;    // Exit: error - invalid worker function pointer
    }

    return true;    // Exit: no errors
}