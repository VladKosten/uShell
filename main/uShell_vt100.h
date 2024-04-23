#ifndef USHELL_VT100_H_
#define USHELL_VT100_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * \brief Escape sequence
*/
#define USHELL_VT100_ESC_SEQ            "\x1B"

/**
 * \brief Escape sequence length
*/
#define USHELL_VT100_ESC_SEQ_LENGTH     2

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/


/**
 * \brief Enumeration of possible error codes returned by the UShell Vt100 module.
 */
typedef enum
{
    USHELL_VT100_NO_ERR = 0,            ///< Exit: no errors (success)
    USHELL_VT100_INVALID_ARGS_ERR,      ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_VT100_NOT_INIT_ERR,          ///< Exit: error - not initialized
    USHELL_VT100_PORT_ERR,              ///< Exit: error - port error (e.g. port layer error)

} UShellVt100Err_e;

/**
 * \brief Enumeration of possible font colors.
 * \note Not all terminals support all colors.
 */
typedef enum
{
    USHELL_VT100_FONT_COLOR_BLACK = 0,
    USHELL_VT100_FONT_COLOR_RED,
    USHELL_VT100_FONT_COLOR_GREEN,
    USHELL_VT100_FONT_COLOR_YELLOW,
    USHELL_VT100_FONT_COLOR_BLUE,
    USHELL_VT100_FONT_COLOR_MAGENTA,
    USHELL_VT100_FONT_COLOR_CYAN,
    USHELL_VT100_FONT_COLOR_WHITE,
    USHELL_VT100_FONT_COLOR_DEFAULT = 9

}UShellVt100FontColor_e;

/**
 * \brief Enumeration of possible background colors.
 * \note Not all terminals support all colors.
 */
typedef enum
{
    USHELL_VT100_BACKGROUND_COLOR_BLACK = 0,
    USHELL_VT100_BACKGROUND_COLOR_RED,
    USHELL_VT100_BACKGROUND_COLOR_GREEN,
    USHELL_VT100_BACKGROUND_COLOR_YELLOW,
    USHELL_VT100_BACKGROUND_COLOR_BLUE,
    USHELL_VT100_BACKGROUND_COLOR_MAGENTA,
    USHELL_VT100_BACKGROUND_COLOR_CYAN,
    USHELL_VT100_BACKGROUND_COLOR_WHITE,
    USHELL_VT100_BACKGROUND_COLOR_DEFAULT = 9

}UShellVt100BackgroundColor_e;

typedef enum
{
    USHELL_VT100_ACTION_NONE = 0,                ///< No action
    USHELL_VT100_ACTION_TERMINAL_CLEAR_LINE,     ///< Clear line (clear from cursor to end of line)
    USHELL_VT100_ACTION_TERMINAL_CLEAR_SCREEN,   ///< Clear screen (clear all)
    USHELL_VT100_ACTION_TERMINAL_CURSOR_UP,      ///< Move cursor up
    USHELL_VT100_ACTION_TERMINAL_CURSOR_DOWN,    ///< Move cursor down
    USHELL_VT100_ACTION_TERMINAL_CURSOR_LEFT,    ///< Move cursor left
    USHELL_VT100_ACTION_TERMINAL_CURSOR_RIGHT,   ///< Move cursor right
    USHELL_VT100_ACTION_TERMINAL_CURSOR_HIDE,    ///< Hide cursor
    USHELL_VT100_ACTION_TERMINAL_CURSOR_SHOW,    ///< Show cursor
    
}UShellVt100KeyAction_e;

/**
 * \brief Descriibe UShellVt100 callback function
*/
typedef struct
{
    UShellVt100Err_e (*keyUpCb)(const void* const uShellVt100);     ///< Key action callback
    UShellVt100Err_e (*keyDownCb)(const void* const uShellVt100);   ///< Key action callback
    UShellVt100Err_e (*keyLeftCb)(const void* const uShellVt100);   ///< Key action callback
    UShellVt100Err_e (*keyRightCb)(const void* const uShellVt100);  ///< Key action callback

}UShellVt100Callback_s;

/**
 * \brief Descriibe UShellVt100 worker function
*/
typedef UShellVt100Err_e (*UShellVt100PrintHook_t)(const void* const uShellVt100, const char* const str, const size_t len);

/**
 * \brief Descriibe UShellVt100 hook function
*/
typedef struct
{
    UShellVt100Err_e (*printHook)(const void* const uShellVt100, const char* const data, const size_t dataSize);    ///< Print

}UShellVt100Hook_s;


typedef struct
{
    char data[USHELL_VT100_CMD_LENGTH_MAX];     ///< Data buffer
    size_t size;        ///< Data size
}UShelVt100Io_s;

/**
 * \brief Descriibe UShellVt100 object
*/
typedef struct
{
    /* Manatory */
    const void* parent;                                     ///< Pointer to the parent object

    const UShellVt100Callback_s* cb;                        ///< Callbacks
    const UShellVt100Hook_s* hook;                          ///< Pointer to the print hook function

    /* Internal use */
    UShellVt100FontColor_e fontColorCurrent;               ///< Font color
    UShellVt100BackgroundColor_e backgroundColorCurrent;   ///< Background color



}UShellVt100_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShell)
 * \param[in] printHook - pointer to the print hook function
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Init(UShellVt100_s* const uShellVt100, const void* const parent, UShellVt100PrintHook_t hook);

/**
 * \brief Deinitialize the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object to be deinitialized
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Deinit(UShellVt100_s* const uShellVt100);

/**
 * \brief Poll the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] data - pointer to the data to be processed
 * \param[in] dataSize - size of the data
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100Poll(UShellVt100_s* const uShellVt100, const char* const data, const size_t dataSize);

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
UShellVt100Err_e UShellVt100FontColorSet(UShellVt100_s* const uShellVt100, const UShellVt100FontColor_e fontColor);

/**
 * \brief Get the font color of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] fontColor - pointer to store the font color
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100FontColorGet(UShellVt100_s* const uShellVt100, UShellVt100FontColor_e* const fontColor);

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
UShellVt100Err_e UShellVt100BackgroundColorGet(UShellVt100_s* const uShellVt100, UShellVt100BackgroundColor_e* const backgroundColor);

/**
 * \brief Attach callback to the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] cb - pointer to the callback function
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CbAttach(UShellVt100_s* const uShellVt100, const UShellVt100Callback_s* const cb);

/**
 * \brief Detach callback from the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100CbDetach(UShellVt100_s* const uShellVt100);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_OSAL_H_ */