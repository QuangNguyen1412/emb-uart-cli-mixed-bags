/**
 * yapi_provision.h
 * 
 * Author: Quang Nguyen <quang.nguyen@goalzero.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // read/write/close/sleep

#include "yapi_provision.h"
#include "yapi_service.h"
#include "gz_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************/
/* Section: Defines & Typedefs                       */
/*****************************************************/

#define MAX_RESPONSE_CB                   4
#define INVALID_CB_ID                     0xFF

static char _serialNumber[25] = {0};
static Yapi_Serial_Respons_e _serialWriteRespStatus = YAPI_SERIAL_RESPONSE_UNKNOWN;
static Yapi_Serial_Respons_e _serialReadRespStatus = YAPI_SERIAL_RESPONSE_UNKNOWN;
/*****************************************************/
/* Section: Private function declarations            */
/*****************************************************/

/**
 * @brief yapi response call back for serial write
**/
static void _yapi_provision_serial_write_resp_cb(yapi_packet_t* yapiPkt);

/**
 * @brief yapi response call back for serial read
**/
static void _yapi_provision_serial_read_resp_cb(yapi_packet_t* yapiPkt);

/*****************************************************/
/* Section: Public function definitions              */
/*****************************************************/
static void _yapi_provision_serial_read_resp_cb(yapi_packet_t* yapiPkt) {
  if (yapiPkt->senderId == YAPI_DEVICE_PCU && yapiPkt->messageData.type == YAPI_MSG_GET_RESP_OK) {
    _serialReadRespStatus = YAPI_SERIAL_RESPONSE_OK;
    yapi_serial_number_read_response_t readId;
    memcpy(&readId, yapiPkt->data, yapiPkt->length);
    printf("\n\r#########\tDevice Info\t#########\n\r");
    printf("\tMAC Address:\t\t%s\n\r", readId.macAddress);
    printf("\tSerial Number:\t\t%s\n\r", readId.serialNumberString);
    printf("\tFirmware WMU :\t\t%d\n\r", readId.fwVersion_WMU);
    printf("\tFirmware PCU:\t\t%d\n\r", readId.fwVersion_PCU);
    printf("\tFirmware BMS:\t\t%d\n\r", readId.fwVersion_BMS);
    printf("\tFirmware MPPT:\t\t%d\n\r", readId.fwVersion_MPPT);
    printf("\tFirmware LV INV:\t%d\n\r", readId.fwVersion_LV_INV);
    printf("\tFirmware HV INV:\t%d\n\r", readId.fwVersion_HV_INV);
    printf("\tHardware PCU_WMU:\t%d\n\r", readId.hwVersion_PCU_WMU);
    printf("\tHardware BMS:\t\t%d\n\r", readId.hwVersion_BMS);
    printf("\tHardware MPPT:\t\t%d\n\r", readId.hwVersion_MPPT);
    printf("\tHardware INV:\t\t%d\n\r", readId.hwVersion_INV);
    printf("\n\r#########\tDevice Info\t#########\n\r");
  } else {
    _serialReadRespStatus = YAPI_SERIAL_RESPONSE_ERR;
  }
}

static void _yapi_provision_serial_write_resp_cb(yapi_packet_t* yapiPkt) {
  if (yapiPkt->senderId == YAPI_DEVICE_PCU && yapiPkt->messageData.type == YAPI_MSG_SET_RESP_OK) {
    _serialWriteRespStatus = YAPI_SERIAL_RESPONSE_OK;
  } else {
    _serialWriteRespStatus = YAPI_SERIAL_RESPONSE_ERR;
    uint8_t factoryResp = yapiPkt->data[0];
    printf("(!) Factory write error code [%d]", factoryResp);
  }
}

const char* yapi_provision_serial_response_status_string(Yapi_Serial_Respons_e respond) {
  switch (respond) {
  case YAPI_SERIAL_RESPONSE_WAITING:
    return "YAPI_SERIAL_RESPONSE_WAITING";
  case YAPI_SERIAL_RESPONSE_OK:
    return "YAPI_SERIAL_RESPONSE_OK";
  case YAPI_SERIAL_RESPONSE_ERR:
    return "YAPI_SERIAL_RESPONSE_ERR";
  default:
    return "YAPI_SERIAL_RESPONSE_UNKNOWN";
  }
}
Yapi_Serial_Respons_e yapi_provision_get_serial_write_respond_status() {
  return _serialWriteRespStatus;
}

Yapi_Serial_Respons_e yapi_provision_get_serial_read_respond_status() {
  return _serialReadRespStatus;
}

void yapi_provision_send_serial_read_request() {
  yapi_service_build_send(YAPI_DEVICE_PCU, YAPI_CMD_SERIAL_READ, YAPI_MSG_GET_RQST, YAPI_PRIORITY_HIGH, NULL, NULL, 0);
  _serialReadRespStatus = YAPI_SERIAL_RESPONSE_WAITING;
}

void yapi_provision_send_serial_number(char* serialNumber, uint8_t size) {
  memcpy(_serialNumber, serialNumber, size);
  yapi_service_build_send(YAPI_DEVICE_PCU, YAPI_CMD_SERIAL_WRITE, YAPI_MSG_SET_RQST, YAPI_PRIORITY_HIGH, NULL, (uint8_t*)serialNumber, size);
  _serialWriteRespStatus = YAPI_SERIAL_RESPONSE_WAITING;
}

void yapi_provision_callbacks_init() {
  yapi_service_register_cmd_cb(_yapi_provision_serial_read_resp_cb, YAPI_CMD_SERIAL_READ);
  yapi_service_register_cmd_cb(_yapi_provision_serial_write_resp_cb, YAPI_CMD_SERIAL_WRITE);
}

#ifdef __cplusplus
}
#endif