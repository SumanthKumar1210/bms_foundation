/**
 * @file hal_stub.c
 * @brief Host-side HAL stub implementation (see hal_stub.h).
 */
#include "hal_stub.h"

#include <string.h>

hal_stub_state_t hal_stub;

void hal_stub_reset(void)
{
    uint8_t i;

    (void)memset(&hal_stub, 0, sizeof(hal_stub));
    for (i = 0U; i < BMS_CELL_COUNT; i++)
    {
        hal_stub.cell_mv[i] = 3700U;
    }
    for (i = 0U; i < BMS_TEMP_SENSOR_COUNT; i++)
    {
        hal_stub.temp[i] = 250;
    }
    hal_stub.read_result = BMS_OK;
    hal_stub.write_result = BMS_OK;
}

bms_status_t hal_read_cell_mv(uint8_t cell, bms_millivolt_t *mv)
{
    if (mv == NULL)
    {
        return BMS_E_NULL_PTR;
    }
    if (cell >= BMS_CELL_COUNT)
    {
        return BMS_E_RANGE;
    }
    *mv = hal_stub.cell_mv[cell];
    return hal_stub.read_result;
}

bms_status_t hal_read_temp(uint8_t sensor, bms_decidegc_t *temp)
{
    if (temp == NULL)
    {
        return BMS_E_NULL_PTR;
    }
    if (sensor >= BMS_TEMP_SENSOR_COUNT)
    {
        return BMS_E_RANGE;
    }
    *temp = hal_stub.temp[sensor];
    return hal_stub.read_result;
}

bms_status_t hal_read_link_mv(bms_pack_millivolt_t *mv)
{
    if (mv == NULL)
    {
        return BMS_E_NULL_PTR;
    }
    *mv = hal_stub.link_mv;
    return hal_stub.read_result;
}

bms_status_t hal_set_balancing(uint16_t mask)
{
    if ((mask >> BMS_CELL_COUNT) != 0U)
    {
        return BMS_E_RANGE;
    }
    if (hal_stub.write_result == BMS_OK)
    {
        hal_stub.balancing_mask = mask;
    }
    return hal_stub.write_result;
}

bms_status_t hal_set_contactor(hal_contactor_t contactor, bool closed)
{
    if ((uint32_t)contactor >= (uint32_t)HAL_CONTACTOR_COUNT)
    {
        return BMS_E_RANGE;
    }
    if (hal_stub.write_result == BMS_OK)
    {
        hal_stub.contactor[contactor] = closed;
    }
    return hal_stub.write_result;
}

bms_status_t hal_can_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    if (data == NULL)
    {
        return BMS_E_NULL_PTR;
    }
    if (len > 8U)
    {
        return BMS_E_RANGE;
    }
    if (hal_stub.write_result == BMS_OK)
    {
        hal_stub.can_count++;
        hal_stub.last_can_id = id;
        hal_stub.last_can_len = len;
        (void)memcpy(hal_stub.last_can_data, data, len);
    }
    return hal_stub.write_result;
}

bms_ms_t hal_now_ms(void)
{
    return hal_stub.now_ms;
}
