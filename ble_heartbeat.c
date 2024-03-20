
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "ble_heartbeat.h"
#include <string.h>
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"


static uint32_t heartbeat_value_char_add(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init);
static uint32_t heartbeat_config_char_add(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init);


uint32_t ble_heartbeat_init(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init)
{
    if (p_heartbeat == NULL || p_heartbeat_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_heartbeat->conn_handle               = BLE_CONN_HANDLE_INVALID;

    p_heartbeat->config_handler = p_heartbeat_init->config_handler;

    // Add Heartbeat Service UUID
    ble_uuid128_t base_uuid = {HEARTBEAT_SERVICE_UUID_BASE};
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_heartbeat->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_heartbeat->uuid_type;
    ble_uuid.uuid = HEARTBEAT_SERVICE_UUID;

    // Add the Heartbeat Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_heartbeat->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = heartbeat_value_char_add(p_heartbeat, p_heartbeat_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return heartbeat_config_char_add(p_heartbeat, p_heartbeat_init);
}

/**@brief Function for adding the Heartbeat Value characteristic.
 *
 * @param[in]   p_heartbeat        Heartbeat Service structure.
 * @param[in]   p_heartbeat_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t heartbeat_value_char_add(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t             initial_value = 8;


    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL; 
    char_md.p_sccd_md         = NULL;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_heartbeat_init->heartbeat_value_char_attr_md.read_perm;
    attr_md.write_perm = p_heartbeat_init->heartbeat_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    ble_uuid.type = p_heartbeat->uuid_type;
    ble_uuid.uuid = HEARTBEAT_VALUE_CHAR_UUID;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);
    attr_char_value.p_value   = &initial_value;

    err_code = sd_ble_gatts_characteristic_add(p_heartbeat->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_heartbeat->heartbeat_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the Heartbeat Config characteristic.
 *
 * @param[in]   p_heartbeat        Heartbeat Service structure.
 * @param[in]   p_heartbeat_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t heartbeat_config_char_add(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    //ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint32_t            initial_config_value = 5 * 60; // 5 minutes


    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 0; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL; 
    char_md.p_sccd_md         = NULL;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_heartbeat_init->heartbeat_config_char_attr_md.read_perm;
    attr_md.write_perm = p_heartbeat_init->heartbeat_config_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    ble_uuid.type = p_heartbeat->uuid_type;
    ble_uuid.uuid = HEARTBEAT_CONFIG_CHAR_UUID;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint32_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint32_t);
    attr_char_value.p_value   = (unsigned char*)&initial_config_value;

    err_code = sd_ble_gatts_characteristic_add(p_heartbeat->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_heartbeat->heartbeat_config_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

static void on_connect(ble_heartbeat_t * p_heartbeat, ble_evt_t const * p_ble_evt);
static void on_disconnect(ble_heartbeat_t * p_heartbeat, ble_evt_t const * p_ble_evt);
static void on_write(ble_heartbeat_t * p_heartbeat, ble_evt_t const * p_ble_evt);


void ble_heartbeat_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_heartbeat_t * p_heartbeat = (ble_heartbeat_t *) p_context;
    
    if (p_heartbeat == NULL || p_ble_evt == NULL)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_heartbeat, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_heartbeat, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
           on_write(p_heartbeat, p_ble_evt);
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_heartbeat       Heartbeat Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_heartbeat_t * p_heartbeat, ble_evt_t const * p_ble_evt)
{
    p_heartbeat->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_heartbeat Heartbeat Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_heartbeat_t * p_heartbeat, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_heartbeat->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_heartbeat Heartbeat Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_heartbeat_t * p_heartbeat, ble_evt_t const * p_ble_evt)
{
    const ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    uint16_t new_config_value;

    // Check if the handle passed with the event matches the Custom Value Characteristic handle.
    if (p_evt_write->handle == p_heartbeat->heartbeat_config_handles.value_handle)
    {
        new_config_value = *(uint16_t*)(p_evt_write->data);
        NRF_LOG_INFO("Config updated to %d\r\n", new_config_value);
        p_heartbeat->config_handler(new_config_value);
    }
}

uint32_t ble_heartbeat_trigger(ble_heartbeat_t * p_heartbeat, uint32_t new_value)
{
    if (p_heartbeat == NULL)
    {
        return NRF_ERROR_NULL;
    }

    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = sizeof(uint32_t);
    gatts_value.offset  = 0;
    gatts_value.p_value = (unsigned char*) &new_value;

    // Update database.
    return sd_ble_gatts_value_set(p_heartbeat->conn_handle,
                                  p_heartbeat->heartbeat_value_handles.value_handle,
                                  &gatts_value);
}