/**
* \file         ushell_vcp.c
* \brief        The file contains the implementation of the UShell vcp module. The module is responsible for the initialization of the UShell
*               and the main loop of vcp UShell.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright    MIT License (c) 2025
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...

*/
//===============================================================================[ INCLUDE ]========================================================================================

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Project includes */
#include "ushell_vcp.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell module.
 */
#ifndef USHELL_VCP_ASSERT
    #define USHELL_VCP_ASSERT(cond)
#endif

/**
 * @brief VCP flag
 */
typedef enum
{
    USHELL_VCP_FLAG_NONE = 0x00,                 ///< No flags set
    USHELL_VCP_FLAG_RX_EVENT = 0x01,             ///< Rx event
    USHELL_VCP_FLAG_TX_TRANSFER_EVENT = 0x02,    ///< TX transfer event flag(From TX stream buffer to HAL)
    USHELL_VCP_FLAG_TX_COMPLETE_EVENT = 0x04,    ///< TX complete event flag
    USHELL_VCP_FLAG_TX_ERROR_EVENT = 0x08,       ///< RX/TX error event flag
    USHELL_VCP_FLAG_ERROR_EVENT = 0x10,          ///< Error event flag

    /* Add other here */

    /* Last */
    USHELL_VCP_FLAG_ALL = 0x1F,    ///< All flags

} UShellVcpFlags_e;

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShell thread worker
 * \param[in] vcp - vcp object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellVcpWorker(void* const vcp);

/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
 */
static void uShellVcpRxReceivedCb(const void* const hal);

/**
 * \brief Callback for the transmitted data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is transmitted to the serial port.
 */
static void uShellVcpTxCpltCb(const void* const hal);

/**
 * \brief Callback for the error
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the error occurs in the serial port.
 */
static void uShellVcpXferErrorCb(const void* const hal);

/**
 * @brief Initialize the runtime environment
 * @param vcp - vcp object
 * @param osal - osal object
 * @param hal - hal object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvInit(UShellVcp_s* const vcp,
                                         UShellOsal_s* const osal,
                                         UShellHal_s* const hal);

/**
 * @brief Deinitialize the runtime environment
 * @param vcp - vcp object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvDeInit(UShellVcp_s* const vcp);

/**
 * @brief Initialize the runtime environment HAL
 * @param vcp - vcp object
 * @param hal - hal object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalInit(UShellVcp_s* const vcp,
                                            UShellHal_s* const hal);

/**
 * @brief Deinitialize the runtime environment HAL
 * @param vcp - vcp object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalDeInit(UShellVcp_s* const vcp);

/**
 * @brief Initialize the runtime environment OSAL
 * @param[in] vcp - vcp object
 * @param[in] osal - osal object
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpRtEnvOsalInit(UShellVcp_s* const vcp,
                                             UShellOsal_s* const osal);

/**
 * @brief Deinitialize the runtime environment OSAL
 * @param vcp - vcp object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvOsalDeInit(UShellVcp_s* const vcp);

/**
 * \brief Lock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpLock(const UShellVcp_s* const vcp);

/**
 * \brief Unlock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpUnlock(const UShellVcp_s* const vcp);

/**
 * @brief Set flags
 * @param vcp - pointer to a Vcp instance;
 * @param flags - flags to be set;
 * @return none;
 */
static void uShellVcpFlagSet(const UShellVcp_s* const vcp,
                             UShellVcpFlags_e flags);

/**
 * @brief Wait flag (blocked and not clear)
 * @param vcp - pointer to a vcp instance;
 * @param flags - flags to be set;
 * @param allWait - wait for all flags
 */
static void uShellVcpFlagsWait(const UShellVcp_s* const vcp,
                               UShellVcpFlags_e flags,
                               const bool allWait);

/**
 * @brief Clear flag
 * @param vcp - pointer to a vcp instance;
 * @param flags - flags to be set;
 */
static void uShellVcpFlagClear(const UShellVcp_s* const vcp,
                               UShellVcpFlags_e flags);

/**
 * @brief Flag is set
 * @param vcp - pointer to a vcp instance
 * @param flags - flags to be set
 */
