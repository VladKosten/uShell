/**
 * \file         ushell_cmd_fs.c
 * \brief        The file contains the implementation of the UShell command for file system operations.
 * \authors      Vladislav Kosten (vladkosten@gmail.com)
 * \copyright    MIT License (c) 2025
 * \warning      A warning may be placed here...
 * \bug          Bug report may be placed here...
 */
//===============================================================================[ INCLUDE ]========================================================================================

#include "ushell_cmd_fs.h"
//=====================================================================[ INTERNAL MACRO DEFINITIONS ]===============================================================================

/**
 * \brief Assert macro for the UShellHal module.
 */
#ifndef USHELL_CMD_FS_ASSERT
    #ifdef USHELL_ASSERT
        #define USHELL_CMD_FS_ASSERT(cond) USHELL_ASSERT(cond)
    #else
        #define USHELL_CMD_FS_ASSERT(cond)
    #endif
#endif

//====================================================================[ INTERNAL DATA TYPES DEFINITIONS ]===========================================================================

//===============================================================[ INTERNAL FUNCTIONS AND OBJECTS DECLARATION ]=====================================================================

/**
 * \brief Execute the cd command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsCdExec(void* const cmd,
                                        const int argc,
                                        char* const argv []);

/**
 * \brief Execute the ls command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsLsExec(void* const cmd,
                                        const int argc,
                                        char* const argv []);

/**
 * \brief Execute the rm command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsRmExec(void* const cmd,
                                        const int argc,
                                        char* const argv []);

/**
 * \brief Execute the mkdir command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsMkdirExec(void* const cmd,
                                           const int argc,
                                           char* const argv []);

/**
 * \brief Execute the cat command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsCatExec(void* const cmd,
                                         const int argc,
                                         char* const argv []);

/**
 * \brief Execute the write command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsWriteExec(void* const cmd,
                                           const int argc,
                                           char* const argv []);

/**
 * \brief Delay function for the xmodem server.
 * \param xmodem - xmodem server object
 * \param ms - delay in milliseconds
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemDelay(void* const xmodem,
                                                const int ms);

/**
 * @brief TxByte function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] byte - byte to transmit
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemTxByte(void* const xmodem,
                                                 const uint8_t byte,
                                                 void* const parent);

/**
 * @brief RxByte function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] byte - byte to receive
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemRxByte(void* const xmodem,
                                                 uint8_t* byte,
                                                 void* const parent);

/**
 * @brief IsRxByte function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] isRx - byte to check
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemIsRxByte(void* const xmodem,
                                                   bool* isRx,
                                                   void* const parent);

/**
 * @brief Write function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] data - data to write
 * @param[in] size - size of the data to write
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e
uShellCmdFsXModemWrite(void* const xmodem,
                       uint8_t* const data,
                       const int size);

/**
 * \brief UShellCmdFsCd object (base object)
 */
UShellCmdFs_s uShellCmdFs = {0};

static XModemServerPort_s uShellCmdFsXModemPort =
    {
        .delayMs = uShellCmdFsXModemDelay,
        .transmitByte = uShellCmdFsXModemTxByte,
        .receiveByte = uShellCmdFsXModemRxByte,
        .writeToMemory = uShellCmdFsXModemWrite,
        .isReceivedByte = uShellCmdFsXModemIsRxByte,
};

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the commands for file system operations.
 * \param [in] lfs - pointer to the lfs object
 * \param [in] vcp - pointer to the UShellVcp object
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdFsInit(lfs_t* lfs,
                    UShellVcp_s* vcp)
{
    /* Local variable */
    int status = 0;                                  // Variable to store command status
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input parameter */
        if ((lfs == NULL) ||
            (vcp == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if lfs or vcp is NULL
            status = -1;                // Set status to error
            break;                      // Exit the loop
        }

        /* Flush  */
        memset(&uShellCmdFs, 0, sizeof(UShellCmdFs_s));    // Clear the command buffer

        /* Init ls command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdLs.cmd,
                                  USHELL_CMD_FS_LS_NAME,
                                  USHELL_CMD_FS_LS_HELP,
                                  uShellCmdFsLsExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {

            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -2;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init cd command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdCd.cmd,
                                  USHELL_CMD_FS_CD_NAME,
                                  USHELL_CMD_FS_CD_HELP,
                                  uShellCmdFsCdExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -3;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init rm command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdRm.cmd,
                                  USHELL_CMD_FS_RM_NAME,
                                  USHELL_CMD_FS_RM_HELP,
                                  uShellCmdFsRmExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -4;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init mkdir command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdMkdir.cmd,
                                  USHELL_CMD_FS_MKDIR_NAME,
                                  USHELL_CMD_FS_MKDIR_HELP,
                                  uShellCmdFsMkdirExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -5;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init cat command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdCat.cmd,
                                  USHELL_CMD_FS_CAT_NAME,
                                  USHELL_CMD_FS_CAT_HELP,
                                  uShellCmdFsCatExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -6;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init rm command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdRm.cmd,
                                  USHELL_CMD_FS_RM_NAME,
                                  USHELL_CMD_FS_RM_HELP,
                                  uShellCmdFsRmExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -7;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init write command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdWrite.cmd,
                                  USHELL_CMD_FS_WRITE_NAME,
                                  USHELL_CMD_FS_WRITE_HELP,
                                  uShellCmdFsWriteExec);

        /* Init xmodem command */
        XModemServerErr_e xmodemStatus = XModemServerInit(&uShellCmdFs.xModemServer,
                                                          &uShellCmdFsXModemPort,
                                                          &uShellCmdFs);
        if (xmodemStatus != XMODEM_SERVER_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -8;                // Set status to error
            break;                      // Exit the loop
        }

        /* Save the lfs object to the command object */
        uShellCmdFs.lfs = lfs;
        /* Save the vcp object to the command object */
        uShellCmdFs.vcp = vcp;

        /* Set the start path */
        strncpy(uShellCmdFs.path, USHELL_CMD_FS_START_PATH, sizeof(uShellCmdFs.path) - 1);    // Copy the start path to the command object

    } while (0);

    /* Return status */
    return status;    // Return success code
}

