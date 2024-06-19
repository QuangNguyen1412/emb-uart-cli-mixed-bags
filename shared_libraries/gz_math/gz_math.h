/**
 * @file gz_math.h
 * @author Alex Stout (astout@goalzero.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-09
 * 
 * @copyright Copyright (c) Goal Zero | 2022
 * 
 */

#ifndef GZ_MATH_H
#define GZ_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define	roundDivideIntMacro(x, y) ((int)( y == 0 ? -1 : (( x + (y >> 1) )/y) ))
#define	round_f_to_i(x) ((int)( (x) + 0.5f ))

#ifndef ROUNDF
#define ROUNDF(f, c) (((float)((int)((f) * (c))) / (c)))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

extern uint32_t powersOfTen[];


/**
 * Bitwise rotate-left function for 16-bit value
 * @param x
 * @param bits
 * @return
 */
uint16_t rol16(uint16_t x, uint16_t bits);

/**
 * Bitwise rotate-right function for 16-bit value
 * @param x
 * @param bits
 * @return
 */
uint16_t ror16(uint16_t x, uint16_t bits);
uint8_t add_u8(uint8_t source, uint8_t adder);
uint16_t add_u16(uint16_t source, uint16_t adder);
uint32_t add_u32(uint32_t source, uint32_t adder);
uint16_t mult_u16(uint16_t source, uint16_t scalar);

uint16_t roundDivide_u16(uint16_t x, uint16_t y);

/**
 * Divide x by y, rounding to the nearest 32-bit integer
 * returns 0 if y is 0
 * Note: this is a redefine of the macro:
 *   #define roundDivideInt(x, y) ((int)( y == 0 ? 0 : (( x + (y >> 1) )/y) ))
 * And significantly reduces the instructions used
 * NOTE: it reduced the instructions because some uses of roundDivideInt are for 32-bit numbers and int is only 16-bit.
 *   This led to bugs in what should have been 32-bit divisions
 * TODO: see if there are any other ways to optimize as this could save 1KB of Program Space
 * @param x
 * @param y
 * @return int
 */
uint32_t roundDivide_u32(uint32_t x, uint32_t y);

/**
 * @brief rounds x to the nearest y
 * @param x 
 * @param y 
 * @return int 
 */
int round_x_to_nearest_y(int x, int y);

/**
 * @brief Maps input value `v`, constricted to the range [in_min, in_max], to the range [out_min, out_max].
 * 
 * @param v 
 * @param in_min 
 * @param in_max 
 * @param out_min 
 * @param out_max 
 * @return int 
 */
int map_int(int v, int in_min, int in_max, int out_min, int out_max);

/**
 * @brief constrict v in range [lo,hi]
 * @return int 
 */
int constrict_int(int v, int lo, int hi);
#ifdef __cplusplus
}
#endif

#endif // GZ_MATH_H
