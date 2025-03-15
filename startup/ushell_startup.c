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
#include <assert.h>

#include "ushell_start.h"
#include "ushell.h"
#include "ushell_osal_port.h"
#include "ushell_hal_port.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief MatrixKbd ASSERT macro definition
 */
#ifndef USHELL_STARTUP_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_STARTUP_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_STARTUP_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

/**
 * \brief Instance of the uShell
 */
UShell_s uShellObj = {0};

/**
 * \brief Instance of the uShell OSAL for the uShell
 */
static UShellOsalPort_s uShellStartupOsalObj = {0};

/**
 * \brief Instance of the uShell HAL port
 */
static UShellHalPort_s uShellStartupHalObj = {0};

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[PUBLIC INTERFACE FUNCTIONS]==============================================================================

/**
 * \brief The uShell startup procedure
 * \param[in] void;
 * \param[out]  no;
 * \return int16_t - error code. non-zero = an error has occurred;
 */
int16_t UShellStartup(void)
{
    UShellHalPortErr_e halPortErr = USHELL_HAL_PORT_NO_ERR;
    UShellOsalPortErr_e osalPortErr = USHELL_OSAL_PORT_NO_ERR;
    UShellErr_e ushellErr = USHELL_NO_ERR;

    /* Initialize the uShell OSAL */
    osalPortErr = UShellOsalPortInit(&uShellStartupOsalObj, &uShellObj, "uShellOsal");
    USHELL_STARTUP_ASSERT(osalPortErr == USHELL_OSAL_PORT_NO_ERR);
    if (osalPortErr != USHELL_OSAL_PORT_NO_ERR)
    {
        return -1;
    }

    /* Initialize the uShell HAL */
    halPortErr = UShell_HalPortInit(&uShellStartupHalObj, &uShellObj, "uShellHal");
    USHELL_STARTUP_ASSERT(halPortErr == USHELL_HAL_PORT_NO_ERR);
    if (halPortErr != USHELL_HAL_PORT_NO_ERR)
    {
        return -1;
    }

    /* Initialize the uShell */
    ushellErr = UShellInit(&uShellObj, &uShellStartupHalObj.base, &uShellStartupOsalObj.base, NULL, "uShell");
    USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR)
    if (ushellErr != USHELL_NO_ERR)
    {
        return -1;
    }

    /* Start the uShell */
    ushellErr = UShellRun(&uShellObj);
    USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR)
    if (ushellErr != USHELL_NO_ERR)
    {
        return -1;
    }

    return 0;
}

/**
 * \brief The uShell shutdown procedure
 * \param[in] void;
 * \param[out]  no;
 * \return int16_t - error code. non-zero = an error has occurred;
 */
int16_t UShellShutdown(void)
{
    UShellErr_e ushellErr = USHELL_NO_ERR;

    /* Stop the uShell */
    ushellErr = UShellStop(&uShellObj);
    USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR)
    if (ushellErr != USHELL_NO_ERR)
    {
        return -1;
    }

    /* Deinitialize the uShell */
    ushellErr = UShellDeInit(&uShellObj);
    USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR)
    if (ushellErr != USHELL_NO_ERR)
    {
        return -1;
    }

    /* Deinitialize the uShell HAL */
    UShell_HalPortDeInit(&uShellStartupHalObj);
    USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR)
    if (ushellErr != USHELL_NO_ERR)
    {
        return -1;
    }

    /* Deinitialize the uShell OSAL */
    UShellOsalPortDeinit(&uShellStartupOsalObj);
    USHELL_STARTUP_ASSERT(ushellErr == USHELL_NO_ERR)
    if (ushellErr != USHELL_NO_ERR)
    {
        return -1;
    }

    return 0;
}

//============================================================================[PRIVATE FUNCTIONS]===================================================================================