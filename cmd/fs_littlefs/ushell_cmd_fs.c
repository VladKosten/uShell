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
 * \brief Assert macro for the UShellFs module.
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
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
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
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
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
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
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
                                           UShellSocket_s* const readSocket,
                                           UShellSocket_s* const writeSocket,
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
                                         UShellSocket_s* const readSocket,
                                         UShellSocket_s* const writeSocket,
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
                                           UShellSocket_s* const readSocket,
                                           UShellSocket_s* const writeSocket,
                                           const int argc,
                                           char* const argv []);

/**
 * \brief Execute the read command.
 * \param[in] cmd - UShellCmd object
 * \param[in] argc - number of arguments
 * \param[in] argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsReadExec(void* const cmd,
                                          UShellSocket_s* const readSocket,
                                          UShellSocket_s* const writeSocket,
                                          const int argc,
                                          char* const argv []);

/**
 * \brief Delay function for the xmodem server.
 * \param xmodem - xmodem server object
 * \param ms - delay in milliseconds
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerDelay(void* const xmodem,
                                                      const int ms);

/**
 * \brief Transmit function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] data - byte to transmit
 * \param[in] size - size of the data to transmit
 * \param[in] timeMs - time in milliseconds to delay
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerTransmit(void* server,
                                                         uint8_t* data,
                                                         const size_t size,
                                                         size_t timeMs);

/**
 * \brief RxByte function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] byte - byte to receive
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerReceive(void* server,
                                                        uint8_t* data,
                                                        const size_t size,
                                                        size_t timeMs);

/**
 * \brief IsRxByte function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] isRx - byte to check
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerIsRxByte(void* const xmodem,
                                                         bool* isRx);

/**
 * \brief Write function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] data - data to write
 * \param[in] size - size of the data to write
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerWrite(void* const xmodem,
                                                      uint8_t* const data,
                                                      const int size);

/**
 * \brief Check byte is rx
 * \param client - xmodem client object
 * \param isRx - buffer to store the rx status
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientIsRxByte(void* client,
                                                         bool* isRx);

/**
 * \brief Write function for the xmodem client.
 * \param client - xmodem client object
 * \param data - data to write
 * \param size - size of the data to write
 * \param usedSize - size of the data written
 * \param offset - offset to write the data
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientReadFromMemory(void* client,
                                                               uint8_t* data,
                                                               const size_t size,
                                                               size_t* const usedSize,
                                                               const size_t offset);

/**
 * \brief Receive byte function for the xmodem client.
 * \param client - xmodem client object
 * \param byte - byte to receive
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientReceive(void* client,
                                                        uint8_t* const data,
                                                        const size_t size,
                                                        size_t timeMs);

/**
 * \brief Transmit function for the xmodem client.
 * \param client - xmodem client object
 * \param data - data to transmit
 * \param size - size of the data to transmit
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientTransmit(void* client,
                                                         uint8_t* const data,
                                                         const size_t size,
                                                         size_t timeMs);

/**
 * \brief UShellCmdFsCd object (base object)
 */
UShellCmdFs_s uShellCmdFs = {0};

/**
 * \brief XModem Server Port
 */
static XModemServerPort_s uShellCmdFsXModemServerPort =
    {
        .transmit = uShellCmdFsXModemServerTransmit,
        .receive = uShellCmdFsXModemServerReceive,
        .writeToMemory = uShellCmdFsXModemServerWrite,
};

/**
 * \brief XModem Client Port
 */
static XModemClientPort_s uShellCmdFsXModemClientPort =
    {
        .readFromMemory = uShellCmdFsXModemClientReadFromMemory,
        .receive = uShellCmdFsXModemClientReceive,
        .transmit = uShellCmdFsXModemClientTransmit,
};

/**
 * \brief For storing the current socket (i now its terrable, but i have no time to fix it)
 */
static UShellSocket_s* uShellCmdFsReadSocket = NULL;     ///< Pointer to the read socket
static UShellSocket_s* uShellCmdFsWriteSocket = NULL;    ///< Pointer to the write socket

//=======================================================================[ PUBLIC INTERFACE FUNCTIONS ]=============================================================================

