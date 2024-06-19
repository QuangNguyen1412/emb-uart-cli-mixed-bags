#ifndef GZ_MEMORY_PRIVATE_H
#define GZ_MEMORY_PRIVATE_H

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif /* __cplusplus */

#ifndef CY8C6116BZI_F54
#include "frozen.h"
#endif /* CY8C6116BZI_F54 */

#define GZ_MEMORY_MAX_PTRS 1024

typedef struct {
  void * mem_pointer;
  char * file_name;
  char * function;
  int line;
  int size;
  int flag;
} gz_memory_node_t ;

gz_memory_node_t mem_node[GZ_MEMORY_MAX_PTRS];

void* gz_memory_malloc(size_t size,
                   char * file_name,
                   int line,
                   char * function);

void* gz_memory_calloc(size_t num,
                   size_t size,
                   char * file_name,
                   int line,
                   char * function);

void* gz_memory_realloc(void * ptr,
                   size_t size,
                   char * file_name,
                   int line,
                   char * function);

void gz_memory_free(void * mem_pointer);

void gz_memory_show_mem_stat(void);

int gz_memory_asprintf(char* file_name, int line, char* function, char** strp, const char *fmt, ...);

#ifndef CY8C6116BZI_F54
char* gz_memory_json_asprintf(char* file_name, int line, char* function, const char *fmt, ...);
#endif /* CY8C6116BZI_F54 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GZ_MEMORY_PRIVATE_H */
