/**
 * \file       ushell_osal_freertos.c
 * \brief      UShell OSAL FreeRTOS portable layer
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
#include <string.h>

/* Port includes */
#include "ushell_osal_freertos.h"

//=====================================================================[ INTERNAL MACRO DEFENITIONS ]==============================================================================

/**
 * \brief UShell OSAL ASSERT macro definition for FreeRTOS.
 *
 * This macro is used to perform assertions in the UShell Operating System Abstraction Layer (OSAL)
 * specific to FreeRTOS. If `USHELL_OSAL_ASSERT` is defined, `USHELL_OSAL_FREERTOS_ASSERT` will use it
 * to perform the assertion. Otherwise, `USHELL_OSAL_FREERTOS_ASSERT` will be defined as an empty macro.
 *
 * \param[in] cond The condition to be asserted.
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
 * \brief Safely converts timeout in ms to FreeRTOS ticks
 * handling potential integer overflow in pdMS_TO_TICKS(timeoutMs) FreeRTOS macro
 */
static TickType_t uShellOsalFreertosSafeTimeoutToTicks(const uint32_t timeoutMs);

/**
 * \brief Create the queue
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueItemSize - the size of the queue item
 * \param[in]   queueDepth    - queue depth
 * \param[out]  queueHandle   - queue handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueCreate(void* const osalFreertos,
                                                     const size_t queueItemSize,
                                                     const size_t queueDepth,
                                                     UShellOsalQueueHandle_t* const queueHandle);

/**
 * \brief Delete the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - queue handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueDelete(void* const osalFreertos,
                                                     const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Put the item to the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be put
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemPut(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      const void* const queueItemPtr);

/**
 * \brief Put the item to the queue with specified waiting time
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be posted.
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemPost(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr,
                                                       const uint32_t timeoutMs);

/**
 * \brief Get the item from the queue [NON-BLOCKING CALL]
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * \param[out]  queueItemPtr  - tointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemGet(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      void* const queueItemPtr);

/**
 * \brief Get the item from the queue [BLOCKING CALL]
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * \param[out]  queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemWait(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr);

/**
 * \brief Get the item from the queue with specified waiting time
 * \param[in]   osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in]   queueHandle   - the handle to the queue from which the item is to be received.
 * \param[out]  queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \param[in]   timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemPend(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr,
                                                       const uint32_t timeoutMs);

/**
 * \brief  Reset the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue to be reset
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueReset(void* const osalFreertos,
                                                    const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Create the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[out] lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosLockObjCreate(void* const osalFreertos,
                                                       UShellOsalLockObjHandle_t* const lockObjHandle);

/**
 * \brief Delete the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosLockObjDelete(void* const osalFreertos,
                                                       const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Lock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosLock(void* const osalFreertos,
                                              const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Unlock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosUnlock(void* const osalFreertos,
                                                const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Create the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[out] threadHandle - thread handle that was created
 * \param[in] threadCfg     - thread configuration structure
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadCreate(void* const osalFreertos,
                                                      UShellOsalThreadHandle_t* const threadHandle,
                                                      UShellOsalThreadCfg_s threadCfg);

/**
 * \brief Delete the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadDelete(void* const osalFreertos,
                                                      const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Suspend the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle to be suspended
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadSuspend(void* const osalFreertos,
                                                       const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Resume the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle to be resumed
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadResume(void* const osalFreertos,
                                                      const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief delay the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] msDelay       - delay in milliseconds
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadDelay(const void* const osalFreertos,
                                                     const uint32_t msDelay);

/**
 * \brief Create the stream buffer
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffCreate(void* const osalFreertos,
                                                          const size_t buffSizeBytes,
                                                          const size_t triggerLevelBytes,
                                                          UShellOsalStreamBuffHandle_t* const streamBuffHandle);

/**
 * \brief Delete the stream buffer
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffDelete(void* const osalFreertos,
                                                          const UShellOsalStreamBuffHandle_t streamBuffHandle);

/**
 * \brief Send data to the stream buffer
 */
static size_t uShellOsalFreertosStreamBuffSend(void* const osalFreertos,
                                               const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                               const void* txData,
                                               const size_t dataLengthBytes,
                                               const uint32_t msToWait);

/**
 * \brief Send data to the stream buffer
 * \param osalFreertos      - pointer to FreeRTOS OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer to which a stream is being sent
 * \param txData            - pointer to the buffer that holds the bytes to be copied into the stream buffer
 * \param dataLengthBytes   - the size of the data in bytes
 * \param msToWait          - the maximum amount of time the task should remain in the blocked state to wait
 * \return size_t the number of bytes written to the stream buffer, will write as many bytes as possible.
 */
static size_t uShellOsalFreertosStreamBuffSendBlocking(void* const osalFreertos,
                                                       const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                       const void* txData,
                                                       const size_t dataLengthBytes);

/**
 * \brief Receive data from the stream buffer
 */
static size_t uShellOsalFreertosStreamBuffReceive(void* const osalFreertos,
                                                  const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                  void* const rxData,
                                                  const size_t dataLengthBytes,
                                                  const uint32_t msToWait);

/**
 * \brief Receive data from the stream buffer (blocking, no timeout)
 */
static size_t uShellOsalFreertosStreamBuffReceiveBlocking(void* const osalFreertos,
                                                          const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                          void* const rxData,
                                                          const size_t dataLengthBytes);

/**
 * \brief Receive data from the stream buffer with specified waiting time
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffIsEmpty(void* const osalFreertos,
                                                           const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                           bool* const isEmpty);

/**
 * \brief Reset a stream buffer to its initial empty state
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffReset(void* const osalFreertos,
                                                         const UShellOsalStreamBuffHandle_t streamBuffHandle);

/**
 * \brief Find the queue handle in the queue handles table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] queueHandle  - queue handle to be found
 * \return uint16_t - index of the queue handle in the table
 */
static inline uint16_t uShellOsalFreertosFindQueueHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                         const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Find the lock object handle in the lock objects table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle to be found
 * \return uint16_t - index of the lock object handle in the table
 */