/**
 * \brief Initialize the commands for file system operations.
 * \param [in] rootCmd - pointer to the root command
 * \param [in] lfs - pointer to the lfs object
 * \param [in] vcp - pointer to the UShellVcp object
 * \param [out] none
 * \return UShellOsalErr_e - error code
 */
int UShellCmdFsInit(UShellCmd_s* const rootCmd,
                    lfs_t* const lfs)
{
    /* Local variable */
    int status = 0;                                  // Variable to store command status
    UShellCmdErr_e cmdStatus = USHELL_CMD_NO_ERR;    // Variable to store command status

    do
    {
        /* Check input parameter */
        if ((lfs == NULL) ||
            (rootCmd == NULL))
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

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdLs.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
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

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdCd.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
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

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdRm.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
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
        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdMkdir.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
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

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdCat.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
            status = -6;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init write command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdWrite.cmd,
                                  USHELL_CMD_FS_WRITE_NAME,
                                  USHELL_CMD_FS_WRITE_HELP,
                                  uShellCmdFsWriteExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -8;                // Set status to error
            break;                      // Exit the loop
        }

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdWrite.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
            status = -8;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init read command */
        cmdStatus = UShellCmdInit(&uShellCmdFs.cmdRead.cmd,
                                  USHELL_CMD_FS_READ_NAME,
                                  USHELL_CMD_FS_READ_HELP,
                                  uShellCmdFsReadExec);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -9;                // Set status to error
            break;                      // Exit the loop
        }

        /* Add cmd to root */
        cmdStatus = UShellCmdListAdd(rootCmd, &uShellCmdFs.cmdRead.cmd);
        if (cmdStatus != USHELL_CMD_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command attachment fails
            status = -9;                // Set status to error
            break;                      // Exit the loop
        }

        /* Init xmodem command */
        XModemServerErr_e xmodemStatus = XModemServerInit(&uShellCmdFs.xModemServer,
                                                          &uShellCmdFsXModemServerPort,
                                                          &uShellCmdFs);
        if (xmodemStatus != XMODEM_SERVER_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if command initialization fails
            status = -8;                // Set status to error
            break;                      // Exit the loop
        }

        XModemClientErr_e xmodemClientStatus = XModemClientInit(&uShellCmdFs.xModemClient,
                                                                &uShellCmdFsXModemClientPort,
                                                                &uShellCmdFs);

        /* Save the lfs object to the command object */
        uShellCmdFs.lfs = lfs;

        /* Set the start path */
        strncpy(uShellCmdFs.path, USHELL_CMD_FS_START_PATH, sizeof(uShellCmdFs.path) - 1);

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
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    (void) socketStatus;
    const char* newDir = argv [0];
    char newPath [USHELL_CMD_FS_MAX_PATH] = {0};    // Buffer for the new path
    struct lfs_info info = {0};                     // Buffer for the file system info
    int statusFs = 0;                               // Variable to store file system status

    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Usage: cd <directory>\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* Check if newDir is ".." */
        if (strcmp(newDir, "..") == 0)
        {
            /* Make sure we are not at root directory */
            if (strcmp(uShellCmdFs.path, "/") == 0)
            {
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "cd: Already at root directory.\n");
                USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "cd: directory %s does not exist or is not a directory \n",
                                             newPath);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* Update the current path */
        strncpy(uShellCmdFs.path, newPath, sizeof(uShellCmdFs.path) - 1);
        uShellCmdFs.path [sizeof(uShellCmdFs.path) - 1] = '\0';

        /* Print the new current path */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "cd: changed directory to %s\n",
                                         uShellCmdFs.path);
        USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK

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
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};             // Buffer for the full path
    int statusFs = 0;                                         // Variable to store file system status
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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "ls: cannot open directory %s \n",
                                             path);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;                                                         // Error opening directory
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
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "  <DIR> %s\n",
                                                 info.name);
                USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            }
            else
            {
                /* Print file name and size */
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "        %s (%u bytes)\n",
                                                 info.name,
                                                 (unsigned) info.size);
                USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            }

            /* Check for errors */
            if (statusFs < 0)
            {
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "ls: error reading directory %s\n",
                                                 path);
                USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
                break;                                                         // Error reading directory
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
                                        UShellSocket_s* const readSocket,
                                        UShellSocket_s* const writeSocket,
                                        const int argc,
                                        char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    const char* filePath = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;    // Variable to store file system status

    /* Remove file or directory */
    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Usage: rm <path>\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "rm: cannot remove %s \n",
                                             fullPath);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* File or directory removed successfully */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "rm: %s removed successfully\n",
                                         fullPath);
        USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK

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
                                           UShellSocket_s* const readSocket,
                                           UShellSocket_s* const writeSocket,
                                           const int argc,
                                           char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    (void) socketStatus;
    const char* dir_path = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;    // Variable to store file system status

    /* Create directory */
    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Usage: mkdir <directory>\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "mkdir: cannot create directory %s \n",
                                             fullPath);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;                                                         // Error creating directory
        }

        /* Directory created successfully */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "mkdir: directory %s created successfully\n",
                                         fullPath);
        USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK

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
                                         UShellSocket_s* const readSocket,
                                         UShellSocket_s* const writeSocket,
                                         const int argc,
                                         char* const argv [])
{
    /* Local variable */
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;                // Variable to store command status
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    (void) socketStatus;
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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Usage: cat <file>\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "cat: cannot open file %s \n",
                                             fullPath);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
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
                socketStatus = UShellSocketPrint(writeSocket,
                                                 "cat: error reading file %s \n",
                                                 fullPath);
                USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
                break;
            }
            printf("%.*s", bytes_read, buffer);
        }

        /* Print new line after reading the file */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "\ncat: file %s read successfully\n",
                                         fullPath);
        USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK

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
                                           UShellSocket_s* const readSocket,
                                           UShellSocket_s* const writeSocket,
                                           const int argc,
                                           char* const argv [])
{
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;
    (void) socketStatus;
    const char* fileName = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;
    lfs_file_t file;

    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Usage: write <file>\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* Save socket for xmodem server */
        uShellCmdFsReadSocket = readSocket;      // Save the read socket for xmodem server
        uShellCmdFsWriteSocket = writeSocket;    // Save the write socket for xmodem server

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
            socketStatus = UShellSocketPrint(writeSocket,
                                             "write: cannot open file %s for writing\n",
                                             fullPath);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* Save the current file object to the xmodem server */
        uShellCmdFs.currentFile = &file;    // Save the current file object to the xmodem server

        /* Initialize the xmodem server */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "write: ready to receive file %s via XModem...\n",
                                         fullPath);
        USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK

        /* Receive file data via XModem transfer and write it to the file */
        XModemServerErr_e xmodemStatus = XModemServerProc(&uShellCmdFs.xModemServer);
        if (xmodemStatus != XMODEM_SERVER_NO_ERR)
        {

            socketStatus = UShellSocketPrint(writeSocket,
                                             "write: XModem transfer error\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
        }
        else
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "write: XModem transfer completed successfully\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
        }

        /* Close the file after transfer */
        lfs_file_close(uShellCmdFs.lfs, &file);

        /* Remove the current file object reference */
        uShellCmdFs.currentFile = NULL;    // Remove the current file object reference

    } while (0);

    /* Flush the socket  */
    uShellCmdFsReadSocket = NULL;     // Save the read socket for xmodem server
    uShellCmdFsWriteSocket = NULL;    // Save the write socket for xmodem server

    return status;
}

