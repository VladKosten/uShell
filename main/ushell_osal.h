#ifndef USHELL_OSAL_H_
#define USHELL_OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "ushell_cfg.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief UShell OSAL queue slots number.
 *
 * Defines the number of queue slots available in the UShell OSAL.
 */
#ifndef USHELL_OSAL_QUEUE_SLOTS_NUM
    #define USHELL_OSAL_QUEUE_SLOTS_NUM (2)
#endif

/**
 * \brief UShell OSAL lock objects number.
 *
 * Defines the number of lock objects available in the UShell OSAL.
 */
#ifndef USHELL_OSAL_LOCK_OBJS_NUM
    #define USHELL_OSAL_LOCK_OBJS_NUM (2)
#endif

/**
 * \brief UShell OSAL threads number.
 *
 * Defines the number of threads available in the UShell OSAL.
 */
#ifndef USHELL_OSAL_THREADS_NUM
    #define USHELL_OSAL_THREADS_NUM (1)
#endif

/**
 * \brief UShell OSAL stream buffer slots number
 */
#ifndef USHELL_OSAL_STREAM_BUFF_SLOTS_NUM
    #define USHELL_OSAL_STREAM_BUFF_SLOTS_NUM (6)
#endif

/**
 * \brief UShell OSAL timer number
 */
#ifndef USHELL_OSAL_TIMER_NUM
    #define USHELL_OSAL_TIMER_NUM (1)
#endif

/**
 * \brief UShell OSAL event groups number.
 */
#ifndef USHELL_OSAL_EVENT_GROUPS_NUM
    #define USHELL_OSAL_EVENT_GROUPS_NUM (1)
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief UShell OSAL errors.
 *
 * This enumeration defines the possible error codes that can be returned by
 * the UShell OSAL (Operating System Abstraction Layer) functions.
 */
typedef enum
{
    /**
     * \brief No errors.
     *
     * Indicates that the operation completed successfully without any errors.
     */
    USHELL_OSAL_NO_ERR = 0x00,

    /**
     * \brief Invalid arguments.
     *
     * Indicates that one or more arguments passed to the function were invalid.
     */
    USHELL_OSAL_INVALID_ARGS,

    /**
     * \brief Port-specific error.
     *
     * Indicates that an error occurred in the underlying port layer implementation.
     */
    USHELL_OSAL_PORT_SPECIFIC_ERR,

    /**
     * \brief Call from ISR error.
     *
     * Indicates that a function was called from an ISR (Interrupt Service Routine),
     * which is not allowed.
     */
    USHELL_OSAL_CALL_FROM_ISR_ERR,

    /**
     * \brief Queue creation error.
     *
     * Indicates that an error occurred while creating a queue.
     */
    USHELL_OSAL_QUEUE_CREATE_ERR,

    /**
     * \brief Queue memory allocation error.
     *
     * Indicates that an error occurred while allocating memory for a queue.
     */
    USHELL_OSAL_QUEUE_MEM_ALLOCATION_ERR,

    /**
     * \brief Queue overflow error.
     *
     * Indicates that an error occurred due to a queue overflow.
     */
    USHELL_OSAL_QUEUE_OVERFLOW_ERR,

    /**
     * \brief Queue is empty error.
     *
     * Indicates that an error occurred because the queue is empty.
     */
    USHELL_OSAL_QUEUE_IS_EMPTY_ERR,

    /**
     * \brief Lock object creation error.
     *
     * Indicates that an error occurred while creating a lock object.
     */
    USHELL_OSAL_LOCK_OBJ_CREATE_ERR,

    /**
     * \brief Lock object memory allocation error.
     *
     * Indicates that an error occurred while allocating memory for a lock object.
     */
    USHELL_OSAL_LOCK_OBJ_MEM_ALLOCATION_ERR,

    /**
     * \brief Thread creation error.
     *
     * Indicates that an error occurred while creating a thread.
     */
    USHELL_OSAL_THREAD_CREATE_ERR,

    /**
     * \brief Thread memory allocation error.
     *
     * Indicates that an error occurred while allocating memory for a thread.
     */
    USHELL_OSAL_THREAD_MEM_ALLOCATION_ERR,

    /**
     * \brief Create stream buffer error.
     *
     * Indicates that an error occurred while creating a stream buffer.
     */
    USHELL_OSAL_STREAM_BUFF_CREATE_ERR,

    /**
     * \brief Memory allocation error for stream buffer.
     *
     * Indicates that an error occurred while allocating memory for a stream buffer.
     */
    USHELL_OSAL_STREAM_BUFF_MEM_ALLOCATION_ERR,

    /**
     * \brief Stream buffer overflow error.
     *
     * Indicates that an error occurred due to a stream buffer overflow.
     */
    USHELL_OSAL_STREAM_BUFF_RESET_ERR,

    /**
     * \brief Timer create error.
     *
     * Indicates that an error occurred while creating a timer.
     */
    USHELL_OSAL_TIMER_MEM_ALLOCATION_ERR,

    /**
     * \brief Timer start error.
     *
     * Indicates that an error occurred while starting a timer.
     */
    USHELL_OSAL_TIMER_START_ERR,

    /**
     * \brief Timer stop error.
     *
     * Indicates that an error occurred while stopping a timer.
     */
    USHELL_OSAL_TIMER_STOP_ERR,

    /**
     * \brief Timer reset error.
     *
     * Indicates that an error occurred while resetting a timer.
     */
    USHELL_OSAL_TIMER_RESET_ERR,

    /**
     * \brief Timer set period error.
     *
     * Indicates that an error occurred while setting the period of a timer.
     */
    USHELL_OSAL_TIMER_SET_PERIOD_ERR,

    /**
     * \brief Event group creation error.
     *
     * Indicates that an error occurred while creating an event group.
     *
     */
    USHELL_OSAL_EVENT_GROUP_CREATE_ERR,

    /**
     * \brief Event group memory allocation error.
     *
     * Indicates that an error occurred while allocating memory for an event group.
     */
    USHELL_OSAL_EVENT_GROUP_MEM_ALLOCATION_ERR,

    /**
     * \brief Event group set error.
     *
     * Indicates that an error occurred while setting an event in the event group.
     */
    USHELL_OSAL_EVENT_GROUP_SET_ERR,

    /**
     * \brief Event group clear error.
     *
     * Indicates that an error occurred while clearing an event in the event group.
     */
    USHELL_OSAL_EVENT_GROUP_CLEAR_ERR,

    /**
     * \brief Event group wait error.
     *
     * Indicates that an error occurred while waiting for an event in the event group.
     */
    USHELL_OSAL_EVENT_GROUP_WAIT_ERR,

} UShellOsalErr_e;

