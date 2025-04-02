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
 * \brief UShellCmdFsCd object (base object)
 */
UShellCmdFs_s uShellCmdFs = {0};

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the commands for file system operations.
 * \param [in] none
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdFsInit(lfs_t* lfs)
{
    /* Local variable */
    int status = 0;                                  // Variable to store command status
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        if (lfs == NULL)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if lfs is NULL
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

        /* Save the lfs object */
        uShellCmdFs.lfs = lfs;    // Save the lfs object to the command object

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
            printf("Usage: cd <directory>\r\n");
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
            printf("ls: cannot open directory %s \r\n", path);
            break;    // Error opening directory
        }

        printf("Listing of %s:\r\n", path);

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
                printf("  <DIR> %s\r\n", info.name);
            }
            else
            {
                /* Print file name and size */
                printf("        %s (%u bytes)\r\n", info.name, (unsigned) info.size);
            }

            /* Check for errors */
            if (statusFs < 0)
            {
                printf("ls: error reading directory %s\r\n", path);
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
            printf("Usage: rm <path>\r\n");
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
            printf("rm: cannot remove %s \r\n", fullPath);
            break;
        }

        /* File or directory removed successfully */
        printf("rm: %s removed successfully\r\n", fullPath);

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
            printf("Usage: mkdir <directory>\r\n");
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
            printf("mkdir: cannot create directory %s \r\n", fullPath);
            break;    // Error creating directory
        }

        /* Directory created successfully */
        printf("mkdir: directory %s created successfully\r\n", fullPath);

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
            printf("Usage: cat <file>\r\n");
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
            printf("cat: cannot open file %s \r\n", fullPath);
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
                printf("cat: error reading file %s \r\n", fullPath);
                break;
            }
            printf("%.*s", bytes_read, buffer);
        }

        /* Close the file */
        lfs_file_close(uShellCmdFs.lfs, &file);

    } while (0);

    return status;
}
