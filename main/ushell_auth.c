/**
 * \file         ushell_auth.c
 * @brief        The file contains the implementation of the UShell authentication module.
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_auth.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * @brief Assert macro for the UShellHal module.
 */
#ifndef USHELL_AUTH_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_AUTH_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_AUTH_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * @brief Initialize UShell auth object
 * @param[in] auth - uShell auth object to be initialized
 * @param[in] parent - parent object
 * @param[in] password - password of the object
 * @return UShellErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthInit(UShellAuth_s* const auth,
                               const void* const parent,
                               const char* const password)
{
    /* Check input parameter */
    USHELL_AUTH_ASSERT(auth != NULL);
    USHELL_AUTH_ASSERT(password != NULL);
    USHELL_AUTH_ASSERT(parent != NULL);

    /* Local variable */
    UShellAuthErr_e status = USHELL_AUTH_NO_ERR;

    /* Initialize the object */
    do
    {
        /* Check input parameter */
        if ((auth == NULL) ||
            (password == NULL) ||
            (parent == NULL))
        {
            status = USHELL_AUTH_INVALID_ARGS_ERR;
            break;
        }

        /* Save the fields */
        auth->password = password;
        auth->parent = parent;

        /* Set the object as not authenticated */
        auth->isAuth = false;

    } while (0);

    return status;
}

/**
 * @brief DeInit UShell auth object
 * @param[in] auth - uShell auth object to be deinitialized
 * @return UShellAuthErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthDeInit(UShellAuth_s* const auth)
{
    /* Check input parameter */
    USHELL_AUTH_ASSERT(auth != NULL);

    /* Local variable */
    UShellAuthErr_e status = USHELL_AUTH_NO_ERR;

    /* Clear the object */
    do
    {
        /* Check input parameter */
        if (auth == NULL)
        {
            status = USHELL_AUTH_INVALID_ARGS_ERR;
            break;
        }

        /* Clear the object */
        auth->password = NULL;
        auth->parent = NULL;
        auth->isAuth = false;

    } while (0);

    return status;
}

/**
 * @brief Process the authentication
 * @param[in] auth - uShell auth object
 * @param[in] password -
 * @return UShellAuthErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellAuthProcess(UShellAuth_s* const auth,
                                  const char* const password)
{
    /* Check input parameter */
    USHELL_AUTH_ASSERT(auth != NULL);
    USHELL_AUTH_ASSERT(password != NULL);

    /* Local variable */
    UShellAuthErr_e status = USHELL_AUTH_NO_ERR;

    /* Process the authentication */
    do
    {
        /* Check input parameter */
        if ((auth == NULL) ||
            (password == NULL))
        {
            status = USHELL_AUTH_INVALID_ARGS_ERR;
            break;
        }

        /* Check the password */
        if (strcmp(auth->password, password) != 0)
        {
            auth->isAuth = false;
            break;
        }

        auth->isAuth = true;

    } while (0);

    return status;
}

/**
 * @brief Is authenticated
 * @param[in] auth - uShell auth object
 * @param[out] isAuth - is authenticated
 * @return UShellAuthErr_e - error code. non-zero = an error has occurred;
 */
UShellAuthErr_e UShellIsAuth(UShellAuth_s* const auth,
                             bool* const isAuth)
{
    /* Check input parameter */
    USHELL_AUTH_ASSERT(auth != NULL);
    USHELL_AUTH_ASSERT(isAuth != NULL);

    /* Local variable */
    UShellAuthErr_e status = USHELL_AUTH_NO_ERR;

    /* Check the authentication */
    do
    {
        /* Check input parameter */
        if ((auth == NULL) ||
            (isAuth == NULL))
        {
            status = USHELL_AUTH_INVALID_ARGS_ERR;
            break;
        }

        /* Get the authentication */
        *isAuth = auth->isAuth;

    } while (0);

    return status;
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