static inline uint16_t uShellOsalFreertosFindLockObjHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                           const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Find the thread handle in the thread objects table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] threadHandle - thread handle to be found
 * \return uint16_t - index of the thread handle in the table
 */
static inline uint16_t uShellOsalFreertosFindThreadHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                          const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Find the stream buffer handle in the stream buffers table
 */
static inline uint16_t uShellOsalFreertosFindStreamBuffHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                              const UShellOsalStreamBuffHandle_t streamBuffHandle);

/**
 * \brief Find the timer handle in the timer objects table
 */
static inline uint16_t uShellOsalFreertosFindTimerHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                         const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Find the event group handle in the event groups table
 */
static inline uint16_t uShellOsalFreertosFindEventGroupHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                              const UShellOsalEventGroupHandle_t eventGroupHandle);

/**
 * \brief Create the timer
 * \param osalFreertos  - pointer to FreeRTOS osal instance
 * \param timerHandle   - pointer to the timer handle that was created
 * \param timerCfg      - timer configuration structure
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosTimerCreate(void* const osalFreertos,
                                                     UShellOsalTimerHandle_t* const timerHandle,
                                                     const UShellOsalTimerCfg_s timerCfg);

/**
 * \brief Delete the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerDelete(void* const osalFreertos,
                                                     const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Start the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerStart(void* const osalFreertos,
                                                    const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Stop the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerStop(void* const osalFreertos,
                                                   const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Reset the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerReset(void* const osalFreertos,
                                                    const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Set the timer period
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \param[in] periodMs - the period of the timer in milliseconds
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerSetPeriod(void* const osalFreertos,
                                                        const UShellOsalTimerHandle_t timerHandle,
                                                        const uint32_t periodMs);

/**
 * \brief Callback function for the FreeRTOS timer
 * \param[in] xTimer - the handle to the timer that was created
 * \return void
 */
static void uShellOsalFreertosTimerCallback(TimerHandle_t xTimer);

/**
 * \brief Create an event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupCreate(void* const osal,
                                                   UShellOsalEventGroupHandle_t* const eventGroupHandle);

/**
 * \brief Create an event group with a specific name.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \param[in] name Name of the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupDelete(void* const osal,
                                                   const UShellOsalEventGroupHandle_t eventGroupHandle);

/**
 * \brief Set a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToSet Bits to set in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupSetBits(void* const osal,
                                                    const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                    const UShellOsalEventGroupBits_e bitsToSet);

/**
 * \brief Clear a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToClear Bits to clear in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupClearBits(void* const osal,
                                                      const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                      const UShellOsalEventGroupBits_e bitsToClear);

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
UShellOsalErr_e uShellOsalFreertosEventGroupBitsWait(void* const osal,
                                                     const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                     const UShellOsalEventGroupBits_e bitsToWait,
                                                     UShellOsalEventGroupBits_e* const bitsReceived,
                                                     const bool clearOnExit,
                                                     const bool waitAllBits);

/**
 * \brief Get the active bits in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[out] bitsActive Pointer to store the active bits.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupBitsActiveGet(void* const osal,
                                                          const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                          UShellOsalEventGroupBits_e* const bitsActive);

/**
 * \brief Perform thread parameters validation procedure
 *        in terms of the requirements of the FreeRTOS
 * \param[in] threadCfg - pointer to the thread configuration structure
 * \return bool - true if the parameters are valid, false otherwise
 */
static bool uShellOsalFreertosCheckParam(const UShellOsalThreadCfg_s* const threadCfg);

/**
 * \brief FreeRTOS priority levels hash-table.
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
 * \brief UShell FreeRTOS OSAL portable structure.
 *
 * This static constant structure defines the function pointers for operations needed to port
 * the UShell Operating System Abstraction Layer (OSAL) to FreeRTOS. It includes functions for
 * queue management, lock management, semaphore management, thread management, and critical section handling.
 */
static const UShellOsalPortable_s FreeRtosPortable =
    {
        .queueCreate = uShellOsalFreertosQueueCreate,
        .queueDelete = uShellOsalFreertosQueueDelete,
        .queueItemPut = uShellOsalFreertosQueueItemPut,
        .queueItemPost = uShellOsalFreertosQueueItemPost,
        .queueItemGet = uShellOsalFreertosQueueItemGet,
        .queueItemWait = uShellOsalFreertosQueueItemWait,
        .queueItemPend = uShellOsalFreertosQueueItemPend,
        .queueReset = uShellOsalFreertosQueueReset,
        .lockObjCreate = uShellOsalFreertosLockObjCreate,
        .lockObjDelete = uShellOsalFreertosLockObjDelete,
        .lock = uShellOsalFreertosLock,
        .unlock = uShellOsalFreertosUnlock,
        .threadCreate = uShellOsalFreertosThreadCreate,
        .threadDelete = uShellOsalFreertosThreadDelete,
        .threadSuspend = uShellOsalFreertosThreadSuspend,
        .threadResume = uShellOsalFreertosThreadResume,
        .threadDelay = uShellOsalFreertosThreadDelay,
        .streamBuffCreate = uShellOsalFreertosStreamBuffCreate,
        .streamBuffDelete = uShellOsalFreertosStreamBuffDelete,
        .streamBuffReset = uShellOsalFreertosStreamBuffReset,
        .streamBuffSend = uShellOsalFreertosStreamBuffSend,
        .streamBuffReceive = uShellOsalFreertosStreamBuffReceive,
        .streamBuffReceiveBlocking = uShellOsalFreertosStreamBuffReceiveBlocking,
        .streamBuffSendBlocking = uShellOsalFreertosStreamBuffSendBlocking,
        .streamBuffIsEmpty = uShellOsalFreertosStreamBuffIsEmpty,
        .timerCreate = uShellOsalFreertosTimerCreate,
        .timerDelete = uShellOsalFreertosTimerDelete,
        .timerStart = uShellOsalFreertosTimerStart,
        .timerStop = uShellOsalFreertosTimerStop,
        .timerReset = uShellOsalFreertosTimerReset,
        .timerPeriodChange = uShellOsalFreertosTimerSetPeriod,
        .eventGroupSetBits = uShellOsalFreertosEventGroupSetBits,
        .eventGroupClearBits = uShellOsalFreertosEventGroupClearBits,
        .eventGroupBitsWait = uShellOsalFreertosEventGroupBitsWait,
        .eventGroupDelete = uShellOsalFreertosEventGroupDelete,
        .eventGroupCreate = uShellOsalFreertosEventGroupCreate,
};

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]==============================================================================

