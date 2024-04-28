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

#include "ushell_vt100.h"

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


/**
 * \brief Escape sequence for key press
*/
#define USHELL_VT100_ESC_SEQ_KEY_UP     "\x1B[A"
#define USHELL_VT100_ESC_SEQ_KEY_DOWN   "\x1B[B"
#define USHELL_VT100_ESC_SEQ_KEY_RIGHT  "\x1B[C"
#define USHELL_VT100_ESC_SEQ_KEY_LEFT   "\x1B[D"
#define USHELL_VT100_ESC_SEQ_KEY_DEL    "\x1B[3~"
#define USHELL_VT100_ESC_SEQ_KEY_HOME   "\x1B[H"
#define USHELL_VT100_ESC_SEQ_KEY_END    "\x1B[F"
#define USHELL_VT100_ESC_SEQ_KEY_PGUP   "\x1B[5~"
#define USHELL_VT100_ESC_SEQ_KEY_PGDN   "\x1B[6~"


/**
 * \brief Escape sequence for command color font
*/
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_BLACK     "\x1B[30m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_RED       "\x1B[31m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_GREEN     "\x1B[32m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_YELLOW    "\x1B[33m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_BLUE      "\x1B[34m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_MAGENTA   "\x1B[35m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_CYAN      "\x1B[36m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_WHITE     "\x1B[37m"
#define USHELL_VT100_ESC_SEQ_FONT_COLOR_DEFAULT   "\x1B[39m"

/**
 * \brief Escape sequence for command background color
*/
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_BLACK     "\x1B[40m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_RED       "\x1B[41m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_GREEN     "\x1B[42m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_YELLOW    "\x1B[43m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_BLUE      "\x1B[44m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_MAGENTA   "\x1B[45m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_CYAN      "\x1B[46m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_WHITE     "\x1B[47m"
#define USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_DEFAULT   "\x1B[49m"

/**
 * \brief Escape sequence for cursor and screen actions
*/
#define USHELL_VT100_ESC_SEQ_CURSOR_UP      "\x1B[A"
#define USHELL_VT100_ESC_SEQ_CURSOR_DOWN    "\x1B[B"
#define USHELL_VT100_ESC_SEQ_CURSOR_RIGHT   "\x1B[C"
#define USHELL_VT100_ESC_SEQ_CURSOR_LEFT    "\x1B[D"
#define USHELL_VT100_ESC_SEQ_CURSOR_HIDE    "\x1B[?25l"
#define USHELL_VT100_ESC_SEQ_CURSOR_SHOW    "\x1B[?25h"
#define USHELL_VT100_ESC_SEQ_SCREEN_CLEAR   "\x1B[2J"
#define USHELL_VT100_ESC_SEQ_LINE_CLEAR     "\x1B[K"

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShell)
 * \param[in] printHook - pointer to the print hook function
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Init(UShellVt100_s* const uShellVt100, const void* const parent, UShellVt100PrintHook_t hook)
{
    /* Check input parametr*/
    if((uShellVt100 == NULL) ||
        (hook == NULL) ||
        (parent == NULL))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Initialize the UShellVt100 object */
    memset(uShellVt100, 0, sizeof(UShellVt100_s));
    uShellVt100->parent = parent;
    uShellVt100->printHook = hook;

    /* Set the default font color and background color */
    uShellVt100->color.font = USHELL_VT100_FONT_COLOR_DEFAULT;
    uShellVt100->color.background = USHELL_VT100_BACKGROUND_COLOR_DEFAULT;

    return USHELL_VT100_NO_ERR;
}

/**
 * \brief Deinitialize the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object to be deinitialized
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Deinit(UShellVt100_s* const uShellVt100)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }



    /* Deinitialize the UShellVt100 object */
    memset(uShellVt100, 0, sizeof(UShellVt100_s));

    return USHELL_VT100_NO_ERR;

}

/**
 * \brief Attach the key callback function to the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] keyType - key type
 * \param[in] keyCb - key callback function
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100KeyCbAttach(UShellVt100_s* const uShellVt100, const UShellVt100KeyCbType_e keyType, const UShellVt100KeyCb_t keyCb)
{
    /* Check input parametr*/
    if((uShellVt100 == NULL) ||
        (keyCb == NULL))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Attach the key callback function */
    switch(keyType)
    {
        case USHELL_VT100_KEY_UP:
        case USHELL_VT100_KEY_DOWN:
        case USHELL_VT100_KEY_LEFT:
        case USHELL_VT100_KEY_RIGHT:
        case USHELL_VT100_KEY_DEL:
        case USHELL_VT100_KEY_HOME:
        case USHELL_VT100_KEY_END:
        case USHELL_VT100_KEY_PGUP:
        case USHELL_VT100_KEY_PGDN:

            uShellVt100->keyCb[keyType] = keyCb;
            break;

        default:
            return USHELL_VT100_INVALID_ARGS_ERR;
    }

    return USHELL_VT100_NO_ERR;
}

/**
 * \brief Detach the key callback function from the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] keyType - key type
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100KeyCbDetach(UShellVt100_s* const uShellVt100, const UShellVt100KeyCbType_e keyType)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Detach the key callback function */
    switch(keyType)
    {
        case USHELL_VT100_KEY_UP:
        case USHELL_VT100_KEY_DOWN:
        case USHELL_VT100_KEY_LEFT:
        case USHELL_VT100_KEY_RIGHT:
        case USHELL_VT100_KEY_DEL:
        case USHELL_VT100_KEY_HOME:
        case USHELL_VT100_KEY_END:
        case USHELL_VT100_KEY_PGUP:
        case USHELL_VT100_KEY_PGDN:

            uShellVt100->keyCb[keyType] = NULL;
            break;

        default:
            return USHELL_VT100_INVALID_ARGS_ERR;
    }

    return USHELL_VT100_NO_ERR;

}

/**
 * \brief Poll the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] data - pointer to the data to be processed
 * \param[in] dataSize - size of the data
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ParseEscapeSequency(UShellVt100_s* const uShellVt100, const UShellVt100Item_t* const data, const size_t dataSize)
{
    /* Check input parametr*/
    if((uShellVt100 == NULL) ||
        (data == NULL) ||
        (dataSize == 0))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;       ///< Exit: error - invalid pointers
    }

    do
    {
        /* Check the size of the data */
        if((dataSize < USHELL_VT100_ESCAPE_SEQUENCE_SIZE_MIN) ||
            (dataSize > USHELL_VT100_ESCAPE_SEQUENCE_SIZE_MAX))
        {
            break;
        }

        uint8_t index = 0;

        /* Find key */
        if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_UP, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_UP;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_DOWN, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_DOWN;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_RIGHT, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_RIGHT;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_LEFT, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_LEFT;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_DEL, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_DEL;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_HOME, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_HOME;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_END, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_END;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_PGUP, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_PGUP;
        }
        else if(strncmp(data, USHELL_VT100_ESC_SEQ_KEY_PGDN, dataSize) == 0)
        {
            index = USHELL_VT100_KEY_PGDN;
        }
        else
        {
            break;
        }

        /* Check the key callback function */
        if(uShellVt100->keyCb[index] == NULL)
        {
            break;
        }

        /* Execute the key callback function */
        uShellVt100->keyCb[index](uShellVt100);

    } while (0);


    return USHELL_VT100_NO_ERR;                     ///< Exit: no errors
}

/**
 * \brief Set the font color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] fontColor - font color to be set
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ColorFontSet(UShellVt100_s* const uShellVt100, const UShellVt100ColorFont_e fontColor)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Find the escape sequence for the font color */
    const char* escSeq = NULL;
    size_t escSeqSize = 0;
    switch(fontColor)
    {
        case USHELL_VT100_FONT_COLOR_BLACK:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_BLACK;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_BLACK);
            break;

        case USHELL_VT100_FONT_COLOR_RED:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_RED;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_RED);
            break;

        case USHELL_VT100_FONT_COLOR_GREEN:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_GREEN;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_GREEN);
            break;

        case USHELL_VT100_FONT_COLOR_YELLOW:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_YELLOW;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_YELLOW);
            break;

        case USHELL_VT100_FONT_COLOR_BLUE:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_BLUE;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_BLUE);
            break;

        case USHELL_VT100_FONT_COLOR_MAGENTA:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_MAGENTA;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_MAGENTA);
            break;

        case USHELL_VT100_FONT_COLOR_CYAN:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_CYAN;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_CYAN);
            break;

        case USHELL_VT100_FONT_COLOR_WHITE:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_WHITE;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_WHITE);
            break;

        case USHELL_VT100_FONT_COLOR_DEFAULT:
            escSeq = USHELL_VT100_ESC_SEQ_FONT_COLOR_DEFAULT;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_FONT_COLOR_DEFAULT);
            break;

        default:
            return USHELL_VT100_INVALID_ARGS_ERR;       ///< Exit: error - invalid pointers
            break;
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, escSeq, escSeqSize);
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;       ///< Exit: error - port error
    }

    /* Set the font color */
    uShellVt100->color.font = fontColor;

    return USHELL_VT100_NO_ERR;             ///< Exit: no errors
}

/**
 * \brief Get the font color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] fontColor - pointer to store the font color
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ColorFontGet(UShellVt100_s* const uShellVt100, UShellVt100ColorFont_e* const fontColor)
{
    /* Check input parametr*/
    if((uShellVt100 == NULL) ||
        (fontColor == NULL))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Get the font color */
    *fontColor = uShellVt100->color.font;

    return USHELL_VT100_NO_ERR;

}

/**
 * \brief Set the background color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] backgroundColor - background color to be set
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ColorBackgroundSet(UShellVt100_s* const uShellVt100, const UShellVt100ColorBackground_e backgroundColor)
{
    /* Check input parametr */
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Find the escape sequence for the background color */
    const char* escSeq = NULL;
    size_t escSeqSize = 0;
    switch(backgroundColor)
    {
        case USHELL_VT100_BACKGROUND_COLOR_BLACK:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_BLACK;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_BLACK);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_RED:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_RED;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_RED);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_GREEN:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_GREEN;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_GREEN);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_YELLOW:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_YELLOW;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_YELLOW);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_BLUE:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_BLUE;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_BLUE);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_MAGENTA:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_MAGENTA;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_MAGENTA);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_CYAN:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_CYAN;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_CYAN);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_WHITE:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_WHITE;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_WHITE);
            break;

        case USHELL_VT100_BACKGROUND_COLOR_DEFAULT:
            escSeq = USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_DEFAULT;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_BACKGROUND_COLOR_DEFAULT);
            break;

        default:
            return USHELL_VT100_INVALID_ARGS_ERR;       ///< Exit: error - invalid pointers
            break;
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, escSeq, escSeqSize);
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;                   ///< Exit: error - port error
    }

    /* Set the background color */
    uShellVt100->color.background = backgroundColor;

    return USHELL_VT100_NO_ERR;                         ///< Exit: no errors
}

/**
 * \brief Get the background color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] backgroundColor - pointer to store the background color
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ColorBackgroundGet(UShellVt100_s* const uShellVt100, UShellVt100ColorBackground_e* const backgroundColor)
{
    /* Check input parametr*/
    if((uShellVt100 == NULL) ||
        (backgroundColor == NULL))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Get the background color */
    *backgroundColor = uShellVt100->color.background;

    return USHELL_VT100_NO_ERR;

}

/**
 * \brief Set next column position of cursor in the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CursorColumnNext(UShellVt100_s* const uShellVt100)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, USHELL_VT100_ESC_SEQ_CURSOR_RIGHT, strlen(USHELL_VT100_ESC_SEQ_CURSOR_RIGHT));
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;       ///< Exit: error - port error
    }

    /* Set the cursor position */
    uShellVt100->cursor.columnPos++;

    return USHELL_VT100_NO_ERR;             ///< Exit: no errors
}

/**
 * \brief Set previous column position of cursor in the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CursorColumnPrevious(UShellVt100_s* const uShellVt100)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Check current state */
    if(uShellVt100->cursor.columnPos == 0)
    {
        return USHELL_VT100_NO_ERR;         ///< Exit: no errors
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, USHELL_VT100_ESC_SEQ_CURSOR_LEFT, strlen(USHELL_VT100_ESC_SEQ_CURSOR_LEFT));
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;       ///< Exit: error - port error
    }

    return USHELL_VT100_NO_ERR;             ///< Exit: no errors
}

/**
 * \brief Set next row position of cursor in the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CursorRowNext(UShellVt100_s* const uShellVt100)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, USHELL_VT100_ESC_SEQ_CURSOR_DOWN, strlen(USHELL_VT100_ESC_SEQ_CURSOR_DOWN));
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;       ///< Exit: error - port error
    }

    /* Set the cursor position */
    uShellVt100->cursor.rowPos++;

    return USHELL_VT100_NO_ERR;             ///< Exit: no errors
}

/**
 * \brief Set previous row position of cursor in the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CursorRowPrevious(UShellVt100_s* const uShellVt100)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Check current state */
    if(uShellVt100->cursor.rowPos == 0)
    {
        return USHELL_VT100_NO_ERR;         ///< Exit: no errors
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, USHELL_VT100_ESC_SEQ_CURSOR_UP, strlen(USHELL_VT100_ESC_SEQ_CURSOR_UP));
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;       ///< Exit: error - port error
    }

    return USHELL_VT100_NO_ERR;             ///< Exit: no errors
}

/**
 * \brief Set cursor show/hide state in the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] show - cursor show/hide state
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CursorShowStateSet(UShellVt100_s* const uShellVt100, const UShellVt100CursorShow_t show)
{
    /* Check input parametr*/
    if(uShellVt100 == NULL)
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Check init state */
    if((uShellVt100->parent == NULL) ||
        (uShellVt100->printHook == NULL))
    {
        return USHELL_VT100_NOT_INIT_ERR;
    }

    /* Find the escape sequence for the cursor show/hide state */
    const char* escSeq = NULL;
    size_t escSeqSize = 0;
    switch(show)
    {
        case true:
            escSeq = USHELL_VT100_ESC_SEQ_CURSOR_SHOW;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_CURSOR_SHOW);
            break;

        case false:
            escSeq = USHELL_VT100_ESC_SEQ_CURSOR_HIDE;
            escSeqSize = strlen(USHELL_VT100_ESC_SEQ_CURSOR_HIDE);
            break;

        default:
            return USHELL_VT100_INVALID_ARGS_ERR;       ///< Exit: error - invalid pointers
            break;
    }

    /* Print the escape sequence */
    UShellVt100Err_e status = uShellVt100->printHook(uShellVt100->parent, escSeq, escSeqSize);
    if(status != USHELL_VT100_NO_ERR)
    {
        return USHELL_VT100_PORT_ERR;       ///< Exit: error - port error
    }

    return USHELL_VT100_NO_ERR;             ///< Exit: no errors
}

/**
 * \brief Get cursor show/hide state in the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] show - pointer to store the cursor show/hide state
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CursorShowStateGet(UShellVt100_s* const uShellVt100, UShellVt100CursorShow_t* const show)
{
    /* Check input parametr*/
    if((uShellVt100 == NULL) ||
        (show == NULL))
    {
        return USHELL_VT100_INVALID_ARGS_ERR;
    }

    /* Get the cursor show/hide state */
    *show = uShellVt100->cursor.show;

    return USHELL_VT100_NO_ERR;
}


//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================