/**
 * \brief UShell OSAL Queue handle type definition.
 *
 * This type defines a handle for a queue in the UShell OSAL.
 */
typedef void* UShellOsalQueueHandle_t;

/**
 * \brief UShell OSAL lock object type definition.
 *
 * This type defines a handle for a lock object in the UShell OSAL.
 */
typedef void* UShellOsalLockObjHandle_t;
/**
 * \brief UShell OSAL time in milliseconds type definition.
 *
 * This type defines the data type used for representing time in milliseconds
 * in the UShell Operating System Abstraction Layer (OSAL).
 */
typedef uint32_t UShellOsalTimeMs_t;

/**
 * \briefUShell OSAL Stream buffers handle type definition
 * \note  This is not usual OS object type so check out the portable OS implementations to make sure whether its supported or not
 */
typedef void* UShellOsalStreamBuffHandle_t;

/**
 * \brief UShell OSAL run-time Thread handle type definition.
 *
 * This type defines a handle for a thread in the UShell OSAL.
 */
typedef void* UShellOsalThreadHandle_t;

/**
 * \brief UShell Osal Timer handle type definition
 */
typedef void* UShellOsalTimerHandle_t;

/**
 * \brief UShell Osal Timer auto reload state type definition
 */
typedef bool UShellOsalTimerAutoReloadState_b;

/**
 * \brief Event group handle type definition.
 *
 * This type defines a handle for an event group in the UShell OSAL.s
 */
typedef void* UShellOsalEventGroupHandle_t;

/**
 * \brief UShell OSAL thread worker prototype.
 *
 * This type defines the prototype for a thread worker function in the UShell OSAL.
 *
 * \note threadParam is a parameter that is assigned in the structure UShellOsalThreadCfg_s
 *       when creating the thread.
 *
 * \param[in] threadParam Pointer to the parameter passed to the thread worker function.
 */
typedef void (*UShellOsalThreadWorker_t)(void* const threadParam);

/**
 * \brief UShell OSAL event group bits enumeration.
 */
typedef enum
{
    USHELL_OSAL_EVENT_GROUP_BIT_NONE = 0x00, /**< No event group bits set */

    USHELL_OSAL_EVENT_GROUP_BIT_0 = 0x01, /**< Event group bit 0 */
    USHELL_OSAL_EVENT_GROUP_BIT_1 = 0x02, /**< Event group bit 1 */
    USHELL_OSAL_EVENT_GROUP_BIT_2 = 0x04, /**< Event group bit 2 */
    USHELL_OSAL_EVENT_GROUP_BIT_3 = 0x08, /**< Event group bit 3 */
    USHELL_OSAL_EVENT_GROUP_BIT_4 = 0x10, /**< Event group bit 4 */
    USHELL_OSAL_EVENT_GROUP_BIT_5 = 0x20, /**< Event group bit 5 */
    USHELL_OSAL_EVENT_GROUP_BIT_6 = 0x40, /**< Event group bit 6 */
    USHELL_OSAL_EVENT_GROUP_BIT_7 = 0x80, /**< Event group bit 7 */

    /* Add more bits as needed */

    USHELL_OSAL_EVENT_GROUP_BIT_ALL = 0xFFFFFFFF,

} UShellOsalEventGroupBits_e;