/**
 * \brief Initialize the UShell FreeRTOS osal instance
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] name         - pointer to the name of the OSAL instance [optional]
 * \param[in] parent       - pointer to a parent object [optional]
 * \return UShellOsalErr_e error code.
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
 * \brief Deinitialize UShell FreeRTOS OSAL instance
 * \param[in] osalFreertos - pointer to osal FreeRTOS instance
 * \return UShellOsalErr_e error code.
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
        if (xPortIsInsideInterrupt())
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
 * \brief Safely converts timeout in ms to FreeRTOS ticks handling potential integer overflow in pdMS_TO_TICKS() FreeRTOS macro
 * \param timeoutMs     - timeout in ms to be checked for integer overflow and safely converted to FreeRTOS ticks
 * \return TickType_t   - timeout in FreeRTOS ticks
 */
static TickType_t uShellOsalFreertosSafeTimeoutToTicks(const uint32_t timeoutMs)
{
    // Make sure there is no integer overflow while using incoming value of timeoutMs
    // in the default implementation of FreeRTOS pdMS_TO_TICKS() macro
    USHELL_OSAL_FREERTOS_ASSERT((timeoutMs / configTICK_RATE_HZ) <= (UINT32_MAX / configTICK_RATE_HZ));

    // Convert timeout in ms to FreeRTOS ticks or use portMAX_DELAY for infinity timeout value
    TickType_t safeTickCount = (USHELL_OSAL_FREERTOS_INFINITY_TIMEOUT == timeoutMs) ? portMAX_DELAY : pdMS_TO_TICKS(timeoutMs);

    return safeTickCount;
}

/**
 * \brief Create the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueItemSize - the size of the queue item
 * \param[in] queueDepth    - queue depth
 * \param[out] queueHandle  - queue handle that was created
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueCreate(void* const osalFreertos,
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
        if (xPortIsInsideInterrupt())
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

        /* : 4. Return the queue handle */
        *queueHandle = osal->queueHandle [queueIndexNum];

    } while (0);

    return status;
}

