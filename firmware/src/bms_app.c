/**
 * @file bms_app.c
 * @brief BMS application: initialisation and the periodic main task.
 */
#include "bms/bms_app.h"

#include "bms/balancing.h"
#include "bms/bms_config.h"
#include "bms/can_protocol.h"
#include "bms/cell_monitor.h"
#include "bms/contactor.h"
#include "bms/fault_manager.h"
#include "bms/hal.h"

static bms_ms_t s_last_can_ms = 0U;
static uint8_t s_alive_counter = 0U;
static uint8_t s_cell_mux = 0U;

static void keep_first_error(bms_status_t *first, bms_status_t result)
{
    if ((*first == BMS_OK) && (result != BMS_OK))
    {
        *first = result;
    }
}

static bms_status_t send_frame(const can_frame_t *frame)
{
    return hal_can_send(frame->id, frame->data, frame->dlc);
}

static bms_status_t send_can(const cell_monitor_data_t *data)
{
    can_frame_t frame;
    bms_status_t result;

    result = can_protocol_encode_status(data, fault_manager_active_mask(), contactor_get_state(),
                                        s_alive_counter, &frame);
    if (result == BMS_OK)
    {
        result = send_frame(&frame);
    }
    s_alive_counter = (uint8_t)((s_alive_counter + 1U) & 0x0FU);

    if (result == BMS_OK)
    {
        result = can_protocol_encode_cells(data, s_cell_mux, &frame);
        if (result == BMS_OK)
        {
            result = send_frame(&frame);
        }
        s_cell_mux = (uint8_t)((s_cell_mux + 1U) % CAN_PROTOCOL_CELL_FRAMES);
    }
    return result;
}

bms_status_t bms_app_init(void)
{
    bms_status_t result = BMS_OK;

    keep_first_error(&result, cell_monitor_init());
    keep_first_error(&result, fault_manager_init());
    keep_first_error(&result, contactor_init());
    keep_first_error(&result, hal_set_balancing(0U));
    s_last_can_ms = hal_now_ms();
    s_alive_counter = 0U;
    s_cell_mux = 0U;
    return result;
}

bms_status_t bms_app_step(void)
{
    bms_status_t result = BMS_OK;
    bms_ms_t now = hal_now_ms();
    const cell_monitor_data_t *data;
    bool faults_active;

    keep_first_error(&result, cell_monitor_update());
    data = cell_monitor_get();

    keep_first_error(&result, fault_manager_update(data, now));
    faults_active = fault_manager_active_mask() != 0U;

    if (faults_active && (contactor_get_state() != CONTACTOR_OPEN) && (contactor_get_state() != CONTACTOR_FAULT))
    {
        keep_first_error(&result, contactor_request_open());
    }
    keep_first_error(&result, contactor_update(data->pack_mv, now));
    keep_first_error(&result, balancing_update(data, faults_active));

    if ((bms_ms_t)(now - s_last_can_ms) >= BMS_CAN_PERIOD_MS)
    {
        s_last_can_ms = now;
        if (data->valid)
        {
            keep_first_error(&result, send_can(data));
        }
    }
    return result;
}

bms_status_t bms_app_request_close(void)
{
    if (fault_manager_active_mask() != 0U)
    {
        return BMS_E_STATE;
    }
    return contactor_request_close(hal_now_ms());
}
