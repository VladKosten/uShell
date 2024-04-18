/**
* \file      ushell_osal_port.c
* \brief     Matrix Kbd operation system abstraction layer (OSAL) FreeRTOS port
* \authors   Vladislav Kosten (vladkosten@gmail.com)
* \copyright Copyright (c) 2023 PassatInnovations LTD. All rights reserved.
* \warning   A warning may be placed here...
* \bug       Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "ushell_osal_port.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief USHELL_OSAL_PORT_ASSERT macro definition
*/
#ifndef USHELL_OSAL_PORT_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_OSAL_PORT_ASSERT(cond)  USHELL_ASSERT(cond)
    #else
        #define USHELL_OSAL_PORT_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/* OSAL port specific functions */
/**
 * \brief       Create thread
 * \param[in]   const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[in]   UShellOsalPortThreadCfg_s* const threadCfg - the thread config
 * \param[out]  no
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred.
 */
static UShellOsalPortErr_e uShellOsalPortThreadCreate(UShellOsalPort_s* const osalPort, const UShellOsalPortThreadCfg_s* const threadCfg);

/**
 * \brief       Thread Port
 * \param[in]   const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[out]  no
 */
static void uShellOsalPortThread( void* const osalPort);

/* OSAL portable functions */

/**
 * \brief Thread resume
 * \param[in] const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[out] no
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
 */
static UShellOsalErr_e uShellOsalPortThreadStart(const void* const osal);

/**
 * \brief Thread suspend
 * \param[in] const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[out] no
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
 */
static UShellOsalErr_e uShellOsalPortThreadStop(const void* const osal);

/**
 * \brief Send a message
 * \param[in] const void* const osal - the OSAL port descriptor
 * \param[in] const UShellOsalMsg_e msg - the message to send
 * \param[out] no
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
*/
static UShellOsalErr_e uShellOsalPortMsgSend(const void* const osal, const UShellOsalMsg_e msg);

/**
 * \brief Get a message
 * \param[in]  const void* const osal - the OSAL port descriptor
 * \param[out] const UShellOsalMsg_e* const msg - buffer to store the message
 * \param[in]  const UShellOsalTimeOut_t timeout - the timeout
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
*/
static UShellOsalErr_e uShellOsalPortMsgGet(const void* const osal, const UShellOsalMsg_e* const msg, const UShellOsalTimeOut_t timeout);

/**
 * \brief       Perform lock operation (acquire a mutex)
 * \param[in]   const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[out]  no
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred.
 */
static UShellOsalErr_e uShellOsalPortLock( const void* const osal);

/**
 * \brief       Perform unlock operation (release a mutex)
 * \param[in]   const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[out]  no
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred.
 */
static UShellOsalErr_e uShellOsalPortUnlock( const void* const osal);

/* Portable functions table */
static const UShellOsalPortableTable_s matrixkbdOsalPortPortableFunc =
{
    .threadStart        = uShellOsalPortThreadStart,    ///< OSAL port port thread resume
    .threadStop         = uShellOsalPortThreadStop,     ///< OSAL port port thread suspend
    .lock               = uShellOsalPortLock,           ///< OSAL port port lock resources
    .unlock             = uShellOsalPortUnlock,         ///< OSAL port port unlock resources
    .msgGet             = uShellOsalPortMsgGet,         ///< OSAL port port get message
    .msgSend            = uShellOsalPortMsgSend         ///< OSAL port port send message
};

static const char* uShellOsalPortName = USHELL_STARTUP_OSAL_PORT_TASK_NAME;

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]===============================================================================

