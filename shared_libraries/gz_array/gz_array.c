#include <stddef.h>
#include <string.h>
#include "gz_array.h"

#ifdef GZ_ARRAY_MVG_AVG_WINDOW_SIZE_USING_DEFAULT
#pragma message "Using the default averaging window size of 16"
#endif

uint16_t gzarray_max_u16(uint16_t *a, uint8_t num_elements) {
  uint8_t i;
  uint16_t max = 0;

  for (i = 0; i < num_elements; i++) {
    if (a[i] > max) {
      max = a[i];
    }
  }
  return (max);
}

bool gzarray_moving_avg_clear(void* pMvgAvg) {
  if (pMvgAvg == NULL) {
    return false;
  }

  TD_DATA_MVG_AVG_UINT_16* temp = (TD_DATA_MVG_AVG_UINT_16*) pMvgAvg;
  if (temp->usable_window_size > GZ_ARRAY_MVG_AVG_WINDOW_SIZE) {
    return false;
  }

  temp->numValidSamples = 0;
  temp->avg = 0;
  temp->pos = 0;
  temp->sum = 0;
  memset(temp->readings, 0, sizeof(temp->usable_window_size));

  return true;
}

bool gzarray_moving_avg_obj_init(void* pMvgAvg, uint8_t windowSize) {
  if (pMvgAvg == NULL || windowSize > GZ_ARRAY_MVG_AVG_WINDOW_SIZE) {
    return false;
  }

  TD_DATA_MVG_AVG_UINT_16* temp = (TD_DATA_MVG_AVG_UINT_16*) pMvgAvg;
  temp->usable_window_size = windowSize;
  return gzarray_moving_avg_clear(pMvgAvg);
}

bool gzarray_moving_avg_add_sample_i16(TD_DATA_MVG_AVG_INT_16* pMvgAvg, int16_t datum) {
  if (pMvgAvg == NULL) {
    return false;
  }

  if (pMvgAvg->numValidSamples < pMvgAvg->usable_window_size) {
    pMvgAvg->numValidSamples++;
  } else {
    pMvgAvg->sum -= pMvgAvg->readings[pMvgAvg->pos]; // Subtract off the oldest sample
  }

  pMvgAvg->readings[pMvgAvg->pos] = datum;

  pMvgAvg->sum += datum;
  pMvgAvg->avg = pMvgAvg->sum / pMvgAvg->numValidSamples;

  pMvgAvg->pos++;
  pMvgAvg->pos %= pMvgAvg->usable_window_size;

  return true;
}

bool gzarray_moving_avg_add_sample_u16(TD_DATA_MVG_AVG_UINT_16* pMvgAvg, uint16_t datum) {
  if (pMvgAvg == NULL) {
    return false;
  }

  if (pMvgAvg->numValidSamples < pMvgAvg->usable_window_size) {
    pMvgAvg->numValidSamples++;
  } else {
    pMvgAvg->sum -= pMvgAvg->readings[pMvgAvg->pos]; // Subtract off the oldest sample
  }

  pMvgAvg->readings[pMvgAvg->pos] = datum;

  pMvgAvg->sum += datum;
  pMvgAvg->avg = pMvgAvg->sum / pMvgAvg->numValidSamples;

  pMvgAvg->pos++;
  pMvgAvg->pos %= pMvgAvg->usable_window_size;

  return true;
}