/**
 * \brief Execute the read command.
 * \param[in] cmd - UShellCmd object
 * \param[in] argc - number of arguments
 * \param[in] argv - array of arguments
 * \return UShellCmdErr_e - error code. non-zero = an error has occurred;
 */
static UShellCmdErr_e uShellCmdFsReadExec(void* const cmd,
                                          UShellSocket_s* const readSocket,
                                          UShellSocket_s* const writeSocket,
                                          const int argc,
                                          char* const argv [])
{
    UShellCmdErr_e status = USHELL_CMD_NO_ERR;
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store socket status
    const char* fileName = argv [0];
    char fullPath [USHELL_CMD_FS_MAX_PATH] = {0};
    int statusFs = 0;
    lfs_file_t file;

    do
    {
        /* Check input parameter */
        if (argc != 1)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "Usage: read <file>\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* Save socket for xmodem server */
        uShellCmdFsReadSocket = readSocket;      // Save the read socket for xmodem server
        uShellCmdFsWriteSocket = writeSocket;    // Save the write socket for xmodem server

        /* Form the full path using the current directory and the provided file name */
        if (uShellCmdFs.path [strlen(uShellCmdFs.path) - 1] == '/')
            snprintf(fullPath, sizeof(fullPath), "%s%s", uShellCmdFs.path, fileName);
        else
            snprintf(fullPath, sizeof(fullPath), "%s/%s", uShellCmdFs.path, fileName);

        /* Open the file for reading */
        statusFs = lfs_file_open(uShellCmdFs.lfs,
                                 &file,
                                 fullPath,
                                 LFS_O_RDONLY);
        if (statusFs < 0)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "read: cannot open file %s for reading\n",
                                             fullPath);
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
            break;
        }

        /* Save the current file object to the xmodem server */
        uShellCmdFs.currentFile = &file;

        /* Initialize the xmodem server for transmitting file data */
        socketStatus = UShellSocketPrint(writeSocket,
                                         "read: ready to send file %s via XModem...\n",
                                         fullPath);
        USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK

        /* Transmit file data via XModem transfer */
        XModemClientErr_e clientStatus = XModemClientProc(&uShellCmdFs.xModemClient);
        if (clientStatus != XMODEM_SERVER_NO_ERR)
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "read: XModem transfer error\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
        }
        else
        {
            socketStatus = UShellSocketPrint(writeSocket,
                                             "read: XModem transfer completed successfully\n");
            USHELL_CMD_FS_ASSERT(socketStatus == USHELL_SOCKET_NO_ERR);    // Check if the socket status is OK
        }

        /* Close the file after transfer */
        lfs_file_close(uShellCmdFs.lfs, &file);

        /* Remove the current file object reference */
        uShellCmdFs.currentFile = NULL;

    } while (0);

    /* Flush the socket  */
    uShellCmdFsReadSocket = NULL;     // Save the read socket for xmodem server
    uShellCmdFsWriteSocket = NULL;    // Save the write socket for xmodem server

    return status;
}

