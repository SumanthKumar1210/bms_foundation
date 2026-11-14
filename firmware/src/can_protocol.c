/**
 * @file can_protocol.c
 * @brief Encoding and decoding of the BMS CAN frames.
 */
#include "bms/can_protocol.h"

#include "bms/bms_config.h"
#include "bms/crc8.h"

/** Offset added to whole degC so the temperature fits in one unsigned byte. */
#define TEMP_OFFSET_DEGC (40)

/** Bytes covered by the CRC (all but the last). */
#define CRC_LEN (CAN_PROTOCOL_DLC - 1U)

static void put_u16_le(uint8_t *dst, uint16_t value)
{
    dst[0] = (uint8_t)(value & 0xFFU);
    dst[1] = (uint8_t)((value >> 8U) & 0xFFU);
}

static uint16_t get_u16_le(const uint8_t *src)
{
    return (uint16_t)((uint16_t)src[0] | (uint16_t)((uint16_t)src[1] << 8U));
}

static uint8_t encode_temp(bms_decidegc_t temp)
{
    int32_t degc = (int32_t)temp / 10;
    int32_t raw = degc + TEMP_OFFSET_DEGC;

    if (raw < 0)
    {
        raw = 0;
    }
    if (raw > 255)
    {
        raw = 255;
    }
    return (uint8_t)raw;
}

static void clear_frame(can_frame_t *frame, uint32_t id)
{
    uint8_t i;

    frame->id = id;
    frame->dlc = CAN_PROTOCOL_DLC;
    for (i = 0U; i < CAN_PROTOCOL_DLC; i++)
    {
        frame->data[i] = 0U;
    }
}

bms_status_t can_protocol_encode_status(const cell_monitor_data_t *data, uint16_t fault_mask,
                                        contactor_state_t state, uint8_t counter, can_frame_t *frame)
{
    if ((data == NULL) || (frame == NULL))
    {
        return BMS_E_NULL_PTR;
    }
    if (!data->valid)
    {
        return BMS_E_NOT_READY;
    }

    clear_frame(frame, BMS_CAN_ID_STATUS);
    put_u16_le(&frame->data[0], data->min_mv);
    put_u16_le(&frame->data[2], data->max_mv);
    frame->data[4] = encode_temp(data->max_temp);
    frame->data[5] = (uint8_t)(fault_mask & 0xFFU);
    frame->data[6] = (uint8_t)(((uint8_t)state & 0x0FU) | (uint8_t)((counter & 0x0FU) << 4U));
    frame->data[7] = crc8_sae_j1850(frame->data, CRC_LEN);
    return BMS_OK;
}

bms_status_t can_protocol_decode_status(const can_frame_t *frame, can_status_t *status)
{
    uint8_t state;

    if ((frame == NULL) || (status == NULL))
    {
        return BMS_E_NULL_PTR;
    }
    if ((frame->id != BMS_CAN_ID_STATUS) || (frame->dlc != CAN_PROTOCOL_DLC) ||
        (crc8_sae_j1850(frame->data, CRC_LEN) != frame->data[7]))
    {
        return BMS_E_RANGE;
    }
    state = (uint8_t)(frame->data[6] & 0x0FU);
    if (state > (uint8_t)CONTACTOR_FAULT)
    {
        return BMS_E_RANGE;
    }

    status->min_mv = get_u16_le(&frame->data[0]);
    status->max_mv = get_u16_le(&frame->data[2]);
    status->max_temp_degc = (int16_t)((int16_t)frame->data[4] - TEMP_OFFSET_DEGC);
    status->fault_mask = frame->data[5];
    status->contactor_state = (contactor_state_t)state;
    status->alive_counter = (uint8_t)(frame->data[6] >> 4U);
    return BMS_OK;
}

bms_status_t can_protocol_encode_cells(const cell_monitor_data_t *data, uint8_t mux, can_frame_t *frame)
{
    uint8_t i;

    if ((data == NULL) || (frame == NULL))
    {
        return BMS_E_NULL_PTR;
    }
    if (mux >= CAN_PROTOCOL_CELL_FRAMES)
    {
        return BMS_E_RANGE;
    }
    if (!data->valid)
    {
        return BMS_E_NOT_READY;
    }

    clear_frame(frame, BMS_CAN_ID_CELLS);
    frame->data[0] = mux;
    for (i = 0U; i < CAN_PROTOCOL_CELLS_PER_FRAME; i++)
    {
        uint32_t cell = ((uint32_t)mux * CAN_PROTOCOL_CELLS_PER_FRAME) + i;
        uint16_t mv = (cell < BMS_CELL_COUNT) ? data->cell_mv[cell] : 0U;
        put_u16_le(&frame->data[1U + (2U * i)], mv);
    }
    frame->data[7] = crc8_sae_j1850(frame->data, CRC_LEN);
    return BMS_OK;
}
