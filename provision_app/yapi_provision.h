/**
 * yapi_provision.h
 * 
 * Author: Quang Nguyen <quang.nguyen@goalzero.com>
*/

#ifndef OTA_YAPI_PROVISION_H
#define OTA_YAPI_PROVISION_H

#include <stdint.h>
#include <stdbool.h>
#include "yapi_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YAPI_MODBUS_FAIL       1
#define YAPI_MODBUS_SUCCESS    0

typedef enum {
  YAPI_SERIAL_RESPONSE_UNKNOWN,
  YAPI_SERIAL_RESPONSE_WAITING,
  YAPI_SERIAL_RESPONSE_OK,
  YAPI_SERIAL_RESPONSE_ERR
} Yapi_Serial_Respons_e;
/**
 * @brief Send serial number YAPI SET request
 * **/
void yapi_provision_send_serial_number(char*, uint8_t);

/**
 * @brief
 * **/
Yapi_Serial_Respons_e yapi_provision_get_serial_write_respond_status();

/**
 * @brief
 * **/
Yapi_Serial_Respons_e yapi_provision_get_serial_read_respond_status();

/**
 * @brief
 * **/
void yapi_provision_send_serial_read_request();

/**
 * @brief initialize the callbacks for yapi provision
 * **/
void yapi_provision_callbacks_init();

const char* yapi_provision_serial_response_status_string(Yapi_Serial_Respons_e);

#ifdef __cplusplus
}
#endif

#endif //OTA_YAPI_PROVISION_H