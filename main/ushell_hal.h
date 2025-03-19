#ifndef USHELL_HAL_H_
#define USHELL_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*================================================================[INCLUDE]================================================*/

/* Standard includes */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*===========================================================[MACRO DEFINITIONS]============================================*/

/**
 * @brief Enumeration of possible error codes returned by the UShell HAL module.
 *
 * This enumeration defines all possible error codes that can be returned by
 * the UShell Hardware Abstraction Layer (HAL) module functions.
 */
typedef enum
{
    /**
     * @brief Operation completed successfully.
     *
     * Indicates that the requested operation was completed without any errors.
     */
    USHELL_HAL_NO_ERR = 0,

    /**
     * @brief Invalid arguments provided.
     *
     * Indicates that one or more arguments passed to the function were invalid,
     * such as NULL pointers.
     */
    USHELL_HAL_INVALID_ARGS_ERR,

    /**
     * @brief Module not initialized.
     *
     * Indicates that an operation was attempted before the module was properly
     * initialized.
     */
    USHELL_HAL_NOT_INIT_ERR,

    /**
     * @brief Port layer error occurred.
     *
     * Indicates that an error occurred in the underlying port layer
     * implementation.
     */
    USHELL_HAL_PORT_ERR,

    /**
     * @brief RX/TX operation error.
     *
     * Indicates that an error occurred during a receive or transmit operation.
     */
    USHELL_RX_TX_ERR,

} UShellHalErr_e;

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * @brief Type definition for an item in the UShell HAL.
 *
 * This type defines the size of one item in the UShell Hardware Abstraction Layer (HAL).
 */
typedef char UShellHalItem_t;

/**
 * @brief Structure defining the function table for UShell HAL portability.
 *
 * This structure contains function pointers for operations needed to port
 * the UShell Hardware Abstraction Layer (HAL) to different platforms.
 */
typedef struct
{
    /**
     * @brief Open function.
     *
     * This function initializes and opens the HAL.
     *
     * @param[in] hal Pointer to the HAL instance.
     * @return Error code indicating the result of the operation.
     */
    UShellHalErr_e (*open)(void* const hal);

    /**
     * @brief Close function.
     *
     * This function closes and deinitializes the HAL.
     *
     * @param[in] hal Pointer to the HAL instance.
     * @return Error code indicating the result of the operation.
     */
    UShellHalErr_e (*close)(void* const hal);

    /**
     * @brief Write function.
     *
     * This function writes data to the HAL.
     *
     * @param[in] hal Pointer to the HAL instance.
     * @param[in] data Pointer to the data to be written.
     * @param[in] size Size of the data to be written.
     * @return Error code indicating the result of the operation.
     */
    UShellHalErr_e (*write)(void* const hal,
                            const UShellHalItem_t* const data,
                            const size_t size);

    /**
     * @brief Read function.
     *
     * This function reads data from the HAL.
     *
     * @param[in] hal Pointer to the HAL instance.
     * @param[out] data Pointer to the buffer to store the read data.
     * @param[in] buffSize Size of the buffer.
     * @param[out] usedSize used size.
     * @return Error code indicating the result of the operation.
     */
    UShellHalErr_e (*read)(void* const hal,
                           UShellHalItem_t* const data,
                           const size_t buffSize,
                           size_t* const usedSize);

    /**
     * @brief Set Tx mode
     *
     * This function tx mode
     *
     * @param[in] hal Pointer to the HAL instance.
     * @return Error code indicating the result of the operation.
     *
     */
    UShellHalErr_e (*setTxMode)(void* const hal);

    /**
     * @brief Set Rx mode
     *
     * This function rx mode
     *
     * @param[in] hal Pointer to the HAL instance.
     * @return Error code indicating the result of the operation.
     */
    UShellHalErr_e (*setRxMode)(void* const hal);

} UShellHalPortTable_s;

/**
 * @brief Enumeration of possible callback types for the UShell HAL module.
 *
 * This enumeration defines the different types of callbacks that can be used
 * in the UShell Hardware Abstraction Layer (HAL) module.
 */
typedef enum
{
    /**
     * @brief No callback.
     *
     * Indicates that no callback is set.
     */
    USHELL_HAL_CB_NONE = 0x00,

    /**
     * @brief Callback for RX/TX error.
     *
     * Indicates that a callback should be invoked when an RX/TX error occurs.
     */
    USHELL_HAL_CB_RX_TX_ERROR = 0x01,

    /**
     * @brief Callback for TX complete.
     *
     * Indicates that a callback should be invoked when a TX operation is complete.
     */
    USHELL_HAL_CB_TX_COMPLETE = 0x02,

    /**
     * @brief Callback for RX received.
     *
     * Indicates that a callback should be invoked when an RX operation is complete.
     */
    USHELL_HAL_CB_RX_RECEIVED = 0x04,

    /**
     * @brief All callbacks.
     *
     * Indicates that all callbacks are set.
     */
    USHELL_HAL_CB_ALL = 0x07,

} UShellHalCallback_e;

