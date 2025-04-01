#ifndef USHELL_CFG_H_
#define USHELL_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/

#include <assert.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/
/**
 * @file dio_monitor_cfg.h
 * \brief Configuration file for the DIO Monitor.
 */

/**
 * @def USHELL_ASSERT
 * \brief Assert macro definition.
 * \param cond Condition to assert.
 */
#define USHELL_ASSERT(cond)                     assert((cond))

/**
 * @def USHELL_THREAD_NAME
 * \brief Name of the DIO monitor thread.
 */
#define USHELL_THREAD_NAME                      "USHELL"

/**
 * @def USHELL_THREAD_STACK_SIZE
 * \brief Stack size for the DIO monitor thread.
 */
#define USHELL_THREAD_STACK_SIZE                1024U

/**
 * @def USHELL_THREAD_PRIORITY
 * \brief Priority for the DIO monitor thread.
 */
#define USHELL_THREAD_PRIORITY                  USHELL_OSAL_THREAD_PRIORITY_MIDDLE

/**
 * @def USHELL_OSAL_QUEUE_SLOTS_NUM
 * \brief Number of OSAL queue slots.
 */
#define USHELL_OSAL_QUEUE_SLOTS_NUM             (4)

/**
 * @def USHELL_OSAL_LOCK_OBJS_NUM
 * \brief Number of OSAL lock objects.
 */
#define USHELL_OSAL_LOCK_OBJS_NUM               (2)

/**
 * @def USHELL_OSAL_THREADS_NUM
 * \brief Number of OSAL threads.
 */
#define USHELL_OSAL_THREADS_NUM                 (1)

/**
 * @def USHELL_OSAL_FREERTOS_INFINITY_TIMEOUT
 * \brief Infinity timeout for FreeRTOS.
 */
#define USHELL_OSAL_FREERTOS_INFINITY_TIMEOUT   UINT32_MAX

/**
 * @def USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW
 * \brief Low priority for FreeRTOS threads.
 */
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW    (tskIDLE_PRIORITY + 1)

/**
 * @def USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE
 * \brief Middle priority for FreeRTOS threads.
 */
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE (tskIDLE_PRIORITY + 2)

/**
 * @def USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH
 * \brief High priority for FreeRTOS threads.
 */
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH   (tskIDLE_PRIORITY + 3)

/**
 * @def USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA
 * \brief Ultra priority for FreeRTOS threads.
 */
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA  (tskIDLE_PRIORITY + 4)

#ifdef __cplusplus
}
#endif

#endif