/**
 * @file hal.h
 * @brief Hardware abstraction layer used by the BMS application.
 *
 * The target platform provides the implementation (ADC front end, GPIO, CAN
 * driver, system timer). Host builds and unit tests link a stub instead
 * (test/hal_stub.c), so all application modules run on a PC.
 */
#ifndef BMS_HAL_H
#define BMS_HAL_H

#include "bms/bms_types.h"

/** Contactors driven by the BMS. */
typedef enum
{
    HAL_CONTACTOR_MAIN_NEG = 0, /**< Main negative contactor. */
    HAL_CONTACTOR_PRECHARGE,    /**< Precharge relay (in series with the precharge resistor). */
    HAL_CONTACTOR_MAIN_POS,     /**< Main positive contactor. */
    HAL_CONTACTOR_COUNT
} hal_contactor_t;

/**
 * @brief Read one cell voltage.
 * @param cell Cell index, 0 .. BMS_CELL_COUNT-1.
 * @param[out] mv Measured voltage.
 * @return BMS_OK, BMS_E_NULL_PTR, BMS_E_RANGE or BMS_E_HW.
 */
bms_status_t hal_read_cell_mv(uint8_t cell, bms_millivolt_t *mv);

/**
 * @brief Read one temperature sensor.
 * @param sensor Sensor index, 0 .. BMS_TEMP_SENSOR_COUNT-1.
 * @param[out] temp Measured temperature.
 * @return BMS_OK, BMS_E_NULL_PTR, BMS_E_RANGE or BMS_E_HW.
 */
bms_status_t hal_read_temp(uint8_t sensor, bms_decidegc_t *temp);

/**
 * @brief Read the DC link voltage (inverter side of the contactors).
 * @param[out] mv Measured voltage.
 * @return BMS_OK, BMS_E_NULL_PTR or BMS_E_HW.
 */
bms_status_t hal_read_link_mv(bms_pack_millivolt_t *mv);

/**
 * @brief Enable the passive balancing resistors of the cells in @p mask.
 * @param mask Bit n set = balance cell n. Bits >= BMS_CELL_COUNT must be 0.
 * @return BMS_OK, BMS_E_RANGE or BMS_E_HW.
 */
bms_status_t hal_set_balancing(uint16_t mask);

/**
 * @brief Drive a contactor coil.
 * @param contactor Which contactor.
 * @param closed true = energise (close), false = open.
 * @return BMS_OK, BMS_E_RANGE or BMS_E_HW.
 */
bms_status_t hal_set_contactor(hal_contactor_t contactor, bool closed);

/**
 * @brief Issue the target-specific trigger pulse for the Pyro Fuse.
 * @return BMS_OK or BMS_E_HW.
 */
bms_status_t hal_fire_pyro_fuse(void);

/**
 * @brief Queue a classic CAN frame for transmission.
 * @param id 11-bit identifier.
 * @param data Payload.
 * @param len Payload length, 0..8.
 * @return BMS_OK, BMS_E_NULL_PTR, BMS_E_RANGE or BMS_E_HW (TX queue full).
 */
bms_status_t hal_can_send(uint32_t id, const uint8_t *data, uint8_t len);

/** @brief Milliseconds since start-up (wraps after ~49 days). */
bms_ms_t hal_now_ms(void);

#endif /* BMS_HAL_H */
