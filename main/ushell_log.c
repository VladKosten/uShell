/**
 * \file         ushell_log.c
 * \brief        The file contains the implementation of the UShell log module.
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_log.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShell log module
 */
#ifndef USHELL_LOG_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_LOG_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_LOG_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief UShell log init the run time environment
 * \param[in] vcp - UShell VCP object
 * \param[in] osal - UShell OSAL object
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static UShellLogErr_e uShellLogRtEnvInit(UShellVcp_s* const vcp,
                                         UShellOsal_s* const osal);

/**
 * \brief UShell log deinit the run time environment
 * \param[in] none
 * \param[out] none
 * \return none
 */
static void uShellLogRtEnvDeinit();

/**
 * \brief UShell log init the run time environment for VCP
 * \param[in] vcp - UShell VCP object
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static UShellLogErr_e uShellLogRtEnvVcpInit(UShellVcp_s* const vcp);

/**
 * \brief UShell log deinit the run time environment for VCP
 * \param[in] none
 * \return none
 */
static void uShellLogRtEnvVcpDeinit();

/**
 * \brief UShell log init the run time environment for OSAL
 * \param[in] osal - UShell OSAL object
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static UShellLogErr_e uShellLogRtEnvOsalInit(UShellOsal_s* const osal);

/**
 * \brief UShell log deinit the run time environment for OSAL
 * \param[in] none
 * \return none
 */
static void uShellLogRtEnvOsalDeinit();

/**
 * \brief UShell log lock the run time environment
 * \param[in] none
 * \return none
 */
static void uShellLogLock(void);

/**
 * \brief UShell log unlock the run time environment
 * \param[in] none
 * \return none
 */
static void uShellLogUnlock(void);

/**
 * \brief Clear the current line in the log output
 * \param[in] none
 * \return none
 */
static void uShellLogClearLine(void);

/**
 * \brief Print a new line in the log output
 * \param[in] none
 * \return none
 */
static void uShellLogNewLine(void);

/**
 * \brief Print the current time in the log output
 * \param[in] none
 * \return none
 */
static void uShellLogPrintTime(void);

/**
 * \brief Print the current log level in the log output
 * \param[in] level - UShellLogLevel_e - log level
 * \return none
 */
static void uShellLogPrintLevel(const UShellLogLevel_e level);

/**
 * \brief Print the current module name in the log output
 * \param[in] nameModule - const char* - module name
 * \return none
 */
static void uShellLogPrintModule(const char* const nameModule);

/**
 * \brief Print the current file name in the log output
 * \param[in] file - const char* - file name
 * \return none
 */
static void uShellLogPrintFileLine(const char* const file,
                                   const int line);

/**
 * \brief Execute the log command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellLogCmdExec(void* const cmd,
                                       UShellSocket_s* const readSocket,
                                       UShellSocket_s* const writeSocket,
                                       const int argc,
                                       char* const argv []);

/**
 * \brief UShell log object (Singleton pattern)
 */
UShellLog_s uShellLog = {0};    ///< UShell log object (base object)

static UShellLogModule_s uShellLogModule [USHELL_LOG_MODULES_MAX];    ///< Array of log modules.
static size_t uShellLogModuleCount;                                   ///< Count of registered log modules.

static const char* UShellLogLevelStrings [] = {
    "[NONE] ",       // USHELL_LOG_LEVEL_DEBUG
    "[DEBUG] ",      // USHELL_LOG_LEVEL_INFO
    "[INFO] ",       // USHELL_LOG_LEVEL_WARNING
    "[WARNING] ",    // USHELL_LOG_LEVEL_ERROR
    "[ERROR]",       // USHELL_LOG_LEVEL_CRITICAL
};

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

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
                             UShellLogParam_s* const config)
{
    /* Local variable */
    UShellLogErr_e status = USHELL_LOG_NO_ERR;       // Variable to store the status of the operation
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (osal == NULL) ||
            (config == NULL))
        {
            status = USHELL_LOG_INVALID_ARGS_ERR;
            break;
        }

        /* Check config */
        if (config->curLevel != USHELL_LOG_LEVEL_DEBUG &&
            config->curLevel != USHELL_LOG_LEVEL_INFO &&
            config->curLevel != USHELL_LOG_LEVEL_WARNING &&
            config->curLevel != USHELL_LOG_LEVEL_ERROR)
        {
            status = USHELL_LOG_INVALID_ARGS_ERR;
            break;
        }

        /* Flush the log */
        memset(&uShellLog, 0, sizeof(UShellLog_s));    // Clear the UShell log object

        /* Init the run time env */
        status = uShellLogRtEnvInit(vcp, osal);    // Initialize the run time environment
        if (status != USHELL_LOG_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Set status to error if run time environment initialization fails
            break;
        }

        /* Init the base class */
        cmdStatus = UShellCmdInit((UShellCmd_s*) &uShellLog.cmd,
                                  USHELL_LOG_CMD_NAME,
                                  USHELL_LOG_CMD_HELP,
                                  uShellLogCmdExec);    // Initialize the command object
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Set status to error if command initialization fails
            status = USHELL_LOG_INIT_ERR;
            break;
        }

        /* Set the parent object */
        uShellLog.parent = parent;    // Set the parent object

        /* Set the configuration */
        uShellLog.cfg = *config;    // Set the configuration

    } while (0);

    return status;
}