/**
 * \brief Enumeration of thread priority levels for UShell OSAL.
 *
 * This enumeration defines the different priority levels that must be supported
 * by all specific OSAL implementations.
 *
 * \note It is recommended to create an array with the size `arr[USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE]`
 *       and put an OS-specific priority level into a specific array section.
 */
typedef enum
{
    /**
     * \brief Low priority level.
     *
     * Represents the lowest priority level for a thread.
     */
    USHELL_OSAL_THREAD_PRIORITY_LOW = 0,

    /**
     * \brief Middle priority level.
     *
     * Represents a middle priority level for a thread.
     */
    USHELL_OSAL_THREAD_PRIORITY_MIDDLE,

    /**
     * \brief High priority level.
     *
     * Represents a high priority level for a thread.
     */
    USHELL_OSAL_THREAD_PRIORITY_HIGH,

    /**
     * \brief Ultra priority level.
     *
     * Represents the highest priority level for a thread.
     */
    USHELL_OSAL_THREAD_PRIORITY_ULTRA,

    /**
     * \brief The last priority level.
     *
     * This value is used only to create an array with a portable size.
     */
    USHELL_OSAL_THREAD_PRIORITY_THE_LAST_ONE,

} UShellOsalThreadPriority_e;

/**
 * \brief UShell OSAL thread parameters structure.
 *
 * This structure defines the parameters required to configure and create a thread
 * in the UShell Operating System Abstraction Layer (OSAL).
 */
typedef struct
{
    /**
     * \brief Thread worker function.
     *
     * This function is the entry point for the thread. It is called when the thread starts.
     */
    UShellOsalThreadWorker_t threadWorker;

    /**
     * \brief Name of the thread.
     *
     * This is a pointer to a string that contains the name of the thread.
     */
    const char* name;

    /**
     * \brief Stack size of the thread.
     *
     * This specifies the size of the stack for the thread, in bytes.
     */
    size_t stackSize;

    /**
     * \brief Parameter for the thread worker function.
     *
     * This is a pointer to the parameter that will be passed to the thread worker function.
     */
    void* threadParam;

    /**
     * \brief Priority of the thread.
     *
     * This specifies the priority level of the thread.
     */
    UShellOsalThreadPriority_e threadPriority;

} UShellOsalThreadCfg_s;

/**
 * \brief UShell OSAL timer expired callback prototype.
 *
 * This callback is invoked when the OSAL timer expires.
 *
 * \param[in] timerParam  Parameter assigned in UShellOsalTimerCfg_s.
 */
typedef void (*UShellOsalTimerExpiredCb_f)(void* const timerParam);

/**
 * \brief UShell OSAL timer parameters structure.
 *
 * This structure holds the configuration required for an OSAL timer.
 */
typedef struct
{
    const char* name;                                    ///< Timer name
    void* timerParam;                                    ///< Parameter passed to the timer callback
    UShellOsalTimerExpiredCb_f timerExpiredCb;           ///< Timer expiration callback
    UShellOsalTimerAutoReloadState_b autoReloadState;    ///< Auto-reload setting (true or false)
    UShellOsalTimeMs_t periodMs;                         ///< Period in milliseconds
} UShellOsalTimerCfg_s;

/**
 * \brief UShell OSAL timer object structure.
 *
 * This structure associates the timer configuration with the underlying timer handle.
 */
typedef struct
{
    UShellOsalTimerCfg_s timerCfg;          ///< Timer configuration
    UShellOsalTimerHandle_t timerHandle;    ///< OS-specific timer handle
} UShellOsalTimer_s;

/**
 * \brief UShell OSAL thread object structure.
 *
 * This structure defines a thread object in the UShell Operating System Abstraction Layer (OSAL).
 * It contains the configuration parameters and handle for the thread.
 */
typedef struct
{
    /**
     * \brief Thread configuration.
     *
     * This field contains the configuration parameters for the thread.
     */
    UShellOsalThreadCfg_s threadCfg;

    /**
     * \brief Thread handle.
     *
     * This field contains the handle for the thread.
     */
    UShellOsalThreadHandle_t threadHandle;

} UShellOsalThread_s;

/**
 * \brief UShell OSAL interface methods prototypes for a particular RTOS port.
 *
 * This structure defines the function pointers for operations needed to port
 * the UShell Operating System Abstraction Layer (OSAL) to different platforms.
 */
