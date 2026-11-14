/**
 * @file contactor.c
 * @brief Contactor sequencing with precharge.
 */
#include "bms/contactor.h"

#include "bms/bms_config.h"
#include "bms/hal.h"

static contactor_state_t s_state = CONTACTOR_OPEN;
static bms_ms_t s_precharge_start = 0U;

static bms_status_t open_all(void)
{
    bms_status_t result = hal_set_contactor(HAL_CONTACTOR_MAIN_POS, false);
    bms_status_t r;

    /* Try every contactor even if one fails; report the first error. */
    r = hal_set_contactor(HAL_CONTACTOR_PRECHARGE, false);
    if (result == BMS_OK)
    {
        result = r;
    }
    r = hal_set_contactor(HAL_CONTACTOR_MAIN_NEG, false);
    if (result == BMS_OK)
    {
        result = r;
    }
    return result;
}

static bool precharge_done(bms_pack_millivolt_t pack_mv)
{
    bms_pack_millivolt_t link_mv = 0U;

    if ((pack_mv == 0U) || (hal_read_link_mv(&link_mv) != BMS_OK))
    {
        return false;
    }
    /* link >= pack * 95 %, in 64-bit to avoid overflow. */
    return ((uint64_t)link_mv * 100U) >= ((uint64_t)pack_mv * BMS_PRECHARGE_DONE_PERCENT);
}

static bms_status_t enter_fault(void)
{
    s_state = CONTACTOR_FAULT;
    return open_all();
}

bms_status_t contactor_init(void)
{
    s_state = CONTACTOR_OPEN;
    s_precharge_start = 0U;
    return open_all();
}

bms_status_t contactor_request_close(bms_ms_t now)
{
    bms_status_t result;

    if (s_state != CONTACTOR_OPEN)
    {
        return BMS_E_STATE;
    }
    result = hal_set_contactor(HAL_CONTACTOR_MAIN_NEG, true);
    if (result == BMS_OK)
    {
        result = hal_set_contactor(HAL_CONTACTOR_PRECHARGE, true);
    }
    if (result != BMS_OK)
    {
        (void)enter_fault();
        return result;
    }
    s_state = CONTACTOR_PRECHARGE;
    s_precharge_start = now;
    return BMS_OK;
}

bms_status_t contactor_request_open(void)
{
    bms_status_t result;

    if (s_state == CONTACTOR_FAULT)
    {
        return open_all();
    }
    result = open_all();
    s_state = (result == BMS_OK) ? CONTACTOR_OPEN : CONTACTOR_FAULT;
    return result;
}

bms_status_t contactor_update(bms_pack_millivolt_t pack_mv, bms_ms_t now)
{
    bms_status_t result = BMS_OK;

    if (s_state != CONTACTOR_PRECHARGE)
    {
        return BMS_OK;
    }

    if (precharge_done(pack_mv))
    {
        result = hal_set_contactor(HAL_CONTACTOR_MAIN_POS, true);
        if (result == BMS_OK)
        {
            result = hal_set_contactor(HAL_CONTACTOR_PRECHARGE, false);
        }
        if (result == BMS_OK)
        {
            s_state = CONTACTOR_CLOSED;
        }
        else
        {
            (void)enter_fault();
        }
    }
    else if ((bms_ms_t)(now - s_precharge_start) >= BMS_PRECHARGE_TIMEOUT_MS)
    {
        result = enter_fault();
    }
    else
    {
        /* still precharging */
    }
    return result;
}

contactor_state_t contactor_get_state(void)
{
    return s_state;
}

bms_status_t contactor_reset(void)
{
    if (s_state != CONTACTOR_FAULT)
    {
        return BMS_E_STATE;
    }
    s_state = CONTACTOR_OPEN;
    return BMS_OK;
}
