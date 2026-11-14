/**
 * @file crc8.h
 * @brief CRC-8 SAE J1850 (poly 0x1D, init 0xFF, final XOR 0xFF), as used by
 *        AUTOSAR E2E profile 1 style frame protection.
 */
#ifndef BMS_CRC8_H
#define BMS_CRC8_H

#include "bms/bms_types.h"

/**
 * @brief Compute the CRC over @p len bytes.
 * @param data Input bytes; may be NULL only if @p len is 0.
 * @param len Number of bytes.
 * @return The CRC; 0x00 if @p data is NULL and @p len > 0.
 */
uint8_t crc8_sae_j1850(const uint8_t *data, uint8_t len);

#endif /* BMS_CRC8_H */
