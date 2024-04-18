#ifndef USHELL_OSAL_PORT_H_
#define USHELL_OSAL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/

#include "ushell_osal.h"

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief Name for the thread required for driver operation
 */
#ifndef USHELL_OSAL_PORT_THREAD_NAME
    #define USHELL_OSAL_PORT_THREAD_NAME "USHELL_THREAD"
#endif

/**
 * \brief Stack size (byte) for the thread required for driver operation
 */
#ifndef USHELL_OSAL_PORT_THREAD_STACK_SIZE
    #define USHELL_OSAL_PORT_THREAD_STACK_SIZE ((uint16_t)256)
#endif

/**
 * \brief  priority levels for thread
 */
#ifndef USHELL_OSAL_PORT_THREAD_PRIO
    #define USHELL_OSAL_PORT_THREAD_PRIO    (tskIDLE_PRIORITY + 1)
#endif


/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief This enum describes possible errors
 */
typedef enum
{
    USHELL_OSAL_PORT_NO_ERR = 0,        ///< Exit: no errors
    USHELL_OSAL_PORT_INVALID_ARGS_ERR,  ///< Exit: error - invalid pointers
    USHELL_OSAL_PORT_NOT_INIT_ERR,      ///< Exit: error - init error
    USHELL_OSAL_PORT_CALL_FROM_ISR_ERR, ///< Exit: error - call from isr

} UShellOsalPortErr_e;


/**
 * \brief Matrix KBD FreeRTOS OSAL structure
 */
typedef struct
{
    // Do not move the base structure from the first position
    UShellOsal_s base;


    // Optional fields
    UShellOsalPortThreadCfg_s threadCfg;

} UShellOsalPort_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief       Initialize the UShell FreeRTOS osal instance
 * \param[in]   UShellOsalPort_s *const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[in]   const void *const parent - pointer to a parent object;
 * \param[in]   const char* const name - the name of the osal object;
 * \param[out]  no;
 * \return      UShellOsalPortErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalPortErr_e UShellOsalPortInit(UShellOsalPort_s *const osalFreeRtos, const void *const parent,
                                                                             const char* const name);

/**
 * \brief       Deinitialize the UShell FreeRTOS osal instance;
 * \param[in]   UShellOsalPort_s *const osalFreeRtos - OSAL FreeRTOS descriptor;
 * \param[out]  no;
 * \return      UShellOsalPortErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalPortErr_e UShellOsalPortDeinit(UShellOsalPort_s *const osalFreeRtos);

#ifdef __cplusplus
}
#endif

#endif /* USHELL_OSAL_PORT_H_ */