/**
 * @brief Deinitialize the uShell log object.
 *
 * This function deinitializes the uShell logging system and releases any allocated resources.
 *
 * @return UShellLogErr_e Error code. Returns 0 on success, non-zero if an error occurred.
 */
UShellLogErr_e UShellLogDeInit()
{
    /* Local variable */
    UShellLogErr_e status = USHELL_LOG_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Deinit the command object */
        UShellCmdDeinit((UShellCmd_s*) &uShellLog.cmd);    // Deinitialize the command object

        /* Deinit the run time env */
        uShellLogRtEnvDeinit();    // Deinitialize the run time environment

        /* Clear the object */
        memset(&uShellLog, 0, sizeof(UShellLog_s));    // Clear the UShell log object

    } while (0);

    return status;
}

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
                             const UShellLogLevel_e level)
{
    /* Local variable */
    UShellLogModule_s* module = NULL;    // Pointer to the log module
    bool isExist = false;

    do
    {
        /* Check input parameters */
        if ((nameModule == NULL) ||
            (uShellLogModuleCount >= USHELL_LOG_MODULES_MAX))
        {
            USHELL_LOG_ASSERT(0);    // Assert that the module name is not NULL and the module count does not exceed the maximum
            break;                   // Exit if input parameters are invalid or module count exceeds maximum
        }

        /* Check level */
        if ((level != USHELL_LOG_LEVEL_DEBUG) &&
            (level != USHELL_LOG_LEVEL_INFO) &&
            (level != USHELL_LOG_LEVEL_WARNING) &&
            (level != USHELL_LOG_LEVEL_ERROR))
        {
            USHELL_LOG_ASSERT(0);    // Assert that the level is valid
            break;                   // Exit if level is invalid
        }

        /* Check duplicate */
        for (size_t i = 0; i < uShellLogModuleCount; i++)
        {
            if (uShellLogModule [i].name == nameModule)
            {
                isExist = true;
                break;    // Exit if module name is duplicated
            }
        }
        if (isExist == true)
        {
            break;    // Exit if module name is duplicated
        }

        /* Set the module name and level */
        module = &uShellLogModule [uShellLogModuleCount++];    // Get the current module
        module->name = nameModule;                             // Set the module name
        module->logLevel = level;                              // Set the module level

    } while (0);
}

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
                            ...)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;    // Variable to store the status of the operation
    UShellLogModule_s* module = NULL;                   // Pointer to the log module
    va_list args;
    va_start(args, format);

    /* lock */
    uShellLogLock();    // Lock the log object

    do
    {
        /* Check input parameters */
        if ((format == NULL) ||
            (uShellLog.sessionCfg.writeSocket == NULL))
        {
            /* Input parameters are invalid */
            break;
        }

        /* Find module in list */
        for (size_t i = 0; i < uShellLogModuleCount; i++)
        {
            if (uShellLogModule [i].name == nameModule)
            {
                module = &uShellLogModule [i];    // Get the current module
                break;                            // Exit if module name is found
            }
        }

        /* Check the level */
        if ((level < uShellLog.cfg.curLevel) ||
            (module == NULL) ||
            (module->logLevel < uShellLog.cfg.curLevel))
        {
            /* Level is lower than the current level or module is not found */
            break;
        }

        /* Clear the current line */
        uShellLogClearLine();    // Clear the current line in the log output

        /* Print the current time */
        uShellLogPrintTime();    // Print the current time in the log output

        /* Print the current log level */
        uShellLogPrintLevel(level);    // Print the current log level in the log output

        /* Print the current module name */
        uShellLogPrintModule(nameModule);    // Print the current module name in the log output

        /* Print the current file name and line number */
        uShellLogPrintFileLine(file, line);    // Print the current file name and line number in the log output

        /* Print the formatted message */
        status = UShellSocketPrintVaList(uShellLog.sessionCfg.writeSocket,
                                         format,
                                         args);    // Print the formatted message to the log output
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Print error message */
            USHELL_LOG_ASSERT(0);    // Set status to error if printing fails
        }

        /* Print a new line */
        uShellLogNewLine();    // Print a new line in the log output

    } while (0);

    /* unlock */
    uShellLogUnlock();    // Unlock the log object
}

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
                    ...)
{
    /* Local variable */
    UShellSocketErr_e status = USHELL_SOCKET_NO_ERR;    // Variable to store the status of the operation
    UShellLogModule_s* module = NULL;                   // Pointer to the log module
    va_list args;
    va_start(args, format);

    /* lock */
    uShellLogLock();    // Lock the log object

    do
    {
        /* Check input parameters */
        if ((format == NULL) ||
            (uShellLog.sessionCfg.writeSocket == NULL))
        {
            /* Input parameters are invalid */
            break;
        }

        /* Find module in list */
        for (size_t i = 0; i < uShellLogModuleCount; i++)
        {
            if (uShellLogModule [i].name == nameModule)
            {
                module = &uShellLogModule [i];    // Get the current module
                break;                            // Exit if module name is found
            }
        }

        /* Check the level */
        if ((level < uShellLog.cfg.curLevel) ||
            (module == NULL) ||
            (module->logLevel < uShellLog.cfg.curLevel))
        {
            /* Level is lower than the current level or module is not found */
            break;
        }

        /* Clear the current line */
        uShellLogClearLine();    // Clear the current line in the log output

        /* Print the current time */
        uShellLogPrintTime();    // Print the current time in the log output

        /* Print the current log level */
        uShellLogPrintLevel(level);    // Print the current log level in the log output

        /* Print the current module name */
        uShellLogPrintModule(nameModule);    // Print the current module name in the log output

        /* Print the formatted message */
        status = UShellSocketPrintVaList(uShellLog.sessionCfg.writeSocket,
                                         format,
                                         args);    // Print the formatted message to the log output
        if (status != USHELL_SOCKET_NO_ERR)
        {
            /* Print error message */
            USHELL_LOG_ASSERT(0);    // Set status to error if printing fails
        }

        /* Print a new line */
        uShellLogNewLine();    // Print a new line in the log output

    } while (0);

    /* unlock */
    uShellLogUnlock();    // Unlock the log object
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief UShell log init the run time environment
 * \param[in] vcp - UShell VCP object
 * \param[in] osal - UShell OSAL object
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static UShellLogErr_e uShellLogRtEnvInit(UShellVcp_s* const vcp,
                                         UShellOsal_s* const osal)
{
    /* Local variable */
    UShellLogErr_e status = USHELL_LOG_NO_ERR;    // Variable to store the status of the operation

    do
    {
        /* Check input parameter */
        if ((vcp == NULL) ||
            (osal == NULL))
        {
            /* Input parameters are invalid */
            status = USHELL_LOG_INVALID_ARGS_ERR;
            break;
        }

        /* Init the run time env for OSAL */
        status = uShellLogRtEnvOsalInit(osal);    // Initialize the run time environment for OSAL
        if (status != USHELL_LOG_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Set status to error if run time environment initialization fails
            status = USHELL_LOG_OSAL_ERR;
        }

        /* Init the run time env for VCP */
        status = uShellLogRtEnvVcpInit(vcp);    // Initialize the run time environment for VCP
        if (status != USHELL_LOG_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Set status to error if run time environment initialization fails
            status = USHELL_LOG_VCP_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief UShell log deinit the run time environment
 * \param[in] none
 * \param[out] none
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static void uShellLogRtEnvDeinit()
{
    /* Deinit the run time env for VCP */
    uShellLogRtEnvVcpDeinit();    // Deinitialize the run time environment for VCP

    /* Deinit the run time env for OSAL */
    uShellLogRtEnvOsalDeinit();    // Deinitialize the run time environment for OSAL
}

/**
 * \brief UShell log init the run time environment for VCP
 * \param[in] vcp - UShell VCP object
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static UShellLogErr_e uShellLogRtEnvVcpInit(UShellVcp_s* const vcp)
{
    /* Local variables */
    UShellLogErr_e status = USHELL_LOG_NO_ERR;    // Variable to store the status of the operation
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;
    UShellSocket_s* writeSocket = NULL;

    /* Initialize the runtime environment vcp */
    do
    {
        /* Check input parameters */
        if (((vcp == NULL)))
        {
            /* Invalid arguments */
            USHELL_ASSERT(0);
            status = USHELL_LOG_INVALID_ARGS_ERR;
            break;
        }

        /* Init config for write */
        uShellLog.sessionCfg.writeParam.owner = &uShellLog;             // Set the owner of the session
        uShellLog.sessionCfg.writeParam.type = USHELL_VCP_DIR_WRITE;    // Set the type of the session

        /* Open session for write */
        vcpStatus = UShellVcpSessionOpen(vcp,
                                         uShellLog.sessionCfg.writeParam,
                                         &writeSocket);
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            /* Open session error */
            USHELL_ASSERT(0);
            status = USHELL_LOG_VCP_ERR;
            break;
        }

        /* Save the write socket */
        uShellLog.sessionCfg.writeSocket = writeSocket;    // Save the write socket

        /* Attach the vcp object */
        uShellLog.vcp = vcp;    // Attach the VCP object

    } while (0);

    return status;
}

/**
 * \brief UShell log deinit the run time environment for VCP
 * \param[in] none
 * \return none
 */
static void uShellLogRtEnvVcpDeinit()
{
    do
    {
        if ((uShellLog.vcp == NULL) ||
            (uShellLog.sessionCfg.writeSocket == NULL) ||
            (uShellLog.sessionCfg.writeParam.owner != &uShellLog))
        {
            break;
        }

        /* Close */
        UShellVcpErr_e vcpStatus = UShellVcpSessionClose(uShellLog.vcp,
                                                         uShellLog.sessionCfg.writeParam);
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            /* Close session error */
            USHELL_LOG_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief UShell log init the run time environment for OSAL
 * \param[in] osal - UShell OSAL object
 * \return none
 */
static UShellLogErr_e uShellLogRtEnvOsalInit(UShellOsal_s* const osal)
{
    /* Local variable */
    UShellLogErr_e status = USHELL_LOG_NO_ERR;    // Variable to store the status of the operation
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsalLockObjHandle_t lockObj = NULL;    // Lock object for the OSAL

    /* Initialize the runtime environment osal */
    do
    {
        /* Check input parameters */
        if (osal == NULL)
        {
            /* Invalid arguments */
            USHELL_ASSERT(0);
            status = USHELL_LOG_INVALID_ARGS_ERR;
            break;
        }

        /* Attach osal object */
        uShellLog.osal = osal;    // Attach the OSAL object

        /* Create mutex for log */
        osalStatus = UShellOsalLockObjCreate(osal, &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Mutex is invalid */
            USHELL_ASSERT(0);
            status = USHELL_LOG_OSAL_ERR;
            break;
        }

    } while (0);

    return status;
}

/**
 * \brief UShell log deinit the run time environment for OSAL
 * \param[in] none
 * \return UShellLogErr_e - error code. non-zero = an error has occurred;
 */
static void uShellLogRtEnvOsalDeinit()
{
    /* Local variable */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;
    UShellOsalLockObjHandle_t lockObj = NULL;    // Lock object for the OSAL

    /* Deinitialize the runtime environment osal */
    do
    {
        /* Check input parameters */
        if (uShellLog.osal == NULL)
        {
            break;
        }

        /* Get the lock object */
        osalStatus = UShellOsalLockObjHandleGet(uShellLog.osal,
                                                0U,
                                                &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Lock object is invalid */
            USHELL_ASSERT(0);
            break;
        }

        /* Destroy mutex for log */
        osalStatus = UShellOsalLockObjDelete(uShellLog.osal, lockObj);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Mutex is invalid */
            USHELL_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief UShell log lock the run time environment
 * \param[in] none
 * \return none
 */
static void uShellLogLock(void)
{
    /* Local variables */
    UShellOsalLockObjHandle_t lockObj = NULL;           // Lock object for the OSAL
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;    // OSAL status

    /* Lock the run time environment osal */
    do
    {
        /* Check input parameters */
        if (uShellLog.osal == NULL)
        {
            break;
        }

        /* Get the lock object */
        osalStatus = UShellOsalLockObjHandleGet(uShellLog.osal,
                                                0U,
                                                &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Lock object is invalid */
            USHELL_ASSERT(0);
            break;
        }

        /* Lock */
        osalStatus = UShellOsalLock(uShellLog.osal, lockObj);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Lock failed */
            USHELL_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief UShell log unlock the run time environment
 * \param[in] none
 * \return none
 */
static void uShellLogUnlock(void)
{
    /* Local variables */
    UShellOsalLockObjHandle_t lockObj = NULL;           // Lock object for the OSAL
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;    // OSAL status

    /* Unlock the run time environment osal */
    do
    {
        /* Check input parameters */
        if (uShellLog.osal == NULL)
        {
            break;
        }

        /* Get the lock object */
        osalStatus = UShellOsalLockObjHandleGet(uShellLog.osal,
                                                0U,
                                                &lockObj);
        if ((osalStatus != USHELL_OSAL_NO_ERR) ||
            (lockObj == NULL))
        {
            /* Lock object is invalid */
            USHELL_ASSERT(0);
            break;
        }

        /* Unlock */
        osalStatus = UShellOsalUnlock(uShellLog.osal, lockObj);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            /* Unlock failed */
            USHELL_ASSERT(0);
            break;
        }

    } while (0);
}

/**
 * \brief Clear the current line in the log output
 * \param[in] none
 * \return none
 */
static void uShellLogClearLine(void)
{
    USHELL_LOG_ASSERT(uShellLog.sessionCfg.writeSocket != NULL);    // Assert that the write socket is not NULL
    UShellSocketErr_e socketStatus = UShellSocketWriteBlocking(uShellLog.sessionCfg.writeSocket,
                                                               USHELL_LOG_CLEAR_LINE,
                                                               strlen(USHELL_LOG_CLEAR_LINE));    // Write the clear line command to the socket
    (void) socketStatus;                                                                          // Unused variable
    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);                                      // Assert that the socket status is no error
}

/**
 * \brief Print a new line in the log output
 * \param[in] none
 * \return none
 */
static void uShellLogNewLine(void)
{
    USHELL_LOG_ASSERT(uShellLog.sessionCfg.writeSocket != NULL);    // Assert that the write socket is not NULL
    UShellSocketErr_e socketStatus = UShellSocketWriteBlocking(uShellLog.sessionCfg.writeSocket,
                                                               USHELL_LOG_NEW_LINE,
                                                               strlen(USHELL_LOG_NEW_LINE));    // Write the new line command to the socket
    (void) socketStatus;                                                                        // Unused variable
    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);                                    // Assert that the socket status is no error
}

/**
 * \brief Print the current time in the log output
 * \param[in] none
 * \return none
 */
static void uShellLogPrintTime(void)
{
    /* Local variable */
    UShellOsalErr_e osalStatus = USHELL_OSAL_NO_ERR;          // OSAL status
    UShellOsalTimeMs_t timeMs = 0U;                           // Time in milliseconds
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Socket status

    do
    {
        /* Check input parameters */
        if ((uShellLog.osal == NULL) ||
            (uShellLog.sessionCfg.writeSocket == NULL))
        {
            USHELL_LOG_ASSERT(0);    // Assert that the OSAL and write socket are not NULL
            break;
        }

        /* Check config */
        if (uShellLog.cfg.timePrintEnable == false)
        {
            break;    // Exit if time is not enabled in the config
        }

        /* Get the current time */
        osalStatus = UShellOsalTimeMsGet(uShellLog.osal, &timeMs);
        if (osalStatus != USHELL_OSAL_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Assert that the OSAL status is no error
            break;
        }

        socketStatus = UShellSocketPrint(uShellLog.sessionCfg.writeSocket,
                                         "[%u mS] ",
                                         timeMs);    // Print the current time to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Assert that the socket status is no error
            break;
        }

    } while (0);
}

/**
 * \brief Print the current log level in the log output
 * \param[in] level - UShellLogLevel_e - log level
 * \return none
 */
static void uShellLogPrintLevel(const UShellLogLevel_e level)
{
    /* Local variable */
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Socket status

    do
    {
        /* Check input parameters */
        if (uShellLog.sessionCfg.writeSocket == NULL)
        {
            break;    // Exit if write socket is NULL
        }

        /* Check config */
        if (uShellLog.cfg.levelPrintEnable == false)
        {
            break;    // Exit if level is not enabled in the config
        }

        /* Check level */
        if (level > USHELL_LOG_LEVEL_ERROR)
        {
            break;    // Exit if level is invalid
        }

        /* Print the log level */
        socketStatus = UShellSocketWriteBlocking(uShellLog.sessionCfg.writeSocket,
                                                 UShellLogLevelStrings [level],
                                                 strlen(UShellLogLevelStrings [level]));    // Write the log level to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Assert that the socket status is no error
            break;
        }

    } while (0);
}

/**
 * \brief Print the current module name in the log output
 * \param[in] nameModule - const char* - module name
 * \return none
 */
static void uShellLogPrintModule(const char* const nameModule)
{
    /* Local variable */
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Socket status

    do
    {
        /* Check input parameters */
        if ((uShellLog.sessionCfg.writeSocket == NULL) ||
            (nameModule == NULL))
        {
            USHELL_LOG_ASSERT(0);    // Assert that the write socket and module name are not NULL
            break;
        }

        /* Check config */
        if (uShellLog.cfg.modulePrintEnable == false)
        {
            break;    // Exit if module name is not enabled in the config
        }

        /* Print the module name */
        socketStatus = UShellSocketPrint(uShellLog.sessionCfg.writeSocket,
                                         "[%s] ",
                                         nameModule);    // Write the module name to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Assert that the socket status is no error
            break;
        }

    } while (0);
}

/**
 * \brief Print the current file name in the log output
 * \param[in] file - const char* - file name
 * \return none
 */
static void uShellLogPrintFileLine(const char* const file,
                                   const int line)
{
    /* Local variable */
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Socket status

    do
    {
        /* Check input parameters */
        if ((uShellLog.sessionCfg.writeSocket == NULL) ||
            (file == NULL) ||
            (line <= 0))
        {
            USHELL_LOG_ASSERT(0);    // Assert that the write socket and file name are not NULL
            break;
        }

        /* Check config */
        if (uShellLog.cfg.filePrintEnable == false)
        {
            break;    // Exit if file name is not enabled in the config
        }

        /* Print the file name and line number */
        socketStatus = UShellSocketPrint(uShellLog.sessionCfg.writeSocket,
                                         "[%s:%d] ",
                                         file,
                                         line);    // Write the file name and line number to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            USHELL_LOG_ASSERT(0);    // Assert that the socket status is no error
            break;
        }

    } while (0);
}

/**
 * \brief Execute the log command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellLogCmdExec(void* const cmd,
                                       UShellSocket_s* const readSocket,
                                       UShellSocket_s* const writeSocket,
                                       const int argc,
                                       char* const argv [])
{
    /* Local variables */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Command execution status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Socket operation status
    (void) socketStatus;                                      // Suppress unused variable warning

    do
    {
        /* Validate input parameters */
        if ((cmd == NULL) || (writeSocket == NULL) || (cmd != &uShellLog.cmd))
        {
            status = USHELL_CMD_INVALID_ARGS_ERR;    // Invalid arguments
            break;                                   // Exit the loop
        }

        /* Ensure at least one argument is provided */
        if (argc < 1)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Error: No command provided. Use 'lvl' or 'module' commands.\r\n");
            USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            break;
        }

        /* Handle "lvl" command */
        if (strcmp(argv [0], "lvl") == 0)
        {
            if (argc < 2)
            {
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Error: Missing subcommand for 'lvl'. Use 'set' or 'get'.\r\n");
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                break;
            }

            if (strcmp(argv [1], "set") == 0)
            {
                if (argc < 3)
                {
                    socketStatus = UShellSocketPrint(writeSocket,
                                                     "Error: Missing log level for 'lvl set'. Use 'debug', 'info', 'warning', or 'error'.\r\n");
                    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                    break;
                }

                /* Set the global log level */
                if (strcmp(argv [2], "debug") == 0)
                {
                    uShellLog.cfg.curLevel = USHELL_LOG_LEVEL_DEBUG;
                }
                else if (strcmp(argv [2], "info") == 0)
                {
                    uShellLog.cfg.curLevel = USHELL_LOG_LEVEL_INFO;
                }
                else if (strcmp(argv [2], "warning") == 0)
                {
                    uShellLog.cfg.curLevel = USHELL_LOG_LEVEL_WARNING;
                }
                else if (strcmp(argv [2], "error") == 0)
                {
                    uShellLog.cfg.curLevel = USHELL_LOG_LEVEL_ERROR;
                }
                else
                {
                    socketStatus = UShellSocketPrint(writeSocket,
                                                     "Error: Unsupported log level '%s'. Use 'debug', 'info', 'warning', or 'error'.\r\n",
                                                     argv [2]);
                    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                    break;
                }

                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Global log level set to '%s'.\r\n",
                                                 argv [2]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            }
            else if (strcmp(argv [1], "get") == 0)
            {
                /* Get the current global log level */
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Current global log level: %s\r\n",
                                                 UShellLogLevelStrings [uShellLog.cfg.curLevel]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            }
            else
            {
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Error: Unknown subcommand '%s' for 'lvl'. Use 'set' or 'get'.\r\n",
                                                 argv [1]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            }
        }
        /* Handle "module" command */
        else if (strcmp(argv [0], "module") == 0)
        {
            if (argc < 2)
            {
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Error: Missing module name. Use 'module <name> set|get <level>'.\r\n");
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                break;
            }

            /* Search for the specified module */
            UShellLogModule_s* logModule = NULL;
            for (size_t i = 0; i < uShellLogModuleCount; i++)
            {
                if (strcmp(argv [1], uShellLogModule [i].name) == 0)
                {
                    logModule = &uShellLogModule [i];
                    break;
                }
            }

            if (logModule == NULL)
            {
                /* Module not found, list available modules */
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Error: Module '%s' not found. Available modules:\r\n",
                                                 argv [1]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);

                for (size_t i = 0; i < uShellLogModuleCount; i++)
                {
                    socketStatus = UShellSocketPrint(writeSocket,
                                                     " - %s\r\n",
                                                     uShellLogModule [i].name);
                    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                }
                break;
            }

            if (argc >= 3 && strcmp(argv [2], "set") == 0)
            {
                if (argc < 4)
                {
                    socketStatus = UShellSocketPrint(writeSocket,
                                                     "Error: Missing log level for 'module %s set'. Use 'debug', 'info', 'warning', or 'error'.\r\n",
                                                     argv [1]);
                    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                    break;
                }

                /* Set the log level for the module */
                if (strcmp(argv [3], "debug") == 0)
                {
                    logModule->logLevel = USHELL_LOG_LEVEL_DEBUG;
                }
                else if (strcmp(argv [3], "info") == 0)
                {
                    logModule->logLevel = USHELL_LOG_LEVEL_INFO;
                }
                else if (strcmp(argv [3], "warning") == 0)
                {
                    logModule->logLevel = USHELL_LOG_LEVEL_WARNING;
                }
                else if (strcmp(argv [3], "error") == 0)
                {
                    logModule->logLevel = USHELL_LOG_LEVEL_ERROR;
                }
                else
                {
                    socketStatus = UShellSocketPrint(writeSocket,
                                                     "Error: Unsupported log level '%s'. Use 'debug', 'info', 'warning', or 'error'.\r\n",
                                                     argv [3]);
                    USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
                    break;
                }

                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Log level for module '%s' set to '%s'.\r\n",
                                                 logModule->name,
                                                 argv [3]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            }
            else if (argc >= 3 && strcmp(argv [2], "get") == 0)
            {
                /* Get the log level for the module */
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Module '%s': Current log level is '%s'.\r\n",
                                                 logModule->name,
                                                 UShellLogLevelStrings [logModule->logLevel]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            }
            else
            {
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "Error: Unknown subcommand '%s' for 'module'. Use 'set' or 'get'.\r\n",
                                                 argv [2]);
                USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
            }
        }
        else
        {
            /* Unknown command */
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Error: Unknown command '%s'. Available commands:\r\n",
                                             argv [0]);
            USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);

            socketStatus = UShellSocketPrint(writeSocket,
                                             " - lvl: Set or get the global log level.\r\n");
            USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);

            socketStatus = UShellSocketPrint(writeSocket,
                                             " - module: Set or get the log level for a specific module.\r\n");
            USHELL_LOG_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);
        }

    } while (0);

    return status;    // Return the status of the command
}