/**
 * @brief Type definition for UShell HAL callback function.
 *
 * This type defines a function pointer for callbacks used in the UShell
 * Hardware Abstraction Layer (HAL) module. The callback function is called
 * when a specific event occurs, such as an RX/TX error or a TX complete event.
 *
 * @param[in] hal Pointer to the HAL instance.
 * @param[in] cbType Type of the callback event.
 */
typedef void (*UShellHalCb)(const void* const hal,
                            const UShellHalCallback_e cbType);

/**
 * @brief Describe UShellHal object.
 *
 * This structure defines the UShell Hardware Abstraction Layer (HAL) object,
 * which includes pointers to the parent object, name, port table, and callback functions.
 */
typedef struct
{
    /**
     * @brief Pointer to the parent object.
     *
     * This pointer references the parent object that contains or uses this HAL object.
     */
    const void* parent;

    /**
     * @brief Pointer to the name of the object.
     *
     * This pointer references the name of the HAL object.
     */
    const char* name;

    /**
     * @brief Port table.
     *
     * This pointer references the port table that contains function pointers
     * for operations needed to port the UShell HAL to different platforms.
     */
    const UShellHalPortTable_s* port;

    /* Callback functions */

    /**
     * @brief Callback for RX received event.
     *
     * This callback function is called when an RX received event occurs.
     */
    UShellHalCb rxReceivedCb;

    /**
     * @brief Callback for TX complete event.
     *
     * This callback function is called when a TX complete event occurs.
     */
    UShellHalCb txCompleteCb;

    /**
     * @brief Callback for RX/TX error event.
     *
     * This callback function is called when an RX/TX error event occurs.
     */
    UShellHalCb rxTxErrorCb;

} UShellHal_s;

/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShellHalStm32) (Not mandatory)
 * \param[in] name - pointer to the name of the object (Not mandatory)
 * \param[in] cbTable - pointer to the callback table to be used
 * \param[in] portTable Pointer to the port table
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success, otherwise error code
 */
UShellHalErr_e UShellHalInit(UShellHal_s* const hal,
                             const void* const parent,
                             const char* const name,
                             const UShellHalPortTable_s* const portTable);

/**
 * \brief Deinitialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be deinitialized
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success, otherwise error code
 */
UShellHalErr_e UShellHalDeinit(UShellHal_s* const hal);

/**
 * @brief Set the parent of the UShellHal object
 * @param[in] hal - UShellHal object to set the parent
 * @param[in] parent - pointer to the parent object
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalParentSet(UShellHal_s* const hal, const void* const parent);

/**
 * \brief Get parent from the UShellHal object
 * \param[in] hal - UShellHal object to get the parent
 * \param[out] parent - pointer to store the parent
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalParentGet(UShellHal_s* const hal, void** const parent);

/**
 * \brief Get the name of the UShellHal object
 * \param[in] hal - UShellHal object to get the name
 * \param[out] name - pointer to store the name
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalNameGet(UShellHal_s* const hal, const char** const name);

/**
 * \brief Attach callback to the UShellHal object
 * \param[in] hal - UShellHal object to attach the callback
 * \param[in] cbType - type of the callback to attach
 * \param[in] cb - callback to attach
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalCbAttach(UShellHal_s* const hal,
                                 const UShellHalCallback_e cbType,
                                 const UShellHalCb cb);

/**
 * \brief Detach callback from the UShellHal object
 * \param[in] hal - UShellHal object to detach the callback
 * \param[in] cbType - type of the callback to detach
 * \param[out] none
 * @return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
 */
UShellHalErr_e UShellHalCbDetach(UShellHal_s* const hal,
                                 const UShellHalCallback_e cbType);

/**
 * @brief  Open the UShellHal object
 * @param[in] hal - UShellHal object to open
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalOpen(UShellHal_s* const hal);

/**
 * @brief Close the UShellHal object
 * @param[in] hal - UShellHal object to close
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalClose(UShellHal_s* const hal);

/**
 * @brief Write data to the UShellHal object
 * @param[in] hal - UShellHal object to write
 * @param[in] data - pointer to the data to write
 * @param[in] size - size of the data to write
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalWrite(UShellHal_s* const hal,
                              const UShellHalItem_t* const data,
                              const size_t size);

/**
 * @brief Read data from the UShellHal object
 * @param[in] hal - UShellHal object to read
 * @param[in] data - pointer to the data to read
 * @param[in] buffSize - size of buffer
 * @param[out] usedSize - size used in buffer
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalRead(UShellHal_s* const hal,
                             UShellHalItem_t* const data,
                             const size_t buffSize,
                             size_t* const usedSize);

/**
 * @brief Set the tx mode of the UShellHal object
 * @param[in] hal - UShellHal object to set the tx mode
 * @param[out] none
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalSetTxMode(UShellHal_s* const hal);

/**
 * @brief Set the rx mode of the UShellHal object
 * @param[in] hal - UShellHal object to set the rx mode
 * @return UShellHalErr_e - error code. non-zero = an error has occurred;
 */
UShellHalErr_e UShellHalSetRxMode(UShellHal_s* const hal);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* USHELL_HAL_H_ */