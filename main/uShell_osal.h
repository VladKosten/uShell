#ifndef USHELL_OSAL_H_
#define USHELL_OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/



/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * @brief Enumeration of possible error codes returned by the UShell Osal module.
 */
typedef enum
{
    USHELL_OSAL_NO_ERR = 0,            ///< Exit: no errors (success)
    USHELL_OSAL_INVALID_ARGS_ERR,      ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_OSAL_NOT_INIT_ERR,          ///< Exit: error - not initialized
    USHELL_OSAL_PORT_ERR,              ///< Exit: error - port error (e.g. port layer error)

} UShellOsalErr_e;

/**
 * \brief Descriibe function needed to port for OSAL
*/
typedef struct
{
    UShellOsalErr_e (*threadStart)(const void* const osal);                     ///< Start the thread
    UShellOsalErr_e (*threadStop)(const void* const osal);                      ///< Stop the thread
    UShellOsalErr_e (*lock)(const void* const osal);                            ///< Lock the resource
    UShellOsalErr_e (*unlock)(const void* const osal);                          ///< Unlock the resource
    UShellOsalErr_e (*eventOcurred)(const void* const osal);                    ///< Check if event occurred
    UShellOsalErr_e (*eventQtyGet)(const void* const osal, uint8_t* const qt);  ///< Wait for event
    UShellOsalErr_e (*eventFlush)(const void* const osal);                      ///< Flush the event queue

}UShellOsalPortableTable_s;

/**
 * \brief Descriibe UShellOsal thread handle
*/
typedef void* UShellOsalThreadHandle_t;

/**
 * \brief Descriibe UShellOsal mutex handle
*/
typedef void* UShellOsalEventHandle_t;

/**
 * \brief Descriibe UShellOsal mutex handle
*/
typedef void* UShellOsalMutexHandle_t;

/**
 * \brief Descriibe UShellOsal worker function
*/
typedef void(*UShellOsalWorker_t)(const void* const UShell);

/**
 * \brief Descriibe UShellOsal object
*/
typedef struct
{
    const void* parent;                         ///< Pointer to the parent object
    const char* name;                           ///< Pointer to the name of the object

    UShellOsalWorker_t worker;                  ///< Worker function
    UShellOsalEventHandle_t eventHandle;        ///< Event handle
    UShellOsalMutexHandle_t mutexHandle;        ///< Mutex handle
    UShellOsalThreadHandle_t threadHandle;      ///< Thread handle

    const UShellOsalPortableTable_s* portTable;  ///< Pointer to the port table

}UShellOsal_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * @brief Initialize the UShell Osal module.
 * \param [in] osal - UShellOsal obj to be initialized
 * \param [in] portTable - port table to be used
 * \param [in] name - name of the object
 * \param [in] parent - pointer to the parent object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalInit(UShellOsal_s* const osal, const UShellOsalPortableTable_s* const portTable, const char* const name, const void* const parent);

/**
 * @brief Deinitialize the UShell Osal module.
 * \param [in] osal - UShellOsal obj to be deinitialized
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalDeinit(UShellOsal_s* const osal);

/**
 * @brief Set the parent object of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to set parent
 * \param [in] parent - pointer to the parent object
 * \param [out] none
*/
UShellOsalErr_e UShellOsalParentSet(UShellOsal_s* const osal, const void* const parent);

/**
 * @brief Get the parent object of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to get parent
 * \param [out] parent - pointer to the parent object
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalParentGet(const UShellOsal_s* const osal, void** const parent);

/**
 * @brief Set the name of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to set name
 * \param [in] name - name of the object
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalNameSet(UShellOsal_s* const osal, const char* const name);

/**
 * @brief Get the name of the UShellOsal module.
 * \param [in] osal - UShellOsal obj to get name
 * \param [out] name - name of the object
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalNameGet(const UShellOsal_s* const osal, const char** const name);

/**
 * @brief Attach a worker function to the UShellOsal module.
 * \param [in] osal - UShellOsal obj to attach worker
 * \param [in] worker - worker function
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalWorkerAttach(UShellOsal_s* const osal, UShellOsalWorker_t worker);

/**
 * @brief Detach a worker function from the UShellOsal module.
 * \param [in] osal - UShellOsal obj to detach worker
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalWorkerDetach(UShellOsal_s* const osal);

/**
 * @brief Lock object
 * \param [in] osal - UShellOsal obj to lock
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalLock(UShellOsal_s* const osal);

/**
 * @brief Unlock object
 * \param [in] osal - UShellOsal obj to unlock
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalUnlock(UShellOsal_s* const osal);

/**
 * @brief Event ocurred
 * \param [in] osal - UShellOsal obj to check event
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalEventOcurred(UShellOsal_s* const osal);

/**
 * @brief Get quantity of events ocurred
 * \param [in] osal - UShellOsal obj to wait for event
 * \param [out] qt - quantity of events
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalEventQtyGet(UShellOsal_s* const osal, uint8_t* const qty);

/**
 * @brief Flush the event queue
 * \param [in] osal - UShellOsal obj to flush event queue
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalEventFlush(UShellOsal_s* const osal);

/**
 * @brief Start the thread
 * \param [in] osal - UShellOsal obj to start thread
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalThreadStart(UShellOsal_s* const osal);

/**
 * @brief Stop the thread
 * \param [in] osal - UShellOsal obj to stop thread
 * \param [out] none
 * \return UShellOsalErr_e - error code
*/
UShellOsalErr_e UShellOsalThreadStop(UShellOsal_s* const osal);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_OSAL_H_ */