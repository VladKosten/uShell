#ifndef USHELL_OSAL_FREERTOS_H_
#define USHELL_OSAL_FREERTOS_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/

#include "ushell_osal.h"
#include <FreeRTOS.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief FreeRTOS-specific infinity timeout value
 */
#define USHELL_OSAL_FREERTOS_INFINITY_TIMEOUT        UINT32_MAX

/**
 * \brief FreeRTOS-specific priority levels
 * \note  Can be redefined in the ushell_cfg file
 */
#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW    (tskIDLE_PRIORITY + 1)
#endif

#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE    (tskIDLE_PRIORITY + 2)
#endif

#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH    (tskIDLE_PRIORITY + 3)
#endif

#ifndef USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA
    #define USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA    (tskIDLE_PRIORITY + configMAX_PRIORITIES)
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief UShell FreeRTOS OSAL structure
 */
typedef struct
{
    // Do not move the base structure from the first position
    UShellOsal_s base;

} UShellOsalFreertos_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShell FreeRTOS osal instance
 * \param[in] osalFreertos - pointer to FreeRTOS osal instance
 * \param[in] name         - pointer to the name of the OSAL instance [optional]
 * \param[in] parent       - pointer to a parent object [optional]
 * \param[in] param        - pointer to FreeRTOS param structure [optional]
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosInit(UShellOsalFreertos_s *const osalFreertos,
                                                   const char *name, void *const parent,
                                                   const UShellOsalFreertosParam_s *const param);

/**
 * \brief Deinitialize UShell FreeRTOS OSAL instance
 * \param[in] osalFreertos - pointer to osal FreeRTOS instance
 * \return UShellOsalErr_e error code.
 */
UShellOsalErr_e UShellOsalFreertosDeinit(void *const osalFreertos);

#ifdef __cplusplus
    }
#endif

#endif /* ifndef USHELL_OSAL_FREERTOS_H_ */