static bool uShellVcpFlagIsSet(const UShellVcp_s* const vcp,
                               UShellVcpFlags_e flags);

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Init vcp object
 * \param[in] vcp - vcp object to be initialized
 * \param[in] osal - osal object
 * \param[in] hal - hal object
 * \param[in] parent - parent object
 * \param[in] name - name of the object
 * \param[out] none
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellInit(UShellVcp_s* const vcp,
                          const UShellOsal_s* const osal,
                          const UShellHal_s* const hal,
                          void* const parent,
                          const char* const name)
{

    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(osal != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Initialize the runtime environment */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            return USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Flush the objects */
        memset(vcp, 0, sizeof(UShellVcp_s));

        /* Set the parent object */
        vcp->parent = parent;
        vcp->name = name;

        /* Initialize the runtime environment */
        status = uShellVcpRtEnvInit(vcp,
                                    (UShellOsal_s*) osal,
                                    (UShellHal_s*) hal);
        if (status != USHELL_VCP_NO_ERR)
        {
            break;
        }

    } while (0);

    return USHELL_VCP_NO_ERR;
}

/**
 * \brief DeInit vcp object
 * \param[in] vcp - vcp object to be deinitialized
 * \param[out] none
 * \return USHELL_VCP_NO_ERR if success, otherwise error code
 */
UShellVcpErr_e UShellDeInit(UShellVcp_s* const vcp)
{

    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variable */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Check input parameter */
    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the runtime environment */
        uShellVcpRtEnvDeInit(vcp);

        /* Clear the object */
        memset(vcp, 0, sizeof(UShellVcp_s));

    } while (0);

    return status;
}

/**
 * @brief Print string to the vcp vcp object
 * @param[in] vcp - vcp object to be printed
 * @param[out] none
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintStr(UShellVcp_s* const vcp);

/**
 * @brief Print char to the vcp vcp object
 * @param vcp - vcp object to be printed
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpPrintChar(UShellVcp_s* const vcp);

/**
 * @brief Scan char from the vcp vcp object
 * @param[in] vcp - vcp object to be scanned
 * @param[in] ch - char to be scanned
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanChar(UShellVcp_s* const vcp,
                                 char* const ch);

/**
 * @brief Scan string from the vcp vcp object
 * @param[in] vcp - vcp object to be scanned
 * @param[in] str - string to be scanned
 * @param[in] size - size of the string to be scanned
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
UShellVcpErr_e UShellVcpScanStr(UShellVcp_s* const vcp,
                                char* const str,
                                const size_t maxSize);

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief UShell thread worker
 * \param[in] vcp - vcp object
 * \param[out] none
 * \return none
 * \note This function is the main loop of the UShell module. It is responsible for the processing of the commands and the interaction with the user.
 */
static void uShellVcpWorker(void* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variables */
    UShellVcp_s* const ushell = (UShellVcp_s*) vcp;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellVcpItem_t item = 0;

    /* Main loop */
    while (1)
    {

        do
        {
        } while (0);
    }
}

/**
 * \brief Callback for the received data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is received from the serial port.
 */
static void uShellVcpRxReceivedCb(const void* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellSemaphoreRxEventSet(ushell);
        USHELL_VCP_ASSERT(status == USHELL_VCP_NO_ERR);
        (void) status;

    } while (0);
}

/**
 * \brief Callback for the transmitted data
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the data is transmitted to the serial port.
 */
static void uShellVcpTxCpltCb(const void* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_TX_COMPLETE);
        USHELL_VCP_ASSERT(status == USHELL_VCP_NO_ERR);
        (void) status;

    } while (0);
}

/**
 * \brief Callback for the error
 * \param[in] hal - hal object
 * \param[out] none
 * \return none
 * \note This function is called when the error occurs in the serial port.
 */