/**
 * \brief Transmit function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] data - byte to transmit
 * \param[in] size - size of the data to transmit
 * \param[in] timeMs - time in milliseconds to delay
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerTransmit(void* server,
                                                         uint8_t* data,
                                                         const size_t size,
                                                         size_t timeMs)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;          // Variable to store the status of the operation
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store the status of the operation
    UShellSocket_s* socket = uShellCmdFsWriteSocket;

    do
    {
        /* Check input parameter */
        if ((socket == NULL))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        socketStatus = UShellSocketWrite(socket,
                                         data,
                                         size,
                                         timeMs);    // Send the byte to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if vcp send fails
            status = (socketStatus == USHELL_SOCKET_TIMEOUT_ERR)
                         ? XMODEM_SERVER_TIMEOUT_ERR
                         : XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                                    // Exit the loop
        }

    } while (0);

    return status;
}

/**
 * \brief RxByte function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] byte - byte to receive
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerReceive(void* server, uint8_t* data, const size_t size, size_t timeMs)

{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;          // Variable to store the status of the operation
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store the status of the operation
    UShellSocket_s* socket = uShellCmdFsReadSocket;

    do
    {
        /* Check input parameter */
        if ((socket == NULL) ||
            (data == NULL) ||
            (size == 0))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        socketStatus = UShellSocketRead(socket,
                                        data,
                                        size,
                                        timeMs);    // Send the byte to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            status = (socketStatus == USHELL_SOCKET_TIMEOUT_ERR)
                         ? XMODEM_SERVER_TIMEOUT_ERR
                         : XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                                    // Exit the loop
        }

    } while (0);

    return status;
}

/**
 * \brief Write function for the xmodem server.
 * \param[in] xmodem - xmodem server object
 * \param[in] data - data to write
 * \param[in] size - size of the data to write
 * \return XModemServerErr_e - error code. non-zero = an error has occurred;
 */
