#ifndef BLE_HEARTBEAT_H
#define BLE_HEARTBEAT_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"


#define HEARTBEAT_SERVICE_UUID_BASE {0x01, 0x02, 0x03, 0x9c, 0x21, 0x69, 0x56, 0xb8, \
                                     0x97, 0x41, 0xc5, 0x99, 0x3b, 0x73, 0x35, 0x02} 


#define HEARTBEAT_SERVICE_UUID               0x1400
#define HEARTBEAT_VALUE_CHAR_UUID            0x1401
#define HEARTBEAT_CONFIG_CHAR_UUID            0x1402

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note 
 *
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 * @param[in]   p_context  Heartbeat Service structure.
 */
void ble_heartbeat_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

typedef void (*ble_heartbeat_config_handler_t) (uint16_t config_value);


/**@brief Heartbeat Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    uint32_t                       initial_heartbeat_value;          /**< Initial heartbeat value */
    uint16_t                       initial_config_value;             /**< Initial config value */
    ble_srv_cccd_security_mode_t   heartbeat_value_char_attr_md;     /**< Initial security level for Heartbeat value characteristics attribute */
    ble_srv_cccd_security_mode_t   heartbeat_config_char_attr_md;    /**< Initial security level for Heartbeat config characteristics attribute */
    ble_heartbeat_config_handler_t config_handler;
} ble_heartbeat_init_t;

/**@brief Heartbeat Service structure. This contains various status information for the service. */
struct ble_heartbeat_s
{
    uint16_t                       service_handle;                 /**< Handle of Heartbeat Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t       heartbeat_value_handles;        /**< Handles related to the Heartbeat Value characteristic. */
    ble_gatts_char_handles_t       heartbeat_config_handles;        /**< Handles related to the Heartbeat Config characteristic. */
    uint16_t                       conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                        uuid_type;
    ble_heartbeat_config_handler_t config_handler;
};

// Forward declaration of the ble_heartbeat_t type.
typedef struct ble_heartbeat_s ble_heartbeat_t;

/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_cus       Custom Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_heartbeat_init(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init);

void ble_heartbeat_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

/**@brief   Macro for defining a ble_heartbeat instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_HEARTBEAT_DEF(_name)                                                                    \
static ble_heartbeat_t _name;

uint32_t ble_heartbeat_trigger(ble_heartbeat_t * p_heartbeat, uint32_t new_value);


#endif