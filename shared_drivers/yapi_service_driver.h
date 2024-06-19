/**
 * @file yapi_service_driver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-08-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef INCLUDES_YAPI_SERVICE_DRIVER_H_
#define INCLUDES_YAPI_SERVICE_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PCU_YAPI_DEVICE_ID YAPI_DEVICE_PCU

void yapi_service_driver_init(void);
void yapi_service_driver_10ms(void* params);
void yapi_service_driver_set_uart_instance(void*);

#ifdef __cplusplus
}
#endif

#endif // INCLUDES_YAPI_SERVICE_DRIVER_H_
