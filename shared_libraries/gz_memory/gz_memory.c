/**
 * @file gz_memory.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-09
 * 
 * @reference https://www.equestionanswers.com/c/memory-leak-detect-and-trace.php
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif /* __cplusplus */

#ifndef CY8C6116BZI_F54
#include "frozen.h"
#include "gz_mos_platform.h"
#else
#define __STDC_WANT_LIB_EXT2__ 1
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include <stdarg.h>
#include <stdio.h>
#endif
#include "gz_memory_private.h"
#include "gz_string.h"

enum {
  DEBUG_CHARS_TO_PRINT = 25
};

static gz_memory_node_t _mem_node[GZ_MEMORY_MAX_PTRS];
static uint32_t _mem_node_count = 0;

void* gz_memory_malloc(size_t size,
                   char * file_name,
                   int line,
                   char * function) {
  int i = 0;
  void *pointer = malloc(size);

  for (i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
    if (_mem_node[i].flag == 0) {
      _mem_node[i].flag = 1;
      _mem_node[i].mem_pointer = pointer;
      _mem_node[i].file_name = file_name;
      _mem_node[i].function = function;
      _mem_node[i].line = line;
      _mem_node[i].size = size;
      if (_mem_node_count < GZ_MEMORY_MAX_PTRS) {
        _mem_node_count++;
      }
      break;
    }
  }

  return pointer;
}

void* gz_memory_calloc(size_t num,
                   size_t size,
                   char * file_name,
                   int line,
                   char * function) {
  int i = 0;
  void *pointer = calloc(num, size);

  for (i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
    if (_mem_node[i].flag == 0) {
      _mem_node[i].flag = 1;
      _mem_node[i].mem_pointer = pointer;
      _mem_node[i].file_name = file_name;
      _mem_node[i].function = function;
      _mem_node[i].line = line;
      _mem_node[i].size = (num * size);
      if (_mem_node_count < GZ_MEMORY_MAX_PTRS) {
        _mem_node_count++;
      }
      break;
    }
  }

  return pointer;
}

void* gz_memory_realloc(void * ptr,
                   size_t size,
                   char * file_name,
                   int line,
                   char * function) {
  void *temp_ptr = ptr;

  void *pointer = realloc(ptr, size);

  // If realloc does not move the original pointer location, we only need to update the size of the allocated memory at that location.
  // If realloc does move the original pointer location, we need to clear the previous location as realloc has freed it, and then update the new location with the new size.
  if (pointer == ptr) {
    for (int i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
      if (_mem_node[i].mem_pointer == ptr) {
        _mem_node[i].file_name = file_name;
        _mem_node[i].function = function;
        _mem_node[i].line = line;
        _mem_node[i].size = size;
        break;
      }
    }
  } else {
    for (int i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
      if (_mem_node[i].mem_pointer == temp_ptr) {
        _mem_node[i].flag = 0;
        _mem_node[i].mem_pointer = NULL;
        _mem_node[i].file_name = 0;
        _mem_node[i].function = 0;
        _mem_node[i].line = 0;
        _mem_node[i].size = 0;
        break;
      }
    }
    for (int i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
      if (_mem_node[i].flag == 0) {
        _mem_node[i].flag = 1;
        _mem_node[i].mem_pointer = pointer;
        _mem_node[i].file_name = file_name;
        _mem_node[i].function = function;
        _mem_node[i].line = line;
        _mem_node[i].size = size;
        break;
      }
    }
  }

  return pointer;
}

void gz_memory_free(void * mem_pointer) {
  int i = 0;

  for (i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
    if (_mem_node[i].mem_pointer == mem_pointer) {
      _mem_node[i].flag = 0;
      _mem_node[i].mem_pointer = NULL;
      _mem_node[i].file_name = 0;
      _mem_node[i].function = 0;
      _mem_node[i].line = 0;
      _mem_node[i].size = 0;
      free(mem_pointer);
      if (_mem_node_count) {
        _mem_node_count--;
      }
      break;
    }
  }
}

void gz_memory_show_mem_stat(void) {
  int i = 0;
  int ret = 0;

  printf("Number of memory nodes not freed: %d\r\n", _mem_node_count);
  for (i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
    if (_mem_node[i].flag == 1) {
      printf("0x%p of %d bytes allocated"
              " from %s:%d in %s() is not freed. First few chars of mem are: %s\r\n",
              _mem_node[i].mem_pointer,
              _mem_node[i].size,
              _mem_node[i].file_name,
              _mem_node[i].line,
              _mem_node[i].function,
              prnstr(_mem_node[i].mem_pointer, DEBUG_CHARS_TO_PRINT, "non empty"));
      ret = 1;
    }
  }

  if (ret == 0) {
    printf("no memory leaks detected!\n");
  }
}

int gz_memory_asprintf(char* file_name, int line, char* function, char** strp, const char *fmt, ...) {
  int i = 0;
  int ret = -1;
  va_list ap;
  int size = 0;

  va_start(ap, fmt);
  size = vasprintf(strp, fmt, ap);

  if (size == -1) {
    va_end(ap);
    return size;
  }

  for (i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
    if (_mem_node[i].flag == 0) {
      _mem_node[i].flag = 1;
      _mem_node[i].mem_pointer = *strp;
      _mem_node[i].file_name = file_name;
      _mem_node[i].function = function;
      _mem_node[i].line = line;
      _mem_node[i].size = size;
      ret = 0;
      if (_mem_node_count < GZ_MEMORY_MAX_PTRS) {
        _mem_node_count++;
      }
      break;
    }
  }

  va_end(ap);
  return size;
}

#ifndef CY8C6116BZI_F54
char* gz_memory_json_asprintf(char* file_name, int line, char* function, const char *fmt, ...) {
  int i = 0;
  int ret = -1;
  va_list ap;
  int size = 0;

  va_start(ap, fmt);
  char *str = json_vasprintf(fmt, ap);

  if (strisset(str)) {
    size = strlen(str);
  } else {
    va_end(ap);
    return str;
  }

  for (i = 0; i < GZ_MEMORY_MAX_PTRS; i++) {
    if (_mem_node[i].flag == 0) {
      _mem_node[i].flag = 1;
      _mem_node[i].mem_pointer = str;
      _mem_node[i].file_name = file_name;
      _mem_node[i].function = function;
      _mem_node[i].line = line;
      _mem_node[i].size = size;
      ret = 0;
      if (_mem_node_count < GZ_MEMORY_MAX_PTRS) {
        _mem_node_count++;
      }
      break;
    }
  }

  va_end(ap);
  return str;
}
#endif /* CY8C6116BZI_F54 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