/**
 * \brief Delete the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - queue handle to be deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueDelete(void* const osalFreertos,
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
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
 * \brief Put the item to the queue
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be put
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemPut(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      const void* const queueItemPtr)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    uint16_t queueIndexNum = 0;
    BaseType_t sendStatus = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Put the item to the queue */
    do
    {
        /* Find the queue handle in the queue handles table */
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
        if (xPortIsInsideInterrupt())
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
 * \brief Put the item to the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue on which the item is to be posted.
 * \param[in] queueItemPtr  - pointer to the item source buff
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemPost(void* const osalFreertos,
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
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
        safeTimeoutInTicks = uShellOsalFreertosSafeTimeoutToTicks(timeoutMs);

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
 * \brief Get the item from the queue [BLOCKING CALL]
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * \param[in] queueItemPtr  - tointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemGet(void* const osalFreertos,
                                                      const UShellOsalQueueHandle_t queueHandle,
                                                      void* const queueItemPtr)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueItemPtr);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
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
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
        if (xPortIsInsideInterrupt())
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
 * \brief Get the item from the queue [BLOCKING CALL]
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * \param[in] queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemWait(void* const osalFreertos,
                                                       const UShellOsalQueueHandle_t queueHandle,
                                                       void* const queueItemPtr)
{
    /* Must be validated by the caller */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
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
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
 * \brief Get the item from the queue
 * \note  (BLOCKING CALL WITH SPECIFIED WAIT)
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] queueHandle   - the handle to the queue from which the item is to be received.
 * \param[in] queueItemPtr  - pointer to the buffer into which the received item will be copied
 * \param[in] timeoutMs     - timeout in milliseconds to wait for the queue being ready to receive the item
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueItemPend(void* const osalFreertos,
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
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
        safeTimeoutInTicks = uShellOsalFreertosSafeTimeoutToTicks(timeoutMs);

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
 * \brief Reset the queue
 * \param[in][in]  osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in][in]  queueHandle   - the handle the queue to reset .
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosQueueReset(void* const osalFreertos,
                                                    const UShellOsalQueueHandle_t queueHandle)
{
    /* Must be validated by the caller */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != queueHandle);

    /* Local variables */
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
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
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Find the queue handle in the queue handles table */
        queueIndexNum = uShellOsalFreertosFindQueueHandle(osalFreertos, queueHandle);
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
 * \brief Create the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosLockObjCreate(void* const osalFreertos,
                                                       UShellOsalLockObjHandle_t* const lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
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
 * \brief Delete the lock object
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object to deleted
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosLockObjDelete(void* const osalFreertos,
                                                       const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    uint16_t lockObjIndex = uShellOsalFreertosFindLockObjHandle(osalFreertos, lockObjHandle);
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
 * \brief Lock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - the lock object handle
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosLock(void* const osalFreertos,
                                              const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t lockObjIndex = uShellOsalFreertosFindLockObjHandle(osalFreertos, lockObjHandle);
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
 * \brief Unlock the operation
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - the lock object handle
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosUnlock(void* const osalFreertos,
                                                const UShellOsalLockObjHandle_t lockObjHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != lockObjHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t lockObjIndex = uShellOsalFreertosFindLockObjHandle(osalFreertos, lockObjHandle);
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
 * \brief Create the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadHandle  - thread handle by which created thread can be referenced
 * \param[in] threadCfg     - thread configuration
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadCreate(void* const osalFreertos,
                                                      UShellOsalThreadHandle_t* const threadHandle,
                                                      UShellOsalThreadCfg_s threadCfg)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    // Check the given thread parameters
    if (!uShellOsalFreertosCheckParam(&threadCfg))
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
 * \brief Delete the thread
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] threadHandle - the handle of the thread being deleted
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadDelete(void* const osalFreertos,
                                                      const UShellOsalThreadHandle_t threadHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    uint16_t threadObjIndex = uShellOsalFreertosFindThreadHandle(osalFreertos, threadHandle);
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
 * \brief Suspend the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadObj     - the handle of the thread being suspend
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosThreadSuspend(void* const osalFreertos,
                                                       const UShellOsalThreadHandle_t threadHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    uint16_t threadObjIndex = uShellOsalFreertosFindThreadHandle(osalFreertos, threadHandle);
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
 * \brief Resume the thread
 * \param[in] osalFreertos  - pointer to FreeRTOS osal instance
 * \param[in] threadObj     - the handle of the thread being resumed
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosThreadResume(void* const osalFreertos,
                                                      const UShellOsalThreadHandle_t threadHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != threadHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    uint16_t threadObjIndex = uShellOsalFreertosFindThreadHandle(osalFreertos, threadHandle);
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
 * \brief       Perform some delay
 * \param[in][in]   osalFreertos - pointer to FreeRTOS osal instance;
 * \param[in][in]   msDelay - delay in ms;
 * \param[in][out]  no;
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosThreadDelay(const void* const osalFreertos,
                                                     const uint32_t msDelay)
{
    /* Checking of params */
    USHELL_OSAL_FREERTOS_ASSERT(osalFreertos);
    (void) osalFreertos;

    /* Check where the call is coming from */
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;    // Exit: Error: Call from isr
    }

    vTaskDelay(pdMS_TO_TICKS(msDelay));

    return USHELL_OSAL_NO_ERR;
}

/**
 * \brief Create the stream buffer
 * \param osalFreertos      - pointer to OSAL instance
 * \param buffSizeBytes     - the size of the stream buffer in bytes
 * \param triggerLevelBytes - trigger level in bytes (watermark)
 * \param streamBuffHandle  - the stream buffer handle was created
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffCreate(void* const osalFreertos,
                                                          const size_t buffSizeBytes,
                                                          const size_t triggerLevelBytes,
                                                          UShellOsalStreamBuffHandle_t* const streamBuffHandle)
{
    // Check income params with assertions because
    // they must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;
    *streamBuffHandle = NULL;    // Clear stored value

    // Create the FreeRTOS stream buffer:
    // 1. check if there is a free slot
    for (int i = 0; i < USHELL_OSAL_STREAM_BUFF_SLOTS_NUM; i++)
    {
        if (NULL == osal->streamBuffHandle [i])
        {
            // Congratulations!
            // The free slot was found, create the stream buffer
            osal->streamBuffHandle [i] = xStreamBufferCreate(buffSizeBytes, triggerLevelBytes);
            if (NULL == osal->streamBuffHandle [i])
            {
                // Stream buffer was not created and must not be used
                // Exit: error - memory required to create the stream butter could not be allocated
                return USHELL_OSAL_STREAM_BUFF_MEM_ALLOCATION_ERR;
            }

            // We can only get there if a stream buffer was created
            // No additional checks needed
            *streamBuffHandle = osal->streamBuffHandle [i];
            // So break the loop
            break;
        }
    }

    // 2. Check if the stream buffer was created
    if (NULL == *streamBuffHandle)
    {
        return USHELL_OSAL_STREAM_BUFF_CREATE_ERR;    // Exit: error - no free slots in the table
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Delete the stream buffer
 * \param osalFreertos      - pointer to OSAL instance
 * \param streamBuffHandle  - stream buffer handle being deleted
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffDelete(void* const osalFreertos,
                                                          const UShellOsalStreamBuffHandle_t streamBuffHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Finally delete the stream buffer
    vStreamBufferDelete((UShellOsalStreamBuffHandle_t) streamBuffHandle);
    // And clear the stream buffer slot in the table
    osal->streamBuffHandle [streamBuffIndexNum - 1] = NULL;    // Subtract 1 because find function increases actual index by 1

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Send data to the stream buffer
 * \param osalFreertos      - pointer to FreeRTOS OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer to which a stream is being sent
 * \param txData            - pointer to the buffer that holds the bytes to be copied into the stream buffer
 * \param dataLengthBytes   - the size of the data in bytes
 * \param msToWait          - the maximum amount of time the task should remain in the blocked state to wait
 * \return size_t the number of bytes written to the stream buffer, will write as many bytes as possible.
 */
static size_t uShellOsalFreertosStreamBuffSend(void* const osalFreertos,
                                               const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                               const void* txData,
                                               const size_t dataLengthBytes,
                                               const uint32_t msToWait)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != txData);
    USHELL_OSAL_FREERTOS_ASSERT(0 != dataLengthBytes);

    size_t bytesWritten = 0;

    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return bytesWritten;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return bytesWritten;
    }

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        bytesWritten = xStreamBufferSendFromISR((UShellOsalStreamBuffHandle_t) streamBuffHandle, txData, dataLengthBytes, &xHigherPriorityTaskWoken);

        // ATTENTION: not sure if this is a correct usage of the return after portYield
        // a possible solution is to leave as is and to add a parameter to which the number of bytes written will be copied
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        return bytesWritten;
    }
    else
    {
        // Safely convert timeout in ms to FreeRTOS ticks
        TickType_t safeTimeoutInTicks = uShellOsalFreertosSafeTimeoutToTicks(msToWait);

        // Put the item to the stream buffer
        bytesWritten = xStreamBufferSend((UShellOsalStreamBuffHandle_t) streamBuffHandle, txData, dataLengthBytes, safeTimeoutInTicks);    // with waiting time
    }

    return bytesWritten;    // Exit: no errors
}

/**
 * \brief Send data to the stream buffer
 * \param osalFreertos      - pointer to FreeRTOS OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer to which a stream is being sent
 * \param txData            - pointer to the buffer that holds the bytes to be copied into the stream buffer
 * \param dataLengthBytes   - the size of the data in bytes
 * \param msToWait          - the maximum amount of time the task should remain in the blocked state to wait
 * \return size_t the number of bytes written to the stream buffer, will write as many bytes as possible.
 */
static size_t uShellOsalFreertosStreamBuffSendBlocking(void* const osalFreertos,
                                                       const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                       const void* txData,
                                                       const size_t dataLengthBytes)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != txData);
    USHELL_OSAL_FREERTOS_ASSERT(0 != dataLengthBytes);

    size_t bytesWritten = 0;

    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return bytesWritten;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return bytesWritten;
    }

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return bytesWritten;
    }
    else
    {

        // Put the item to the stream buffer
        bytesWritten = xStreamBufferSend((UShellOsalStreamBuffHandle_t) streamBuffHandle, txData, dataLengthBytes, portMAX_DELAY);    // with waiting time
    }

    return bytesWritten;    // Exit: no errors
}

