/**
 * @file bms_types.h
 * @brief Common types and status codes shared by all BMS modules.
 *
 * Physical quantities use fixed-point integer units throughout the firmware:
 * millivolts, deci-degrees Celsius (0.1 degC) and milliamps.
 */
#ifndef BMS_TYPES_H
#define BMS_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Result of a BMS function call. */
typedef enum
{
    BMS_OK = 0,        /**< Success. */
    BMS_E_NULL_PTR,    /**< A required pointer argument was NULL. */
    BMS_E_RANGE,       /**< An argument or measurement was out of range. */
    BMS_E_NOT_READY,   /**< Module not initialised or data not yet valid. */
    BMS_E_HW,          /**< The hardware abstraction layer reported an error. */
    BMS_E_STATE        /**< Request not allowed in the current state. */
} bms_status_t;

/** Voltage in millivolts. */
typedef uint16_t bms_millivolt_t;

/** Pack-level voltage in millivolts (sum of cells). */
typedef uint32_t bms_pack_millivolt_t;

/** Temperature in 0.1 degC, e.g. 253 = 25.3 degC. */
typedef int16_t bms_decidegc_t;

/** Current in milliamps; positive = discharge. */
typedef int32_t bms_milliamp_t;

/** Monotonic time in milliseconds. */
typedef uint32_t bms_ms_t;

#endif /* BMS_TYPES_H */
