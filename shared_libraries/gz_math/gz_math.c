#include "gz_math.h"

uint32_t powersOfTen[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

#ifdef __cplusplus
extern "C" {
#endif

uint16_t rol16(uint16_t x, uint16_t bits) {
  return x << bits | x >> (16 - bits);
}

uint16_t ror16(uint16_t x, uint16_t bits) {
  return x >> bits | x << (16 - bits);
}

uint8_t add_u8(uint8_t source, uint8_t adder) {
  return (source > (UINT8_MAX - adder)) ? UINT8_MAX : source + adder;
}

uint16_t add_u16(uint16_t source, uint16_t adder) {
  return (source > (UINT16_MAX - adder)) ? UINT16_MAX : source + adder;
}

uint32_t add_u32(uint32_t source, uint32_t adder) {
  return (source > (UINT32_MAX - adder)) ? UINT32_MAX : source + adder;
}

uint16_t mult_u16(uint16_t source, uint16_t scalar) {
  uint16_t product = source * scalar;
  if (source == 0 || scalar == 0)
  {
    return product;
  }

  return (source == product / scalar) ? product : UINT16_MAX;
}

uint32_t roundDivide_u32(uint32_t x, uint32_t y) {
  if (y == 0)
  {
    return -1;
  }
  uint32_t result = add_u32(x, y >> 1);
  return (result / y);
}

uint16_t roundDivide_u16(uint16_t x, uint16_t y) {
  if (y == 0)
  {
    return -1;
  }
  uint16_t result = add_u16(x, y >> 1);
  return (result / y);
}

int round_x_to_nearest_y(int x, int y) {
  if (y == 0) {
    return 0;
  }
  int result = (x + y - 1);
  result = result / y;
  return result * y;
}

int map_int(int v, int in_min, int in_max, int out_min, int out_max) {
  v = max(v, in_min);
  v = min(v, in_max);
  v -= in_min;
  v *= (out_max - out_min);
  v = roundDivideIntMacro(v, (in_max - in_min));
  v += out_min;
  v = min(v, out_max);
  v = max(v, out_min);
  return v;
}

int constrict_int(int v, int lo, int hi) {
  v = max(v, lo);
  v = min(v, hi);
  return v;
}

#ifdef __cplusplus
}
#endif
