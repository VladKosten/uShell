/**
* \file         uShell_vt100.c
* \brief        This file contains the implementation of the UShellVt100 module.
*               This module is responsible for handling the VT100 terminal commands.
* \authors      Vladislav Kosten (vladkosten@gmail.com)
* \copyright    2024. Vladislav Kosten. All rights reserved.
* \warning      A warning may be placed here...
* \bug          Bug report may be placed here...
*/
//===============================================================================[ INCLUDE ]========================================================================================

#include "uShell_vt100.h"

//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
* \brief Assert macro for the UShell module.
*/
#ifndef USHELL_VT100_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_VT100_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_VT100_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================




//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShellVt100 object
 *
 *
 * \param[in] uShellVt100 - pointer to the UShellVt100 object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShell)
 * \param[in] printHook - pointer to the print hook function
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Init(UShellVt100_s* const uShellVt100, const void* const parent, UShellVt100PrintHook_t hook)
{
    /* Check input parameter */
    if ((uShellVt100 == NULL) ||
        (hook == NULL) ||
        (parent == NULL))      ttt
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Initialize the UShellVt100 object */
    uShellVt100->parent = parent;
    uShellVt100->hook = hook;
    uShellVt100->fontColor = USHELL_VT100_FONT_COLOR_WHITE;
    uShellVt100->backgroundColor = USHELL_VT100_BACKGROUND_COLOR_BLACK;
    uShellVt100->cb = NULL;


    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)

}

/**
 * \brief Deinitialize the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object to be deinitialized
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Deinit(UShellVt100_s* const uShellVt100)
{
    /* Check input parameter */
    if (uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Deinitialize the UShellVt100 object */
    memset(uShellVt100, 0, sizeof(UShellVt100_s));

    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)

}

/**
 * \brief Poll the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] data - pointer to the data to be processed
 * \param[in] dataSize - size of the data
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Poll(UShellVt100_s* const uShellVt100, const char* const data, const size_t dataSize)
{
    /* Check input parameter */
    if ((uShellVt100 == NULL) ||
        (data == NULL) ||
        (dataSize == 0))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Check if the print hook is set */
    if (uShellVt100->hook == NULL)
    {
        return USHELL_VT100_NOT_INIT_ERR;           // Exit: error - not initialized
    }

    if()

    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)


}

/**
 * \brief Print the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] action - action to be performed
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100KeyAction(UShellVt100_s* const uShellVt100, const UShellVt100KeyAction_e action);

/**
 * \brief Set the font color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] fontColor - font color to be set
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100FontColorSet(UShellVt100_s* const uShellVt100, const UShellVt100FontColor_e fontColor)
{

}

/**
 * \brief Get the font color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] fontColor - pointer to store the font color
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100FontColorGet(UShellVt100_s* const uShellVt100, UShellVt100FontColor_e* const fontColor)
{
    /* Check input parameter */
    if (uShellVt100 == NULL || fontColor == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Get the font color */
    *fontColor = uShellVt100->fontColor;

    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)

}

/**
 * \brief Set the background color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] backgroundColor - background color to be set
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100BackgroundColorSet(UShellVt100_s* const uShellVt100, const UShellVt100BackgroundColor_e backgroundColor);

/**
 * \brief Get the background color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] backgroundColor - pointer to store the background color
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100BackgroundColorGet(UShellVt100_s* const uShellVt100, UShellVt100BackgroundColor_e* const backgroundColor)
{
    /* Check input parameter */
    if (uShellVt100 == NULL || backgroundColor == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Get the background color */
    *backgroundColor = uShellVt100->backgroundColor;


    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)
}

/**
 * \brief Attach callback to the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] cb - pointer to the callback function
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CbAttach(UShellVt100_s* const uShellVt100, const UShellVt100Callback_s* const cb)
{
    /* Check input parameter */
    if (uShellVt100 == NULL || cb == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Attach callback */
    uShellVt100->cb = (UShellVt100Callback_s*)cb;

    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)

}

/**
 * \brief Detach callback from the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CbDetach(UShellVt100_s* const uShellVt100)
{
    /* Check input parameter */
    if (uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       // Exit: error - invalid pointers (e.g. null pointers)
    }

    /* Detach callback */
    uShellVt100->cb = NULL;

    return USHELL_VT100_NO_ERR;                     // Exit: no errors (success)
}


//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
