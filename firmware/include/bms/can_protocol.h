/**
 * @file can_protocol.h
 * @brief Encoding and decoding of the BMS CAN frames.
 *
 * Status frame (ID BMS_CAN_ID_STATUS, 8 bytes, little-endian):
 *
 * | Byte | Content                                              |
 * |------|------------------------------------------------------|
 * | 0-1  | lowest cell voltage, mV                              |
 * | 2-3  | highest cell voltage, mV                             |
 * | 4    | highest temperature, degC + 40 (0 = -40 degC)        |
 * | 5    | fault mask, bits 0-7 (see fault_id_t)                |
 * | 6    | bits 0-3: contactor_state_t, bits 4-7: alive counter |
 * | 7    | CRC-8 SAE J1850 over bytes 0-6                       |
 *
 * Cell voltage frame (ID BMS_CAN_ID_CELLS, 8 bytes), multiplexed:
 * byte 0 = mux index m, bytes 1-6 = cells 3m..3m+2 (mV, little-endian),
 * byte 7 = CRC over bytes 0-6. Cells past BMS_CELL_COUNT are sent as 0.
 */
#ifndef BMS_CAN_PROTOCOL_H
#define BMS_CAN_PROTOCOL_H

#include "bms/bms_types.h"
#include "bms/cell_monitor.h"
#include "bms/contactor.h"

/** Payload length of every BMS frame. */
#define CAN_PROTOCOL_DLC (8U)

/** Cells per multiplexed cell voltage frame. */
#define CAN_PROTOCOL_CELLS_PER_FRAME (3U)

/** Number of multiplexed cell voltage frames needed for all cells. */
#define CAN_PROTOCOL_CELL_FRAMES ((BMS_CELL_COUNT + CAN_PROTOCOL_CELLS_PER_FRAME - 1U) / CAN_PROTOCOL_CELLS_PER_FRAME)

/** One classic CAN frame. */
typedef struct
{
    uint32_t id;                     /**< 11-bit identifier. */
    uint8_t dlc;                     /**< Payload length. */
    uint8_t data[CAN_PROTOCOL_DLC];  /**< Payload. */
} can_frame_t;

/** Decoded content of a status frame. */
typedef struct
{
    bms_millivolt_t min_mv;            /**< Lowest cell voltage. */
    bms_millivolt_t max_mv;            /**< Highest cell voltage. */
    int16_t max_temp_degc;             /**< Highest temperature, whole degC. */
    uint8_t fault_mask;                /**< Fault bits 0-7. */
    contactor_state_t contactor_state; /**< Contactor state. */
    uint8_t alive_counter;             /**< 0..15. */
} can_status_t;

/**
 * @brief Build the status frame.
 * @param data Latest cell monitor data.
 * @param fault_mask Active fault mask (only bits 0-7 are sent).
 * @param state Contactor state.
 * @param counter Alive counter; only the low 4 bits are sent.
 * @param[out] frame Encoded frame.
 * @return BMS_OK, BMS_E_NULL_PTR, or BMS_E_NOT_READY if @p data is invalid.
 */
bms_status_t can_protocol_encode_status(const cell_monitor_data_t *data, uint16_t fault_mask,
                                        contactor_state_t state, uint8_t counter, can_frame_t *frame);

/**
 * @brief Decode and check a status frame.
 * @param frame Received frame.
 * @param[out] status Decoded content.
 * @return BMS_OK, BMS_E_NULL_PTR, or BMS_E_RANGE on wrong ID/DLC/CRC.
 */
bms_status_t can_protocol_decode_status(const can_frame_t *frame, can_status_t *status);

/**
 * @brief Build one multiplexed cell voltage frame.
 * @param data Latest cell monitor data.
 * @param mux Frame index, 0 .. CAN_PROTOCOL_CELL_FRAMES-1.
 * @param[out] frame Encoded frame.
 * @return BMS_OK, BMS_E_NULL_PTR, BMS_E_RANGE (bad @p mux) or BMS_E_NOT_READY.
 */
bms_status_t can_protocol_encode_cells(const cell_monitor_data_t *data, uint8_t mux, can_frame_t *frame);

#endif /* BMS_CAN_PROTOCOL_H */