/**
 * \brief Receive data from the stream buffer
 * \param osalFreertos      - pointer to OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer from which bytes are to be received
 * \param rxData            - pointer to the buffer into which the received bytes will be copied
 * \param dataLengthBytes   - the size of the data in bytes
 * \param msToWait          - the maximum amount of time the task should remain in the blocked state to wait
 *                            for data to become available if the stream buffer is empty
 * \return size_t the number of bytes read from the stream buffer, 0 = means no bytes were read or an err occurred.
 */
static size_t
uShellOsalFreertosStreamBuffReceive(void* const osalFreertos,
                                    const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                    void* const rxData,
                                    const size_t dataLengthBytes,
                                    const uint32_t msToWait)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != rxData);
    USHELL_OSAL_FREERTOS_ASSERT(0 != dataLengthBytes);

    size_t bytesRead = 0;

    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return bytesRead;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return bytesRead;
    }

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        bytesRead = xStreamBufferReceiveFromISR((UShellOsalStreamBuffHandle_t) streamBuffHandle, rxData, dataLengthBytes, &xHigherPriorityTaskWoken);
        // ATTENTION: not sure if this is a correct usage of the return after portYield
        // a possible solution is to leave as is and to add a parameter to which the number of bytes written will be copied
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        return bytesRead;
    }
    else
    {
        // Safely convert timeout in ms to FreeRTOS ticks
        TickType_t safeTimeoutInTicks = uShellOsalFreertosSafeTimeoutToTicks(msToWait);

        // Put the item to the stream buffer
        bytesRead = xStreamBufferReceive((UShellOsalStreamBuffHandle_t) streamBuffHandle, rxData, dataLengthBytes, safeTimeoutInTicks);
    }

    return bytesRead;    // Exit: no errors
}

/**
 * \brief Receive data from the stream buffer (blocking, no timeout)
 */
static size_t uShellOsalFreertosStreamBuffReceiveBlocking(void* const osalFreertos,
                                                          const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                          void* const rxData,
                                                          const size_t dataLengthBytes)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != rxData);
    USHELL_OSAL_FREERTOS_ASSERT(0 != dataLengthBytes);

    size_t bytesRead = 0;

    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return bytesRead;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return bytesRead;
    }

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }
    else
    {

        // Put the item to the stream buffer
        bytesRead = xStreamBufferReceive((UShellOsalStreamBuffHandle_t) streamBuffHandle, rxData, dataLengthBytes, portMAX_DELAY);
    }

    return bytesRead;    // Exit: no errors
}

/**
 * \brief Reset a stream buffer to its initial empty state
 * \param osalFreertos      - pointer to OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer being reset
 * \return UShellOsalErr_e error code.
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffReset(void* const osalFreertos,
                                                         const UShellOsalStreamBuffHandle_t streamBuffHandle)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);

    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Reset the stream buffer
    BaseType_t resetStatus = xStreamBufferReset((UShellOsalStreamBuffHandle_t) streamBuffHandle);
    if (pdPASS != resetStatus)
    {
        // If there was a task blocked waiting to send to
        // or read from the stream buffer then the stream buffer will not be reset and pdFAIL is returned.
        return USHELL_OSAL_STREAM_BUFF_RESET_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Receive data from the stream buffer with specified waiting time
 */
