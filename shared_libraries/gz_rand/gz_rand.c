#include "gz_rand.h"

#ifdef __cplusplus
extern "C" {
#endif

static unsigned long int _randNext = 1;
static gz_rand_seed_cb_t _seed_fn;

void gzrand_seed_uint(unsigned int seed) {
  _randNext = seed;
}

void gzrand_set_seed_fn(gz_rand_seed_cb_t seed_fn) {
  _seed_fn = seed_fn;
}

void gzrand_reseed() {
  if (_seed_fn) {
    gzrand_seed_uint(_seed_fn());
  }
}

int gzrand(void) {
  _randNext = _randNext * 1103515245 + 12345;
  return (unsigned int)(_randNext/65536) % 32768;
}

int gzrand_in_range(int low, int high) {
  if (low >= high) {
    return 0;
  }
  if (_randNext == 1) { // if unseeded, seed now
    gzrand_reseed();
  }
  return (gzrand() % (high - low + 1)) + low;
}

#ifdef __cplusplus
}
#endif
