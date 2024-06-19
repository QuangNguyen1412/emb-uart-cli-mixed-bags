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
#include "yapi_service_driver.h"
#include "gz_log.h"
#include "yapi_provision.h"

void yapi_init(void) {
  yapi_service_driver_init();
  yapi_provision_callbacks_init();
}

void yapi_task_10ms(void* params) {
  yapi_service_driver_10ms(params);
}

#ifdef __cplusplus
}
#endif
