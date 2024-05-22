#ifndef USHELL_OSAL_H_
#define USHELL_OSAL_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>  // is used for size_t

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief UShell OSAL queue slots number
 */
#ifndef USHELL_OSAL_QUEUE_SLOTS_NUM
    #define USHELL_OSAL_QUEUE_SLOTS_NUM          (8)
#endif

/**
 * \brief UShell OSAL stream buffer slots number
 */
#ifndef USHELL_OSAL_STREAM_BUFF_SLOTS_NUM
    #define USHELL_OSAL_STREAM_BUFF_SLOTS_NUM    (8)
#endif

/**
 * \brief UShell OSAL lock objects number
 */
#ifndef USHELL_OSAL_LOCK_OBJS_NUM
    #define USHELL_OSAL_LOCK_OBJS_NUM            (8)
#endif

/**
 * \brief UShell OSAL semaphore objects number
 */
#ifndef USHELL_OSAL_SEMAPHORE_OBJS_NUM
    #define USHELL_OSAL_SEMAPHORE_OBJS_NUM       (8)
#endif

/**
 * \brief UShell OSAL threads number
 */
#ifndef USHELL_OSAL_THREADS_NUM
    #define USHELL_OSAL_THREADS_NUM              (8)
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief UShell OSAL errors
 */
typedef enum
{
    USHELL_OSAL_NO_ERR = 0x00,                      ///< No errors
    USHELL_OSAL_INVALID_ARGS,                       ///< Invalid arguments
    USHELL_OSAL_QUEUE_CREATE_ERR,                   ///< Queue creation error
    USHELL_OSAL_QUEUE_MEM_ALLOCATION_ERR,           ///< Queue memory allocation error
    USHELL_OSAL_STREAM_BUFF_CREATE_ERR,             ///< Stream buffer creation error
    USHELL_OSAL_STREAM_BUFF_MEM_ALLOCATION_ERR,     ///< Stream buffer memory allocation error
    USHELL_OSAL_STREAM_BUFF_RESET_ERR,              ///< Stream buffer reset error
    USHELL_OSAL_LOCK_OBJ_CREATE_ERR,                ///< Lock object creation error
    USHELL_OSAL_SEMAPHORE_OBJ_CREATE_ERR,           ///< Semaphore object creation error
    USHELL_OSAL_THREAD_CREATE_ERR,                  ///< Thread creation error
    USHELL_OSAL_QUEUE_OVERFLOW_ERR,                 ///< Queue overflow error
    USHELL_OSAL_SEMAPHORE_ACQUIRE_ERR,              ///< Semaphore acquire error
    USHELL_OSAL_SEMAPHORE_RELEASE_ERR,              ///< Semaphore release error
    USHELL_OSAL_CALL_FROM_ISR_ERR,                  ///< Call from ISR error
    USHELL_OSAL_PORT_SPECIFIC_ERR,                  ///< Port-specific error
    USHELL_OSAL_QUEUE_IS_EMPTY_ERR,                 ///< Queue is empty error
    USHELL_OSAL_LOCK_OBJ_MEM_ALLOCATION_ERR,        ///< Lock object memory allocation error
    USHELL_OSAL_SEMAPHORE_MEM_ALLOC_ERR,            ///< Semaphore memory allocation error
    USHELL_OSAL_THREAD_MEM_ALLOCATION_ERR,          ///< Thread memory allocation error
} UShellOsalErr_e;


/**
 * \brief UShell OSAL Queue handle type definition
 */
typedef void *UShellOsalQueueHandle_t;

/**
 * \brief UShell OSAL Stream buffers handle type definition
 * \note  This is not usual OS object type so check out the portable OS implementations to make sure whether its supported or not
 */
typedef void *UShellOsalStreamBuffHandle_t;

/**
 * \brief UShell OSAL lock object type definition
 */
typedef void *UShellOsalLockObjHandle_t;

/**
 * \brief UShell OSAL counting semaphore handle type definition
 */
typedef void *UShellOsalSemaphoreHandle_t;

/**
 * \brief UShell OSAL semaphore counter data type
 */
typedef uint32_t UShellOsalSemaphoreCount_t;

/**
 * \brief UShell OSAL run-time Thread handle type definition
 */
typedef void *UShellOsalThreadHandle_t;

/**
 * \brief UShell OSAL thread worker prototype
 * \note  threadParam is a parameter that is assigned in the structure UShellOsalThreadCfg_s
 *        when creating the thread
 */
typedef void (*UShellOsalThreadWorker_t)(void * const threadParam);

/**
 * \brief All specific OSAL implementations MUST support such priority levels
 * \note  It is recommended to create an array with the size arr[USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE]
 *        and put a OS-specific priority level into a specific array section
 */
typedef enum
{
    USHELL_OSAL_THREAD_PRIORITY_LOW = 0,
    USHELL_OSAL_THREAD_PRIORITY_MIDDLE,
    USHELL_OSAL_THREAD_PRIORITY_HIGH,
    USHELL_OSAL_THREAD_PRIORITY_ULTRA,
    USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE,  // is used only to create array with portable size
} UShellOsalThreadPriority_e;

/**
 * \brief UShell OSAL thread parameters structure
 */
typedef struct
{
    UShellOsalThreadWorker_t   threadWorker;
    const char                       *name;
    size_t                           stackSize; // in bytes
    void                             *threadParam;
    UShellOsalThreadPriority_e threadPriority;
} UShellOsalThreadCfg_s;

/**
 * \brief UShell OSAL thread object structure
 */
typedef struct
{
    UShellOsalThreadCfg_s    threadCfg;
    UShellOsalThreadHandle_t threadHandle;
} UShellOsalThread_s;


/**
 * \brief UShell OSAL interface methods prototypes for a particular RTOS port
 */
typedef struct
{
    // TODO: provide doxygen description for all portable functions
    UShellOsalErr_e (*queueCreate)(void* const osal, const size_t queueItemSize, const size_t queueDepth, UShellOsalQueueHandle_t* const queueHandle);
    UShellOsalErr_e (*queueDelete)(void* const osal, const UShellOsalQueueHandle_t queueHandle);
    UShellOsalErr_e (*queueItemPut)(void* const osal, const UShellOsalQueueHandle_t queueHandle, const void *const queueItemPtr);
    UShellOsalErr_e (*queueItemPost)(void* const osal, const UShellOsalQueueHandle_t queueHandle, void *const queueItemPtr, const uint32_t timeoutMs); // blocking function call with specified timeout
    UShellOsalErr_e (*queueItemGet)(void* const osal, const UShellOsalQueueHandle_t queueHandle, void *const queueItemPtr);  // non-blockcing function call
    UShellOsalErr_e (*queueItemWait)(void* const osal, const UShellOsalQueueHandle_t queueHandle, void *const queueItemPtr);  // blocking function call
    UShellOsalErr_e (*queueItemPend)(void* const osal, const UShellOsalQueueHandle_t queueHandle, void *const queueItemPtr, const uint32_t timeoutMs); // blocking function call with specified timeout
    UShellOsalErr_e (*queueReset)(void* const osal, const UShellOsalQueueHandle_t queueHandle);
    UShellOsalErr_e (*streamBuffCreate)(void* const osal, const size_t buffSizeBytes, const size_t triggerLevelBytes, UShellOsalStreamBuffHandle_t* const streamBuffHandle);
    UShellOsalErr_e (*streamBuffDelete)(void* const osal, const UShellOsalStreamBuffHandle_t streamBuffHandle);
    size_t (*streamBuffSend)(void* const osal, const UShellOsalStreamBuffHandle_t streamBuffHandle, const void *txData, const size_t dataLengthBytes);
    size_t (*streamBuffReceive)(void* const osal, const UShellOsalStreamBuffHandle_t streamBuffHandle, void* const rxData, const size_t dataLengthBytes, const uint32_t msToWait);
    UShellOsalErr_e (*streamBuffReset)(void* const osal, const UShellOsalStreamBuffHandle_t streamBuffHandle);
    UShellOsalErr_e (*lockObjCreate)(void* const osal, UShellOsalLockObjHandle_t* const lockObjHandle);
    UShellOsalErr_e (*lockObjDelete)(void* const osal, const UShellOsalLockObjHandle_t lockObjHandle);
    UShellOsalErr_e (*lock)(void* const osal, const UShellOsalLockObjHandle_t lockObjHandle);
    UShellOsalErr_e (*unlock)(void* const osal, const UShellOsalLockObjHandle_t lockObjHandle);
    UShellOsalErr_e (*semaphoreCreate)(void* const osal, const UShellOsalSemaphoreCount_t semaphoreCountMax, const UShellOsalSemaphoreCount_t semaphoreInitValue, UShellOsalSemaphoreHandle_t *const semaphoreHandle);
    UShellOsalErr_e (*semaphoreDelete)(void* const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle);
    UShellOsalErr_e (*semaphoreAcquire)(void* const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle);
    UShellOsalErr_e (*semaphoreRelease)(void* const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle);
    UShellOsalErr_e (*semaphoreCountGet)(void* const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle, UShellOsalSemaphoreCount_t *const semaphoreCount);
    UShellOsalErr_e (*threadCreate)(void* const osal, UShellOsalThreadHandle_t* const threadHandle, UShellOsalThreadCfg_s threadCfg);
    UShellOsalErr_e (*threadDelete)(void* const osal, const UShellOsalThreadHandle_t threadHandle);
    UShellOsalErr_e (*threadSuspend)(void* const osal, const UShellOsalThreadHandle_t threadHandle);
    UShellOsalErr_e (*threadResume)(void* const osal, const UShellOsalThreadHandle_t threadHandle);
    UShellOsalErr_e (*threadDelay)(const void* const osal, const uint32_t msDelay);
    UShellOsalErr_e (*criticalSectionEnter)(void* const osal);
    UShellOsalErr_e (*criticalSectionExit)(void* const osal);

} UShellOsalPortable_s;


