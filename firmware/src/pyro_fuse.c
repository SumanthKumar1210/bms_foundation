/**
 * @file pyro_fuse.c
 * @brief One-shot driver for the pyrotechnic fuse.
 */
#include "bms/pyro_fuse.h"

#include "bms/hal.h"

static pyro_fuse_state_t s_state = PYRO_FUSE_ARMED;

bms_status_t pyro_fuse_fire(void)
{
    bms_status_t result;

    if (s_state != PYRO_FUSE_ARMED)
    {
        return BMS_E_STATE;
    }

    /* Consume the one-shot request before touching hardware; never retry. */
    s_state = PYRO_FUSE_FAULT;
    result = hal_fire_pyro_fuse();
    if (result == BMS_OK)
    {
        s_state = PYRO_FUSE_FIRED;
    }
    return result;
}

pyro_fuse_state_t pyro_fuse_get_state(void)
{
    return s_state;
}
