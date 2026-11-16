/**
 * @file hal_stub.h
 * @brief Host-side HAL stub: tests set the simulated hardware state and inspect outputs.
 */
#ifndef BMS_HAL_STUB_H
#define BMS_HAL_STUB_H

#include "bms/bms_config.h"
#include "bms/hal.h"

/** Simulated hardware. Reset with hal_stub_reset(); modify fields directly in tests. */
typedef struct
{
    bms_millivolt_t cell_mv[BMS_CELL_COUNT];
    bms_decidegc_t temp[BMS_TEMP_SENSOR_COUNT];
    bms_pack_millivolt_t link_mv;
    bms_status_t read_result;               /**< Returned by every read function. */
    bms_status_t write_result;              /**< Returned by every output function. */
    uint16_t balancing_mask;                /**< Last mask set. */
    bool contactor[HAL_CONTACTOR_COUNT];    /**< Last coil state per contactor. */
    bool pyro_fuse_triggered;               /**< Whether a trigger was accepted. */
    bms_ms_t now_ms;                        /**< Value returned by hal_now_ms(). */
    uint32_t can_count;                     /**< Frames sent since reset. */
    uint32_t last_can_id;
    uint8_t last_can_data[8];
    uint8_t last_can_len;
} hal_stub_state_t;

/** The simulated hardware state. */
extern hal_stub_state_t hal_stub;

/** Reset to a healthy pack: all cells 3700 mV, 25.0 degC, link 0 V, all outputs off. */
void hal_stub_reset(void);

#endif /* BMS_HAL_STUB_H */
