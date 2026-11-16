/**
 * @file pyro_fuse.h
 * @brief One-shot driver for the pyrotechnic fuse.
 */
#ifndef BMS_PYRO_FUSE_H
#define BMS_PYRO_FUSE_H

#include "bms/bms_types.h"

/** Pyro fuse driver state. */
typedef enum
{
    PYRO_FUSE_ARMED = 0, /**< A trigger request may be made. */
    PYRO_FUSE_FIRED,     /**< The HAL accepted the trigger request. */
    PYRO_FUSE_FAULT      /**< A trigger was attempted but the HAL reported an error. */
} pyro_fuse_state_t;

/**
 * @brief Attempt to trigger the pyro fuse once.
 *
 * The driver locks out further attempts before calling the HAL. This remains
 * true if the HAL reports an error, because the physical action may have
 * occurred despite that error.
 * @return BMS_OK, BMS_E_STATE if already attempted, or the HAL error.
 */
bms_status_t pyro_fuse_fire(void);

/** @brief Current driver state. */
pyro_fuse_state_t pyro_fuse_get_state(void);

#endif /* BMS_PYRO_FUSE_H */
