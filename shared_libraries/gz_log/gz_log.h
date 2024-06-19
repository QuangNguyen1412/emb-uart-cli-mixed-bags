/**
 * @file gz_log.h
 * @brief 
 * @version 0.1
 * @date 2022-09-02
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef _GZ_LOG_H
#define _GZ_LOG_H
#ifdef __cplusplus
extern "C" {
#endif

///\todo add mutex lock/unlock
#define GZ_LOG_DEBUG(...) {\
	__gz_log(__FILE__, __FUNCTION__, __LINE__, GZ_LOG_LEVEL_DEBUG, __VA_ARGS__);\
}
#define GZ_LOG_ALL(...) {\
	__gz_log(__FILE__, __FUNCTION__, __LINE__, GZ_LOG_LEVEL_ALL, __VA_ARGS__);\
}
#define GZ_LOG_INFO(...) {\
	__gz_log(__FILE__, __FUNCTION__, __LINE__, GZ_LOG_LEVEL_INFO, __VA_ARGS__);\
}
#define GZ_LOG_WARN(...) {\
	__gz_log(__FILE__, __FUNCTION__, __LINE__, GZ_LOG_LEVEL_WARNING, __VA_ARGS__);\
}
#define GZ_LOG_ERROR(...) {\
	__gz_log(__FILE__, __FUNCTION__, __LINE__, GZ_LOG_LEVEL_ERROR, __VA_ARGS__);\
}

typedef enum {
  GZ_LOG_LEVEL_ALL = 0,
  GZ_LOG_LEVEL_DEBUG = 1,
  GZ_LOG_LEVEL_INFO = 2,
  GZ_LOG_LEVEL_WARN = 3,
  GZ_LOG_LEVEL_ERROR = 4
} Gz_Log_Level_t;

void gz_log_set_level(int level);

/**
 * @brief: not intended for direct use. Use macro functions instead:
 * - GZ_LOG_ALL
 * - GZ_LOG_DEBUG
 * - GZ_LOG_INFO
 * - GZ_LOG_WARN
 * - GZ_LOG_ERROR
*/
void __gz_log(const char* file, const char* func, int line, int level, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif