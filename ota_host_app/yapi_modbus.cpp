/**
 * yapi_modbus.h
 * 
 * Defines all the YAPI requests and callbacks that relate to any MODBUS request
 * This file is supposed to be used by yapi_manager and by who want to use yapi service for yapi to modbus command
 * 
 * Author: Devan Hone <devan.hone@goalzero.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // read/write/close/sleep

#include "yapi_modbus.h"
#include "yapi_service.h"
#include "gz_log.h"
#include "gz_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************/
/* Section: Defines & Typedefs                       */
/*****************************************************/

#define MAX_RESPONSE_CB                   4
#define INVALID_CB_ID                     0xFF

/*****************************************************/
/* Section: Private variables                        */
/*****************************************************/

static v_fp_yapi_ptr_t _yapi_modbus_resp_cbs[MAX_RESPONSE_CB] = { NULL };

/*****************************************************/
/* Section: Private function declarations            */
/*****************************************************/

/**
 * @brief general handler for yapi_modbus command responses. 
**/
static void _yapi_modbus_response_handler(yapi_packet_t* yapiPkt);

/**
 * @brief yapi response call back for get_boot_info
**/
static void _yapi_modbus_get_boot_info_resp_cb(yapi_packet_t* yapiPkt);

/**
 * @brief handle yapi enter bootloader response
**/
static void _yapi_modbus_enter_bootloader_resp_cb(yapi_packet_t* yapiPkt);

/*****************************************************/
/* Section: Public function definitions              */
/*****************************************************/

void yapi_modbus_silence(bool isSilenced) {
  uint8_t data = isSilenced;
  yapi_service_build_send(YAPI_DEVICE_PCU, 
                        YAPI_CMD_MODBUS_SILENCE,
                        YAPI_MSG_SET_RQST,
                        YAPI_PRIORITY_LOW,
                        NULL,
                        &data,
                        1);
}

void yapi_modbus_enter_bootloader(yapi_device_id_enum_t yapiDeviceId) {
  GZ_LOG_INFO("yapi_modbus_enter_bootloader_request: device[0x%02X]\n", yapiDeviceId);
  yapi_service_build_send(yapiDeviceId, 
                          YAPI_CMD_MODBUS_ENTER_BOOTLOADER,
                          YAPI_MSG_SET_RQST,
                          YAPI_PRIORITY_LOW,
                          NULL,
                          NULL,
                          0);
}

void yapi_modbus_get_boot_info(yapi_device_id_enum_t yapiDeviceId) {
  GZ_LOG_INFO("yapi_modbus_get_boot_info_request: device[0x%02X]\n", yapiDeviceId);
  yapi_service_build_send(yapiDeviceId,
                          YAPI_CMD_MOBUS_GET_BOOTINFO,
                          YAPI_MSG_GET_RQST,
                          YAPI_PRIORITY_LOW,
                          NULL,
                          NULL,
                          0);
}

uint8_t yapi_modbus_register_cb(v_fp_yapi_ptr_t cb) {
  for (int i = 0; i < MAX_RESPONSE_CB; i++) {
    if (!_yapi_modbus_resp_cbs[i]) {
      _yapi_modbus_resp_cbs[i] = cb;
      return i;
    }
  }
  return INVALID_CB_ID;
}

void yapi_modbus_callbacks_init() {
  yapi_service_register_cmd_cb(_yapi_modbus_enter_bootloader_resp_cb, YAPI_CMD_MODBUS_ENTER_BOOTLOADER);
  yapi_service_register_cmd_cb(_yapi_modbus_get_boot_info_resp_cb, YAPI_CMD_MOBUS_GET_BOOTINFO);
  yapi_modbus_register_cb(_yapi_modbus_response_handler);
}

/*****************************************************/
/* Section: Private function definitions             */
/*****************************************************/

static void _yapi_modbus_response_handler(yapi_packet_t* yapiPkt) {
  switch (yapiPkt->command)
  {
    case YAPI_CMD_MODBUS_ENTER_BOOTLOADER:
      if (yapiPkt->messageData.type == YAPI_MSG_SET_RESP_OK) {
        GZ_LOG_INFO("_yapi_modbus_enter_bootloader_resp_cb: [SUCCESS]\n");
      } else {
        GZ_LOG_INFO("_yapi_modbus_enter_bootloader_resp_cb: [FAIL]\n");
      }
      break;
    case YAPI_CMD_MOBUS_GET_BOOTINFO:
      if (yapiPkt->messageData.type != YAPI_MSG_GET_RESP_OK) {
        GZ_LOG_INFO("_yapi_modbus_get_boot_info_resp_cb: [ERROR]\n");
      } else {
        GZ_LOG_INFO("_yapi_modbus_get_boot_info_resp_cb: appUpdateState[0x%02X]\n", yapiPkt->data[0]);
      }
      break;
  }
}

static void _yapi_modbus_get_boot_info_resp_cb(yapi_packet_t* yapiPkt) {
  for (int i = 0; i < MAX_RESPONSE_CB; i++) {
    if (_yapi_modbus_resp_cbs[i]) {
      _yapi_modbus_resp_cbs[i](yapiPkt);
    }
  }
}

static void _yapi_modbus_enter_bootloader_resp_cb(yapi_packet_t* yapiPkt) {
  for (int i = 0; i < MAX_RESPONSE_CB; i++) {
    if (_yapi_modbus_resp_cbs[i]) {
      _yapi_modbus_resp_cbs[i](yapiPkt);
    }
  }
}

#ifdef __cplusplus
}
#endif