/**
 * \brief Deinitialize the UShell cmd
 * \param [in] cmd - UShellCmd obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdFsDeinit()
{
    /* Local variable */
    int status = 0;                                  // Variable to store command status
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Deinit ls command */
        cmdStatus = UShellCmdDeinit(&uShellCmdFs.cmdLs.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command deinitialization fails
        }

        /* Deinit cd command */
        cmdStatus = UShellCmdDeinit(&uShellCmdFs.cmdCd.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command deinitialization fails
        }

        /* Deinit rm command */
        cmdStatus = UShellCmdDeinit(&uShellCmdFs.cmdRm.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command deinitialization fails
        }

        /* Deinit mkdir command */
        cmdStatus = UShellCmdDeinit(&uShellCmdFs.cmdMkdir.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command deinitialization fails
        }

    } while (0);

    /* Return status */
    return status;    // Return success code
}

//============================================================================ [PRIVATE FUNCTIONS ]=================================================================================

/**
 * \brief Execute the cd command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsCdExec(void* const cmd,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;    // Variable to store command status
    const char* newDir = argv [0];
    char newPath [USHELL_CMD_FS_MAX_PATH] = {0};    // Buffer for the new path
    struct lfs_info info = {0};                     // Buffer for the file system info
    int statusFs = 0;                               // Variable to store file system status

    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            printf("Usage: cd <directory>\n");
            break;
        }

        /* Check if newDir is ".." */
        if (strcmp(newDir, "..") == 0)
        {
            /* Make sure we are not at root directory */
            if (strcmp(uShellCmdFs.path, "/") == 0)
            {
                printf("cd: Already at root directory.\n");
                break;
            }

            /* Remove the last directory from the path */
            char* lastSlash = strrchr(uShellCmdFs.path, '/');
            if (lastSlash != NULL)
            {
                *lastSlash = '\0';    // Remove the last slash
                if (strlen(uShellCmdFs.path) == 0)
                {
                    strcpy(uShellCmdFs.path, "/");    // Set to root if empty
                }
            }
            else
            {
                strcpy(uShellCmdFs.path, "/");    // Set to root if no slashes found
            }

            /* Update newPath with the new current path */
            strncpy(newPath, uShellCmdFs.path, sizeof(newPath) - 1);
            newPath [sizeof(newPath) - 1] = '\0';
        }
        else
        {
            /* If newDir is absolute, use it directly; otherwise, build newPath relative to current path */
            if (newDir [0] == '/')
            {
                strncpy(newPath, newDir, sizeof(newPath) - 1);
                newPath [sizeof(newPath) - 1] = '\0';
            }
            else
            {
                /* Determine separator: add '/' if current path doesn't end with one */
                const char* sep = (uShellCmdFs.path [strlen(uShellCmdFs.path) - 1] == '/') ? "" : "/";
                snprintf(newPath, sizeof(newPath), "%s%s%s", uShellCmdFs.path, sep, newDir);
            }
        }

        /* Check if the new path is valid */
        statusFs = lfs_stat(uShellCmdFs.lfs, newPath, &info);
        if ((statusFs < 0) || (info.type != LFS_TYPE_DIR))
        {
            printf("cd: directory %s does not exist or is not a directory \n", newPath);
            break;
        }

        /* Update the current path */
        strncpy(uShellCmdFs.path, newPath, sizeof(uShellCmdFs.path) - 1);
        uShellCmdFs.path [sizeof(uShellCmdFs.path) - 1] = '\0';

        /* Print the new current path */
        printf("cd: changed directory to %s\n", uShellCmdFs.path);

    } while (0);

    /* Return status */
    return status;
}

