/**
 * @file crc8.c
 * @brief CRC-8 SAE J1850, bitwise implementation (no lookup table, small flash footprint).
 */
#include "bms/crc8.h"

#define CRC8_POLY (0x1DU)
#define CRC8_INIT (0xFFU)
#define CRC8_XOR_OUT (0xFFU)

uint8_t crc8_sae_j1850(const uint8_t *data, uint8_t len)
{
    uint8_t crc = CRC8_INIT;
    uint8_t i;
    uint8_t bit;

    if ((data == NULL) && (len > 0U))
    {
        return 0x00U;
    }

    for (i = 0U; i < len; i++)
    {
        crc ^= data[i];
        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x80U) != 0U)
            {
                crc = (uint8_t)((uint8_t)(crc << 1U) ^ CRC8_POLY);
            }
            else
            {
                crc = (uint8_t)(crc << 1U);
            }
        }
    }
    return (uint8_t)(crc ^ CRC8_XOR_OUT);
}