typedef struct
{
    /**
     * \brief Create a queue.
     *
     * This function creates a queue with the specified item size and depth.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueItemSize Size of each item in the queue.
     * \param[in] queueDepth Depth of the queue.
     * \param[out] queueHandle Pointer to store the handle of the created queue.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueCreate)(void* const osal,
                                   const size_t queueItemSize,
                                   const size_t queueDepth,
                                   UShellOsalQueueHandle_t* const queueHandle);

    /**
     * \brief Delete a queue.
     *
     * This function deletes the specified queue.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue to be deleted.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueDelete)(void* const osal,
                                   const UShellOsalQueueHandle_t queueHandle);

    /**
     * \brief Put an item into a queue.
     *
     * This function puts an item into the specified queue.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue.
     * \param[in] queueItemPtr Pointer to the item to be put into the queue.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueItemPut)(void* const osal,
                                    const UShellOsalQueueHandle_t queueHandle,
                                    const void* const queueItemPtr);

    /**
     * \brief Post an item into a queue with timeout.
     *
     * This function posts an item into the specified queue, blocking for the specified timeout if necessary.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue.
     * \param[in] queueItemPtr Pointer to the item to be posted into the queue.
     * \param[in] timeoutMs Timeout in milliseconds.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueItemPost)(void* const osal,
                                     const UShellOsalQueueHandle_t queueHandle,
                                     void* const queueItemPtr,
                                     const UShellOsalTimeMs_t time);

    /**
     * \brief Get an item from a queue (non-blocking).
     *
     * This function gets an item from the specified queue without blocking.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue.
     * \param[out] queueItemPtr Pointer to store the retrieved item.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueItemGet)(void* const osal,
                                    const UShellOsalQueueHandle_t queueHandle,
                                    void* const queueItemPtr);

    /**
     * \brief Wait for an item from a queue (blocking).
     *
     * This function waits for an item from the specified queue, blocking until an item is available.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue.
     * \param[out] queueItemPtr Pointer to store the retrieved item.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueItemWait)(void* const osal,
                                     const UShellOsalQueueHandle_t queueHandle,
                                     void* const queueItemPtr);

    /**
     * \brief Pend for an item from a queue with timeout.
     *
     * This function pends for an item from the specified queue, blocking for the specified timeout if necessary.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue.
     * \param[out] queueItemPtr Pointer to store the retrieved item.
     * \param[in] timeoutMs Timeout in milliseconds.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueItemPend)(void* const osal,
                                     const UShellOsalQueueHandle_t queueHandle,
                                     void* const queueItemPtr,
                                     const UShellOsalTimeMs_t timeoutMs);

    /**
     * \brief Reset a queue.
     *
     * This function resets the specified queue.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] queueHandle Handle of the queue to be reset.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*queueReset)(void* const osal,
                                  const UShellOsalQueueHandle_t queueHandle);

    /**
     * \brief Create a lock object.
     *
     * This function creates a lock object.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[out] lockObjHandle Pointer to store the handle of the created lock object.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*lockObjCreate)(void* const osal,
                                     UShellOsalLockObjHandle_t* const lockObjHandle);

    /**
     * \brief Delete a lock object.
     *
     * This function deletes the specified lock object.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] lockObjHandle Handle of the lock object to be deleted.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*lockObjDelete)(void* const osal,
                                     const UShellOsalLockObjHandle_t lockObjHandle);

    /**
     * \brief Acquire a lock.
     *
     * This function acquires the specified lock.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] lockObjHandle Handle of the lock object to be acquired.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*lock)(void* const osal,
                            const UShellOsalLockObjHandle_t lockObjHandle);

    /**
     * \brief Release a lock.
     *
     * This function releases the specified lock.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] lockObjHandle Handle of the lock object to be released.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*unlock)(void* const osal,
                              const UShellOsalLockObjHandle_t lockObjHandle);

    /**
     * \brief Create a thread.
     *
     * This function creates a thread with the specified configuration.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[out] threadHandle Pointer to store the handle of the created thread.
     * \param[in] threadCfg Configuration parameters for the thread.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*threadCreate)(void* const osal,
                                    UShellOsalThreadHandle_t* const threadHandle,
                                    UShellOsalThreadCfg_s threadCfg);

    /**
     * \brief Delete a thread.
     *
     * This function deletes the specified thread.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] threadHandle Handle of the thread to be deleted.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*threadDelete)(void* const osal,
                                    const UShellOsalThreadHandle_t threadHandle);

    /**
     * \brief Suspend a thread.
     *
     * This function suspends the specified thread.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] threadHandle Handle of the thread to be suspended.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*threadSuspend)(void* const osal,
                                     const UShellOsalThreadHandle_t threadHandle);

    /**
     * \brief Resume a thread.
     *
     * This function resumes the specified thread.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] threadHandle Handle of the thread to be resumed.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*threadResume)(void* const osal,
                                    const UShellOsalThreadHandle_t threadHandle);

    /**
     * \brief Delay a thread.
     *
     * This function delays the execution of the current thread for the specified time.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] msDelay Delay time in milliseconds.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*threadDelay)(const void* const osal,
                                   const UShellOsalTimeMs_t delay);

    /**
     * \brief Stream buffer create.
     *
     * This function creates a stream buffer with the specified size and trigger level.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] buffSizeBytes Size of the stream buffer in bytes.
     * \param[in] triggerLevelBytes Trigger level of the stream buffer in bytes.
     * \param[out] streamBuffHandle Pointer to store the handle of the created stream buffer.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*streamBuffCreate)(void* const osal,
                                        const size_t buffSizeBytes,
                                        const size_t triggerLevelBytes,
                                        UShellOsalStreamBuffHandle_t* const streamBuffHandle);

    /**
     * \brief Delete a stream buffer.
     *
     * This function deletes the specified stream buffer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] streamBuffHandle Handle of the stream buffer to be deleted.
     * \return Error code indicating the result of the operation.
     *
     */
    UShellOsalErr_e (*streamBuffDelete)(void* const osal, const UShellOsalStreamBuffHandle_t streamBuffHandle);

    /**
     * \brief Send data to a stream buffer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] streamBuffHandle Handle of the stream buffer.
     * \param[in] txData Pointer to the data to be sent.
     * \param[in] dataLengthBytes Length of the data to be sent in bytes.
     * \param[in] msToWait Timeout in milliseconds.
     *
     */
    size_t (*streamBuffSend)(void* const osal,
                             const UShellOsalStreamBuffHandle_t streamBuffHandle,
                             const void* txData,
                             const size_t dataLengthBytes,
                             const uint32_t msToWait);

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
    size_t (*streamBuffSendBlocking)(void* const osal,
                                     const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                     const void* txData,
                                     const size_t dataLengthBytes);

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
    size_t (*streamBuffReceive)(void* const osal,
                                const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                void* const rxData,
                                const size_t dataLengthBytes,
                                const uint32_t msToWait);

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
    size_t (*streamBuffReceiveBlocking)(void* const osal,
                                        const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                        void* const rxData,
                                        const size_t dataLengthBytes);

    /**
     * \brief Reset a stream buffer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] streamBuffHandle Handle of the stream buffer to be reset.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*streamBuffReset)(void* const osal,
                                       const UShellOsalStreamBuffHandle_t streamBuffHandle);

    /**
     * \brief Stream buffer is empty.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] streamBuffHandle Handle of the stream buffer.
     * \param[out] isEmpty Pointer to store the result indicating if the stream buffer is empty.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*streamBuffIsEmpty)(void* const osal,
                                         const UShellOsalStreamBuffHandle_t streamBuffHandle,
                                         bool* const isEmpty);

    /**
     * \brief Create a timer.
     *
     * This function creates a timer with the specified configuration.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[out] timerHandle Pointer to store the handle of the created timer.
     * \param[in] timerCfg Configuration parameters for the timer.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*timerCreate)(void* const osal,
                                   UShellOsalTimerHandle_t* const timerHandle,
                                   UShellOsalTimerCfg_s timerCfg);

    /**
     * \brief Delete a timer.
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] timerHandle Handle of the timer to be deleted.
     * \return Error code indicating the result of the operation.
     * \note  This function should be called when the timer is not used anymore.
     */
    UShellOsalErr_e (*timerDelete)(void* const osal,
                                   const UShellOsalTimerHandle_t timerHandle);

    /**
     * \brief Start a timer.
     *
     * This function starts the specified timer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] timerHandle Handle of the timer to be started.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*timerStart)(void* const osal,
                                  const UShellOsalTimerHandle_t timerHandle);

    /**
     * \brief Stop a timer.
     *
     * This function stops the specified timer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] timerHandle Handle of the timer to be stopped.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*timerStop)(void* const osal,
                                 const UShellOsalTimerHandle_t timerHandle);

    /**
     * \brief Reset a timer.
     *
     * This function resets the specified timer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] timerHandle Handle of the timer to be reset.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*timerReset)(void* const osal,
                                  const UShellOsalTimerHandle_t timerHandle);

    /**
     * \brief Change the period of a timer.
     *
     * This function changes the period of the specified timer.
     *
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] timerHandle Handle of the timer.
     * \param[in] periodMs New period in milliseconds.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*timerPeriodChange)(void* const osal,
                                         const UShellOsalTimerHandle_t timerHandle,
                                         const UShellOsalTimeMs_t periodMs);

    /**
     * \brief Create an event group.
     * \param[in] osal Pointer to the OSAL instance.
     * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*eventGroupCreate)(void* const osal,
                                        UShellOsalEventGroupHandle_t* const eventGroupHandle);

    /**
     * \brief Create an event group with a specific name.
     * \param[in] osal Pointer to the OSAL instance.
     * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
     * \param[in] name Name of the event group.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*eventGroupDelete)(void* const osal,
                                        const UShellOsalEventGroupHandle_t eventGroupHandle);

    /**
     * \brief Set a bit in the event group.
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] eventGroupHandle Handle of the event group.
     * \param[in] bitsToSet Bits to set in the event group.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*eventGroupSetBits)(void* const osal,
                                         const UShellOsalEventGroupHandle_t eventGroupHandle,
                                         const UShellOsalEventGroupBits_e bitsToSet);

    /**
     * \brief Clear a bit in the event group.
     * \param[in] osal Pointer to the OSAL instance.
     * \param[in] eventGroupHandle Handle of the event group.
     * \param[in] bitsToClear Bits to clear in the event group.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*eventGroupClearBits)(void* const osal,
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
    UShellOsalErr_e (*eventGroupBitsWait)(void* const osal,
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
    UShellOsalErr_e (*eventGroupBitsActiveGet)(void* const osal,
                                               const UShellOsalEventGroupHandle_t eventGroupHandle,
                                               UShellOsalEventGroupBits_e* const bitsActive);

    /**
     * \brief Get the current time in milliseconds.
     * \param[in] osal Pointer to the OSAL instance.
     * \param[out] timeMs Pointer to store the current time in milliseconds.
     * \return Error code indicating the result of the operation.
     */
    UShellOsalErr_e (*timeMsGet)(void* const osal, UShellOsalTimeMs_t* const timeMs);

} UShellOsalPortable_s;

