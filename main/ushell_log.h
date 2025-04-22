#ifndef USHELL_LOG_H_
#define USHELL_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "ushell_cfg.h"
#include "ushell_vcp.h"
#include "ushell_cmd.h"
#include "ushell_socket.h"

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * @brief Register a logging module.
 *
 * This macro registers a logging module with the specified name and default log level.
 *
 * @param moduleName The name of the module to register.
 * @param defaultLogLevel The default log level for the module.
 */
#define USHELL_REGISTER_MODULE(moduleName, defaultLogLevel)                      \
    const char* moduleName##_name = #moduleName;                                 \
    static void register_module_##moduleName(void) __attribute__((constructor)); \
    static void register_module_##moduleName(void)                               \
    {                                                                            \
        UShellLogModuleRegister(moduleName##_name, defaultLogLevel);             \
    }

/**
 * @brief Logs a DEBUG-level message.
 *
 * This macro logs a message at the DEBUG level. It automatically includes the file name and line number
 * where the macro is called, along with the module name and the formatted message.
 *
 * @param module The name of the module generating the log.
 * @param format A printf-style format string.
 * @param ... Additional arguments for the format string.
 */
#ifdef DEBUG
    #define USHELL_LOG_DEBUG(module, format, ...) \
        UShellLogPrint(USHELL_LOG_LEVEL_DEBUG, module##_name, format, ##__VA_ARGS__)
#else
    #define USHELL_LOG_DEBUG(module, format, ...) \
        do                                        \
        {                                         \
        } while (0)    // Mock in release mode
#endif
/**
 * @brief Logs an INFO-level message.
 *
 * This macro logs a message at the INFO level. It automatically includes the file name and line number
 * where the macro is called, along with the module name and the formatted message.
 *
 * @param module The name of the module generating the log.
 * @param format A printf-style format string.
 * @param ... Additional arguments for the format string.
 */
#ifdef DEBUG
    #define USHELL_LOG_INFO(module, format, ...) \
        UShellLogPrint(USHELL_LOG_LEVEL_INFO, module##_name, format, ##__VA_ARGS__)
#else
    #define USHELL_LOG_DEBUG(module, format, ...) \
        do                                        \
        {                                         \
        } while (0)    // Mock in release mode
#endif

/**
 * @brief Logs a WARNING-level message.
 *
 * This macro logs a message at the WARNING level. It automatically includes the file name and line number
 * where the macro is called, along with the module name and the formatted message.
 *
 * @param module The name of the module generating the log.
 * @param format A printf-style format string.
 * @param ... Additional arguments for the format string.
 */
#ifdef DEBUG
    #define USHELL_LOG_WARNING(module, format, ...) \
        UShellLogPrint(USHELL_LOG_LEVEL_WARNING, module##_name, format, ##__VA_ARGS__)
#else
    #define USHELL_LOG_DEBUG(module, format, ...) \
        do                                        \
        {                                         \
        } while (0)    // Mock in release mode
#endif

/**
 * @brief Logs an ERROR-level message.
 *
 * This macro logs a message at the ERROR level. It automatically includes the file name and line number
 * where the macro is called, along with the module name and the formatted message.
 *
 * @param module The name of the module generating the log.
 * @param format A printf-style format string.
 * @param ... Additional arguments for the format string.
 */
#define USHELL_LOG_ERROR(module, format, ...) \
    UShellLogPrint(USHELL_LOG_LEVEL_ERROR, module##_name, format, ##__VA_ARGS__)

/**
 * @brief Name of the uShell log command.
 *
 * This macro defines the name of the command used to control the logging system in uShell.
 */
#ifndef USHELL_LOG_CMD_NAME
    #define USHELL_LOG_CMD_NAME "log"    ///< UShell command log name
#endif

/**
 * @brief Help text for the uShell log command.
 *
 * This macro defines the help text displayed for the `log` command in uShell.
 */
#ifndef USHELL_LOG_CMD_HELP
    #define USHELL_LOG_CMD_HELP "log - Cmd to control log level and disable/enable log"
#endif

/**
 * @brief ANSI escape sequence to clear the current line in the terminal.
 *
 * This macro defines the escape sequence used to clear the current line and move the cursor to the beginning.
 */
#ifndef USHELL_LOG_CLEAR_LINE
    #define USHELL_LOG_CLEAR_LINE "\033[2K\r"    ///< UShell log clear line string
#endif

/**
 * @brief Newline character for log messages.
 *
 * This macro defines the newline character used in log messages.
 */
#ifndef USHELL_LOG_NEW_LINE
    #define USHELL_LOG_NEW_LINE "\n"    ///< UShell log new line string
#endif

/**
 * @brief Maximum number of log modules supported by uShell.
 *
 * This macro defines the maximum number of log modules that can be registered in the uShell logging system.
 */
#ifndef USHELL_LOG_MODULES_MAX
    #define USHELL_LOG_MODULES_MAX 30U    ///< UShell log max modules count
#endif

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Enumeration of possible error codes
 */
typedef enum
{
    USHELL_LOG_NO_ERR = 0,          ///< Exit: no errors (success)
    USHELL_LOG_INVALID_ARGS_ERR,    ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_LOG_INIT_ERR,            ///< Exit: error - not initialized
    USHELL_LOG_VCP_ERR,             ///< Exit: error - VCP error (e.g. port layer error)
    USHELL_LOG_OSAL_ERR,            ///< Exit: error - OSAL error (e.g. port layer error)

} UShellLogErr_e;

/**
 * \brief Description of the uShell log feature
 */
typedef bool UShellLogFeature_b;

/**
 * @brief Enumeration of uShell log levels.
 *
 * This enumeration defines the different levels of logging supported by uShell.
 */
typedef enum
{
    USHELL_LOG_LEVEL_NONE,       ///< None - no logging.
    USHELL_LOG_LEVEL_DEBUG,      ///< Debug-level messages, used for detailed debugging information.
    USHELL_LOG_LEVEL_INFO,       ///< Info-level messages, used for general informational messages.
    USHELL_LOG_LEVEL_WARNING,    ///< Warning-level messages, used for non-critical issues.
    USHELL_LOG_LEVEL_ERROR,      ///< Error-level messages, used for critical errors.
} UShellLogLevel_e;

/**
 * @brief Configuration structure for uShell logging.
 *
 * This structure contains configuration options for the uShell logging system.
 */
typedef struct
{
    UShellLogLevel_e curLevel;               ///< Current log level.
    UShellLogFeature_b timePrintEnable;      ///< Flag to enable or disable timestamp printing in logs.
    UShellLogFeature_b modulePrintEnable;    ///< Flag to enable or disable module name printing in logs.
    UShellLogFeature_b levelPrintEnable;     ///< Flag to enable or disable log level printing in logs.
    UShellLogFeature_b filePrintEnable;      ///< Flag to enable or disable file name printing in logs.
} UShellLogParam_s;

/**
 * @brief Structure representing a logging module in uShell.
 *
 * This structure contains the name of the module and its associated log level.
 */
typedef struct
{
    const char* name;             ///< Имя модуля
    UShellLogLevel_e logLevel;    ///< Уровень логирования для модуля
} UShellLogModule_s;

/**
 * @brief Configuration structure for uShell VCP sessions.
 *
 * This structure contains configuration for the uShell VCP sessions.
 */
typedef struct
{
    UShellVcpSessionParam_s writeParam;
    UShellSocket_s* writeSocket;
} UShellLogSessionConfig_s;

/**
 * @brief Command structure for uShell logging.
 *
 * This structure represents a command object for the uShell logging system.
 */
typedef struct
{
    UShellCmd_s cmd;    ///< Base command object for uShell.
} UShellLogCmd_s;

/**
 * @brief Main structure for uShell logging.
 *
 * This structure contains all the dependencies and configuration for the uShell logging system.
 */
typedef struct
{
    /* Optional fields */
    const void* parent;    ///< Parent object (optional, can be NULL).

    /* Dependencies */
    UShellOsal_s* osal;      ///< Pointer to the OSAL (Operating System Abstraction Layer) object.
    UShellVcp_s* vcp;        ///< Pointer to the UShell VCP (Virtual Communication Port) object.
    UShellLogParam_s cfg;    ///< Configuration for the logging system.

    /* Internal use */
    UShellLogCmd_s cmd;                     ///< Command object for the logging system.
    UShellLogSessionConfig_s sessionCfg;    ///< Session configuration for the logging system.

} UShellLog_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief ONE and ONLY ONE instance of the uShell log object.(Singleton pattern)
 */
extern UShellLog_s uShellLog;    ///< Global instance of the uShell log object.

/**
 * @brief Initialize the uShell log object.
 *
 * This function initializes the uShell logging system with the provided dependencies and configuration.
 *
 * @param[in] parent Pointer to the parent object (optional, can be NULL).
 * @param[in] vcp Pointer to the UShell VCP (Virtual Communication Port) object.
 * @param[in] osal Pointer to the OSAL (Operating System Abstraction Layer) object.
 * @param[in] config Pointer to the configuration structure for the logging system.
 *
 * @return UShellLogErr_e Error code. Returns 0 on success, non-zero if an error occurred.
 */
UShellLogErr_e UShellLogInit(const void* const parent,
                             UShellVcp_s* const vcp,
                             UShellOsal_s* const osal,
                             UShellLogParam_s* const config);

/**
 * @brief Deinitialize the uShell log object.
 *
 * This function deinitializes the uShell logging system and releases any allocated resources.
 *
 * @return UShellLogErr_e Error code. Returns 0 on success, non-zero if an error occurred.
 */
UShellLogErr_e UShellLogDeInit();

/**
 * @brief Register a new logging module.
 *
 * This function registers a new logging module with the specified name and log level.
 *
 * @param[in] nameModule The name of the module to register.
 * @param[in] level The log level for the module.
 *
 * @return UShellLogErr_e Error code. Returns 0 on success, non-zero if an error occurred.
 */
void UShellLogModuleRegister(const char* const nameModule,
                             const UShellLogLevel_e level);

/**
 * @brief Print a log message with file and name
 *
 * This function logs a message with the specified level, module name, file name, and line number.
 * The message can include formatted text, similar to printf.
 *
 * @param[in] level The logging level (e.g., DEBUG, INFO, WARNING, etc.).
 * @param[in] nameModule The name of the module generating the log.
 * @param[in] file The name of the file where the log is generated.
 * @param[in] line The line number in the file where the log is generated.
 * @param[in] format A printf-style format string for the log message.
 * @param[in] ... Additional arguments for the format string.
 */
void UShellLogPrintWithFile(const UShellLogLevel_e level,
                            const char* const nameModule,
                            const char* const file,
                            const int line,
                            const char* const format,
                            ...);

/**
 * @brief Print a log message
 *
 * This function logs a message with the specified level, module name, file name, and line number.
 * The message can include formatted text, similar to printf.
 *
 * @param[in] level The logging level (e.g., DEBUG, INFO, WARNING, etc.).
 * @param[in] nameModule The name of the module generating the log.
 * @param[in] format A printf-style format string for the log message.
 * @param[in] ... Additional arguments for the format string.
 */
void UShellLogPrint(const UShellLogLevel_e level,
                    const char* const nameModule,
                    const char* const format,
                    ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_AUTH_H_ */