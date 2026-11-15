/**
 * @file balancing.h
 * @brief Passive cell balancing.
 *
 * Cells more than BMS_BALANCE_DELTA_MV above the lowest cell, and above
 * BMS_BALANCE_MIN_MV, have their bleed resistor enabled. Balancing is
 * disabled while any protection fault is active or the data is invalid.
 */
#ifndef BMS_BALANCING_H
#define BMS_BALANCING_H

#include "bms/bms_types.h"
#include "bms/cell_monitor.h"

/**
 * @brief Compute which cells to balance (pure function, no hardware access).
 * @param data Latest cell monitor data.
 * @param faults_active true if any protection fault is latched.
 * @return Bit mask, bit n = balance cell n; 0 if @p data is NULL or invalid.
 */
uint16_t balancing_compute_mask(const cell_monitor_data_t *data, bool faults_active);

/**
 * @brief Compute the balancing mask and apply it to the hardware.
 * @param data Latest cell monitor data.
 * @param faults_active true if any protection fault is latched.
 * @return BMS_OK, BMS_E_NULL_PTR, or the HAL error.
 */
bms_status_t balancing_update(const cell_monitor_data_t *data, bool faults_active);

/** @brief Mask applied by the last successful balancing_update(). */
uint16_t balancing_active_mask(void);

#endif /* BMS_BALANCING_H */
