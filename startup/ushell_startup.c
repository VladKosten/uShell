/**
 * \file      ushell_startup.c
 * \brief     This module represents initialization procedure and uShell startup
 *            in which all other application tasks are created and additional modules are initialized,
 *            allocated all required resources for the application
 * \authors   Vladislav Kosten (vladkosten@gmail.com)
 * \copyright MIT License (c) 2025
 * \warning   A warning may be placed here...
 * \bug       Bug report may be placed here...
 */

//===============================================================================[ INCLUDE ]=======================================================================================

#include <stdint.h>
#include <stddef.h>

#include "ushell_startup.h"
#include "ushell.h"
#include "ushell_vcp.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief MatrixKbd ASSERT macro definition
 */
#ifndef USHELL_STARTUP_ASSERT
    #define USHELL_STARTUP_ASSERT(cond)
#endif

/**
 * @brief DioMonitor startup OS-port type
 *
 * This type defines the OSAL port implementation for the DioMonitor startup.
 */
typedef USHELL_STARTUP_OSAL_PORT_TYPE UShellStartupOsal_s;

/**
 * @brief DioMonitor startup HW-port type
 *
 * This type defines the HAL port implementation for the DioMonitor startup.
 */
typedef USHELL_STARTUP_HAL_PORT_TYPE UShellStartupHal_s;

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

/**
 * \brief Instance of the uShell
 */
UShell_s uShellObj = {0};

/**
 * @brief Instance of the uShell VCP
 */
UShellVcp_s uShellVcpObj = {0};

/**
 * \brief Instance of the uShell OSAL
 */
static UShellStartupOsal_s uShellStartupOsalObj = {0};

/**
 * \brief Instance of the uShell OSAL for the uShell
 */
static UShellStartupOsal_s uShellStartupVcpOsalObj = {0};

/**
 * \brief Instance of the uShell HAL port
 */
static UShellStartupHal_s uShellStartupVcpHalObj = {0};

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * @brief uShell VCP initialization
 * @param void
 * @return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpInit(void);

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]==============================================================================

/**
 * \brief The uShell startup procedure
 * \param[in] void;
 * \param[out]  no;
 * \return int16_t - error code. non-zero = an error has occurred;
 */
int16_t UShellStartup(void)
{
    /* Local variable */
    int16_t status = 0;

    do
    {
        /* Initialize the uShell VCP */
        status = uShellVcpInit();
        if (status != 0)
        {
            USHELL_STARTUP_ASSERT(0);
            break;
        }

#ifdef USHELL_STARTUP_OSAL_PORT_FREERTOS
        /* Initialize the uShell OSAL */
        UShellOsalErr_e osalErr = UShellOsalFreertosInit(&uShellStartupOsalObj,
                                                         &uShellObj,
                                                         USHELL_STARTUP_OSAL_PORT_NAME);
        USHELL_STARTUP_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
        if (osalErr != USHELL_OSAL_NO_ERR)
        {
            return -1;
        }
#endif

        /* Initialize the uShell */
        UShellCfg_s ushellCfg = {
            .authIsEn = USHELL_STARTUP_AUTH_IS_EN,
            .echoIsEn = USHELL_STARTUP_ECHO_IS_EN,
            .historyIsEn = USHELL_STARTUP_HISTORY_IS_EN,
            .promptIsEn = USHELL_STARTUP_PROMPT_IS_EN};
        UShellErr_e ushellErr = UShellInit(&uShellObj,
                                           &uShellStartupOsalObj.base,
                                           &uShellVcpObj,
                                           ushellCfg,
                                           NULL,
                                           USHELL_STARTUP_NAME);
        USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR);
        if (ushellErr != USHELL_NO_ERR)
        {
            return -1;
        }

    } while (0);

    return status;
}

/**
 * @brief uShell VCP initialization
 * @param void
 * @return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpInit(void)
{
    UShellHalPortErr_e halPortErr = USHELL_HAL_PORT_NO_ERR;
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    UShellVcpErr_e vcpErr = USHELL_VCP_NO_ERR;

#ifdef USHELL_STARTUP_OSAL_PORT_FREERTOS
    /* Initialize the uShell OSAL */
    osalErr = UShellOsalFreertosInit(&uShellStartupVcpOsalObj,
                                     &uShellVcpObj,
                                     USHELL_STARTUP_VCP_OSAL_PORT_NAME);
    USHELL_STARTUP_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
    if (osalErr != USHELL_OSAL_NO_ERR)
    {
        return -1;
    }
#endif

#ifdef USHELL_STARTUP_HAL_PORT_ATMEL

    /* Initialize the uShell HAL */
    UShellHalPortCfg_s halPortCfg = {
        .transceiverEnabled = USHELL_STARTUP_HAL_PORT_ASF_TRANSCEIVER_ENABLED,
        .transceiverPins.rxPin = USHELL_STARTUP_HAL_PORT_ASF_TRANSCEIVER_RX_PIN,
        .transceiverPins.rxPinActive = USHELL_STARTUP_HAL_PORT_ASF_TRANSCEIVER_RX_PIN_ACTIVE,
        .transceiverPins.txPin = USHELL_STARTUP_HAL_PORT_ASF_TRANSCEIVER_TX_PIN,
        .transceiverPins.txPinActive = USHELL_STARTUP_HAL_PORT_ASF_TRANSCEIVER_TX_PIN_ACTIVE};

    halPortErr = UShellHalPortInit(&uShellStartupVcpHalObj,
                                   USHELL_STARTUP_HAL_PORT_UART_DESC,
                                   halPortCfg,
                                   &uShellObj,
                                   USHELL_STARTUP_HAL_PORT_NAME);
    USHELL_STARTUP_ASSERT(halPortErr == USHELL_HAL_PORT_NO_ERR);
    if (halPortErr != USHELL_HAL_PORT_NO_ERR)
    {
        return -1;
    }

#endif

    /* Initialize the uShell VCP */
    vcpErr = UShellVcpInit(&uShellVcpObj,
                           &uShellStartupVcpOsalObj.base,
                           &uShellStartupVcpHalObj.base,
                           true,
                           &uShellObj,
                           USHELL_VCP_NAME);
    USHELL_STARTUP_ASSERT(vcpErr == USHELL_VCP_NO_ERR);
    if (vcpErr != USHELL_VCP_NO_ERR)
    {
        return -1;
    }

    return 0;
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================