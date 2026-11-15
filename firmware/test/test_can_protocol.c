/**
 * @file test_can_protocol.c
 * @brief Unit tests for can_protocol and crc8.
 */
#include "bms/can_protocol.h"

#include "bms/bms_config.h"
#include "bms/crc8.h"
#include "unit_test.h"

static cell_monitor_data_t sample(void)
{
    cell_monitor_data_t d = {0};
    uint8_t i;

    for (i = 0U; i < BMS_CELL_COUNT; i++)
    {
        d.cell_mv[i] = (bms_millivolt_t)(3600U + i);
    }
    d.min_mv = 3600U;
    d.max_mv = (bms_millivolt_t)(3600U + BMS_CELL_COUNT - 1U);
    d.max_temp = 253; /* 25.3 degC -> 25 degC */
    d.valid = true;
    return d;
}

static void test_crc8_check_value(void)
{
    const uint8_t check[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

    UT_ASSERT_EQ(0x4B, crc8_sae_j1850(check, (uint8_t)sizeof(check)));
    UT_ASSERT_EQ(0x00, crc8_sae_j1850(NULL, 1U));
}

static void test_status_round_trip(void)
{
    cell_monitor_data_t d = sample();
    can_frame_t frame;
    can_status_t status;

    UT_ASSERT_EQ(BMS_OK, can_protocol_encode_status(&d, 0x0105U, CONTACTOR_CLOSED, 0x1AU, &frame));
    UT_ASSERT_EQ(BMS_CAN_ID_STATUS, frame.id);
    UT_ASSERT_EQ(8, frame.dlc);
    UT_ASSERT_EQ(65, frame.data[4]); /* 25 + 40 */

    UT_ASSERT_EQ(BMS_OK, can_protocol_decode_status(&frame, &status));
    UT_ASSERT_EQ(3600, status.min_mv);
    UT_ASSERT_EQ(3611, status.max_mv);
    UT_ASSERT_EQ(25, status.max_temp_degc);
    UT_ASSERT_EQ(0x05, status.fault_mask);
    UT_ASSERT_EQ(CONTACTOR_CLOSED, status.contactor_state);
    UT_ASSERT_EQ(0x0A, status.alive_counter);
}

static void test_decode_rejects_corrupted_frame(void)
{
    cell_monitor_data_t d = sample();
    can_frame_t frame;
    can_status_t status;

    (void)can_protocol_encode_status(&d, 0U, CONTACTOR_OPEN, 0U, &frame);
    frame.data[0] ^= 0x01U;
    UT_ASSERT_EQ(BMS_E_RANGE, can_protocol_decode_status(&frame, &status));
}

static void test_encode_requires_valid_data(void)
{
    cell_monitor_data_t d = sample();
    can_frame_t frame;

    d.valid = false;
    UT_ASSERT_EQ(BMS_E_NOT_READY, can_protocol_encode_status(&d, 0U, CONTACTOR_OPEN, 0U, &frame));
    UT_ASSERT_EQ(BMS_E_NULL_PTR, can_protocol_encode_status(NULL, 0U, CONTACTOR_OPEN, 0U, &frame));
}

static void test_cell_frames(void)
{
    cell_monitor_data_t d = sample();
    can_frame_t frame;

    UT_ASSERT_EQ(BMS_OK, can_protocol_encode_cells(&d, 1U, &frame));
    UT_ASSERT_EQ(BMS_CAN_ID_CELLS, frame.id);
    UT_ASSERT_EQ(1, frame.data[0]);
    UT_ASSERT_EQ(3603, (uint16_t)(frame.data[1] | (frame.data[2] << 8)));
    UT_ASSERT_EQ(3605, (uint16_t)(frame.data[5] | (frame.data[6] << 8)));
    UT_ASSERT_EQ(crc8_sae_j1850(frame.data, 7U), frame.data[7]);
    UT_ASSERT_EQ(BMS_E_RANGE, can_protocol_encode_cells(&d, CAN_PROTOCOL_CELL_FRAMES, &frame));
}

int main(void)
{
    UT_RUN(test_crc8_check_value);
    UT_RUN(test_status_round_trip);
    UT_RUN(test_decode_rejects_corrupted_frame);
    UT_RUN(test_encode_requires_valid_data);
    UT_RUN(test_cell_frames);
    return UT_REPORT();
}