/**
 * \brief       Initialize the matrix kbd FreeRTOS osal instance
 * \param[in]   UShellOsalPort_s *const osalPort - OSAL FreeRTOS descriptor;
 * \param[in]   const UShellOsalPortThreadCfg_s* const threadCfg - thread parameters descriptor;
 * \param[in]   const void *const parent - pointer to a parent object;
 * \param[in]   const char* const name - the name of the osal object;
 * \param[out]  no;
 * \return      UShellOsalPortErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalPortErr_e UShellOsalPortInit(UShellOsalPort_s *const osalPort, const UShellOsalPortThreadCfg_s* const threadCfg,
                                                                                                  const void *const parent,
                                                                                                  const char* const name)
{
    /* Checking of params */
    if(NULL == osalPort)
    {
        return USHELL_OSAL_PORT_INVALID_ARGS_ERR;   // Exit: Error: Invalid args
    }

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_CALL_FROM_ISR_ERR ; // Exit: Error: Call from isr
    }

    /* Init the OSAL  */
    UShellOsal_s* osal = &osalPort->base;
    UShellOsalErr_e osalStatus = UShellOsalInit(osal, parent, name);
    if(osalStatus != USHELL_OSAL_NO_ERR)
    {
        return USHELL_OSAL_PORT_NOT_INIT_ERR;       // Exit: Error: Could not init the base
    }

    /* Try to create mutex */
    SemaphoreHandle_t mutexHandle = xSemaphoreCreateRecursiveMutex();
    if(NULL == mutexHandle)
    {
        osalStatus = UShellOsalDeinit(osal);
        USHELL_OSAL_PORT_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        return USHELL_OSAL_PORT_NOT_INIT_ERR;       // Exit: Error: Could not create the lock/unlock mutex
    }
    osalPort->base.mutexHandle = mutexHandle;

    /* Try to create the msg queue */
    QueueHandle_t msgHandle = xQueueCreate(4, sizeof(UShellOsalMsg_e));
    if(NULL == msgHandle)
    {
        osalStatus = UShellOsalDeinit(osal);
        USHELL_OSAL_PORT_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        return USHELL_OSAL_PORT_NOT_INIT_ERR;       // Exit: Error: Could not create the message queue
    }
    osalPort->base.msgHandle = msgHandle;

    /* Create the thread */
    UShellOsalPortErr_e osalPortStatus = uShellOsalPortThreadCreate(osalPort);
    if(USHELL_OSAL_PORT_NO_ERR != osalPortStatus)
    {
        osalPortStatus = UShellOsalPortDeinit(osalPort);
        USHELL_OSAL_PORT_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        return USHELL_OSAL_PORT_NOT_INIT_ERR;  // Exit: Error: Could not init the base
    }

    /* FreeRTOS port specific methods */
    osal->portable = &matrixkbdOsalPortPortableFunc;

    return USHELL_OSAL_PORT_NO_ERR;             // Exit: no errors
}

/**
 * \brief       Deinitialize the matrix kbd FreeRTOS osal instance
 * \param[in]   UShellOsalPort_s *const osalPort - OSAL FreeRTOS descriptor;
 * \param[out]  no;
 * \return      UShellOsalPortErr_e  - error code. non-zero = an error has occurred;
 */
UShellOsalPortErr_e UShellOsalPortDeinit(UShellOsalPort_s *const osalPort)
{
    /* Checking of params */
    if(NULL == osalPort)
    {
        return USHELL_OSAL_PORT_INVALID_ARGS_ERR;  // Exit: Error: Invalid args
    }

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_CALL_FROM_ISR_ERR ; // Exit: Error: Call from isr
    }

    /* equating pointers */
    UShellOsal_s* osal = (UShellOsal_s*)osalPort;

    /* Delete the requested resources */
    if(NULL != osal->mutexHandle)
    {
        vSemaphoreDelete(osal->mutexHandle);
    }

    if(NULL != osal->threadHandle)
    {
        vTaskDelete(osal->threadHandle);
    }

    if(NULL != osal->msgHandle)
    {
        vQueueDelete(osal->msgHandle);
    }

    /* Flush thread parameter*/
    osalPort->lastWakeTime = 0;
    osalPort->threadCfg.stackSize = 0;
    osalPort->threadCfg.threadPriority = 0;
    osalPort->threadCfg.name = NULL;

    /* Deinit the parent */
    UShellOsalErr_e osalStatus = UShellOsalDeinit(osal);
    (void)osalStatus;
    USHELL_OSAL_PORT_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    return USHELL_OSAL_PORT_NO_ERR;                 // Exit: no errors
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================

