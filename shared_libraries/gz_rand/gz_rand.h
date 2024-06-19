#ifndef GZ_RAND_H
#define GZ_RAND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int (*gz_rand_seed_cb_t)(void);

/**
 * @brief seeds the random number generator.
 * @param seed 
 */
void gzrand_seed_uint(unsigned int seed);

/**
 * @brief Sets the seeding function to call if not yet
 * 
 * @param seed_fn 
 */
void gzrand_set_seed_fn(gz_rand_seed_cb_t seed_fn);

/**
 * @brief if the seeding function is set, calls it and updates the seed.
 */
void gzrand_reseed();

/**
 * @brief returns a random number.
 * See gzsrand to seed the generator.
 * @return int 
 */
int gzrand(void);

/**
 * @brief return random integer between given low, high
 * @param low
 * @param high
 * @return a random intenger between given low and high
 */
int gzrand_in_range(int low, int high);

#ifdef __cplusplus
}
#endif

#endif // GZ_MATH_H
