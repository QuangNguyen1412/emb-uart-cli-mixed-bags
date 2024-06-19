/**
 * @file gz_memory.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef GZ_MEMORY_H
#define GZ_MEMORY_H

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif /* __cplusplus */

// #define DEBUG_MEMORY_LEAKS

#ifdef DEBUG_MEMORY_LEAKS

#include "gz_memory_private.h"

#undef malloc
#define malloc(size) gz_memory_malloc(size, \
                                  __FILE__, \
                                  __LINE__, \
                                  (char*)__FUNCTION__)

#undef calloc
#define calloc(num, size) gz_memory_calloc(num, \
                                  size, \
                                  __FILE__, \
                                  __LINE__, \
                                  (char*)__FUNCTION__)

#undef realloc
#define realloc(ptr, size) gz_memory_realloc(ptr, \
                                  size, \
                                  __FILE__, \
                                  __LINE__, \
                                  (char*)__FUNCTION__)

#undef free
#define free(_p) gz_memory_free(_p)

#define GZ_MEMORY_SHOW_MEM_STAT() gz_memory_show_mem_stat()

#undef asprintf
#define asprintf(strp, fmt, ...) gz_memory_asprintf(__FILE__, \
                                  __LINE__, \
                                  (char*)__FUNCTION__, \
                                  strp, \
                                  fmt, \
                                  __VA_ARGS__)

#ifndef CY8C6116BZI_F54
/*
#undef json_asprintf
#define json_asprintf(fmt, ...) gz_memory_json_asprintf(__FILE__, \
                                  __LINE__, \
                                  (char*)__FUNCTION__, \
                                  fmt, \
                                  __VA_ARGS__)
                                  */
#endif /* CY8C6116BZI_F54 */

#else
#define GZ_MEMORY_SHOW_MEM_STAT()
#endif /* DEBUG_MEMORY_LEAKS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GZ_MEMORY_H */