/**
 * \brief UShell OSAL interface descriptor.
 *
 * This structure defines the interface descriptor for the UShell Operating System Abstraction Layer (OSAL).
 * It includes optional fields for the parent object and name, as well as mandatory fields for queue handles,
 * lock object handles, thread objects, and portable methods.
 */
typedef struct
{
    /* Optional fields: */

    /**
     * \brief Pointer to the parent object.
     *
     * This pointer references the parent object that contains or uses this OSAL object.
     */
    void* parent;

    /**
     * \brief Name of the specific OSAL implementation.
     *
     * This is a pointer to a string that contains the name of the specific OSAL implementation.
     */
    const char* name;

    /* Mandatory fields: */

    /**
     * \brief Queues handles table.
     *
     * This array contains handles for the queues available in the OSAL.
     */
    UShellOsalQueueHandle_t queueHandle [USHELL_OSAL_QUEUE_SLOTS_NUM];

    /**
     * \brief Lock objects handles table.
     *
     * This array contains handles for the lock objects available in the OSAL.
     */
    UShellOsalLockObjHandle_t lockObjHandle [USHELL_OSAL_LOCK_OBJS_NUM];

    /**
     * \brief Threads table.
     *
     * This array contains the thread objects available in the OSAL.
     */
    UShellOsalThread_s threadObj [USHELL_OSAL_THREADS_NUM];

    /**
     * \brief Stream buffers handles table.
     *
     * This array contains handles for the stream buffers available in the OSAL.
     *
     */
    UShellOsalStreamBuffHandle_t streamBuffHandle [USHELL_OSAL_STREAM_BUFF_SLOTS_NUM];

    /**
     * \brief Event groups handles table.
     *
     * This array contains handles for the event groups available in the OSAL.
     */
    UShellOsalEventGroupHandle_t eventGroupHandle [USHELL_OSAL_EVENT_GROUPS_NUM];

    /**
     * \brief Timer objects handles table.
     *
     * This array contains handles for the timer objects available in the OSAL.
     * \note  The number of timers is limited by the USHELL_OSAL_TIMER_NUM constant.
     */
    UShellOsalTimer_s timerObj [USHELL_OSAL_TIMER_NUM];

    /**
     * \brief Portable methods table.
     *
     * This pointer references the table of portable methods needed to port the OSAL to different platforms.
     */
    const UShellOsalPortable_s* portable;

} UShellOsal_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

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
                               const UShellOsalPortable_s* portable);