static void uShellVcpXferErrorCb(const void* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellHal_s* const ushellHal = (UShellHal_s*) hal;
    UShellVcp_s* const ushell = (UShellVcp_s*) ushellHal->parent;
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    do
    {
        /* Check input parameters */
        if ((ushellHal == NULL) ||
            (ushell == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Send msg */
        status = uShellQueueMsgSend(ushell, USHELL_MSG_RX_TX_ERROR);
        USHELL_VCP_ASSERT(status == USHELL_VCP_NO_ERR);
        (void) status;

    } while (0);
}

/**
 * @brief Initialize the runtime environment
 * @param vcp - vcp object
 * @param osal - osal object
 * @param hal - hal object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvInit(UShellVcp_s* const vcp,
                                         UShellOsal_s* const osal,
                                         UShellHal_s* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(osal != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    /* Rt env init */
    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (hal == NULL))
        {
            return USHELL_VCP_INVALID_ARGS_ERR;
        }

        /* Initialize the runtime environment HAL */
        status = uShellVcpRtEnvHalInit(vcp, hal);
        if (status != USHELL_VCP_NO_ERR)
        {
            break;
        }

        /* Initialize the runtime environment OSAL */
        status = uShellVcpRtEnvOsalInit(vcp, osal);
        if (status != USHELL_VCP_NO_ERR)
        {
            break;
        }

    } while (0);

    /* Check status */
    if (status != USHELL_VCP_NO_ERR)
    {
        /* Deinitialize the runtime environment */
        uShellVcpRtEnvDeInit(vcp);
    }

    return status;
}

/**
 * @brief Deinitialize the runtime environment
 * @param vcp - vcp object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;

    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Deinitialize the runtime environment OSAL */
        uShellVcpRtEnvOsalDeInit(vcp);

        /* Deinitialize the runtime environment HAL */
        uShellVcpRtEnvHalDeInit(vcp);

    } while (0);

    return status;
}

/**
 * @brief Initialize the runtime environment HAL
 * @param vcp - vcp object
 * @param hal - hal object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalInit(UShellVcp_s* const vcp,
                                            UShellHal_s* const hal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(hal != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (hal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Save the hal object */
        vcp->hal = hal;

        /* Attach the parent object */
        halStatus = UShellHalParentSet(hal, vcp);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the received data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_RECEIVED,
                                      uShellVcpRxReceivedCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the transmitted data */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_TX_COMPLETE,
                                      uShellVcpTxCpltCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Attach callback for the error */
        halStatus = UShellHalCbAttach(hal,
                                      USHELL_HAL_CB_RX_TX_ERROR,
                                      uShellVcpXferErrorCb);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Open the hal */
        halStatus = UShellHalOpen(hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            return;
        }

    } while (0);

    return status;
}

/**
 * @brief Deinitialize the runtime environment HAL
 * @param vcp - vcp object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvHalDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local variables */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellHalErr_e halStatus = USHELL_HAL_NO_ERR;

    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (vcp->hal == NULL))
        {
            status = USHELL_VCP_INVALID_ARGS_ERR;
            break;
        }

        /* Detach callback for the received data */
        halStatus = UShellHalCbDetach(vcp->hal,
                                      USHELL_HAL_CB_RX_RECEIVED);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach callback for the transmitted data */
        halStatus = UShellHalCbDetach(vcp->hal,
                                      USHELL_HAL_CB_TX_COMPLETE);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach callback for the error */
        halStatus = UShellHalCbDetach(vcp->hal,
                                      USHELL_HAL_CB_RX_TX_ERROR);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Detach the parent object */
        halStatus = UShellHalParentSet(vcp->hal, NULL);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            status = USHELL_VCP_PORT_ERR;
            break;
        }

        /* Close */
        halStatus = UShellHalClose(vcp->hal);
        if (halStatus != USHELL_HAL_NO_ERR)
        {
            return;
        }

        /* Remove the hal object */
        vcp->hal = NULL;

    } while (0);

    return status;
}

/**
 * @brief Initialize the runtime environment OSAL
 * @param[in] vcp - vcp object
 * @param[in] osal - osal object
 * @return UShellVcpErr_e - error code. non-zero = an error has occurred;
 */
