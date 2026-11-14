/**
 * @file bms_config.h
 * @brief Compile-time configuration of the battery pack and protection limits.
 *
 * All limits are for a 12s lithium-ion (NMC) pack. Change them here only;
 * modules must not hard-code thresholds.
 */
#ifndef BMS_CONFIG_H
#define BMS_CONFIG_H

/* ----------------------------------------------------------- pack layout */

/** Number of series cells monitored. */
#define BMS_CELL_COUNT (12U)

/** Number of cell temperature sensors. */
#define BMS_TEMP_SENSOR_COUNT (4U)

/* ------------------------------------------------- measurement plausibility */

/** Lowest cell voltage reading considered a valid measurement (mV). */
#define BMS_CELL_MV_PLAUSIBLE_MIN (500U)

/** Highest cell voltage reading considered a valid measurement (mV). */
#define BMS_CELL_MV_PLAUSIBLE_MAX (5000U)

/** Lowest temperature reading considered valid (0.1 degC). */
#define BMS_TEMP_PLAUSIBLE_MIN (-400)

/** Highest temperature reading considered valid (0.1 degC). */
#define BMS_TEMP_PLAUSIBLE_MAX (1250)

/* ---------------------------------------------------------- protection */

/** Cell overvoltage limit (mV). */
#define BMS_CELL_OV_MV (4200U)

/** Cell undervoltage limit (mV). */
#define BMS_CELL_UV_MV (2800U)

/** Cell overtemperature limit (0.1 degC). */
#define BMS_CELL_OT_DDEGC (600)

/** Cell undertemperature limit (0.1 degC). */
#define BMS_CELL_UT_DDEGC (-200)

/** A fault condition must persist this long before the fault is set (ms). */
#define BMS_FAULT_DEBOUNCE_MS (100U)

/* ----------------------------------------------------------- balancing */

/** Cells this far above the lowest cell are balanced (mV). */
#define BMS_BALANCE_DELTA_MV (20U)

/** No balancing below this cell voltage (mV). */
#define BMS_BALANCE_MIN_MV (3600U)

/* ----------------------------------------------------------- contactors */

/** Precharge is complete when the link voltage reaches this percentage of the pack. */
#define BMS_PRECHARGE_DONE_PERCENT (95U)

/** Precharge must complete within this time (ms). */
#define BMS_PRECHARGE_TIMEOUT_MS (500U)

/* ----------------------------------------------------------------- CAN */

/** CAN ID of the BMS status frame (11-bit). */
#define BMS_CAN_ID_STATUS (0x3A0U)

/** CAN ID of the multiplexed cell voltage frame (11-bit). */
#define BMS_CAN_ID_CELLS (0x3A1U)

/** Period of all BMS CAN frames (ms). */
#define BMS_CAN_PERIOD_MS (100U)

/** Main task period (ms). */
#define BMS_TASK_PERIOD_MS (10U)

#endif /* BMS_CONFIG_H */
