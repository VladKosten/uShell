#ifndef USHELL_CFG_H_
#define USHELL_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*================================================================[INCLUDE]=================================================*/

/* Specific includes */
#include <atmel_start.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

// Common macros
#define USHELL_ASSERT(cond)                     ASSERT(cond)

// UShell configuration macros
// Maximum number of commands in the UShell
#define USHELL_MAX_CMD                          15
// UShell update time in milliseconds
#define USHELL_UPD_TIME_MS                      100U
// Size of the buffer in the UShell
#define USHELL_BUFFER_SIZE                      256
// Password for authentication
#define USHELL_AUTH_PASSWORD                    "admin"
// UShell thread name
#define USHELL_THREAD_NAME                      "USHELL"
// UShell thread stack size in bytes
#define USHELL_THREAD_STACK_SIZE_BYTE           2048U
// UShell thread priority
#define USHELL_THREAD_PRIORITY                  USHELL_OSAL_THREAD_PRIORITY_LOW
// Prompt for the UShell
#define USHELL_USER_PROMPT                      "uShell> "
// Hello message for the UShell
#define USHELL_HELLO_MSG                        "Press Enter to start ..."
// Prompt for authentication state
#define USHELL_AUTH_PROMPT                      "Password: "
// Authentication success message
#define USHELL_AUTH_OK_MSG                      "Authentication OK \n"
// Authentication failure message
#define USHELL_AUTH_FAIL_MSG                    "Authentication FAIL \n"
// Command not found message
#define USHELL_CMD_NOT_FOUND_MSG                "Command not found \n"

// UShell VCP configuration macros
// Maximum size of the buffer in the UShell VCP
#define USHELL_VCP_BUFFER_SIZE                  128U
// Name of the uShell VCP thread
#define USHELL_VCP_THREAD_NAME                  "USHELL_VCP"
// Stack size for the uShell VCP thread
#define USHELL_VCP_THREAD_STACK_SIZE_BYTE       512U
// Priority of the uShell VCP thread
#define USHELL_VCP_THREAD_PRIORITY              USHELL_OSAL_THREAD_PRIORITY_LOW
// Timeout for tx operation in the uShell VCP
#define USHELL_VCP_TX_TIMEOUT_MS                3000U
// Redirect standard input/output to the uShell VCP
#define USHELL_VCP_REDIRECT_STDIO               TRUE
// Timer name for the uShell VCP
#define USHELL_VCP_TIMER_INSPECT_NAME           "USHELL_VCP_TIMER"
// Timer period for the uShell VCP
#define USHELL_VCP_TIMER_INSPECT_PERIOD_MS      1000U

// UShell history configuration macros
// Maximum number of command strings stored in the history buffer
#define USHELL_HISTORY_SIZE                     2
// Maximum length of each command string in the history buffer
#define USHELL_HISTORY_MAX_STR_LEN              40

// UShell CMD configuration macros
// Maximum number of arguments in the UShell command
#define USHELL_CMD_MAX_ARGV                     5

// UShell OSAL configuration macros
// Number of queue slots in the UShell OSAL
#define USHELL_OSAL_QUEUE_SLOTS_NUM             (2)
// Number of lock objects in the UShell OSAL
#define USHELL_OSAL_LOCK_OBJS_NUM               (2)
// Number of threads in the UShell OSAL
#define USHELL_OSAL_THREADS_NUM                 (1)
// Number of stream buffer slots in the UShell OSAL
#define USHELL_OSAL_STREAM_BUFF_SLOTS_NUM       (6)
// Number of timers in the UShell OSAL
#define USHELL_OSAL_TIMER_NUM                   (1)
// Number of event groups in the UShell OSAL
#define USHELL_OSAL_EVENT_GROUPS_NUM            (1)

// UShell HAL configuration macros
// None

// UShell OSAL Freertos configuration macros (Not mandatory)
// FreeRTOS-specific priority levels
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_LOW    (tskIDLE_PRIORITY + 1)
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_MIDDLE (tskIDLE_PRIORITY + 2)
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_HIGH   (tskIDLE_PRIORITY + 3)
#define USHELL_OSAL_FREERTOS_THREAD_PRIO_ULTRA  (tskIDLE_PRIORITY + configMAX_PRIORITIES)

#ifdef __cplusplus
}
#endif

#endif