/*
 * app_yapi.cpp
 *
 *  Created on: Jun 1, 2022
 *      Author: zacharygarrard
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PLATFORM_linux
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#include "yapi_manager.h"
#include "yapi_service.h"
#include "yapi_flash.h"
#include "yapi_service_driver.h"
#include "yapi_modbus.h"
#include "gz_log.h"

void yapi_init(void) {
  yapi_service_driver_init();
  
  yapi_service_register_cmd_cb(_yapi_flash_read_resp_cb, YAPI_CMD_FLASH_READ);
  yapi_service_register_cmd_cb(_yapi_flash_write_resp_cb, YAPI_CMD_FLASH_WRITE);
  yapi_service_register_cmd_cb(_yapi_flash_erase_resp_cb, YAPI_CMD_FLASH_ERASE);
  yapi_service_register_cmd_cb(_yapi_flash_verify_resp_cb, YAPI_CMD_FLASH_VERIFY);
  yapi_modbus_callbacks_init();
}

void yapi_task_10ms(void* params) {
  yapi_service_driver_10ms(params);
}

#ifdef __cplusplus
}
#endif
