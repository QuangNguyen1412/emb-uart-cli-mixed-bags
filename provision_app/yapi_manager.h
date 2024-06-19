/*
 * yapi_manager.h
 *
 *  Created on: Jun 1, 2022
 *      Author: zacharygarrard
 */

#ifndef HEADER_FILES_YAPI_H_
#define HEADER_FILES_YAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The yapi manager initialization function
 * 
 */
void yapi_init(void);

/**
 * @brief The yapi manager 10 mS task. Currently the parameters argument is unused.
 * 
 * @param params Void * to a number of parameters. Currently unused.
 */
void yapi_task_10ms(void* params);

#ifdef __cplusplus
}
#endif

#endif /* HEADER_FILES_YAPI_H_ */
