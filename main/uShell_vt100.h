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
 * \brief Description of the maximum size of the buffer in the UShell Vt100
*/
#define USHELL_BUFFER_SIZE 128


/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Descriibe size of one item in the UShell Vt100q
*/
typedef char UShellVt100Item_t;

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

/**
 * \brief Enumeration of possible actions.
 * \note Not all terminals support all actions.
*/
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

}UShellVt100Action_e;

/**
 * \brief Enumeration of possible key actions.
 * \note Not all terminals support all key actions.
*/
typedef enum
{

    USHELL_VT100_KEY_UP = 0,    ///< Key up
    USHELL_VT100_KEY_DOWN,      ///< Key down
    USHELL_VT100_KEY_LEFT,      ///< Key left
    USHELL_VT100_KEY_RIGHT,     ///< Key right
    USHELL_VT100_KEY_DEL,    ///< Key delete
    USHELL_VT100_KEY_HOME,      ///< Key home
    USHELL_VT100_KEY_END,       ///< Key end
    USHELL_VT100_KEY_INSERT,    ///< Key insert
    USHELL_VT100_KEY_PGUP,   ///< Key page up
    USHELL_VT100_KEY_PGDN, ///< Key page down

    USHELL_VT100_KEY_QTY        ///< Max key

}UShellVt100KeyCbType_e;


/**
 * \brief Descriibe UShellVt100 print hook function
 * \note This function is used to print the data
 * \note This function should be implemented in the port layer
*/
typedef UShellVt100Err_e (*UShellVt100PrintHook_t)(const void* const uShellVt100, const char* const data, const size_t dataSize);

/**
 * \brief Descriibe UShellVt100 key callback function
 * \note This function is used to handle the key actions
*/
typedef UShellVt100Err_e (*UShellVt100KeyCb_t)(const void* const uShellVt100);

/**
 * \brief Descriibe UShellVt100 object
*/
typedef struct
{
    /* Manatory */
    const void* parent;                                     ///< Pointer to the parent object
    UShellVt100PrintHook_t printHook;                      ///< Print hook function

    /* Internal use */
    UShellVt100FontColor_e fontColorCurrent;               ///< Font color
    UShellVt100BackgroundColor_e backgroundColorCurrent;   ///< Background color

    /* Optional */
    UShellVt100KeyCb_t keyCb[USHELL_VT100_KEY_QTY];        ///< Key callback functions

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
 * \brief Attach the key callback function to the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] keyType - key type
 * \param[in] keyCb - key callback function
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100KeyCbAttach(UShellVt100_s* const uShellVt100, const UShellVt100KeyCbType_e keyType, const UShellVt100KeyCb_t keyCb);

/**
 * \brief Detach the key callback function from the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] keyType - key type
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100KeyCbDetach(UShellVt100_s* const uShellVt100, const UShellVt100KeyCbType_e keyType);

/**
 * \brief Poll the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] data - pointer to the data to be processed
 * \param[in] dataSize - size of the data
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ParseKeyEscapeSequency(UShellVt100_s* const uShellVt100, const UShellVt100Item_t* const data, const size_t dataSize);

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
 * \brief Execute the action of the UShellVt100 object
 * \note This function is used to execute the action of the UShellVt100 object
 * \param[in] uShellVt100 - pointer to the UShellVt100 object
 * \param[in] action - action to be performed
 * \param[out] none
 * \return UShellVt100Err_e
*/
UShellVt100Err_e UShellVt100ActionExectute(UShellVt100_s* const uShellVt100, const UShellVt100Action_e action);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_OSAL_H_ */