static XModemServerErr_e uShellCmdFsXModemServerWrite(void* const xmodem,
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

/**
 * \brief Write function for the xmodem client.
 * \param client - xmodem client object
 * \param data - data to write
 * \param size - size of the data to write
 * \param usedSize - size of the data written
 * \param offset - offset to write the data
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientReadFromMemory(void* client,
                                                               uint8_t* data,
                                                               const size_t size,
                                                               size_t* const usedSize,
                                                               const size_t offset)
{
    {
        /* Local variable */
        XModemClientErr_e status = XMODEM_SERVER_NO_ERR;
        XModemClient_s* clientXmodem = (XModemClient_s*) client;
        UShellCmdFs_s* cmd = (UShellCmdFs_s*) clientXmodem->parent;
        int ret = 0;    // Variable to store the return value of the file write operation

        do
        {
            /* Check input parameter */
            if ((clientXmodem == NULL) ||
                (cmd == NULL) ||
                (data == NULL) ||
                (cmd->currentFile == NULL))
            {
                USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
                status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
                break;                                      // Exit the loop
            }

            /* Offset the file pointer to the specified offset */
            ret = lfs_file_seek(cmd->lfs,
                                cmd->currentFile,
                                offset,
                                LFS_SEEK_SET);
            if (ret < 0)
            {
                status = XMODEM_SERVER_PORT_ERR;
                USHELL_CMD_FS_ASSERT(0);    // Set status to error if file seek fails
                break;                      // Exit the loop
            }

            /* Write received data using LittleFS */
            ret = lfs_file_read(cmd->lfs,
                                cmd->currentFile,
                                data,
                                size);

            /* Return size of data read */
            *usedSize = ret;    // Set the used size to the number of bytes read

        } while (0);

        return status;
    }
}

/**
 * \brief Receive byte function for the xmodem client.
 * \param client - xmodem client object
 * \param byte - byte to receive
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientReceive(void* client, uint8_t* const data, const size_t size, size_t timeMs)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;          // Variable to store the status of the operation
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store the status of the operation
    UShellSocket_s* socket = uShellCmdFsReadSocket;

    do
    {
        /* Check input parameter */
        if ((socket == NULL) ||
            (data == NULL) ||
            (size == 0))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        socketStatus = UShellSocketRead(socket,
                                        data,
                                        size,
                                        timeMs);    // Send the byte to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            status = (socketStatus == USHELL_SOCKET_TIMEOUT_ERR)
                         ? XMODEM_SERVER_TIMEOUT_ERR
                         : XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                                    // Exit the loop
        }

    } while (0);

    return status;
}

/**
 * \brief Transmit function for the xmodem client.
 * \param client - xmodem client object
 * \param data - data to transmit
 * \param size - size of the data to transmit
 * \return XModemClientErr_e - error code. non-zero = an error has occurred;
 */
static XModemClientErr_e uShellCmdFsXModemClientTransmit(void* client, uint8_t* const data, const size_t size, size_t timeMs)
{
    /* Local variable */
    XModemServerErr_e status = XMODEM_SERVER_NO_ERR;          // Variable to store the status of the operation
    UShellSocketErr_e socketStatus = USHELL_SOCKET_NO_ERR;    // Variable to store the status of the operation
    UShellSocket_s* socket = uShellCmdFsWriteSocket;

    do
    {
        /* Check input parameter */
        if ((socket == NULL) ||
            (data == NULL) ||
            (size == 0))
        {
            USHELL_CMD_FS_ASSERT(0);                    // Set status to error if xmodem or cmd is NULL
            status = XMODEM_SERVER_INVALID_ARGS_ERR;    // Set status to error
            break;                                      // Exit the loop
        }

        /* Send byte */
        socketStatus = UShellSocketWrite(socket,
                                         data,
                                         size,
                                         timeMs);    // Send the byte to the socket
        if (socketStatus != USHELL_SOCKET_NO_ERR)
        {
            USHELL_CMD_FS_ASSERT(0);    // Set status to error if vcp send fails
            status = (socketStatus == USHELL_SOCKET_TIMEOUT_ERR)
                         ? XMODEM_SERVER_TIMEOUT_ERR
                         : XMODEM_SERVER_PORT_ERR;    // Set status to error
            break;                                    // Exit the loop
        }

    } while (0);

    return status;
}
