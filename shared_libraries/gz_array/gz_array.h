/**
 * @file gz_array.h
 * @author Alex Stout (astout@goalzero.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-09
 * 
 * @copyright Copyright (c) Goal Zero | 2022
 * 
 */

#ifndef GZ_ARRAY_H
#define GZ_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define arrayLength(x) (sizeof(x)/sizeof(x[0]))

/**
 * @brief get the value of element with the max uint16_t value of the given array
 * 
 * @param a 
 * @param num_elements 
 * @return uint16_t 
 */
uint16_t gzarray_max_u16(uint16_t *a, uint8_t num_elements);

// It is recommended that this is set by the project settings as a pre-defined macro
// passed to the compiler so that this submodule remains portable
#ifndef GZ_ARRAY_MVG_AVG_WINDOW_SIZE // If NOT defined...
#define GZ_ARRAY_MVG_AVG_WINDOW_SIZE_USING_DEFAULT
#define GZ_ARRAY_MVG_AVG_WINDOW_SIZE  16
#endif

typedef struct __attribute__ ((packed)) {
  uint8_t usable_window_size;
  uint8_t pos;
  uint8_t numValidSamples;
  int32_t sum;
  int16_t avg;
  int16_t readings[GZ_ARRAY_MVG_AVG_WINDOW_SIZE];
} TD_DATA_MVG_AVG_INT_16;

typedef struct __attribute__ ((packed)) {
  uint8_t usable_window_size;
  uint8_t pos;
  uint8_t numValidSamples;
  uint32_t sum;
  uint16_t avg;
  uint16_t readings[GZ_ARRAY_MVG_AVG_WINDOW_SIZE];
} TD_DATA_MVG_AVG_UINT_16;

/**
 * @brief Clears the buffer and averaging attributes of an TD_DATA_MVG_AVG_UINT_16 or
 * a TD_DATA_MVG_AVG_INT_16 object.
 * 
 * @param pMvgAvg A pointer to the caller allocated averaging object.
 * @return Returns true if the operation was successful, false otherwise.
 */
bool gzarray_moving_avg_clear(void* pMvgAvg);

/**
 * @brief Initializes a TD_DATA_MVG_AVG_UINT_16 or a TD_DATA_MVG_AVG_INT_16 object
 * allocated by the calling code.
 * 
 * @param pMvgAvg A pointer to the caller allocated averaging object.
 * @param windowSize Must be <= to the defined DATA_MVG_AVG_WINDOW_SIZE. This is the 
 * desired size of the averaging buffer.
 * @return Returns true if the operation was successful, false otherwise.
 */
bool gzarray_moving_avg_obj_init(void* pMvgAvg, uint8_t windowSize);

/**
 * @brief Calculates the moving average of a set of signed data given a new sample. Utilizes a 
 * "running start" to provide access to the average before a full buffer of samples is
 * collected. 
 * 
 * @param pMvgAvg A pointer to caller allocated object. The calling code must initialize 
 * the object before using this function.
 * @param datum The sample to be added to the moving average.
 * @return Returns true if the operation was successful, false otherwise.
 */
bool gzarray_moving_avg_add_sample_i16(TD_DATA_MVG_AVG_INT_16* pMvgAvg, int16_t datum);

/**
 * @brief Calculates the moving average of a set of unsigned data given a new sample. Utilizes a 
 * "running start" to provide access to the average before a full buffer of samples is
 * collected. 
 * 
 * @param pMvgAvg A pointer to caller allocated object. The calling code must initialize 
 * the object before using this function.
 * @param datum The sample to be added to the moving average.
 * @return Returns true if the operation was successful, false otherwise.
 */
bool gzarray_moving_avg_add_sample_u16(TD_DATA_MVG_AVG_UINT_16* pMvgAvg, uint16_t datum);

#ifdef __cplusplus
}
#endif

#endif // GZ_ARRAY_H
