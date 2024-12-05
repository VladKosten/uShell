#ifndef USHELL_OSAL_FREERTOS_H_
#define USHELL_OSAL_FREERTOS_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "ushell_osal.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/


/**
 * @brief FreeRTOS-specific infinity timeout value.
 *
 * This macro defines the value used to represent an infinite timeout in the
 * UShell Operating System Abstraction Layer (OSAL) when using FreeRTOS.
 */
#define USHELL_OSAL_FREERTOS_INFINITY_TIMEOUT        UINT32_MAX


/**
 * @brief Low priority level for FreeRTOS threads.
 *
 * This macro defines the low priority level for threads in FreeRTOS.
 */
#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW    (tskIDLE_PRIORITY + 1)
#endif


/**
 * @brief Middle priority level for FreeRTOS threads.
 *
 * This macro defines the middle priority level for threads in FreeRTOS.
 */
#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE    (tskIDLE_PRIORITY + 2)
#endif


/**
 * @brief High priority level for FreeRTOS threads.
 *
 * This macro defines the high priority level for threads in FreeRTOS.
 */
#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH    (tskIDLE_PRIORITY + 3)
#endif


/**
 * @brief Ultra priority level for FreeRTOS threads.
 *
 * This macro defines the ultra priority level for threads in FreeRTOS.
 */
#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA    (tskIDLE_PRIORITY + configMAX_PRIORITIES)
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * @brief UShell FreeRTOS OSAL structure.
 *
 * This structure defines the UShell Operating System Abstraction Layer (OSAL)
 * specific to FreeRTOS. It includes the base OSAL structure and any additional
 * fields specific to the FreeRTOS implementation.
 */
typedef struct
{
    /**
     * @brief Base OSAL structure.
     *
     * This field contains the base OSAL structure. It must be the first field
     * in the structure to ensure proper alignment and compatibility.
     */
    UShellOsal_s base;

} UShellOsalFreertos_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Initialize the UShell FreeRTOS osal instance
 * @param[in] osalFreertos - pointer to FreeRTOS osal instance
 * @param[in] name         - pointer to the name of the OSAL instance [optional]
 * @param[in] parent       - pointer to a parent object [optional]
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosInit(UShellOsalFreertos_s *const osalFreertos,
                                       const char *name,
                                       void *const parent);

/**
 * @brief Deinitialize UShell FreeRTOS OSAL instance
 * @param[in] osalFreertos - pointer to osal FreeRTOS instance
 * @return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosDeinit(UShellOsalFreertos_s *const osalFreertos);

#ifdef __cplusplus
    }
#endif

#endif /* ifndef USHELL_OSAL_FREERTOS_H_ */