/**
 * \brief UShell OSAL interface descriptor
 */
typedef struct
{
    /* Optional fields: */
    void       *parent;  // pointer to the parent object
    const char *name;    // name of the specific OSAL implementation

    /* Mandatory fields: */
    // Queues handles table
    UShellOsalQueueHandle_t      queueHandle[USHELL_OSAL_QUEUE_SLOTS_NUM];
    // Stream buffers handles table
    UShellOsalStreamBuffHandle_t streamBuffHandle[USHELL_OSAL_STREAM_BUFF_SLOTS_NUM];
    // Lock objects handles table
    UShellOsalLockObjHandle_t    lockObjHandle[USHELL_OSAL_LOCK_OBJS_NUM];
    // Counting semaphores
    UShellOsalSemaphoreHandle_t  semaphoreHandle[USHELL_OSAL_SEMAPHORE_OBJS_NUM];
    // Threads table
    UShellOsalThread_s           threadObj[USHELL_OSAL_THREADS_NUM];

    // Portable methods table
    const UShellOsalPortable_s *portable;

} UShellOsal_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize UShell OSAL instance
 *        Sets name, parent, clears all internal objects etc.
 * \param[in] osal - pointer to OSAL instance
 * \param[in] name - pointer to the name of the OSAL instance
 * \param[in] parent - pointer to a parent object
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalInit(UShellOsal_s *osal, const char *name, void *const parent);