static UShellVcpErr_e uShellVcpRtEnvOsalInit(UShellVcp_s* const vcp,
                                             UShellOsal_s* const osal)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(osal != NULL);

    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsal_s* thisOsal = (UShellOsal_s*) osal;

    /* Attach osal object */
    vcp->osal = thisOsal;

    /* Create osal objects */
    do
    {
        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, vcp);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* : Create mutex */
        UShellOsalLockObjHandle_t lockObj = NULL;
        osalStatus = UShellOsalLockObjCreate(thisOsal, &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* Create the event system */
        UShellOsalEventGroupHandle_t eventGroup = NULL;
        osalStatus = UShellOsalEventGroupCreate(thisOsal, &eventGroup);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(eventGroup != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

        /* Create stream buffer for transfer to write in VCP */
        UShellOsalStreamBuffHandle_t streamBufferTx = NULL;
        osalStatus = UShellOsalStreamBuffCreate(thisOsal,
                                                USHELL_BUFFER_SIZE,
                                                1U,
                                                &streamBufferTx);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBufferTx == NULL))
        {
            break;
        }

        /* Create stream buffer for transfer to read from VCP */
        UShellOsalStreamBuffHandle_t streamBufferRx = NULL;
        osalStatus = UShellOsalStreamBuffCreate(thisOsal,
                                                USHELL_BUFFER_SIZE,
                                                1U,
                                                &streamBufferRx);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (streamBufferRx == NULL))
        {
            break;
        }

        /* : Create task */
        UShellOsalThreadHandle_t thread = NULL;
        UShellOsalThreadCfg_s threadCfg =
            {
                .name = USHELL_THREAD_NAME,
                .stackSize = USHELL_THREAD_STACK_SIZE,
                .threadParam = vcp,
                .threadPriority = USHELL_THREAD_PRIORITY,
                .threadWorker = uShellVcpWorker};
        osalStatus = UShellOsalThreadCreate(thisOsal, &thread, threadCfg);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(thread != NULL);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            break;
        }

    } while (0);

    if (osalStatus != USHELL_OSAL_NO_ERR)
    {
        uShellVcpRtEnvOsalDeInit(vcp);
        return USHELL_VCP_PORT_ERR;
    }

    return USHELL_VCP_NO_ERR;
}

/**
 * @brief Deinitialize the runtime environment OSAL
 * @param vcp - vcp object
 * @return USHELL_VCP_NO_ERR if success, otherwise error code
 */
static UShellVcpErr_e uShellVcpRtEnvOsalDeInit(UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);
    USHELL_VCP_ASSERT(vcp->osal != NULL);

    /* Status obj */
    UShellVcpErr_e status = USHELL_VCP_NO_ERR;
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    (void) osalStatus;
    UShellOsal_s* thisOsal = (UShellOsal_s*) vcp->osal;

    do
    {
        /* Destroy osal objects */
        /* : Delete the thread */
        do
        {
            /* : : Find the thread handle */
            UShellOsalThreadHandle_t thread = NULL;
            osalStatus = UShellOsalThreadHandleGet(thisOsal, 0U, &thread);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (thread == NULL))
            {
                break;
            }

            /* : : Delete the thread */
            osalStatus = UShellOsalThreadDelete(thisOsal, thread);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* : Delete the lock object */
        do
        {
            /* : : Find the lock object handle */
            UShellOsalLockObjHandle_t lockObj = NULL;
            osalStatus = UShellOsalLockObjHandleGet(thisOsal, 0U, &lockObj);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (lockObj == NULL))
            {
                break;
            }

            /* : : Delete the lock object */
            osalStatus = UShellOsalLockObjDelete(thisOsal, lockObj);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        } while (0);

        /* : Delete the event group */
        do
        {
            /* : : Find the event group handle */
            UShellOsalEventGroupHandle_t eventGroup = NULL;
            osalStatus = UShellOsalEventGroupHandleGet(thisOsal, 0U, &eventGroup);
            if ((osalStatus != USHELL_OSAL_NO_ERR) ||
                (eventGroup == NULL))
            {
                break;
            }

            /* : : Delete the event group */
            osalStatus = UShellOsalEventGroupDelete(thisOsal, eventGroup);
            USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        } while (0);

        /* Set parent */
        osalStatus = UShellOsalParentSet(thisOsal, NULL);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

        /* Flush */
        vcp->osal = NULL;

    } while (0);

    return status;
}

/**
 * \brief Lock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpLock(const UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                0U,
                                                                &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Lock */
        osalStatus = UShellOsalLock(osal, lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * \brief Unlock the vcp monitor
 * \param[in] vcp - pointer to a UShellVcp_s instance;
 * \return no;
 */
static void uShellVcpUnlock(const UShellVcp_s* const vcp)
{
    /* Check input parameters */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameters */
        if ((vcp == NULL) ||
            (vcp->osal == NULL))
        {
            break;
        }

        /* Cast */
        UShellOsal_s* osal = (UShellOsal_s*) vcp->osal;

        /* Get the lock object */
        UShellOsalLockObjHandle_t lockObj = NULL;
        UShellOsalErr_e osalStatus = UShellOsalLockObjHandleGet(osal,
                                                                0U,
                                                                &lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);
        USHELL_VCP_ASSERT(lockObj != NULL);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            break;
        }

        /* Unlock */
        osalStatus = UShellOsalUnlock(osal, lockObj);
        USHELL_VCP_ASSERT(osalStatus == USHELL_OSAL_NO_ERR);

    } while (0);
}

