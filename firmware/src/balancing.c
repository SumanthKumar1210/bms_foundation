/**
 * @file balancing.c
 * @brief Passive cell balancing.
 */
#include "bms/balancing.h"

#include "bms/bms_config.h"
#include "bms/hal.h"

static uint16_t s_active_mask = 0U;

uint16_t balancing_compute_mask(const cell_monitor_data_t *data, bool faults_active)
{
    uint16_t mask = 0U;
    uint8_t i;

    if ((data == NULL) || !data->valid || data->sensor_fault || faults_active)
    {
        return 0U;
    }

    for (i = 0U; i < BMS_CELL_COUNT; i++)
    {
        bms_millivolt_t mv = data->cell_mv[i];
        if ((mv >= BMS_BALANCE_MIN_MV) && ((uint32_t)mv > ((uint32_t)data->min_mv + BMS_BALANCE_DELTA_MV)))
        {
            mask |= (uint16_t)(1U << i);
        }
    }
    return mask;
}

bms_status_t balancing_update(const cell_monitor_data_t *data, bool faults_active)
{
    bms_status_t result;
    uint16_t mask;

    if (data == NULL)
    {
        return BMS_E_NULL_PTR;
    }

    mask = balancing_compute_mask(data, faults_active);
    result = hal_set_balancing(mask);
    if (result == BMS_OK)
    {
        s_active_mask = mask;
    }
    return result;
}

uint16_t balancing_active_mask(void)
{
    return s_active_mask;
}
