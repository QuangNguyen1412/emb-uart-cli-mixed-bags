/**
 * @file gz_bits.h
 * @author Alex Stout (astout@goalzero.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-09
 * 
 * @copyright Copyright (c) Goal Zero | 2022
 * 
 */

#ifndef GZ_BITS_H
#define GZ_BITS_H

#ifdef __cplusplus
extern "C" {
#endif

#define enableBit(n, k) (n | (1 << k))
#define disableBit(n, k) (n & (~(1 << k)))
#define getBit(n, k) ((n & ( 1 << k )) >> k)
#define setBit(n, k, v) (v ? enableBit(n, k) : disableBit(n, k))
#define toggleBit(n, k) (n ^ (1 << k))

#define getBit64(n, k) (((uint64_t)n & ( (uint64_t)1 << k )) >> k)
#define enableBit64(n, k) ((uint64_t)n | ((uint64_t)1 << k))
#define disableBit64(n, k) ((uint64_t)n & (~((uint64_t)1 << k)))
#ifdef __cplusplus
}
#endif

#endif // GZ_BITS_H
