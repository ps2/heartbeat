
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "ble_heartbeat.h"
#include <string.h>
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"


static uint32_t heartbeat_value_char_add(ble_heartbeat_t * p_heartbeat, const ble_heartbeat_init_t * p_heartbeat_init);


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

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {HEARTBEAT_SERVICE_UUID_BASE};
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_heartbeat->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_heartbeat->uuid_type;
    ble_uuid.uuid = HEARTBEAT_SERVICE_UUID;

    // Add the Custom Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_heartbeat->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return heartbeat_value_char_add(p_heartbeat, p_heartbeat_init);
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
    //ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;


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

    attr_md.read_perm  = p_heartbeat_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_heartbeat_init->custom_value_char_attr_md.write_perm;
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

    err_code = sd_ble_gatts_characteristic_add(p_heartbeat->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_heartbeat->heartbeat_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}