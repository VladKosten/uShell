#ifndef USHELL_AUTH_H_
#define USHELL_AUTH_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * @brief Enumeration of possible error codes returned
 */
typedef enum
{
    USHELL_AUTH_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_AUTH_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_AUTH_NOT_INIT_ERR,        ///< Exit: error - not initialized

} UShellAuthErr_e;

/**
 * @brief Description of the uShell authentefiaction object
 */
typedef struct
{
    const void* parent;      ///< Parent object
    const char* password;    ///< Password of the object
    bool isAuth;             ///< Is object authenticated
} UShellAuth_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Initialize UShell auth object
 * @param[in] auth - uShell auth object to be initialized
 * @param[in] parent - parent object
 * @param[in] password - password of the object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthInit(UShellAuth_s* const auth,
                               const void* const parent,
                               const char* const password);

/**
 * @brief DeInit UShell auth object
 * @param[in] auth - uShell auth object to be deinitialized
 * @return UShellAuthErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthDeInit(UShellAuth_s* const auth);

/**
 * @brief Process the authentication
 * @param[in] auth - uShell auth object
 * @param[in] password -
 * @return UShellAuthErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthProcess(UShellAuth_s* const auth,
                                  const char* const password);

/**
 * @brief Is authenticated
 * @param[in] auth - uShell auth object
 * @param[out] isAuth - is authenticated
 * @return UShellAuthErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellIsAuth(UShellAuth_s* const auth, bool* const isAuth);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_AUTH_H_ */