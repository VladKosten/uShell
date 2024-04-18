
#ifndef USHELL_START_H_
#define USHELL_START_H_

#ifdef __cplusplus
extern "C"{
#endif


/*================================================================[INCLUDE]================================================*/
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "ushell.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief uShell assert definition
*/
#ifndef USHELL_STARTUP_ASSERT
    #define USHELL_STARTUP_ASSERT(cond)                    assert((cond))
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
* \brief The uShell startup procedure
* \param[in] void;
* \param[out]  no;
* \return int16_t - error code. non-zero = an error has occurred;
 */
int16_t UShellStartup(void);

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