/**
 * \brief       Create thread
 * \param[in]   UShellOsalPort_s* const osalPort - OSAL FreeRTOS descriptor;
 * \param[out]  no;
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
static UShellOsalPortErr_e uShellOsalPortThreadCreate(UShellOsalPort_s* const osalPort)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osalPort);
    if(NULL != osalPort->base.threadHandle)
    {
        return USHELL_OSAL_PORT_INVALID_ARGS_ERR;   // Exit: Error: Invalid args
    }

     /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_CALL_FROM_ISR_ERR ; // Exit: Error: Call from isr
    }

    /* Creating a thread  */
    BaseType_t xReturned = pdFALSE;
    TaskHandle_t threadHandle = NULL;
    xReturned = xTaskCreate(uShellOsalPortThread, uShellOsalPortName,
                                                  USHELL_OSAL_PORT_THREAD_STACK_SIZE,
                                                  (void*)osalPort,
                                                  USHELL_OSAL_PORT_THREAD_PRIO,
                                                  &threadHandle);

    if (pdPASS != xReturned)
    {
        return USHELL_OSAL_PORT_NOT_INIT_ERR;   // Exit: error - memory required to create the thread could not be allocated
    }

    vTaskSuspend(threadHandle);
    osalPort->base.threadHandle = threadHandle;
    osalPort->threadCfg = thisThreadCfg;

    return USHELL_OSAL_PORT_NO_ERR;                      // Exit: no errors
}

/**
 * \brief  UShell OSAL Port thread(task) function
 * \param[in] UShellOsal_s* const osal - OSAL descriptor;
 * \param[out] no;
 */
static void uShellOsalPortThread(void* const osalPort)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);

    /* equating pointers */
    UShellOsalPort_s* thisOsalPort = (UShellOsalPort_s*)osalPort;
    USHELL_OSAL_PORT_ASSERT(osal->threadHandle);

    thisOsalPort->lastWakeTime = xTaskGetTickCount();
    /* Thread execution loop */
    do
    {
        /* If the load (an worker) of the thread was assigned then carry out it */
        if(NULL != thisOsalPort->base.threadWorker)
        {
            USHELL_OSAL_PORT_ASSERT(thisOsalPort->base.parent);
            thisOsalPort->base.threadWorker(thisOsalPort->base.parent);
        }
        else
        {
            /* If the load of the thread (an worker) was not assigned then suspend the current thread */
            vTaskSuspend(NULL);
        }

    } while (1);

    /* Delete the current thread */
    thisOsalPort->base.threadHandle = NULL;
    vTaskDelete(NULL);
    USHELL_OSAL_PORT_ASSERT(0);
    while(1);
}

/**
 * \brief Thread Resume
 * \param[in] const UShellOsal_s* const osal - the OSAL port descriptor;
 * \param[out] no;
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred;
 */
static UShellOsalErr_e uShellOsalPortThreadStart(const void* const osal)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);
    USHELL_OSAL_PORT_ASSERT(NULL == osal->threadHandle);

    /* Equate pointers */
    UShellOsalPort_s* osalPort = (UShellOsalPort_s*)osal;

    /* Check where the call is coming from */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_LAYER_ERR ; // Exit: Error: Call from isr
    }

    vTaskResume(osalPort->base.threadHandle);

    return USHELL_OSAL_NO_ERR;              // Exit: no errors
}

/**
 * \brief Thread suspend
 * \param[in] const UShellOsal_s* const osal - the OSAL port descriptor
 * \param[out] no
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
 */
static UShellOsalErr_e uShellOsalPortThreadStop(const void* const osal)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);
    USHELL_OSAL_PORT_ASSERT(NULL == osal->threadHandle);

    /* Equate pointers */
    UShellOsal_s* thisOsal = (UShellOsal_s*)osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_LAYER_ERR ; // Exit: Error: Call from isr
    }

    vTaskSuspend(thisOsal->threadHandle);

    return USHELL_OSAL_NO_ERR;              // Exit: no errors
}

