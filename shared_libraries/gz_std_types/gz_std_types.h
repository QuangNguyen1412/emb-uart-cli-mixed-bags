/**
 * @file gz_math.h
 * @author Zachary Garrard
 * @brief 
 * @version 0.1
 * @date 2023-09-01
 * 
 * @copyright Copyright (c) Goal Zero | 2023
 * 
 */

#ifndef GZ_STD_TYPES_H
#define GZ_STD_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef bool (*b_fp_vp_t)(void *arg);
typedef bool (*b_fp_v_t)(void);
typedef void (*v_fp_vp_t)(void *arg);
typedef void (*v_fp_v_t)(void);
typedef void (*v_fp_bp_t)(bool* arg);
typedef void (*v_fp_c_chp_t)(const char* arg);
typedef char* (*chp_fp_vp_t)(void *arg);
typedef char* (*chp_fp_v_t)(void);

#ifdef __cplusplus
}
#endif

#endif // GZ_STD_TYPES_H