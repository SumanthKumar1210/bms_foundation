/**
 * @file cell_monitor.c
 * @brief Acquisition and statistics of cell voltages and temperatures.
 */
#include "bms/cell_monitor.h"

#include "bms/hal.h"

static cell_monitor_data_t s_data;

static bool is_cell_mv_plausible(bms_millivolt_t mv)
{
    return (mv >= BMS_CELL_MV_PLAUSIBLE_MIN) && (mv <= BMS_CELL_MV_PLAUSIBLE_MAX);
}

static bool is_temp_plausible(bms_decidegc_t temp)
{
    return (temp >= BMS_TEMP_PLAUSIBLE_MIN) && (temp <= BMS_TEMP_PLAUSIBLE_MAX);
}

static void compute_voltage_stats(cell_monitor_data_t *data, const bms_millivolt_t *cell_mv)
{
    bms_pack_millivolt_t sum = 0U;
    uint8_t i;

    data->min_mv = cell_mv[0];
    data->max_mv = cell_mv[0];
    data->min_cell = 0U;
    data->max_cell = 0U;

    for (i = 0U; i < BMS_CELL_COUNT; i++)
    {
        data->cell_mv[i] = cell_mv[i];
        sum += cell_mv[i];
        if (cell_mv[i] < data->min_mv)
        {
            data->min_mv = cell_mv[i];
            data->min_cell = i;
        }
        if (cell_mv[i] > data->max_mv)
        {
            data->max_mv = cell_mv[i];
            data->max_cell = i;
        }
    }
    data->pack_mv = sum;
}

static void compute_temp_stats(cell_monitor_data_t *data, const bms_decidegc_t *temp)
{
    uint8_t i;

    data->min_temp = temp[0];
    data->max_temp = temp[0];
    for (i = 0U; i < BMS_TEMP_SENSOR_COUNT; i++)
    {
        data->temp[i] = temp[i];
        if (temp[i] < data->min_temp)
        {
            data->min_temp = temp[i];
        }
        if (temp[i] > data->max_temp)
        {
            data->max_temp = temp[i];
        }
    }
}

bms_status_t cell_monitor_init(void)
{
    uint8_t i;

    for (i = 0U; i < BMS_CELL_COUNT; i++)
    {
        s_data.cell_mv[i] = 0U;
    }
    for (i = 0U; i < BMS_TEMP_SENSOR_COUNT; i++)
    {
        s_data.temp[i] = 0;
    }
    s_data.min_mv = 0U;
    s_data.max_mv = 0U;
    s_data.min_cell = 0U;
    s_data.max_cell = 0U;
    s_data.pack_mv = 0U;
    s_data.min_temp = 0;
    s_data.max_temp = 0;
    s_data.sensor_fault = false;
    s_data.valid = false;
    return BMS_OK;
}

bms_status_t cell_monitor_update(void)
{
    bms_millivolt_t cell_mv[BMS_CELL_COUNT];
    bms_decidegc_t temp[BMS_TEMP_SENSOR_COUNT];
    bms_status_t result = BMS_OK;
    uint8_t i;

    for (i = 0U; (i < BMS_CELL_COUNT) && (result == BMS_OK); i++)
    {
        result = hal_read_cell_mv(i, &cell_mv[i]);
        if ((result == BMS_OK) && !is_cell_mv_plausible(cell_mv[i]))
        {
            result = BMS_E_RANGE;
        }
    }
    for (i = 0U; (i < BMS_TEMP_SENSOR_COUNT) && (result == BMS_OK); i++)
    {
        result = hal_read_temp(i, &temp[i]);
        if ((result == BMS_OK) && !is_temp_plausible(temp[i]))
        {
            result = BMS_E_RANGE;
        }
    }

    if (result == BMS_OK)
    {
        compute_voltage_stats(&s_data, cell_mv);
        compute_temp_stats(&s_data, temp);
        s_data.sensor_fault = false;
        s_data.valid = true;
    }
    else
    {
        s_data.sensor_fault = true; /* keep the last valid statistics */
    }
    return result;
}

const cell_monitor_data_t *cell_monitor_get(void)
{
    return &s_data;
}