static UShellOsalErr_e uShellOsalFreertosStreamBuffIsEmpty(void* const osalFreertos,
                                                           const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                                           bool* const isEmpty)
{
    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);

    // Find handle
    uint16_t streamBuffIndexNum = uShellOsalFreertosFindStreamBuffHandle(osalFreertos, streamBuffHandle);
    if (0 == streamBuffIndexNum)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    if (USHELL_OSAL_STREAM_BUFF_SLOTS_NUM < streamBuffIndexNum)
    {
        return USHELL_OSAL_PORT_SPECIFIC_ERR;
    }

    // Reset the stream buffer
    BaseType_t resetStatus = xStreamBufferIsEmpty((UShellOsalStreamBuffHandle_t) streamBuffHandle);

    // Return the result
    *isEmpty = (pdTRUE == resetStatus) ? true : false;

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Find the queue handle in the queue handles table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] queueHandle  - queue handle to be found
 * \return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t uShellOsalFreertosFindQueueHandle(UShellOsalFreertos_s* const osalFreeRtos,
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
 * \brief Find the lock object handle in the lock objects table
 * \param[in] osalFreeRtos  - pointer to FreeRTOS osal instance
 * \param[in] lockObjHandle - lock object handle to be found
 * \return uint16_t handle index + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t uShellOsalFreertosFindLockObjHandle(UShellOsalFreertos_s* const osalFreeRtos,
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
 * \brief Find the thread handle in the thread objects table
 * \param[in] osalFreeRtos - pointer to FreeRTOS osal instance
 * \param[in] threadHandle - thread handle to be found
 * \return uint16_t handleIndex + 1, 0 - if the handle wasn't found.
 */
static inline uint16_t uShellOsalFreertosFindThreadHandle(UShellOsalFreertos_s* const osalFreeRtos,
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
 * \brief Find the timer handle in the timer objects table
 */
static inline uint16_t uShellOsalFreertosFindTimerHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                         const UShellOsalTimerHandle_t timerHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);    // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);     // Must be validated by the caller

    UShellOsal_s* osal = (UShellOsal_s*) osalFreeRtos;
    uint16_t handleIndex = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_TIMER_NUM; i++)
    {
        if (timerHandle == osal->timerObj [i].timerHandle)
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}

/**
 * \brief Create the timer
 * \param osalFreertos  - pointer to FreeRTOS osal instance
 * \param timerHandle   - pointer to the timer handle that was created
 * \param timerCfg      - timer configuration structure
 * \return UShellOsalErr_e error code
 */
static UShellOsalErr_e uShellOsalFreertosTimerCreate(void* const osalFreertos,
                                                     UShellOsalTimerHandle_t* const timerHandle,
                                                     const UShellOsalTimerCfg_s timerCfg)
{
    /* Check the income parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);

    /* Check the level at which the function was called */
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    /* Check the given timer parameters */
    if ((timerCfg.timerExpiredCb == NULL) ||
        (timerCfg.timerParam == NULL))
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Create the FreeRTOS timer */
    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    /* Check if there is a free slot */
    *timerHandle = NULL;    // clear stored object

    /* Check if there is a free slot */
    for (int i = 0; i < USHELL_OSAL_TIMER_NUM; i++)
    {
        if (NULL == osal->timerObj [i].timerHandle)
        {
            /* Copy the timer configuration */
            osal->timerObj [i].timerCfg = timerCfg;

            /* Congratulations! The free slot was found, create the timer */
            osal->timerObj [i].timerHandle = xTimerCreate(timerCfg.name,
                                                          pdMS_TO_TICKS(timerCfg.periodMs),
                                                          timerCfg.autoReloadState,
                                                          (void*) osalFreertos,
                                                          uShellOsalFreertosTimerCallback);
            if (NULL == osal->timerObj [i].timerHandle)
            {
                /* Timer was not created and must not be used */
                return USHELL_OSAL_TIMER_MEM_ALLOCATION_ERR;
            }

            /* We can only get there if the timer was created */
            /* No additional checks needed */
            *timerHandle = osal->timerObj [i].timerHandle;
            /* So break the loop */
            break;
        }
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Delete the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerDelete(void* const osalFreertos,
                                                     const UShellOsalTimerHandle_t timerHandle)
{
    /* Check the income parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);

    /* Check the level at which the function was called */
    if (xPortIsInsideInterrupt())
    {
        return USHELL_OSAL_CALL_FROM_ISR_ERR;
    }

    UShellOsal_s* osal = (UShellOsal_s*) osalFreertos;

    uint16_t timerObjIndex = uShellOsalFreertosFindTimerHandle(osalFreertos, timerHandle);
    if (0 == timerObjIndex)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Delete the timer */
    xTimerDelete((TimerHandle_t) timerHandle, 0U);
    /* Clear the timer object slot in the table */
    osal->timerObj [timerObjIndex - 1].timerHandle = NULL;    // Subtract 1 because find function increases actual index by 1

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Start the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerStart(void* const osalFreertos,
                                                    const UShellOsalTimerHandle_t timerHandle)
{
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);

    BaseType_t status = pdFALSE;
    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Start the timer from ISR
        status = xTimerStartFromISR(timerHandle, &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        status = xTimerStart(timerHandle, 0);
    }

    if (pdTRUE != status)
    {
        return USHELL_OSAL_TIMER_START_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Stop the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerStop(void* const osalFreertos,
                                                   const UShellOsalTimerHandle_t timerHandle)
{
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);

    BaseType_t status = pdFALSE;
    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Stop the timer from ISR
        status = xTimerStopFromISR(timerHandle, &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        status = xTimerStop(timerHandle, 0);
    }

    if (pdTRUE != status)
    {
        return USHELL_OSAL_TIMER_STOP_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Reset the timer
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerReset(void* const osalFreertos,
                                                    const UShellOsalTimerHandle_t timerHandle)
{
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);

    BaseType_t status = pdFALSE;
    // Check the level at which the function was called
    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // Reset the timer from ISR
        status = xTimerResetFromISR(timerHandle, &xHigherPriorityTaskWoken);
        // and switch context
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        status = xTimerReset(timerHandle, 0);
    }

    if (pdTRUE != status)
    {
        return USHELL_OSAL_TIMER_RESET_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Set the timer period
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] timerHandle - the handle to the timer that was created
 * \param[in] periodMs - the period of the timer in milliseconds
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalFreertosTimerSetPeriod(void* const osalFreertos,
                                                        const UShellOsalTimerHandle_t timerHandle,
                                                        const uint32_t periodMs)
{
    /* Check the income parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreertos);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != timerHandle);

    /* Find the index of the timer handle */
    uint16_t timerHandleIndex = uShellOsalFreertosFindTimerHandle(osalFreertos, timerHandle);

    /* If the timer handle index is 0, exit the function */
    if (timerHandleIndex == 0)
    {
        return USHELL_OSAL_INVALID_ARGS;
    }

    /* Adjust the index to match the 0-based indexing of the array */
    timerHandleIndex--;

    /* Safe timeout in ticks */
    TickType_t safeTimeoutInTicks = uShellOsalFreertosSafeTimeoutToTicks(periodMs);

    /* Check level at which the function was called */
    BaseType_t isInsideInterrupt = xPortIsInsideInterrupt();

    /* Set the timer period */
    BaseType_t status = pdFALSE;
    if (isInsideInterrupt)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        status = xTimerChangePeriodFromISR(timerHandle, safeTimeoutInTicks, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    else
    {
        status = xTimerChangePeriod(timerHandle, safeTimeoutInTicks, 0);
    }

    if (pdTRUE != status)
    {
        return USHELL_OSAL_TIMER_SET_PERIOD_ERR;
    }

    return USHELL_OSAL_NO_ERR;    // Exit: no errors
}

/**
 * \brief Callback function for the FreeRTOS timer
 * \param[in] xTimer - the handle to the timer that was created
 * \return void
 */
static void uShellOsalFreertosTimerCallback(TimerHandle_t xTimer)
{
    /* Assert that the incoming timer is not NULL */
    USHELL_OSAL_FREERTOS_ASSERT(xTimer != NULL);

    /* Retrieve the timer parameters */
    UShellOsalFreertos_s* freertosTimerParameters = (UShellOsalFreertos_s*) pvTimerGetTimerID(xTimer);

    /* Find the index of the timer handle */
    uint16_t timerHandleIndex = uShellOsalFreertosFindTimerHandle(freertosTimerParameters, xTimer);

    /* If the timer handle index is 0, exit the function */
    if (timerHandleIndex == 0)
    {
        return;
    }

    /* Adjust the index to match the 0-based indexing of the array */
    timerHandleIndex--;

    /* Retrieve the timer callback function */
    UShellOsalTimerExpiredCb_f timerExpiredCallback = freertosTimerParameters->base.timerObj [timerHandleIndex].timerCfg.timerExpiredCb;

    /* If the timer callback function is not NULL, call it with the appropriate parameters */
    if (timerExpiredCallback != NULL)
    {
        timerExpiredCallback(freertosTimerParameters->base.timerObj [timerHandleIndex].timerCfg.timerParam);
    }
}

/**
 * \brief Create an event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupCreate(void* const osal,
                                                   UShellOsalEventGroupHandle_t* const eventGroupHandle)
{
    // Check income params with assertions because
    // they must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);

    /* Local variable */
    UShellOsal_s* thisOsal = (UShellOsal_s*) osal;
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;

    /* Crate the event group */
    do
    {
        /* Check status */
        if ((NULL == thisOsal) ||
            (NULL == eventGroupHandle))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        // Create the FreeRTOS event group:
        // 1. check if there is a free slot
        for (int i = 0; i < USHELL_OSAL_EVENT_GROUPS_NUM; i++)
        {
            if (NULL == thisOsal->eventGroupHandle [i])
            {
                // Congratulations!
                // The free slot was found, create the event group
                thisOsal->eventGroupHandle [i] = xEventGroupCreate();
                if (NULL == thisOsal->eventGroupHandle [i])
                {
                    // Event group was not created and must not be used
                    // Exit: error - the RAM required to hold the event group cannot be allocated
                    return USHELL_OSAL_EVENT_GROUP_MEM_ALLOCATION_ERR;
                }

                // We can only get there if the event group was created
                // No additional checks needed
                *eventGroupHandle = thisOsal->eventGroupHandle [i];
                // So break the loop
                break;
            }
        }

        // 2. Check if the event group was created
        if (NULL == *eventGroupHandle)
        {
            status = USHELL_OSAL_EVENT_GROUP_CREATE_ERR;    // Exit: error - no free slots in the table
            break;
        }

    } while (0);

    return status;    // Exit: no errors
}

/**
 * \brief Create an event group with a specific name.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \param[in] name Name of the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupDelete(void* const osal,
                                                   const UShellOsalEventGroupHandle_t eventGroupHandle)
{
    /* Check income params with assertions because
     * they must be validated by the caller */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);

    /* Local variable */
    UShellOsal_s* thisOsal = (UShellOsal_s*) osal;
    UShellOsalErr_e status = USHELL_OSAL_NO_ERR;
    uint16_t eventGroupIndex = 0;
    /* Delete the event group */
    do
    {
        /* Check input */
        if ((NULL == thisOsal) ||
            (NULL == eventGroupHandle))
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        // Check the level at which the function was called
        if (xPortIsInsideInterrupt())
        {
            status = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        // Find the event group
        eventGroupIndex = uShellOsalFreertosFindEventGroupHandle(osal, eventGroupHandle);
        if (0 == eventGroupIndex)
        {
            status = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        // Check if the event group index is valid
        if (USHELL_OSAL_EVENT_GROUPS_NUM < eventGroupIndex)
        {
            return USHELL_OSAL_PORT_SPECIFIC_ERR;
        }

        // Delete the event group
        vEventGroupDelete((EventGroupHandle_t) eventGroupHandle);

        // Clear the event group slot in the table
        thisOsal->eventGroupHandle [eventGroupIndex - 1] = NULL;    // Subtract 1 because find function increases actual index by 1

    } while (0);

    return status;    // Exit: no errors
}

/**
 * \brief Set a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToSet Bits to set in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupSetBits(void* const osal,
                                                    const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                    const UShellOsalEventGroupBits_e bitsToSet)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);

    /* Local variable */
    UShellOsalErr_e retVal = USHELL_OSAL_NO_ERR;
    uint16_t eventGroupIndex = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t statusFreertos = pdFALSE;

    /* Set bits */
    do
    {

        /* Check input */
        if ((osal == NULL) ||
            (eventGroupHandle == NULL))
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Find the event group */
        eventGroupIndex = uShellOsalFreertosFindEventGroupHandle(osal, eventGroupHandle);
        if (0 == eventGroupIndex)
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the event group index is valid */
        if (USHELL_OSAL_EVENT_GROUPS_NUM < eventGroupIndex)
        {
            retVal = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortIsInsideInterrupt())
        {
            /* Call from ISR */
            statusFreertos = xEventGroupSetBitsFromISR((EventGroupHandle_t) eventGroupHandle,
                                                       bitsToSet,
                                                       &xHigherPriorityTaskWoken);
            if (pdTRUE != statusFreertos)
            {
                retVal = USHELL_OSAL_EVENT_GROUP_SET_ERR;
                break;
            }

            /* Switch context */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        else
        {
            /* Call from task */
            xEventGroupSetBits((EventGroupHandle_t) eventGroupHandle, bitsToSet);
        }

    } while (0);

    return retVal;
}

/**
 * \brief Clear a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToClear Bits to clear in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupClearBits(void* const osal,
                                                      const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                      const UShellOsalEventGroupBits_e bitsToClear)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);

    /* Local variable */
    UShellOsalErr_e retVal = USHELL_OSAL_NO_ERR;
    uint16_t eventGroupIndex = 0;
    BaseType_t statusFreertos = pdFALSE;
    EventBits_t xResult = 0;

    /* Set bits */
    do
    {

        /* Check input */
        if ((osal == NULL) ||
            (eventGroupHandle == NULL))
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Find the event group */
        eventGroupIndex = uShellOsalFreertosFindEventGroupHandle(osal, eventGroupHandle);
        if (0 == eventGroupIndex)
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the event group index is valid */
        if (USHELL_OSAL_EVENT_GROUPS_NUM < eventGroupIndex)
        {
            retVal = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortIsInsideInterrupt())
        {
            /* Call from ISR */
            statusFreertos = xEventGroupClearBitsFromISR((EventGroupHandle_t) eventGroupHandle,
                                                         bitsToClear);
            if (pdTRUE != statusFreertos)
            {
                retVal = USHELL_OSAL_EVENT_GROUP_SET_ERR;
                break;
            }
        }
        else
        {
            /* Call from task */
            xResult = xEventGroupClearBits((EventGroupHandle_t) eventGroupHandle, bitsToClear);

            /* Check if all desired bits are cleared */
            if ((xResult & bitsToClear) != 0)
            {
                retVal = USHELL_OSAL_EVENT_GROUP_SET_ERR;
                break;
            }
        }

    } while (0);

    return retVal;
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
UShellOsalErr_e uShellOsalFreertosEventGroupBitsWait(void* const osal,
                                                     const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                     const UShellOsalEventGroupBits_e bitsToWait,
                                                     UShellOsalEventGroupBits_e* const bitsReceived,
                                                     const bool clearOnExit,
                                                     const bool waitAllBits)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != bitsReceived);

    /* Local variable */
    UShellOsalErr_e retVal = USHELL_OSAL_NO_ERR;
    uint16_t eventGroupIndex = 0;
    EventBits_t xResult = 0;

    /* Set bits */
    do
    {

        /* Check input */
        if ((osal == NULL) ||
            (eventGroupHandle == NULL) ||
            (bitsReceived == NULL))
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Find the event group */
        eventGroupIndex = uShellOsalFreertosFindEventGroupHandle(osal, eventGroupHandle);
        if (0 == eventGroupIndex)
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the event group index is valid */
        if (USHELL_OSAL_EVENT_GROUPS_NUM < eventGroupIndex)
        {
            retVal = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortIsInsideInterrupt())
        {
            retVal = USHELL_OSAL_CALL_FROM_ISR_ERR;
            break;
        }

        /* Call from task */
        xResult = xEventGroupWaitBits((EventGroupHandle_t) eventGroupHandle,
                                      bitsToWait,
                                      clearOnExit,
                                      waitAllBits,
                                      portMAX_DELAY);    // TODO: to change to 0 ?

        /* Check */
        if ((xResult & bitsToWait) == 0)
        {
            retVal = USHELL_OSAL_EVENT_GROUP_WAIT_ERR;
            break;
        }

        /* Set the received bits */
        *bitsReceived = xResult;

    }

    while (0);

    return retVal;
}

/**
 * \brief Get the active bits in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[out] bitsActive Pointer to store the active bits.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e uShellOsalFreertosEventGroupBitsActiveGet(void* const osal,
                                                          const UShellOsalEventGroupHandle_t eventGroupHandle,
                                                          UShellOsalEventGroupBits_e* const bitsActive)
{
    /* Check input parameters */
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osal);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);
    USHELL_OSAL_FREERTOS_ASSERT(NULL != bitsActive);

    /* Local variable */
    UShellOsalErr_e retVal = USHELL_OSAL_NO_ERR;
    uint16_t eventGroupIndex = 0;
    EventBits_t xResult = 0;

    /* Set bits */
    do
    {

        /* Check input */
        if ((osal == NULL) ||
            (eventGroupHandle == NULL) ||
            (bitsActive == NULL))
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Find the event group */
        eventGroupIndex = uShellOsalFreertosFindEventGroupHandle(osal, eventGroupHandle);
        if (0 == eventGroupIndex)
        {
            retVal = USHELL_OSAL_INVALID_ARGS;
            break;
        }

        /* Check if the event group index is valid */
        if (USHELL_OSAL_EVENT_GROUPS_NUM < eventGroupIndex)
        {
            retVal = USHELL_OSAL_PORT_SPECIFIC_ERR;
            break;
        }

        /* Check the level at which the function was called */
        if (xPortIsInsideInterrupt())
        {
            xResult = xEventGroupGetBitsFromISR((EventGroupHandle_t) eventGroupHandle);
        }
        else
        {

            /* Get current bits */
            xResult = xEventGroupGetBits((EventGroupHandle_t) eventGroupHandle);
        }

        /* Return active bits  */
        *bitsActive = xResult;

    }

    while (0);

    return retVal;
}

