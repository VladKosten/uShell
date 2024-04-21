#ifndef USHELL_HAL_H_
#define USHELL_HAL_H_

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
 * \brief Enumeration of possible error codes returned by the UShell Hal module.
 */
typedef enum
{
    USHELL_HAL_NO_ERR = 0,            ///< Exit: no errors (success)
    USHELL_HAL_INVALID_ARGS_ERR,      ///< Exit: error - invalid pointers (e.g. null pointers)
    USHELL_HAL_NOT_INIT_ERR,          ///< Exit: error - not initialized
    USHELL_HAL_PORT_ERR,              ///< Exit: error - port error (e.g. port layer error)

} UShellHalErr_e;

/*========================================================[DATA TYPES DEFINITIONS]==========================================*/

/**
 * \brief Descriibe size of one item in the UShellHal
*/
typedef uint8_t UShellHalItem_t;


/**
 * \brief Descriibe function needed to port
*/
typedef struct
{
    UShellHalErr_e (*send)(const void* const hal, const UShellHalItem_t* const item, const size_t numberOfItems);       ///< Send data to the port
    UShellHalErr_e (*receieve)(const void* const hal, UShellHalItem_t* const item, const size_t* const numberOfItems);  ///< Get data from the port

}UShellHalPortableTable_s;

typedef enum
{
    USHELL_HAL_CB_NONE = 0x00,          ///< No callback
    USHELL_HAL_CB_RX_TX_ERROR = 0x01,   ///< Callback for error
    USHELL_HAL_CB_TX_COMPLETE = 0x02,   ///< Callback for tx complete
    USHELL_HAL_CB_RX_RECEIVED = 0x03,   ///< Callback for rx received
    USHELL_HAL_CB_RX_COMPLETED = 0x04,  ///< Callback for rx received
    USHELL_HAL_CB_ALL = 0xFF            ///< All callbacks
}UShellHalCallback_e;

typedef void(*UShellHalCb)(const void* const hal);

/**
 * \brief Descriibe UShellHal object
*/
typedef struct
{
    const void* parent;                         ///< Pointer to the parent object
    const char* name;                           ///< Pointer to the name of the object

    const UShellHalPortableTable_s* portTable;  ///< Port table

    /* Callback */
    UShellHalCb rxReceivedCb;                   ///< Callback for rx received event
    UShellHalCb txCompleteCb;                   ///< Callback for tx complete event
    UShellHalCb rxTxErrorCb;                    ///< Callback for rx tx error event

}UShellHal_s;


/*===========================================================[PUBLIC INTERFACE]=============================================*/

/**
 * \brief Initialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be initialized
 * \param[in] parent - pointer to the parent object (e.g. UShellHalStm32)
 * \param[in] name - pointer to the name of the object (e.g. "UShellHal")
 * \param[in] cbTable - pointer to the callback table to be used ()
 * \param[in] portTable Pointer to the port table
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success, otherwise error code
*/
UShellHalErr_e UShellHalInit(UShellHal_s* const hal, const void* const parent, const char* const name,
                                        const UShellHalPortableTable_s* const portTable);

/**
 * \brief Deinitialize the UShellHal object
 * \param[in] hal - pointer to the UShellHal object to be deinitialized
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success, otherwise error code
*/
UShellHalErr_e UShellHalDeinit(UShellHal_s* const hal);

/**
 * \brief Send data to the port
 * \param[in] hal - UShellHal object to set parent
 * \param[in] parent - parent to be set to the hal object
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalParentSet(UShellHal_s* const hal, const void* const parent);

/**
 * \brief Get parent from the UShellHal object
 * \param[in] hal - UShellHal object to get the parent
 * \param[out] parent - pointer to store the parent
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalParentGet(UShellHal_s* const hal, void** const parent);

/**
 * \brief Set the name of the UShellHal object
 * \param[in] hal - UShellHal object to set the name
 * \param[in] name - name to be set
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalNameSet(UShellHal_s* const hal, const char* const name);

/**
 * \brief Get the name of the UShellHal object
 * \param[in] hal - UShellHal object to get the name
 * \param[out] name - pointer to store the name
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalNameGet(UShellHal_s* const hal, const char** const name);

/**
 * \brief Attach callback to the UShellHal object
 * \param[in] hal - UShellHal object to attach the callback
 * \param[in] cbType - type of the callback to attach
 * \param[in] cb - callback to attach
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalCbAttach(UShellHal_s* const hal, const UShellHalCallback_e cbType, const UShellHalCb cb);

/**
 * \brief Detach callback from the UShellHal object
 * \param[in] hal - UShellHal object to detach the callback
 * \param[in] cbType - type of the callback to detach
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalCbDetach(UShellHal_s* const hal, const UShellHalCallback_e cbType);

/**
 * \brief Send data
 * \param[in] hal - UShellHal object to send data
 * \param[in] item - pointer to the data to be sent
 * \param[in] numberOfItems - number of items to be sent
 * \param[out] none
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalSend(UShellHal_s* const hal, const UShellHalItem_t* const item, const size_t numberOfItems);

/**
 * \brief Receive data
 * \param[in] hal - UShellHal object to receive data
 * \param[in] buff - pointer to the buffer to store the received data
 * \param[in] sizeOfBuff - size of the buffer
 * \param[out] numberOfItems - number of items to be received
 * \return UShellHalErr_e Error code. UShell_NO_ERR if success otherwise, error code
*/
UShellHalErr_e UShellHalReceive(UShellHal_s* const hal, UShellHalItem_t* const buff, const size_t sizeOfBuff, const size_t* const numberOfItems);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* USHELL_HAL_H_ */