/**
 * @brief Set flags
 * @param vcp - pointer to a Vcp instance;
 * @param flags - flags to be set;
 * @return none;
 */
static void uShellVcpFlagSet(const UShellVcp_s* const vcp,
                             UShellVcpFlags_e flags)
{
    /* Check input parameter */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Check flags */
        if ((flags & USHELL_VCP_FLAG_ALL) == 0)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Find event handler */
        UShellOsalEventGroupHandle_t eventsHandler = NULL;
        UShellOsalErr_e osalStatus = UShellOsalEventGroupHandleGet((UShellOsal_s*) vcp->osal, 0U, &eventsHandler);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (eventsHandler == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set the flags */
        osalStatus = UShellEventGroupSetBits((UShellOsal_s*) vcp->osal,
                                             eventsHandler,
                                             flags);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * @brief Wait flag (blocked and not clear)
 * @param vcp - pointer to a vcp instance;
 * @param flags - flags to be set;
 * @param allWait - wait for all flags
 */
static void uShellVcpFlagsWait(const UShellVcp_s* const vcp,
                               UShellVcpFlags_e flags,
                               const bool allWait)
{
    /* Check input parameter */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Check flags */
        if ((flags & USHELL_VCP_FLAG_ALL) == 0)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Find event handler */
        UShellOsalEventGroupHandle_t eventsHandler = NULL;
        UShellOsalErr_e osalStatus = UShellOsalEventGroupHandleGet((UShellOsal_s*) vcp->osal, 0U, &eventsHandler);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (eventsHandler == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set the flags */
        UShellOsalEventGroupBits_e activeFlags = 0;
        osalStatus = UShellEventGroupBitsWait((UShellOsal_s*) vcp->osal,
                                              eventsHandler,
                                              flags,
                                              &activeFlags,
                                              false,
                                              allWait);

        /* Check status */
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * @brief Clear flag
 * @param vcp - pointer to a vcp instance;
 * @param flags - flags to be set;
 */
static void uShellVcpFlagClear(const UShellVcp_s* const vcp,
                               UShellVcpFlags_e flags)
{
    /* Check input parameter */
    USHELL_VCP_ASSERT(vcp != NULL);

    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Check flags */
        if ((flags & USHELL_VCP_FLAG_ALL) == 0)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Find event handler */
        UShellOsalEventGroupHandle_t eventsHandler = NULL;
        UShellOsalErr_e osalStatus = UShellOsalEventGroupHandleGet((UShellOsal_s*) vcp->osal, 0U, &eventsHandler);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (eventsHandler == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Set the flags */
        osalStatus = UShellEventGroupClearBits((UShellOsal_s*) vcp->osal,
                                               eventsHandler,
                                               flags);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * @brief Flag is set
 * @param vcp - pointer to a vcp instance
 * @param flags - flags to be set
 */
static bool uShellVcpFlagIsSet(const UShellVcp_s* const vcp,
                               UShellVcpFlags_e flags)
{
    /* Check input parameter */
    USHELL_VCP_ASSERT(vcp != NULL);

    /* Local */
    bool isSet = false;
    UShellOsalEventGroupBits_e activeFlags = 0;

    do
    {
        /* Check input parameter */
        if (vcp == NULL)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Check flags */
        if ((flags & USHELL_VCP_FLAG_ALL) == 0)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Find event handler */
        UShellOsalEventGroupHandle_t eventsHandler = NULL;
        UShellOsalErr_e osalStatus = UShellOsalEventGroupHandleGet((UShellOsal_s*) vcp->osal, 0U, &eventsHandler);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (eventsHandler == NULL))
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Get the flags */
        osalStatus = UShellEventGroupBitsActiveGet((UShellOsal_s*) vcp->osal,
                                                   eventsHandler,
                                                   &activeFlags);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            USHELL_VCP_ASSERT(0);
            break;
        }

        /* Return */
        isSet = (flags & activeFlags) ? true : false;

    } while (0);
}