
#ifndef USHELL_START_H_
#define USHELL_START_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]================================================*/
#include <stdint.h>
#include <stddef.h>

#include "ushell_cfg.h"
#include "ushell.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief Check define for uShell startup name
 */
#ifndef USHELL_STARTUP_NAME
    #error USHELL_STARTUP_NAME not defined
#endif

/**
 * \brief Check define for uShell startup version
 */
#ifndef USHELL_STARTUP_OSAL_PORT_TYPE
    #error USHELL_STARTUP_OSAL_PORT_TYPE not defined
#endif

/**
 * \brief Check define for uShell startup version
 */
#ifndef USHELL_STARTUP_HAL_PORT_TYPE
    #error USHELL_STARTUP_HAL_PORT_TYPE not defined
#endif

/**
 * \brief Check define for uShell startup version
 */
#ifndef USHELL_STARTUP_OSAL_PORT_TYPE
    #error USHELL_STARTUP_OSAL_PORT_TYPE not defined
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief The uShell startup procedure
 * \param[in] void* const littleFs - pointer to the littlefs object(optional)
 * \param[out]  no;
 * \return int16_t - error code. non-zero = an error has occurred;
 */
int16_t UShellStartup(void* const littleFs);

/**
 * \brief The uShell shutdown procedure
 * \param[in] void;
 * \param[out]  no;
 * \return int16_t - error code. non-zero = an error has occurred;
 */
int16_t UShellShutdown(void);

/**
 * \brief The uShell object
 */
extern UShell_s uShellObj;

#ifdef __cplusplus
}
#endif

#endif /* USHELL_START_H_ */