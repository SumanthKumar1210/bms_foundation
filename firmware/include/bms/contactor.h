/**
 * @file contactor.h
 * @brief Contactor sequencing with precharge.
 *
 * Close sequence: main negative + precharge relay close; when the DC link
 * reaches BMS_PRECHARGE_DONE_PERCENT of the pack voltage, main positive closes
 * and the precharge relay opens. If that does not happen within
 * BMS_PRECHARGE_TIMEOUT_MS, everything opens and the state becomes FAULT.
 */
#ifndef BMS_CONTACTOR_H
#define BMS_CONTACTOR_H

#include "bms/bms_types.h"

/** Contactor state machine states. */
typedef enum
{
    CONTACTOR_OPEN = 0,  /**< All contactors open. */
    CONTACTOR_PRECHARGE, /**< Main negative + precharge closed, waiting for the link voltage. */
    CONTACTOR_CLOSED,    /**< Main negative + main positive closed. */
    CONTACTOR_FAULT      /**< Precharge failed or a HAL error; all open until contactor_reset(). */
} contactor_state_t;

/**
 * @brief Open all contactors and enter CONTACTOR_OPEN.
 * @return BMS_OK or the HAL error.
 */
bms_status_t contactor_init(void);

/**
 * @brief Request closing; starts precharge.
 * @param now Current time.
 * @return BMS_OK, BMS_E_STATE if not in CONTACTOR_OPEN, or the HAL error.
 */
bms_status_t contactor_request_close(bms_ms_t now);

/**
 * @brief Open all contactors immediately (allowed in every state except FAULT).
 * @return BMS_OK or the HAL error.
 */
bms_status_t contactor_request_open(void);

/**
 * @brief Advance the state machine; call every task period.
 * @param pack_mv Current pack voltage (sum of cells).
 * @param now Current time.
 * @return BMS_OK or the HAL error.
 */
bms_status_t contactor_update(bms_pack_millivolt_t pack_mv, bms_ms_t now);

/** @brief Current state. */
contactor_state_t contactor_get_state(void);

/**
 * @brief Leave CONTACTOR_FAULT (contactors stay open).
 * @return BMS_OK, or BMS_E_STATE if not in CONTACTOR_FAULT.
 */
bms_status_t contactor_reset(void);

#endif /* BMS_CONTACTOR_H */
