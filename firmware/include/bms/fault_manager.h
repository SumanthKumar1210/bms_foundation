/**
 * @file fault_manager.h
 * @brief Debounced, latching protection faults.
 *
 * A fault is set when its condition holds continuously for
 * BMS_FAULT_DEBOUNCE_MS. Once set it stays latched until
 * fault_manager_clear() is called while the condition is no longer present.
 */
#ifndef BMS_FAULT_MANAGER_H
#define BMS_FAULT_MANAGER_H

#include "bms/bms_types.h"
#include "bms/cell_monitor.h"

/** Protection faults; the value is the bit position in the fault mask. */
typedef enum
{
    FAULT_CELL_OV = 0, /**< A cell is above BMS_CELL_OV_MV. */
    FAULT_CELL_UV,     /**< A cell is below BMS_CELL_UV_MV. */
    FAULT_CELL_OT,     /**< A sensor is above BMS_CELL_OT_DDEGC. */
    FAULT_CELL_UT,     /**< A sensor is below BMS_CELL_UT_DDEGC. */
    FAULT_SENSOR,      /**< Measurements failed or were implausible. */
    FAULT_COUNT
} fault_id_t;

/**
 * @brief Clear all faults and debounce timers.
 * @return BMS_OK.
 */
bms_status_t fault_manager_init(void);

/**
 * @brief Evaluate every fault condition against the latest measurements.
 * @param data Latest cell monitor data.
 * @param now Current time.
 * @return BMS_OK, or BMS_E_NULL_PTR if @p data is NULL.
 */
bms_status_t fault_manager_update(const cell_monitor_data_t *data, bms_ms_t now);

/**
 * @brief Whether one fault is currently latched.
 * @param fault Fault to query; out-of-range values return false.
 */
bool fault_manager_is_active(fault_id_t fault);

/** @brief Bit mask of latched faults (bit n = fault_id_t n). */
uint16_t fault_manager_active_mask(void);

/**
 * @brief Clear latched faults whose condition is no longer present.
 * @return BMS_OK, or BMS_E_STATE if at least one fault could not be cleared.
 */
bms_status_t fault_manager_clear(void);

#endif /* BMS_FAULT_MANAGER_H */