/**
 * \brief Deinitialize UShell OSAL instance
 * \note  Call this function when all functionality has been stopped
 * \param[in] osal - pointer to OSAL instance
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s* osal);

/**
 * \brief Get pointer to a parent of the given OSAL object
 * \param[in]  osal      - pointer to osal instance which parent object will be returned
 * \param[out] parent    - pointer to an object into which the current osal parent pointer will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentGet(UShellOsal_s* const osal,
                                    void** const parent);

/**
 * \brief Set the parent object for the given OSAL instance
 * \param[in] osal      - pointer to osal instance being modified
 * \param[in] parent    - pointer to parent object being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s* const osal,
                                    void* const parent);

/**
 * \brief Get pointer to the name field of the given OSAL instance
 * \param[in] osal  - pointer to osal instance
 * \param[out] name  - pointer to an object into which the current osal name will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameGet(UShellOsal_s* const osal,
                                  const char** const name);

/**
 * \brief Set name for the given OSAL instance
 * \param[in] osal  - pointer to osal instance being modified
 * \param[in] name  - pointer to name string being set
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s* const osal,
                                  char* const name);

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
                                      UShellOsalQueueHandle_t* const queueHandle);

/**
 * \brief Delete the queue
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   queueHandle   - the queue handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueDelete(UShellOsal_s* const osal,
                                      const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Put item to the queue
 * \param[in] osal - OSAL descriptor;
 * \param[in] queueHandle   - the queue handle in which to put the item
 * \param[in] queueItemPtr  - pointer to the item source buffer
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueItemPut(UShellOsal_s* const osal,
                                       const UShellOsalQueueHandle_t queueHandle,
                                       const void* const queueItemPtr);

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
                                        const uint32_t timeoutMs);

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
                                       void* const queueItemPtr);

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
                                        void* const queueItemPtr);

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
                                        const uint32_t timeoutMs);

/**
 * \brief Reset queue
 * \param[in] osal          - pointer to OSAL instance
 * \param[in] queueHandle   - the queue handle to reset
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalQueueReset(UShellOsal_s* const osal,
                                     const UShellOsalQueueHandle_t queueHandle);

/**
 * \brief Create the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[out]  lockObjHandle - lock object handle that was created
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjCreate(UShellOsal_s* const osal,
                                        UShellOsalLockObjHandle_t* const lockObjHandle);

/**
 * \brief Delete the lock object
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle to delete
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLockObjDelete(UShellOsal_s* const osal,
                                        const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Lock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalLock(UShellOsal_s* const osal,
                               const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Unlock access to the resource for third-party collaborators
 * \param[in]   UShellOsal_s* const osal - OSAL descriptor;
 * \param[in]   lockObjHandle - lock object handle
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s* const osal,
                                 const UShellOsalLockObjHandle_t lockObjHandle);

/**
 * \brief Create the thread
 * \param[in]   osal - OSAL descriptor;
 * \param[out]  threadHandle  - thread handle by which created thread can be referenced
 * \param[in]   threadCfg     - thread configuration
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadCreate(UShellOsal_s* const osal,
                                       UShellOsalThreadHandle_t* const threadHandle,
                                       UShellOsalThreadCfg_s threadCfg);

/**
 * \brief Delete the thread
 * \note The operation must be stopped before deleting the thread
 *        to not to damage the system.
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   threadHandle  - the handle of the thread being deleted
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadDelete(UShellOsal_s* const osal,
                                       const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Suspend the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being suspend
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadSuspend(UShellOsal_s* const osal,
                                        const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief Resume the thread
 * \param[in] osal - OSAL descriptor;
 * \param[in] threadHandle  - the handle of the thread being resumed
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadResume(UShellOsal_s* const osal,
                                       const UShellOsalThreadHandle_t threadHandle);

/**
 * \brief       Perform some delay
 * \param[in]   osal - OSAL descriptor;
 * \param[in]   msDelay - the delay timeout value expressed in ms;
 * \param[out]  no;
 * \return      MatrixKbdOsalErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalThreadDelay(const UShellOsal_s* const osal,
                                      const uint32_t msDelay);

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
                                           UShellOsalStreamBuffHandle_t* const streamBuffHandle);

/**
 * \brief Delete the stream buffer
 * \param osal              - pointer to OSAL instance
 * \param streamBuffHandle  - stream buffer handle being deleted
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalStreamBuffDelete(UShellOsal_s* const osal,
                                           const UShellOsalStreamBuffHandle_t streamBuffHandle);

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
                                const uint32_t msToWait);

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
                                        const size_t dataLengthBytes);

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
                                   const uint32_t msToWait);

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
                                           const size_t dataLengthBytes);

/**
 * \brief Reset a stream buffer to its initial empty state
 * \param osal              - pointer to OSAL instance
 * \param streamBuffHandle  - handle of the stream buffer being reset
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalStreamBuffReset(UShellOsal_s* const osal,
                                          const UShellOsalStreamBuffHandle_t streamBuffHandle);

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
                                            bool* const isEmpty);

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
                                      UShellOsalTimerCfg_s timerCfg);

/**
 * \brief Delete the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - the handle of the timer being deleted
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerDelete(UShellOsal_s* const osal,
                                      const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Start the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to start
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerStart(UShellOsal_s* const osal,
                                     const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Stop the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to stop
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerStop(UShellOsal_s* const osal, const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Reset the timer
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerHandle - handle of the timer to reset
 * \param[out] none
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerReset(UShellOsal_s* const osal, const UShellOsalTimerHandle_t timerHandle);

/**
 * \brief Get a timer handle of the given OSAL object
 * \param[in] osal - pointer to OSAL instance
 * \param[in] timerSlotInd - index of timer slots
 * \param[in] timerHandle - pointer to an object into which the timer handle will be copied
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimerHandleGet(UShellOsal_s* const osal,
                                         const size_t timerSlotInd,
                                         UShellOsalTimerHandle_t* const timerHandle);

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
                                            const UShellOsalTimeMs_t periodMs);

/**
 * \brief Create an event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupCreate(UShellOsal_s* const osal,
                                       UShellOsalEventGroupHandle_t* const eventGroupHandle);

/**
 * \brief Create an event group with a specific name.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[out] eventGroupHandle Pointer to store the handle of the created event group.
 * \param[in] name Name of the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupDelete(UShellOsal_s* const osal,
                                       const UShellOsalEventGroupHandle_t eventGroupHandle);

/**
 * \brief Set a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToSet Bits to set in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupSetBits(UShellOsal_s* const osal,
                                        const UShellOsalEventGroupHandle_t eventGroupHandle,
                                        const UShellOsalEventGroupBits_e bitsToSet);

/**
 * \brief Clear a bit in the event group.
 * \param[in] osal Pointer to the OSAL instance.
 * \param[in] eventGroupHandle Handle of the event group.
 * \param[in] bitsToClear Bits to clear in the event group.
 * \return Error code indicating the result of the operation.
 */