/**
 * \brief Deinitialize UShell OSAL instance
 * \note  Call this function when all functionality has been stopped
 * \param[in] osal - pointer to OSAL instance
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s *osal);

/**
 * \brief Get pointer to a parent of the given OSAL object
 * \param[in]  osal      - pointer to osal instance which parent object will be returned
 * \param[out] parent    - pointer to an object into which the current osal parent pointer will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentGet(UShellOsal_s *const osal, void **const parent);

/**
 * \brief Set the parent object for the given OSAL instance
 * \param[in] osal      - pointer to osal instance being modified
 * \param[in] parent    - pointer to parent object being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s *const osal, void *const parent);

/**
 * \brief Get pointer to the name field of the given OSAL instance
 * \param[in] osal  - pointer to osal instance
 * \param[out] name  - pointer to an object into which the current osal name will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameGet(UShellOsal_s *const osal, const char **const name);

/**
 * \brief Set name for the given OSAL instance
 * \param[in] osal  - pointer to osal instance being modified
 * \param[in] name  - pointer to name string being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s *const osal, char *const name);

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
                                                                UShellOsalQueueHandle_t * const queueHandle);

/**
 * \brief Delete the queue
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueDelete(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Put item to the queue
 * \param[in] osal - OSAL descriptor;
 * \param[in] queueHandle   - the queue handle in which to put the item
 * \param[in] queueItemPtr  - pointer to the item source buffer
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemPut(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                 const void *const queueItemPtr);

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
                                                                  const uint32_t timeoutMs);

/**
 * \brief Get item from the queue
 * \note  (NON-BLOCKING CALL)
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle in which to put the item
 * \param[out]  queueItemPtr  - pointer to the destination buffer in which the item should be places
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemGet(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                 void *const queueItemPtr);

/**
 * \brief Get item from the queue
 * \note  (BLOCKING CALL WITH INFINITE WAIT)
 * \param[in]  osal - OSAL descriptor;
 * \param[in]  queueHandle   - the queue handle in which to put the item
 * \param[out] queueItemPtr  - pointer to the destination buffer in which the item should be places
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemWait(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle,
                                                                  void *const queueItemPtr);

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
                                                                   const uint32_t timeoutMs);

/**
 * @brief Reset queue
 * @param[in] osal          - pointer to OSAL instance
 * @param[in] queueHandle   - the queue handle to reset
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueReset(UShellOsal_s *const osal, const UShellOsalQueueHandle_t queueHandle);

/**
* \brief Enter critical section
* \param[in] osal - pointer to osal instance being modified
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalCriticalSectionEnter(UShellOsal_s *const osal);

/**
* \brief Exit critical section
* \param[in] osal - pointer to osal instance being modified
* \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalCriticalSectionExit(UShellOsal_s *const osal);


/**
 * \brief Create the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[out]  lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjCreate(UShellOsal_s *const osal, UShellOsalLockObjHandle_t * const lockObjHandle);

/**
 * \brief Delete the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjDelete(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Lock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLock(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Unlock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s *const osal, const UShellOsalLockObjHandle_t lockObjHandle);

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
                                                                    UShellOsalSemaphoreHandle_t *const semaphoreHandle);

/**
 * \brief Delete the semaphore object
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreDelete(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Acquire the semaphore
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle to acquire
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreAcquire(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Release the semaphore
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle to release
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreRelease(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle);

/**
 * \brief Get the current count of the semaphore
 * \param[in] osal              - pointer to OSAL instance
 * \param[in] semaphoreHandle   - semaphore object handle
 * \param[in] semaphoreCount    - the current count of the semaphore
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalSemaphoreCountGet(UShellOsal_s *const osal, const UShellOsalSemaphoreHandle_t semaphoreHandle, UShellOsalSemaphoreCount_t *const semaphoreCount);

/**
 * \brief Create the thread
 * \param[in]   osal - OSAL descriptor;
 * \param[out]  threadHandle  - thread handle by which created thread can be referenced
 * \param[in]   threadCfg     - thread configuration
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadCreate(UShellOsal_s *const osal, UShellOsalThreadHandle_t * const threadHandle, UShellOsalThreadCfg_s threadCfg);

/**
 * \brief Delete the thread
 * \note The operation must be stopped before deleting the thread
 *        to not to damage the system.
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   threadHandle  - the handle of the thread being deleted
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadDelete(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Suspend the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being suspend
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadSuspend(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Resume the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being resumed
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadResume(UShellOsal_s *const osal, const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief       Perform some delay
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   msDelay - the delay timeout value expressed in ms;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalThreadDelay(const UShellOsal_s* const osal, const uint32_t msDelay);

#ifdef __cplusplus
    }
#endif

#endif // ifdef USHELL_OSAL_H_