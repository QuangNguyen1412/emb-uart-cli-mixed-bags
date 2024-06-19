/**
 * @file gz_hash.h
 * @author Alex Stout (astout@goalzero.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-09
 * 
 * @copyright Copyright (c) Goal Zero | 2022
 * 
 */

#ifndef GZ_HASH_H
#define GZ_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief CRC8 calculator function optimized for speed at the expense of code size.
 * CRC8 is not recommended for buffers longer than 16 bytes.
 * Use this variant in MCU applications.
 * Reference: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * 
 * @param data A pointer to the buffer of data to compute a checksum on
 * @param length The number of bytes in the buffer to compute a checksum on
 * @return uint8_t The CRC8 checksum
 */
uint8_t gz_crc8(uint8_t* data, uint16_t length, uint8_t seed);

/**
 * @brief CRC8 calculator function optimized for code size at the expense of speed.
 * CRC8 is not recommended for buffers longer than 16 bytes.
 * Use this variant in bootloaders or where code size is constricted.
 * Reference: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * 
 * @param data A pointer to the buffer of data to compute a checksum on
 * @param length The number of bytes in the buffer to compute a checksum on
 * @return uint8_t The CRC8 checksum
 */
uint8_t gz_crc8_size_optimized(uint8_t* data, uint16_t length, uint8_t seed);

/**
 * @brief CRC16 CCIT custom seed calculator function optimized for speed at the
 * expense of code size. Use this variant in MCU applications.
 * Reference: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * 
 * @param data_p A pointer to the buffer of data to compute a checksum on
 * @param length The number of bytes in the buffer to compute a checksum on
 * @param seed The starting value to beging the CRC operation. Can be used with the
 * return value of a previous CRC16 calculation to perform a CRC over a larger
 * region of memory in chunks.
 * @return uint16_t The CRC16 checksum
 */
uint16_t gz_crc16_seeded(const uint8_t* data_p, uint16_t length, uint16_t seed);

/**
 * @brief CRC16 CCIT custom seed calculator function optimized for code size at the
 * expense of speed. Use this variant in bootloaders or where code size is constricted.
 * Reference: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * 
 * @param data_p A pointer to the buffer of data to compute a checksum on
 * @param length The number of bytes in the buffer to compute a checksum on
 * @param seed The starting value to beging the CRC operation. Can be used with the
 * return value of a previous CRC16 calculation to perform a CRC over a larger
 * region of memory in chunks.
 * @return uint16_t The CRC16 checksum
 */
uint16_t gz_crc16_seeded_size_optimized(const uint8_t* data_p, uint16_t length, uint16_t seed);

/**
 * @brief CRC16 CCIT Seed 0 calculator function optimized for speed at the
 * expense of code size. Use this variant in MCU applications.
 * Reference: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * 
 * @param data_p A pointer to the buffer of data to compute a checksum on
 * @param length The number of bytes in the buffer to compute a checksum on
 * @return uint16_t The CRC16 checksum
 */
uint16_t gz_crc16(const uint8_t* data_p, uint16_t length);

/**
 * @brief CRC16 CCIT Seed 0 calculator function optimized for code size at the
 * expense of speed. Use this variant in bootloaders or where code size is constricted.
 * Reference: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 * 
 * @param data_p A pointer to the buffer of data to compute a checksum on
 * @param length The number of bytes in the buffer to compute a checksum on
 * @return uint16_t The CRC16 checksum
 */
uint16_t gz_crc16_size_optimized(const uint8_t* data_p, uint16_t length);

/**
 * @brief Returns a hash of a null-terminated string using djb2 hashing method.
 * Ignores characters that are not alphanumeric or a dash during hash computation.
 * Reference: http://www.cse.yorku.ca/~oz/hash.html
 * 
 * @param input A pointer to the string to compute a djb2 hash on.
 * @return uint16_t The djb2 hashed value of the string.
 */ 
uint16_t gz_hash_djb2(char *input);

#ifdef __cplusplus
}
#endif

#endif // GZ_HASH_H