/**
 * \brief Execute the ls command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsLsExec(void* const cmd,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;       // Variable to store command status
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};    // Buffer for the full path
    int statusFs = 0;                                // Variable to store file system status
    const char* path = (argc > 0) ? argv [0] : uShellCmdFs.path;
    lfs_dir_t dir;
    struct lfs_info info;

    /* Process the ls command */
    do
    {
        /* Try to open the directory */
        statusFs = lfs_dir_open(uShellCmdFs.lfs, &dir, path);
        if (statusFs < 0)
        {
            printf("ls: cannot open directory %s \n", path);
            break;    // Error opening directory
        }

        printf("Listing of %s:\n", path);

        /* Read the directory entries */
        do
        {
            /* Read the next entry in the directory */
            statusFs = lfs_dir_read(uShellCmdFs.lfs, &dir, &info);

            /* Check if we reached the end of the directory */
            if (statusFs == 0)
            {
                break;    // End of directory
            }

            /* Check if we dir or file */
            if (info.type == LFS_TYPE_DIR)
            {
                /* Print dir name */
                printf("  <DIR> %s\n", info.name);
            }
            else
            {
                /* Print file name and size */
                printf("        %s (%u bytes)\n", info.name, (unsigned) info.size);
            }

            /* Check for errors */
            if (statusFs < 0)
            {
                printf("ls: error reading directory %s\n", path);
                break;    // Error reading directory
            }

        } while (1);

        /* Close the directory */
        lfs_dir_close(uShellCmdFs.lfs, &dir);

    } while (0);

    return status;
}

/**
 * \brief Execute the rm command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsRmExec(void* const cmd,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    const char* filePath = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;    // Variable to store file system status

    /* Remove file or directory */
    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            printf("Usage: rm <path>\n");
            break;
        }

        /* Add the directory path to the full path */
        strncpy(fullPath, uShellCmdFs.path, sizeof(fullPath) - 1);
        fullPath [sizeof(fullPath) - 1] = '\0';

        /* Add the directory path to the full path */
        if (uShellCmdFs.path [strlen(uShellCmdFs.path) - 1] == '/')
        {
            snprintf(fullPath, sizeof(fullPath), "%s%s", uShellCmdFs.path, filePath);
        }
        else
        {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", uShellCmdFs.path, filePath);
        }

        /* Check if the file exists */
        statusFs = lfs_remove(uShellCmdFs.lfs, fullPath);
        if (statusFs < 0)
        {
            /* File or directory not found */
            printf("rm: cannot remove %s \n", fullPath);
            break;
        }

        /* File or directory removed successfully */
        printf("rm: %s removed successfully\n", fullPath);

    } while (0);

    return status;
}

/**
 * \brief Execute the mkdir command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsMkdirExec(void* const cmd,
                                           const int argc,
                                           char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;    // Variable to store command status
    const char* dir_path = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;    // Variable to store file system status

    /* Create directory */
    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            printf("Usage: mkdir <directory>\n");
            break;
        }

        /* Add the directory path to the full path */
        strncpy(fullPath, uShellCmdFs.path, sizeof(fullPath) - 1);
        fullPath [sizeof(fullPath) - 1] = '\0';

        /* Add the directory path to the full path */
        if (uShellCmdFs.path [strlen(uShellCmdFs.path) - 1] == '/')
        {
            snprintf(fullPath, sizeof(fullPath), "%s%s", uShellCmdFs.path, dir_path);
        }
        else
        {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", uShellCmdFs.path, dir_path);
        }

        /* Check if the directory already exists */
        statusFs = lfs_mkdir(uShellCmdFs.lfs, fullPath);
        if (statusFs < 0)
        {
            /* Directory already exists or error creating directory */
            printf("mkdir: cannot create directory %s \n", fullPath);
            break;    // Error creating directory
        }

        /* Directory created successfully */
        printf("mkdir: directory %s created successfully\n", fullPath);

    } while (0);

    return status;    // Return success code
}

