/**
 * @file fault_manager.c
 * @brief Debounced, latching protection faults.
 */
#include "bms/fault_manager.h"

#include "bms/bms_config.h"

/** Debounce state of one fault. */
typedef struct
{
    bool condition;      /**< Condition present in the latest update. */
    bool pending;        /**< Condition present, waiting for the debounce time. */
    bms_ms_t since;      /**< Time the condition was first seen. */
    bool latched;        /**< Fault set. */
} fault_state_t;

static fault_state_t s_faults[FAULT_COUNT];

static void evaluate(fault_state_t *fault, bool condition, bms_ms_t now)
{
    fault->condition = condition;
    if (!condition)
    {
        fault->pending = false;
        return;
    }
    if (!fault->pending)
    {
        fault->pending = true;
        fault->since = now;
    }
    /* Unsigned subtraction handles timer wrap-around. */
    if ((bms_ms_t)(now - fault->since) >= BMS_FAULT_DEBOUNCE_MS)
    {
        fault->latched = true;
    }
}

bms_status_t fault_manager_init(void)
{
    uint8_t i;

    for (i = 0U; i < (uint8_t)FAULT_COUNT; i++)
    {
        s_faults[i].condition = false;
        s_faults[i].pending = false;
        s_faults[i].since = 0U;
        s_faults[i].latched = false;
    }
    return BMS_OK;
}

bms_status_t fault_manager_update(const cell_monitor_data_t *data, bms_ms_t now)
{
    bool valid;

    if (data == NULL)
    {
        return BMS_E_NULL_PTR;
    }

    valid = data->valid && !data->sensor_fault;
    evaluate(&s_faults[FAULT_SENSOR], !valid, now);
    evaluate(&s_faults[FAULT_CELL_OV], valid && (data->max_mv > BMS_CELL_OV_MV), now);
    evaluate(&s_faults[FAULT_CELL_UV], valid && (data->min_mv < BMS_CELL_UV_MV), now);
    evaluate(&s_faults[FAULT_CELL_OT], valid && (data->max_temp > BMS_CELL_OT_DDEGC), now);
    evaluate(&s_faults[FAULT_CELL_UT], valid && (data->min_temp < BMS_CELL_UT_DDEGC), now);
    return BMS_OK;
}

bool fault_manager_is_active(fault_id_t fault)
{
    if ((uint32_t)fault >= (uint32_t)FAULT_COUNT)
    {
        return false;
    }
    return s_faults[fault].latched;
}

uint16_t fault_manager_active_mask(void)
{
    uint16_t mask = 0U;
    uint8_t i;

    for (i = 0U; i < (uint8_t)FAULT_COUNT; i++)
    {
        if (s_faults[i].latched)
        {
            mask |= (uint16_t)(1U << i);
        }
    }
    return mask;
}

bms_status_t fault_manager_clear(void)
{
    bms_status_t result = BMS_OK;
    uint8_t i;

    for (i = 0U; i < (uint8_t)FAULT_COUNT; i++)
    {
        if (s_faults[i].latched)
        {
            if (s_faults[i].condition)
            {
                result = BMS_E_STATE;
            }
            else
            {
                s_faults[i].latched = false;
            }
        }
    }
    return result;
}