/**
 * \brief Find the event group handle in the event groups table
 */
static inline uint16_t uShellOsalFreertosFindEventGroupHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                              const UShellOsalEventGroupHandle_t eventGroupHandle)
{
    {
        // Check income parameters
        USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);        // Must be validated by the caller
        USHELL_OSAL_FREERTOS_ASSERT(NULL != eventGroupHandle);    // Must be validated by the caller

        UShellOsal_s* osal = (UShellOsal_s*) osalFreeRtos;
        uint16_t handleIndex = 0;

        // Try to find
        for (uint16_t i = 0; i < USHELL_OSAL_EVENT_GROUPS_NUM; i++)
        {
            if (eventGroupHandle == osal->eventGroupHandle [i])
            {
                handleIndex = i + 1;
                break;
            }
        }

        return handleIndex;
    }
}

/**
 * \brief Perform thread configuration parameters validation procedure
 *        in terms of the requirements of the FreeRTOS
 * \param[in] threadCfg - pointer to a thread configuration structure being checked
 * \return true - if configuration is OK\
 * \return false - if not
 */
static bool uShellOsalFreertosCheckParam(const UShellOsalThreadCfg_s* const threadCfg)
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

/**
 * \brief Find the stream buffer handle in the stream buffers table
 */
static inline uint16_t uShellOsalFreertosFindStreamBuffHandle(UShellOsalFreertos_s* const osalFreeRtos,
                                                              const UShellOsalStreamBuffHandle_t streamBuffHandle)
{
    // Check income parameters
    USHELL_OSAL_FREERTOS_ASSERT(NULL != osalFreeRtos);        // Must be validated by the caller
    USHELL_OSAL_FREERTOS_ASSERT(NULL != streamBuffHandle);    // Must be validated by the caller

    UShellOsal_s* osal = (UShellOsal_s*) osalFreeRtos;
    uint16_t handleIndex = 0;

    // Try to find
    for (uint16_t i = 0; i < USHELL_OSAL_STREAM_BUFF_SLOTS_NUM; i++)
    {
        if (streamBuffHandle == osal->streamBuffHandle [i])
        {
            handleIndex = i + 1;
            break;
        }
    }

    return handleIndex;
}