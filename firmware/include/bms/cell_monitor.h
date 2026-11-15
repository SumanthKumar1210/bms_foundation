/**
 * @file cell_monitor.h
 * @brief Acquisition and statistics of cell voltages and temperatures.
 *
 * cell_monitor_update() reads every cell and sensor through the HAL, checks
 * each value for plausibility and computes min/max/sum. Other modules read
 * the latest result with cell_monitor_get().
 */
#ifndef BMS_CELL_MONITOR_H
#define BMS_CELL_MONITOR_H

#include "bms/bms_config.h"
#include "bms/bms_types.h"

/** Latest measurements and derived statistics. */
typedef struct
{
    bms_millivolt_t cell_mv[BMS_CELL_COUNT];      /**< Per-cell voltage. */
    bms_decidegc_t temp[BMS_TEMP_SENSOR_COUNT];  /**< Per-sensor temperature. */
    bms_millivolt_t min_mv;                       /**< Lowest cell voltage. */
    bms_millivolt_t max_mv;                       /**< Highest cell voltage. */
    uint8_t min_cell;                             /**< Index of the lowest cell. */
    uint8_t max_cell;                             /**< Index of the highest cell. */
    bms_pack_millivolt_t pack_mv;                 /**< Sum of all cell voltages. */
    bms_decidegc_t min_temp;                      /**< Lowest temperature. */
    bms_decidegc_t max_temp;                      /**< Highest temperature. */
    bool sensor_fault;                            /**< A reading failed or was implausible. */
    bool valid;                                   /**< At least one complete update succeeded. */
} cell_monitor_data_t;

/**
 * @brief Reset all measurements; data is invalid until the first update.
 * @return BMS_OK.
 */
bms_status_t cell_monitor_init(void);

/**
 * @brief Read all cells and sensors and recompute the statistics.
 *
 * On a HAL error or an implausible value, sensor_fault is set and the
 * previous valid statistics are kept.
 *
 * @return BMS_OK, or BMS_E_HW / BMS_E_RANGE if any reading failed.
 */
bms_status_t cell_monitor_update(void);

/**
 * @brief Latest measurements.
 * @return Pointer to module-owned data; never NULL. Check ->valid.
 */
const cell_monitor_data_t *cell_monitor_get(void);

#endif /* BMS_CELL_MONITOR_H */