UShellOsalErr_e UShellEventGroupClearBits(UShellOsal_s* const osal,
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
UShellOsalErr_e UShellEventGroupBitsWait(UShellOsal_s* const osal,
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
UShellOsalErr_e UShellEventGroupBitsActiveGet(UShellOsal_s* const osal,
                                              const UShellOsalEventGroupHandle_t eventGroupHandle,
                                              UShellOsalEventGroupBits_e* const bitsActive);

/**
 * @brief Get time in milliseconds
 * @param[in] osal - pointer to OSAL instance
 * @param[in] timeMs - pointer to an object into which the current time in milliseconds will be copied
 * @return PiTalkVoiceOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalTimeMsGet(UShellOsal_s* const osal,
                                    UShellOsalTimeMs_t* const timeMs);

/**
 * \brief Get an event group handle of the given OSAL object
 * \param[in] osal - pointer to OSAL instance
 * \param[in] eventGroupSlotInd - index of event group slots
 * \param[in] eventGroupHandle - pointer to an object into which the event group handle will be copied
 * \return UShellOsalErr_e - error code. non-zero = an error has occurred;
 */
UShellOsalErr_e UShellOsalEventGroupHandleGet(UShellOsal_s* const osal,
                                              const size_t eventGroupSlotInd,
                                              UShellOsalEventGroupHandle_t* const eventGroupHandle);

/**
 * \brief Get a queue handle of the given OSAL object
 * \param[in] osal          - pointer to OSAL instance
 * \param[in] queueSlotInd  - index of queue slot
 * \param[out] queuehandle  - pointer to an object into which the current queue handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalQueueHandleGet(UShellOsal_s* const osal,
                                         const size_t queueSlotInd,
                                         UShellOsalQueueHandle_t* const queueHandle);

/**
 * \brief Get a lockobj handle of the given OSAL object
 * \param[in]   osal            - pointer to OSAL instance
 * \param[in]   lockObjSlotInd  - index of lockobj slots
 * \param[out]  lockObjhandle   - pointer to an object into which the lockobj handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalLockObjHandleGet(UShellOsal_s* const osal,
                                           const size_t lockObjSlotInd,
                                           UShellOsalLockObjHandle_t* const lockObjHandle);

/**
 * \brief Get a thread handle of the given OSAL object
 * \param[in]   osal            - pointer to OSAL instance
 * \param[in]   threadSlotInd   - index of thread slots
 * \param[out]  threadHandle	- pointer to an object into which the thread handle handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalThreadHandleGet(UShellOsal_s* const osal,
                                          const size_t threadSlotInd,
                                          UShellOsalThreadHandle_t* const threadHandle);

/**
 * \brief Get a streambuff handle of the given OSAL object
 * \param[in]   osal          	    - pointer to OSAL instance
 * \param[in]   streamBuffSlotInd   - index of streambuff slots
 * \param[out]  streamBuffHandle    - pointer to an object into which the streambuff handle handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalStreamBuffHandleGet(UShellOsal_s* const osal,
                                              const size_t streamBuffSlotInd,
                                              UShellOsalStreamBuffHandle_t* const streamBuffHandle);

/**
 * \brief Get a streambuff handle of the given OSAL object
 * \param[in]   osal          	    - pointer to OSAL instance
 * \param[in]   streamBuffSlotInd   - index of streambuff slots
 * \param[out]  streamBuffHandle    - pointer to an object into which the streambuff handle handle will be copied
 * \return UShellOsalErr_e error code
 */
UShellOsalErr_e UShellOsalStreamBuffHandleGet(UShellOsal_s* const osal,
                                              const size_t streamBuffSlotInd,
                                              UShellOsalStreamBuffHandle_t* const streamBuffHandle);

#ifdef __cplusplus
}
#endif

#endif    // ifdef USHELL_OSAL_H_