/**
 * \brief Send a message
 * \param[in] const void* const osal - the OSAL port descriptor
 * \param[in] const UShellOsalMsg_e msg - the message to send
 * \param[out] no
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
*/
static UShellOsalErr_e uShellOsalPortMsgSend(const void* const osal, const UShellOsalMsg_e msg)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);
    USHELL_OSAL_PORT_ASSERT(osal->msgHandle);

    /* Equate pointers */
    UShellOsal_s* thisOsal = (UShellOsal_s*)osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    BaseType_t xReturned = pdFALSE;
    if (statusIsr == pdTRUE)
    {
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xReturned = xQueueSendFromISR(thisOsal->msgHandle, &msg, &higherPriorityTaskWoken);
        if(higherPriorityTaskWoken == pdTRUE)
        {
            portYIELD_FROM_ISR();
        }

    }
    else
    {
        xReturned = xQueueSend(thisOsal->msgHandle, &msg, 0);
    }

    if(pdTRUE != xReturned)
    {
        return USHELL_OSAL_PORT_ERR;      // Exit: Error: Could not send the message
    }

    return USHELL_OSAL_NO_ERR;                  // Exit: no errors

}

/**
 * \brief Get a message
 * \param[in]  const void* const osal - the OSAL port descriptor
 * \param[out] const UShellOsalMsg_e* const msg - buffer to store the message
 * \param[in]  const UShellOsalTimeOut_t timeout - the timeout
 * \return UShellOsalErr_e  - error code. non-zero = an error has occurred.
*/
static UShellOsalErr_e uShellOsalPortMsgGet(const void* const osal, const UShellOsalMsg_e* const msg, const UShellOsalTimeOut_t timeout)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);
    USHELL_OSAL_PORT_ASSERT(osal->msgHandle);

    /* Equate pointers */
    UShellOsal_s* thisOsal = (UShellOsal_s*)osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_ERR ;      // Exit: Error: Call from isr
    }

    BaseType_t xReturned = xQueueReceive(thisOsal->msgHandle, msg, timeout);
    if(pdTRUE != xReturned)
    {
        return USHELL_OSAL_PORT_ERR;      // Exit: Error: Could not get the message
    }

    return USHELL_OSAL_NO_ERR;                  // Exit: no errors

}

/**
 * \brief       Perform lock operation (acquire a mutex)
 * \param[in]   const UShellOsal_s* const osal - the OSAL port descriptor;
 * \param[out]  no;
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred;

 */
static UShellOsalErr_e uShellOsalPortLock( const void* const osal)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);
    USHELL_OSAL_PORT_ASSERT(osal->mutexHandle);

    /* Equate pointers */
    UShellOsal_s* thisOsal = (UShellOsal_s*)osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_LAYER_ERR ; // Exit: Error: Call from isr
    }

    BaseType_t smphrStatus = xSemaphoreTakeRecursive(thisOsal->mutexHandle, portMAX_DELAY);
    if(pdTRUE != smphrStatus)
    {
        return USHELL_OSAL_PORT_LAYER_ERR;      // Exit: Error: Could not lock
    }

    return USHELL_OSAL_NO_ERR;                  // Exit: no errors
}

/**
 * \brief       Perform unlock operation (release a mutex)
 * \param[in]   const UShellOsal_s* const osal - the OSAL port descriptor;
 * \param[out]  no;
 * \return      UShellOsalErr_e  - error code. non-zero = an error has occurred;

 */
static UShellOsalErr_e uShellOsalPortUnlock( const void* const osal)
{
    /* Checking of params */
    USHELL_OSAL_PORT_ASSERT(osal);
    USHELL_OSAL_PORT_ASSERT(osal->mutexHandle);

    /* Equate pointers */
    UShellOsal_s* thisOsal = (UShellOsal_s*)osal;

    /* Check the level at which the function was called */
    BaseType_t statusIsr = xPortIsInsideInterrupt();
    if (statusIsr == pdTRUE)
    {
        return USHELL_OSAL_PORT_LAYER_ERR ; // Exit: Error: Call from isr
    }

    BaseType_t smphrStatus = xSemaphoreGiveRecursive(thisOsal->mutexHandle);
    if(pdTRUE != smphrStatus)
    {
        return USHELL_OSAL_PORT_LAYER_ERR;  // Exit: Error: Could not unlock
    }

    return USHELL_OSAL_NO_ERR;              // Exit: no errors
}