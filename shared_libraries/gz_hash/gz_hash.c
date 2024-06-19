#include "gz_hash.h"
#include "gz_math.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  CRC_TABLE_SIZE = 256,
};

const uint8_t _crc8LookupTable[CRC_TABLE_SIZE] = {
    0,   7,  14,   9,  28,  27,  18,  21,  56,  63,  54,  49,  36,  35,  42,  45,
    112, 119, 126, 121, 108, 107,  98, 101,  72,  79,  70,  65,  84,  83,  90,  93,
    224, 231, 238, 233, 252, 251, 242, 245, 216, 223, 214, 209, 196, 195, 202, 205,
    144, 151, 158, 153, 140, 139, 130, 133, 168, 175, 166, 161, 180, 179, 186, 189,
    199, 192, 201, 206, 219, 220, 213, 210, 255, 248, 241, 246, 227, 228, 237, 234,
    183, 176, 185, 190, 171, 172, 165, 162, 143, 136, 129, 134, 147, 148, 157, 154,
    39,  32,  41,  46,  59,  60,  53,  50,  31,  24,  17,  22,   3,   4,  13,  10,
    87,  80,  89,  94,  75,  76,  69,  66, 111, 104,  97, 102, 115, 116, 125, 122,
    137, 142, 135, 128, 149, 146, 155, 156, 177, 182, 191, 184, 173, 170, 163, 164,
    249, 254, 247, 240, 229, 226, 235, 236, 193, 198, 207, 200, 221, 218, 211, 212,
    105, 110, 103,  96, 117, 114, 123, 124,  81,  86,  95,  88,  77,  74,  67,  68,
    25,  30,  23,  16,   5,   2,  11,  12,  33,  38,  47,  40,  61,  58,  51,  52,
    78,  73,  64,  71,  82,  85,  92,  91, 118, 113, 120, 127, 106, 109, 100,  99,
    62,  57,  48,  55,  34,  37,  44,  43,   6,   1,   8,  15,  26,  29,  20,  19,
    174, 169, 160, 167, 178, 181, 188, 187, 150, 145, 152, 159, 138, 141, 132, 131,
    222, 217, 208, 215, 194, 197, 204, 203, 230, 225, 232, 239, 250, 253, 244, 243,
};

const uint16_t _crc16LookupTable[] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

uint8_t gz_crc8(uint8_t* data, uint16_t length, uint8_t seed) {
  // generate and return the CRC on a set of data
  // crc polynomial: x^8 + x^2 + x^1 + 1

  uint16_t i;
  uint8_t crcData = seed;

  // calculate crc checksum from lookup table
  for (i = 0; i < length; ++i) {
    crcData = _crc8LookupTable[crcData ^ data[i]];
  }

  return crcData;
}

uint8_t gz_crc8_size_optimized(uint8_t* data, uint16_t length, uint8_t seed) {
  uint16_t i;
  uint8_t crcData = seed;
  const uint8_t generator = 0x07;

  for (i = 0; i < length; ++i) {
    crcData ^= data[i];

    for (uint8_t j = 0; j < 8; j++) {
      if ((crcData & 0x80) != 0) {
        crcData = ((crcData << 1) ^ generator);
      } else {
        crcData <<= 1;
      }
    }
  }

  return crcData;
}

uint16_t gz_crc16_seeded(const uint8_t* data_p, uint16_t length, uint16_t seed) {
  uint16_t crc = seed;
  uint16_t i;

  for (i = 0; i < length; ++i) {
    crc = (crc << 8) ^ _crc16LookupTable[(crc >> 8) ^ data_p[i]];
  }

  return crc;
}

uint16_t gz_crc16_seeded_size_optimized(const uint8_t* data_p, uint16_t length, uint16_t seed) {
  uint8_t x;
  uint16_t crc = seed;

  while (length--) {
    x = crc >> 8 ^ *data_p++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^ ((uint16_t) x);
  }
  return crc;
}

uint16_t gz_crc16(const uint8_t* data_p, uint16_t length) {
  return gz_crc16_seeded(data_p, length, 0);
}

uint16_t gz_crc16_size_optimized(const uint8_t* data_p, uint16_t length) {
  return gz_crc16_seeded_size_optimized(data_p, length, 0);
}

static bool _is_char_valid(char c) {
  return (c >= '0' && c <= '9') || (c == '-') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

uint16_t gz_hash_djb2(char *input) {
  uint16_t result = 0x5555;
  char *ptr = input;
  while (*ptr) {
    uint8_t c = *ptr;
    // only use A-Za-Z0-9 & '-' characters in hash
    if (_is_char_valid(c)) {
        result ^= c;
    }
    #ifdef BOOTLOADER_BUILD
    result = (result << 5 | result >> (16 - 5));
    #else
    result = rol16(result, 5);
    #endif  // BOOTLOADER_BUILD
    ptr++;
  }
  return result;
}

#ifdef __cplusplus
}
#endif
