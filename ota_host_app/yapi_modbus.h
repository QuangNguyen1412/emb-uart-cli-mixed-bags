/**
 * yapi_modbus.h
 * 
 * Defines all the YAPI requests and callbacks that relate to any MODBUS request
 * This file is supposed to be used by yapi_manager and by who want to use yapi service for yapi to modbus command
 * 
 * Author: Quang Nguyen <quang.nguyen@goalzero.com>
*/

#ifndef OTA_YAPI_MODBUS_H
#define OTA_YAPI_MODBUS_H

#include <stdint.h>
#include <stdbool.h>
#include "yapi_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YAPI_MODBUS_FAIL       1
#define YAPI_MODBUS_SUCCESS    0

/**
 * @brief: send yapi command to PCU to silence everyone on the modbus bus
**/
void yapi_modbus_silence(bool isSilenced);

/**
 * @brief: send yapi command to put a modbus slave to boot loader
**/
void yapi_modbus_enter_bootloader(yapi_device_id_enum_t yapiDeviceId);

/**
 * @brief: send yapi command to query boot flag from a modbus slave
**/
void yapi_modbus_get_boot_info(yapi_device_id_enum_t yapiDeviceId);

/**
 * @brief for registering the cb for the responses
 * @return [0-3] callback id, 0xFF
 * **/
uint8_t yapi_modbus_register_cb(v_fp_yapi_ptr_t cb);

/**
 * @brief initialize the callbacks for yapi modbus
 * **/
void yapi_modbus_callbacks_init();

#ifdef __cplusplus
}
#endif

#endif //OTA_YAPI_MODBUS_H