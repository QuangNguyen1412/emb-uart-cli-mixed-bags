/**
 * yapi_flash.h
 * 
 * This file is supposed to used by yapi_manager and by who want to use yapi service for flash
 * 
 * Author: Quang Nguyen <quang.nguyen@goalzero.com>
*/

#ifndef _YAPI_FLASH_H
#define _YAPI_FLASH_H

#include <stdint.h>
#include "yapi_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YAPI_FLASH_FAIL       1
#define YAPI_FLASH_SUCCESS    0

int yapi_flash_write_request(uint32_t addressOffset, const char* content, int wordsLength);
int yapi_flash_upload_request(uint32_t addressOffset, const char* fileName);
void yapi_flash_read_request(uint32_t addressOffset, int readSize);
int yapi_flash_verify_request(uint32_t startAddressOffset, uint32_t endAddressOffset, uint16_t* crc16);
int yapi_flash_erase_request(uint32_t addressOffset, int numberOfPages);
void _yapi_flash_read_resp_cb(yapi_packet_t* yapi_pkt);
void _yapi_flash_write_resp_cb(yapi_packet_t* yapi_pkt);
void _yapi_flash_erase_resp_cb(yapi_packet_t* yapi_pkt);
void _yapi_flash_verify_resp_cb(yapi_packet_t* yapi_pkt);

#ifdef __cplusplus
}
#endif

#endif
