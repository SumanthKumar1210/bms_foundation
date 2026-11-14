/**
 * @file bms_app.h
 * @brief BMS application: initialisation and the periodic main task.
 */
#ifndef BMS_APP_H
#define BMS_APP_H

#include "bms/bms_types.h"

/**
 * @brief Initialise all BMS modules; contactors open, no balancing.
 * @return BMS_OK or the first module error.
 */
bms_status_t bms_app_init(void);

/**
 * @brief Main task, called every BMS_TASK_PERIOD_MS.
 *
 * Order: measure -> evaluate faults -> open contactors on fault -> advance
 * contactor sequence -> balance -> send CAN frames every BMS_CAN_PERIOD_MS.
 *
 * @return BMS_OK, or the first error of this cycle (the cycle still completes).
 */
bms_status_t bms_app_step(void);

/**
 * @brief Request the contactors to close (e.g. from the vehicle controller).
 * @return BMS_OK, BMS_E_STATE if a fault is active or not open, or the HAL error.
 */
bms_status_t bms_app_request_close(void);

#endif /* BMS_APP_H */
