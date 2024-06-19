/*
 * yapi_service_platform.c
 *
 *  Created on: Aug 18, 2022
 *      Author: quang.nguyen <quang.nguyen@goalzero.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TODO: Need to move this file into the yapi service submodule shortly to help cut down
 * on unnecessary duplication of common code in multiple projects that needs to be in sync.
 * Moving this into the submodule will make it easier to avoid copying and pasting code changes.
 * We will need to use macros in here specific to each platform to avoid platform specific
 * function calls from being used in the wrong platform.
 */
#include <unistd.h>

#include "yapi_service_driver.h"
#include "yapi_service.h"
#include "uart.h"

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

void yapi_service_driver_init() {
  yapi_service_init(YAPI_DEVICE_EXTERNAL_PC, &uart_register_read_one_byte_callback);
}

void yapi_service_driver_10ms(void* params) {
  yapi_service_task_10ms(params);
}

void yapi_platform_log_debug(const char *fmt, ...) {
  UNUSED(fmt);
}

void yapi_platform_log_info(const char *fmt, ...) {
  UNUSED(fmt);
}

void yapi_platform_log_warn(const char *fmt, ...) {
  UNUSED(fmt);
}

void yapi_platform_log_error(const char *fmt, ...) {
  UNUSED(fmt);
}

uint16_t yapi_platform_transmit(uint8_t* data, uint16_t len) {
  uint8_t* transmitIdx = data;
  uint16_t bytesSent = 0;
  uint16_t bytesAddedToBuff = 0;
  uint8_t timeout_ms = 0;
  int fd = uart_get_connected_device();
  if (fd == UART_UNCONNECTED) {
    return 0;
  }

  // TODO: Make sure this isn't too aggressive trying to add bytes to the buffer.
  while (bytesSent < len && timeout_ms < YAPI_RECOMMENDED_SEND_TIMEOUT_MS) {
    bytesAddedToBuff = uart_write(fd, transmitIdx, len - bytesSent);
    transmitIdx += bytesAddedToBuff; // advance the data pointer by the number of bytes sent.
    bytesSent += bytesAddedToBuff;
    if (bytesSent < len) {
      usleep(1000);
      timeout_ms++;
    }
  }

  return bytesSent;
}

uint16_t yapi_platform_transmit_blocking(uint8_t* data, uint16_t len) {
  int fd = uart_get_connected_device();
  if (fd == UART_UNCONNECTED) {
    return 0;
  }
  // Lets not block for now
  return uart_write(fd, data, len);
}

#ifdef __cplusplus
}
#endif
