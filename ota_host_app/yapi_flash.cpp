/**
 * yapi_flash.cpp
 * 
 * This file is supposed to used by yapi_manager and by who want to use yapi service for flash
 * 
 * Author: Quang Nguyen <quang.nguyen@goalzero.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // read/write/close/sleep

#include "yapi_flash.h"
#include "yapi_service.h"
#include "gz_log.h"
#include "gz_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TARGET_DEVICE YAPI_DEVICE_MPPT

#define FLASH_UPLOAD_CHUNK_SIZE_BYTE      128
static bool _uploadStarted = false;
static uint32_t _uploadAddressOffset = 0;
static uint32_t _uploadCrcFirstByteOffset = 0;
static uint32_t _uploadCrcLastByteOffset = 0;
static uint16_t _uploadFileCRC = 0x0000;
static int _fd = 0;
typedef enum {
  GZM_SKU_Y6G_1000_120V = 37200,
  GZM_SKU_Y6G_1000_240V = 37210,
  GZM_SKU_Y6G_1500_120V = 37300,
  GZM_SKU_Y6G_1500_240V = 37310,
  GZM_SKU_Y6G_2000_120V = 37400,
  GZM_SKU_Y6G_2000_240V = 37410,
  GZM_SKU_Y6G_120V = 37500,
  GZM_SKU_Y6G_240V = 37510,
  GZM_SKU_HAVEN_12 = 70025,
  GZM_SKU_RVES = 45005,
  GZM_SKU_TANK = 23110,
} GZM_PLATFORM_SKU;

void yapi_flash_read_request(uint32_t addressOffset, int readSize) {
  uint8_t data[YAPI_DATA_SIZE] = { 0 };
  uint8_t index = 0;
  data[index++] = (uint8_t)addressOffset;
  data[index++] = (uint8_t)(addressOffset >> 8);
  data[index++] = (uint8_t)(addressOffset >> 16);
  data[index++] = (uint8_t)(addressOffset >> 24);
  data[index++] = readSize;
  yapi_service_build_send(TARGET_DEVICE,
                          YAPI_CMD_FLASH_READ,
                          YAPI_MSG_GET_RQST,
                          YAPI_PRIORITY_LOW,
                          NULL,
                          data,
                          index);
}

int yapi_flash_write_request(uint32_t addressOffset, const char* content, int wordsLength) {
  uint8_t payloadData[YAPI_DATA_SIZE] = { 0 };
  uint8_t index = 0;
  if (!content) {
    return 0;
  }
  payloadData[index++] = (uint8_t)addressOffset;
  payloadData[index++] = (uint8_t)(addressOffset >> 8);
  payloadData[index++] = (uint8_t)(addressOffset >> 16);
  payloadData[index++] = (uint8_t)(addressOffset >> 24);
  memcpy(payloadData + index, content, wordsLength * sizeof(uint32_t));
  GZ_LOG_INFO("yapi_flash_write_request: addressOffset[%d], dataLength_byte[%d]\n", addressOffset, index + wordsLength * sizeof(uint32_t));
  yapi_ops_status_t status = yapi_service_build_send( TARGET_DEVICE,
                                                      YAPI_CMD_FLASH_WRITE,
                                                      YAPI_MSG_SET_RQST,
                                                      YAPI_PRIORITY_LOW,
                                                      NULL,
                                                      payloadData,
                                                      index + wordsLength * sizeof(uint32_t));
  return status == YAPI_OPS_SUCCESS ? wordsLength : 0;
}

int yapi_flash_erase_request(uint32_t addressOffset, int numberOfPages) {
  uint8_t payloadData[YAPI_DATA_SIZE] = { 0 };
  uint8_t index = 0;
  payloadData[index++] = (uint8_t)addressOffset;
  payloadData[index++] = (uint8_t)(addressOffset >> 8);
  payloadData[index++] = (uint8_t)(addressOffset >> 16);
  payloadData[index++] = (uint8_t)(addressOffset >> 24);
  payloadData[index++] = numberOfPages;
  yapi_ops_status_t status = yapi_service_build_send( TARGET_DEVICE,
                                                      YAPI_CMD_FLASH_ERASE,
                                                      YAPI_MSG_SET_RQST,
                                                      YAPI_PRIORITY_LOW,
                                                      NULL,
                                                      payloadData,
                                                      index);
  return status;
}

int yapi_flash_upload_request(uint32_t addressOffset, const char* fileName) {
  _uploadStarted = true;
  _uploadAddressOffset = addressOffset;
  _uploadCrcFirstByteOffset = _uploadAddressOffset;
  _uploadCrcLastByteOffset = 0;
  _uploadFileCRC = 0x0000;
  if (_fd) {
    close(_fd);
  }
  _fd = open(fileName, O_RDONLY | O_NONBLOCK);
  if (_fd < 0) {
    // GZ_LOG_ERROR("Fail opening file\n");
    return YAPI_FLASH_FAIL;
  }
  uint8_t buffer[128] = { 0 };
  int readByte = read(_fd, buffer, sizeof(buffer));
  int readWord = readByte / sizeof(uint32_t);
  if (readByte % sizeof(uint32_t)) {
    readWord++;
  }
  _uploadFileCRC = gz_crc16_seeded(buffer, readByte, _uploadFileCRC);
  return yapi_flash_write_request(addressOffset, (const char *)buffer, readWord);
}

int yapi_flash_verify_request(uint32_t startAddressOffset, uint32_t endAddressOffset, uint16_t* crc16) {
  GZ_LOG_INFO("yapi_flash_verify_request: crcBin[0x%04X]\n", _uploadFileCRC);
  uint8_t payloadData[YAPI_DATA_SIZE] = { 0 };
  uint8_t index = 0;
  payloadData[index++] = (uint8_t)startAddressOffset;
  payloadData[index++] = (uint8_t)(startAddressOffset >> 8);
  payloadData[index++] = (uint8_t)(startAddressOffset >> 16);
  payloadData[index++] = (uint8_t)(startAddressOffset >> 24);

  payloadData[index++] = (uint8_t)endAddressOffset;
  payloadData[index++] = (uint8_t)(endAddressOffset >> 8);
  payloadData[index++] = (uint8_t)(endAddressOffset >> 16);
  payloadData[index++] = (uint8_t)(endAddressOffset >> 24);

  payloadData[index++] = (uint8_t)(*crc16);
  payloadData[index++] = (uint8_t)(*crc16 >> 8);
  yapi_service_build_send( TARGET_DEVICE,
                            YAPI_CMD_FLASH_VERIFY,
                            YAPI_MSG_GET_RQST,
                            YAPI_PRIORITY_LOW,
                            NULL,
                            payloadData,
                            index);

  return 0;
}

void _yapi_flash_read_resp_cb(yapi_packet_t* yapi_pkt) {
  GZ_LOG_INFO("_yapi_flash_read_resp_cb: type(%d)\n", yapi_pkt->messageData.type);
  GZ_LOG_INFO("_yapi_flash_read_resp_cb: package_length(%d)\n", yapi_pkt->length);
  if (yapi_pkt->messageData.type == YAPI_MSG_GET_RESP_OK) {
    uint8_t readSize = 0;
    GZ_LOG_INFO("Flash_read response data:\n");
    while (readSize < yapi_pkt->length) {
      printf("%02x\t'%c'\n", yapi_pkt->data[readSize], yapi_pkt->data[readSize++]);
    }
  }
}

void _yapi_flash_write_resp_cb(yapi_packet_t* yapi_pkt) {
  if (yapi_pkt->messageData.type == YAPI_MSG_SET_RESP_OK) {
    GZ_LOG_INFO("_yapi_flash_write_resp_cb: writtenWords[%d]\n", yapi_pkt->data[0]);
    int16_t writtenWords = yapi_pkt->data[0] | (yapi_pkt->data[1] << 8);
    uint32_t sku = GZM_SKU_Y6G_2000_120V;
    if (_uploadStarted && writtenWords) {
      uint8_t buffer[FLASH_UPLOAD_CHUNK_SIZE_BYTE] = { 0 };
      int readByte = 0;
      // MPPT addressing is done differently, must account for that here
      if (yapi_pkt->senderId == YAPI_DEVICE_MPPT && (sku == GZM_SKU_Y6G_120V || sku == GZM_SKU_Y6G_240V)) { // Need to change if we have a different MPPT MCU on the Yeti 4000
        _uploadAddressOffset += writtenWords * sizeof(uint32_t) / 2;
      } else {
        _uploadAddressOffset += writtenWords * sizeof(uint32_t);
      }
      while (readByte < FLASH_UPLOAD_CHUNK_SIZE_BYTE) {
        if (read(_fd, buffer + readByte, 1)) {
          readByte++;
        } else {
          GZ_LOG_INFO("Flash upload end\n");
          close(_fd);
          _uploadStarted = false;
          _uploadCrcLastByteOffset = readByte;
          break;
        }
      }
      _uploadFileCRC = gz_crc16_seeded(buffer, readByte, _uploadFileCRC);
      if (readByte) {
        int readWord = readByte / sizeof(uint32_t);
        if (readByte % sizeof(uint32_t)) {
          readWord++;
        }
        yapi_flash_write_request(_uploadAddressOffset, (const char *)buffer, readWord);
      }
    } else if (!_uploadStarted) {
      if (_uploadAddressOffset) {
        yapi_flash_verify_request(_uploadCrcFirstByteOffset, _uploadAddressOffset + _uploadCrcLastByteOffset, &_uploadFileCRC);
        _uploadAddressOffset = 0;
      }
    }
  } else if (yapi_pkt->messageData.type == YAPI_MSG_UNSOLICITED) {
    GZ_LOG_INFO("UNSOLICITED write writtenWords[%d]/total[%d]\n", yapi_pkt->data[1], yapi_pkt->data[3]);
  }
}

void _yapi_flash_erase_resp_cb(yapi_packet_t* yapi_pkt) {
  uint8_t readSize = 0;
  if (yapi_pkt->messageData.type == YAPI_MSG_SET_RESP_OK) {
    GZ_LOG_INFO("Flash_erase success:\n");
  } else {
    GZ_LOG_INFO("Flash_erase FAILED:\n");
  }
  while (readSize < yapi_pkt->length) {
    printf("%02x\t'%c'\n", yapi_pkt->data[readSize], yapi_pkt->data[readSize++]);
  }
}

void _yapi_flash_verify_resp_cb(yapi_packet_t* yapi_pkt) {
  if (yapi_pkt->messageData.type == YAPI_MSG_GET_RESP_OK) {
    GZ_LOG_INFO("_yapi_flash_verify_resp_cb: crcMatch[SUCCESS]\n");
  } else if (yapi_pkt->messageData.type == YAPI_MSG_UNSOLICITED) {
    GZ_LOG_INFO("_yapi_flash_verify_resp_cb YAPI_MSG_UNSOLICITED: crcBLResp[0x%02X%02X]\n", yapi_pkt->data[1], yapi_pkt->data[0]);
  } else {
    GZ_LOG_INFO("_yapi_flash_verify_resp_cb: crcMatch[FAIL] crcBLResp[0x%02X%02X]\n", yapi_pkt->data[1], yapi_pkt->data[0]);
  }
}

#ifdef __cplusplus
}
#endif