/**
 * \brief Execute the cat command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsCatExec(void* const cmd,
                                         const int argc,
                                         char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;       // Variable to store command status
    const char* file_path = argv [0];                // Relative file path
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};    // Buffer for the full path
    lfs_file_t file;
    int lfsStatus = 0;
    char buffer [32] = {0};    // Buffer for reading the file
    int bytes_read = 0;

    /* Cat command */
    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            printf("Usage: cat <file>\n");
            break;
        }

        /* Add the directory path to the full path */
        strncpy(fullPath, uShellCmdFs.path, sizeof(fullPath) - 1);
        fullPath [sizeof(fullPath) - 1] = '\0';

        /* Add the directory path to the full path */
        if (uShellCmdFs.path [strlen(uShellCmdFs.path) - 1] == '/')
        {
            snprintf(fullPath, sizeof(fullPath), "%s%s", uShellCmdFs.path, file_path);
        }
        else
        {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", uShellCmdFs.path, file_path);
        }

        /* Try to open the file */
        lfsStatus = lfs_file_open(uShellCmdFs.lfs, &file, fullPath, LFS_O_RDONLY);
        if (lfsStatus < 0)
        {
            printf("cat: cannot open file %s \n", fullPath);
            break;
        }

        /* Read the file and print its content */
        while (1)
        {
            bytes_read = lfs_file_read(uShellCmdFs.lfs, &file, buffer, sizeof(buffer));
            if (bytes_read == 0)
            {
                break;    // End of file reached
            }
            else if (bytes_read < 0)
            {
                printf("cat: error reading file %s \n", fullPath);
                break;
            }
            printf("%.*s", bytes_read, buffer);
        }

        /* Print new line after reading the file */
        printf("\n");

        /* Close the file */
        lfs_file_close(uShellCmdFs.lfs, &file);

    } while (0);

    return status;
}

/**
 * \brief Execute the write command.
 * \param cmd - UShellCmd object
 * \param argc - number of arguments
 * \param argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsWriteExec(void* const cmd,
                                           const int argc,
                                           char* const argv [])
{
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    const char* fileName = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;
    lfs_file_t file;

    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            printf("Usage: write <file>\n");
            break;
        }

        /* Form the full path using the current directory and the provided file name */
        if (uShellCmdFs.path [strlen(uShellCmdFs.path) - 1] == '/')
            snprintf(fullPath, sizeof(fullPath), "%s%s", uShellCmdFs.path, fileName);
        else
            snprintf(fullPath, sizeof(fullPath), "%s/%s", uShellCmdFs.path, fileName);

        /* Open the file for writing (create or truncate if it exists) */
        statusFs = lfs_file_open(uShellCmdFs.lfs,
                                 &file,
                                 fullPath,
                                 LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND);
        if (statusFs < 0)
        {
            printf("write: cannot open file %s for writing\n", fullPath);
            break;
        }

        /* Save the current file object to the xmodem server */
        uShellCmdFs.currentFile = &file;    // Save the current file object to the xmodem server

        /* Initialize the xmodem server */
        printf("write: ready to receive file %s via XModem...\n", fullPath);

        /* Receive file data via XModem transfer and write it to the file */
        XModemServerErr_e xmodemStatus = XModemServerProc(&uShellCmdFs.xModemServer);
        if (xmodemStatus != XMODEM_SERVER_NO_ERR)
        {

            printf("write: XModem transfer error: %d\n", xmodemStatus);
        }
        else
        {
            printf("write: XModem transfer completed successfully\n");
        }

        /* Close the file after transfer */
        lfs_file_close(uShellCmdFs.lfs, &file);

        /* Remove the current file object reference */
        uShellCmdFs.currentFile = NULL;    // Remove the current file object reference

    } while (0);

    return status;
}

/**
 * \brief Delay function for the xmodem server.
 * \param xmodem - xmodem server object
 * \param ms - delay in milliseconds
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemDelay(void* const xmodem,
                                                const int ms)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;      // Variable to store the status of the operation
    XModemServer_s* xdm = (XModemServer_s*) xmodem;       // Cast the xmodem object to UShellCmdFs_s type
    UShellCmdFs_s* cmd = (UShellCmdFs_s*) xdm->parent;    // Cast the parent object to UShellCmdFs_s type

    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (cmd == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Delay */
        vTaskDelay(pdMS_TO_TICKS(ms));    // Delay for the specified time

    } while (0);

    return status;
}

