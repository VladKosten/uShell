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
#include "ushell_cmd_help.h"
#include "ushell_cmd_clear.h"
#include "ushell_cmd_whoami.h"
#include "ushell_cmd_fs.h"
#include "msp.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief MatrixKbd ASSERT macro definition
 */
#ifndef USHELL_STARTUP_ASSERT
    #define USHELL_STARTUP_ASSERT(cond)
#endif

/**
 * \brief DioMonitor startup OS-port type
 *
 * This type defines the OSAL port implementation for the DioMonitor startup.
 */
typedef USHELL_STARTUP_OSAL_PORT_TYPE UShellStartupOsal_s;

/**
 * \brief DioMonitor startup HW-port type
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
 * \brief Instance of the uShell VCP
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
 * \brief uShell OSAL initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellOsalInit(void);

/**
 * \brief uShell VCP OSAL initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpOsalInit(void);

/**
 * \brief uShell HAL initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpHalInit(void);

/**
 * \brief uShell VCP initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpInit(void);

/**
 * \brief uShell command help initialization
 * \param rootCmd - The first cmd in the list of commands to be initialized
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdHelpInit(void);

/**
 * \brief uShell command clear initialization
 * \param none
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdClearInit(void);

/**
 * \brief uShell command whoami initialization
 * \param none
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdWhoAmiInit(void);

/**
 * \brief uShell Fs  command fs initialization
 * \param none
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdFsInit(void);

/**
 * \brief uShell command initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdInit(void);

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

        /* Initialize the uShell OSAL */
        status = uShellOsalInit();
        if (status != 0)
        {
            USHELL_STARTUP_ASSERT(0);
            break;
        }

        /* Initialize the uShell command */
        status = uShellCmdInit();
        if (status != 0)
        {
            USHELL_STARTUP_ASSERT(0);
            break;
        }

        /* Prepare the uShell cfg */
        UShellCfg_s ushellCfg = {
            .authIsEn = USHELL_STARTUP_AUTH_IS_EN,
            .historyIsEn = USHELL_STARTUP_HISTORY_IS_EN,
            .promptIsEn = USHELL_STARTUP_PROMPT_IS_EN};

        /* Initialize the uShell */
        UShellErr_e ushellErr = UShellInit(&uShellObj,
                                           &uShellStartupOsalObj.base,
                                           &uShellVcpObj,
                                           &ushellCfg,
                                           NULL,
                                           USHELL_STARTUP_NAME,
                                           &uShellCmdHelp.cmd);
        USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR);
        if (ushellErr != USHELL_NO_ERR)
        {
            return -1;
        }

    } while (0);

    return status;
}

/**
 * \brief uShell OSAL initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellOsalInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the uShell OSAL */
#ifdef USHELL_STARTUP_OSAL_PORT_FREERTOS
    /* Initialize the uShell OSAL */
    UShellOsalErr_e osalErr = UShellOsalFreertosInit(&uShellStartupOsalObj,
                                                     (void*) &uShellObj,
                                                     USHELL_STARTUP_OSAL_PORT_NAME);
    USHELL_STARTUP_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
    if (osalErr != USHELL_OSAL_NO_ERR)
    {
        return -1;
    }
#endif

    return status;
}

/**
 * \brief uShell VCP OSAL initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpOsalInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the uShell OSAL */
#ifdef USHELL_STARTUP_OSAL_PORT_FREERTOS
    UShellOsalErr_e osalErr = USHELL_OSAL_NO_ERR;
    /* Initialize the uShell OSAL */
    osalErr = UShellOsalFreertosInit(&uShellStartupVcpOsalObj,
                                     (void*) &uShellVcpObj,
                                     USHELL_STARTUP_VCP_OSAL_PORT_NAME);
    USHELL_STARTUP_ASSERT(osalErr == USHELL_OSAL_NO_ERR);
    if (osalErr != USHELL_OSAL_NO_ERR)
    {
        return -1;
    }
#endif

    return status;
}

/**
 * \brief uShell HAL initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpHalInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the uShell HAL */
#ifdef USHELL_STARTUP_HAL_PORT_ATMEL
    UShellHalPortErr_e halPortErr = USHELL_HAL_PORT_NO_ERR;
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

    return status;
}

/**
 * \brief uShell VCP initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellVcpInit(void)
{
    /* Local variable */
    int16_t status = 0;
    UShellVcpErr_e vcpErr = USHELL_VCP_NO_ERR;

    /* Initialize the uShell HAL */
    status = uShellVcpHalInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Initialize the uShell VCP OSAL */
    status = uShellVcpOsalInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

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

/**
 * \brief uShell command help initialization
 * \param rootCmd - The first cmd in the list of commands to be initialized
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdHelpInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the UShell command help */
    status = UShellCmdHelpInit(&uShellCmdHelp.cmd);
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    return status;
}

/**
 * \brief uShell command clear initialization
 * \param rootCmd - The first cmd in the list of commands to be initialized
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdClearInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the UShell command clear */
    status = UShellCmdClearInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    UShellCmdErr_e cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                                &uShellCmdClear.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    return status;
}

/**
 * \brief uShell command whoami initialization
 * \param none
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdWhoAmiInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the UShell command clear */
    status = UShellCmdWhoAmIInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    UShellCmdErr_e cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                                &uShellCmdWhoAmI.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    return status;
}

/**
 * \brief uShell Fs  command fs initialization
 * \param none
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdFsInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the UShell command fs */
    lfs_t* lfs = MspSearch(22);
    status = UShellCmdFsInit(lfs, &uShellVcpObj);
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    UShellCmdErr_e cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                                &uShellCmdFs.cmdLs.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                 &uShellCmdFs.cmdCd.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                 &uShellCmdFs.cmdRm.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                 &uShellCmdFs.cmdMkdir.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                 &uShellCmdFs.cmdCat.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    /* Add the command to the list of commands */
    cmdStatus = UShellCmdListAdd(&uShellCmdHelp.cmd,
                                 &uShellCmdFs.cmdWrite.cmd);
    if (cmdStatus != USHELL_CMD_NO_ERR)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    return status;
}

/**
 * \brief uShell command initialization
 * \param void
 * \return int16_t - error code. non-zero = an error has occurred;
 */
static int16_t uShellCmdInit(void)
{
    /* Local variable */
    int16_t status = 0;

    /* Initialize the UShell command */

    status = uShellCmdHelpInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    status = uShellCmdClearInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    status = uShellCmdWhoAmiInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    status = uShellCmdFsInit();
    if (status != 0)
    {
        USHELL_STARTUP_ASSERT(0);
        return -1;
    }

    return status;
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================