/**
 * @brief TxByte function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] byte - byte to transmit
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemTxByte(void* const xmodem,
                                                 const uint8_t byte,
                                                 void* const parent)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;      // Variable to store the status of the operation
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;         // Variable to store the status of the operation
    XModemServer_s* xdm = (XModemServer_s*) xmodem;       // Cast the xmodem object to UShellCmdFs_s type
    UShellCmdFs_s* cmd = (UShellCmdFs_s*) xdm->parent;    // Cast the parent object to UShellCmdFs_s type
    UShellVcp_s* vcp = (UShellVcp_s*) cmd->vcp;           // Cast the vcp object to UShellVcp_s type
    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (cmd == NULL) ||
            (vcp == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        vcpStatus = UShellVcpPrintChar(vcp, byte);    // Send the byte to the vcp object
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);            // Set status to error if vcp send fails
            status = XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                              // Exit the loop
        }

    } while (0);

    return status;
}

/**
 * @brief RxByte function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] byte - byte to receive
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemRxByte(void* const xmodem,
                                                 uint8_t* byte,
                                                 void* const parent)

{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;      // Variable to store the status of the operation
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;         // Variable to store the status of the operation
    XModemServer_s* xdm = (XModemServer_s*) xmodem;       // Cast the xmodem object to UShellCmdFs_s type
    UShellCmdFs_s* cmd = (UShellCmdFs_s*) xdm->parent;    // Cast the parent object to UShellCmdFs_s type
    UShellVcp_s* vcp = (UShellVcp_s*) cmd->vcp;           // Cast the vcp object to UShellVcp_s type

    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (cmd == NULL) ||
            (vcp == NULL) ||
            (byte == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        vcpStatus = UShellVcpScanChar(vcp, byte);    // Receive the byte from the vcp object
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);            // Set status to error if vcp send fails
            status = XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                              // Exit the loop
        }

    } while (0);

    return status;
}

/**
 * @brief IsRxByte function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] byte - byte to check
 * @param[in] parent - pointer to the parent object (optional)
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemIsRxByte(void* const xmodem,
                                                   bool* isRx,
                                                   void* const parent)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;      // Variable to store the status of the operation
    UShellVcpErr_e vcpStatus = USHELL_VCP_NO_ERR;         // Variable to store the status of the operation
    XModemServer_s* xdm = (XModemServer_s*) xmodem;       // Cast the xmodem object to UShellCmdFs_s type
    UShellCmdFs_s* cmd = (UShellCmdFs_s*) xdm->parent;    // Cast the parent object to UShellCmdFs_s type
    UShellVcp_s* vcp = (UShellVcp_s*) cmd->vcp;           // Cast the vcp object to UShellVcp_s type
    bool empty = false;                                   // Variable to check if the vcp is empty

    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (cmd == NULL) ||
            (vcp == NULL) ||
            (isRx == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        vcpStatus = UShellVcpScanIsEmpty(vcp, &empty);    // Check if the byte is empty from the vcp object
        if (vcpStatus != USHELL_VCP_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);            // Set status to error if vcp send fails
            status = XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                              // Exit the loop
        }

        /* Set the isRx flag based on the empty status */
        *isRx = (empty) ? false : true;    // Set the isRx flag based on the empty status

    } while (0);

    return status;
}

/**
 * @brief Write function for the xmodem server.
 * @param[in] xmodem - xmodem server object
 * @param[in] data - data to write
 * @param[in] size - size of the data to write
 * @return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemWrite(void* const xmodem,
                                                uint8_t* const data,
                                                const int size)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;
    XModemServer_s* xdm = (XModemServer_s*) xmodem;
    UShellCmdFs_s* cmd = (UShellCmdFs_s*) xdm->parent;
    int ret = 0;    // Variable to store the return value of the file write operation

    do
    {
        /* Check input parameter */
        if ((xdm == NULL) ||
            (cmd == NULL) ||
            (data == NULL) ||
            (cmd->currentFile == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Write received data using LittleFS */
        ret = lfs_file_write(cmd->lfs,
                             cmd->currentFile,
                             data,
                             size);
        if ((ret < 0) ||
            (ret != size))
        {
            status = XMODEM_SERVER_PORT_ERR;
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if file write fails
            break;                      // Exit the loop
        }

        /* Flush the file to ensure data is written */
        ret = lfs_file_sync(cmd->lfs, cmd->currentFile);
        if (ret < 0)
        {
            status = XMODEM_SERVER_PORT_ERR;
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if file sync fails
            break;                      // Exit the loop
        }

    } while (0);

    return status;
}
