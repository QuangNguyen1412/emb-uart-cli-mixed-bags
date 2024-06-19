/*
 * yapi_service.cpp
 *
 *  Created on: Jun 1, 2022
 *      Author: zacharygarrard
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "yapi_service.h"
#ifdef BOOTLOADER_BUILD
#include "gz_hash/gz_hash.h" // TODO: we could allow dependency injection for this as well
#else
#include "../gz_hash/gz_hash.h" // TODO: we could allow dependency injection for this as well
#endif
#include <stdio.h>
#include <string.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

/**
 * @brief These enumed values describe the possible states of the yapi packet receiver state machine.
 * 
 */
typedef enum {
  YAPI_SERVICE_START_1,
  YAPI_SERVICE_START_2,
  YAPI_SERVICE_LENGTH,
  YAPI_SERVICE_HEADER,
  YAPI_SERVICE_PAYLOAD,
  YAPI_SERVICE_CRC_1,
  YAPI_SERVICE_CRC_2
} yapi_service_recption_state_enum_t;

yapi_register_rx_byte_func_t _registerRxByteFunc;
/**
 * @brief The buffer to hold incoming bytes from the sending module waiting for processing. This buffer is circular, meaning
 * it wraps around to the beginning when full and begins writing over older data.
 * 
 * At 115200 Baud, that is about 14400 Bytes/sec, or a little more than 69 us per byte. So an extra 256 bytes of room 
 * gives us about 35 ms to process the first packet of 256 bytes before we run out of buffer space for another 256 size packet.
 */
static uint8_t _receiveBuff[RECEIVE_BUFFER_LENGTH]; 

/**
 * @brief The buffer to fill with the contents of a possible yapi packet once the start signal has been received. This buffer is 
 * NOT circular, and is intended to only hold a single YAPI packet at a time for further processing.
 * 
 */
static uint8_t _processingBuff[PROCESSING_BUFFER_LENGTH];

/**
 * @brief A @ref yapi_packet_t pointer to the processing buffer. Useful for parsing the packet.
 * 
 */
static yapi_packet_t* _pPacket = (yapi_packet_t*) &_processingBuff;

/**
 * @brief The receiver processing state as defined in the @ref yapi_service_recption_state_enum_t enum
 * 
 */
static yapi_service_recption_state_enum_t _processingState;

/**
 * @brief The index of the receive buffer to the location of the next position for an incoming byte to
 * be written to. If the Head wraps around to the tail and another incoming byte is added, the Tail is 
 * advanced to maintain buffer integrity.
 * 
 */
volatile uint16_t _receiveBuffHead;

/**
 * @brief The index of the receive buffer to the location of the last popped off byte from the FIFO buffer.
 * If the Head wraps around to the tail and another incoming byte is added, the Tail is advanced to maintain 
 * buffer integrity.
 * 
 */
volatile uint16_t _receiveBuffTail;

static yapi_device_id_enum_t _yapiSelfDeviceId = __YAPI_DEVICE_UNKNOWN;

///////////////////////////
// YAPI CALLBACK FUNCTIONS: these should mirror the yapi_command_enum_t enumeration.
///////////////////////////
/**
 * @brief A callback function pointer to handle incoming HELLO command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_hello_cb = NULL;

#ifndef BOOTLOADER_BUILD
/**
 * @brief A callback function pointer to handle incoming summary status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming verbose status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_verbose_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming config command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_config_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU output port status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_output_ports_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU input port status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_input_ports_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU summary status flag command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_summary_status_flags_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU summary status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU output status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_outputs_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU input status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_inputs_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU config command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_configs_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU PMICs status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_pmics_status_cb = NULL;

/**

 * @brief A callback function pointer to handle incoming PCU PMICs accumulated energy command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_pmics_accumulated_energy_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming PCU device status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_pcu_device_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming config command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_wmu_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming WMU OTA status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_wmu_ota_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming WMU config command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_wmu_configs_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming WMU ota start messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_wmu_ota_start_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming WMU power messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_wmu_power_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming inverter summary status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_inverter_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming BMS summary status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_bms_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming Tank summary status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_tank_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming Link summary status command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_link_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle RVES command messages.
*/
static v_fp_yapi_ptr_t _yapi_cmd_rves_summary_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming print log command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_print_log_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming file log command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_log_to_file_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming diagnostic page display command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_display_diagnostic_page_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming file log command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_hep_pcu_set_circuit_states_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming file log command messages.
 * 
 */
static v_fp_yapi_ptr_t _yapi_cmd_hep_pcu_set_power_configuration_states_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming hep yeti status command messages.
*/
static v_fp_yapi_ptr_t _yapi_cmd_hep_yeti_status_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming hep yeti control command messages.
*/
static v_fp_yapi_ptr_t _yapi_cmd_hep_yeti_control_cb = NULL;

/**
 * @brief A callback function pointer to handle incoming hep yeti demo mode command messages.
*/
static v_fp_yapi_ptr_t _yapi_cmd_hep_yeti_demo_mode_cb = NULL;

/**
 * @brief A callback function to handle incoming AC line state messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_hep_line_states_cb = NULL;

/**
 * @brief A callback function to handle incoming yeti pairing state messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_hep_yeti_pairing_states_cb = NULL;

/**
 * @brief A callback function to handle incoming 240V breaker presence messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_hep_240v_breaker_presence_cb = NULL;

/**
 * @brief A callback function pointer to handle boot info get.
 */
static v_fp_yapi_ptr_t _yapi_cmd_modbus_bootinfo = NULL;
/**
 * @brief A callback function pointer to handle fw version get.
 */
static v_fp_yapi_ptr_t _yapi_cmd_modbus_get_fw_version = NULL;

/* @brief A callback function pointer to handle request paired tank. */
static v_fp_yapi_ptr_t _yapi_cmd_modbus_get_paired_tank = NULL;
/**
 * @brief A callback function pointer to handle modbus silence.
 */
static v_fp_yapi_ptr_t _yapi_cmd_modbus_silence = NULL;
/**
 * @brief A callback function pointer to handle bootloader jump.
 */
static v_fp_yapi_ptr_t _yapi_cmd_modbus_enter_bootloader = NULL;

/* A callback function pointer to toggle ac port. */
static v_fp_yapi_ptr_t _yapi_cmd_port_ac_toggle = NULL;

/* A callback function pointer to put ac port to update mode. */
static v_fp_yapi_ptr_t _yapi_cmd_port_ac_update_mode = NULL;

/* A callback function pointer to toggle 12v port. */
static v_fp_yapi_ptr_t _yapi_cmd_port_12v_toggle = NULL;

/* A callback function pointer to put 12v port to update mode. */
static v_fp_yapi_ptr_t _yapi_cmd_port_12v_update_mode = NULL;

/* A callback function pointer to toggle usb port. */
static v_fp_yapi_ptr_t _yapi_cmd_port_usb_toggle = NULL;

/* A callback function pointer to handle combiner summary status requests/responses. */
static v_fp_yapi_ptr_t _yapi_cmd_combiner_summary_status_cb = NULL;

/* A callback function pointer to handle serial number write requests/responses. */
static v_fp_yapi_ptr_t _yap_cmd_serial_write = NULL;

/* A callback function pointer to handle serial number read requests/responses. */
static v_fp_yapi_ptr_t _yap_cmd_serial_read = NULL;

/* A callback function pointer to handle system id read requests/responses. */
static v_fp_yapi_ptr_t _yapi_cmd_wmu_system_id = NULL;
#endif

/**
 * @brief A callback function pointer to handle incoming flash read command messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_flash_read = NULL;

/**
 * @brief A callback function pointer to handle incoming flash write command messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_flash_write = NULL;

/**
 * @brief A callback function pointer to handle incoming flash erase command messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_flash_erase = NULL;

/**
 * @brief A callback function pointer to handle incoming flash erase command messages.
 */
static v_fp_yapi_ptr_t _yapi_cmd_flash_verify = NULL;

/**
 * @brief Performs a lookup for a given yapi command and returns a reference to its registered
 * callback function.
 * 
 * @param cmd A @ref yapi_command_enum_t enumerated command endpoint
 * @return v_fp_yapi_ptr_t* pointer to the callback function registered for the given command. Returns
 * NULL if no callback function is registered for the provided command.
 */
v_fp_yapi_ptr_t* _yapi_cmd_to_cb(yapi_command_enum_t cmd);

/**
 * @brief Buffers incoming bytes from the UART hardware for later processing in the yapi 10 ms task.
 * 
 * @param incomingByte The incoming byte from the UART hardware peripheral.
 */
void _receive_incoming_byte(uint8_t incomingByte);

/**
 * @brief Processes the received packet once we believe we have obtained a packet. Performs a CRC 
 * calculation on the received data to verify packet integrity and calls the registered callback 
 * function for the current packet's command if it is registered.
 * 
 */
void _process_received_packet(void);

/**
 * @brief Receives @ref len bytes from the UART peripheral. Blocking until the designated number of bytes is received.
 * 
 * @param data A pointer to the data buffer to populate with received bytes.
 * @param len The number of expected bytes to wait for reception of.
 */
void _receiveBlocking(uint8_t* data, uint8_t len);

void yapi_service_init(yapi_device_id_enum_t deviceId, yapi_register_rx_byte_func_t registerRxByteFunc) {
  memset(_receiveBuff, 0x00, sizeof(_receiveBuff));
  _receiveBuffHead = 0;
  _receiveBuffTail = 0;
  _processingState = YAPI_SERVICE_START_1;
  yapi_platform_log_debug("DEBUG: yapi_service_init\n");
  if (registerRxByteFunc) {
    if (_registerRxByteFunc) {
      _registerRxByteFunc(NULL); // De-register the current function
    }
    _registerRxByteFunc = registerRxByteFunc;
    _registerRxByteFunc(_receive_incoming_byte);
  }

  _yapiSelfDeviceId = deviceId;
}

// TODO: add a timeout mechanism to set the processing state back to START_1 if we haven't received the expected number of bytes in a reasonable amount of time.
void yapi_service_task_10ms(void* param) {
  static uint8_t payloadLen;
  static uint16_t processIdx = 0;
  uint8_t incomingByte;

  while (_receiveBuffTail != _receiveBuffHead) { // _receiveBuffHead can move as this loop is executing asynchronously as interrupts occur.
    incomingByte = _receiveBuff[_receiveBuffTail];

    switch(_processingState) {
      case YAPI_SERVICE_START_1:
        if (incomingByte == YAPI_START_BYTE) {
          _processingBuff[processIdx++] = incomingByte;
          _processingState = YAPI_SERVICE_START_2;
        }
        break;

      case YAPI_SERVICE_START_2:
        if (incomingByte == YAPI_START_BYTE) {
          _processingBuff[processIdx++] = incomingByte;
          _processingState = YAPI_SERVICE_LENGTH;
        } else {
          _processingState = YAPI_SERVICE_START_1;
          processIdx = 0;
        }
        break;

      case YAPI_SERVICE_LENGTH:
        _processingBuff[processIdx++] = incomingByte;
        payloadLen = incomingByte;
        _processingState = YAPI_SERVICE_HEADER;
        break;

      case YAPI_SERVICE_HEADER:
        _processingBuff[processIdx++] = incomingByte;

        if (processIdx == YAPI_HEADER_LENGTH) {
          _processingState = (payloadLen > 0) ? YAPI_SERVICE_PAYLOAD : YAPI_SERVICE_CRC_1;
        }
        break;

      case YAPI_SERVICE_PAYLOAD:  // It is useful to separate the payload and header processing for debugging.
        _processingBuff[processIdx++] = incomingByte;

        if (processIdx == (payloadLen + YAPI_HEADER_LENGTH)) {
          _processingState = YAPI_SERVICE_CRC_1;
        }
        break;

      case YAPI_SERVICE_CRC_1:
        _processingBuff[processIdx++] = incomingByte;
        _processingState = YAPI_SERVICE_CRC_2;

        break;

      case YAPI_SERVICE_CRC_2:
        _processingBuff[processIdx++] = incomingByte;
        _process_received_packet();

        payloadLen = 0;
        _processingState = YAPI_SERVICE_START_1;
        processIdx = 0;
        break;

      default:
        _processingState = YAPI_SERVICE_START_1;
        processIdx = 0;
        break;
    }
  
    _receiveBuffTail = _receiveBuffTail == (RECEIVE_BUFFER_LENGTH - 1) ? 0 : _receiveBuffTail + 1;
  }

  UNUSED(param);
}

yapi_ops_status_t yapi_service_register_cmd_cb(v_fp_yapi_ptr_t cb, yapi_command_enum_t cmd) {
  yapi_ops_status_t result = YAPI_OPS_FAIL;

  if (cb) {
    *_yapi_cmd_to_cb(cmd) = cb;
    result = YAPI_OPS_SUCCESS;
  }

  return result;
}

yapi_ops_status_t yapi_service_build_pkt(yapi_packet_t* pkt,
  yapi_device_id_enum_t senderId,
  yapi_device_id_enum_t targetId,
  yapi_command_enum_t command,
  yapi_message_type_enum_t messageType,
  yapi_message_priority_enum_t priority,
  uint8_t *options,
  uint8_t *data,
  uint8_t length) {
  uint16_t crc;

  if (length > 238) {
    return YAPI_OPS_FAIL;
  }

  pkt->start = YAPI_START_SIGNAL;
  pkt->length = length;
  pkt->apiVersion = YAPI_API_VERSION;
  pkt->command = command;
  pkt->messageData.type = messageType;
  pkt->messageData.priority = priority;
  pkt->senderId = senderId;
  pkt->targetId = targetId;
  if (options) {
    memcpy(&pkt->options, options, sizeof(pkt->options));
  }
  memset(&pkt->_reserved, 0xFF, sizeof(pkt->_reserved));
  if (data) {
    memcpy(&pkt->data, data, length);
  }

#ifdef BOOTLOADER_BUILD
  crc = gz_crc16_size_optimized((uint8_t *) pkt, YAPI_HEADER_LENGTH + length);
#else
  crc = gz_crc16((uint8_t *) pkt, YAPI_HEADER_LENGTH + length);
#endif
  memcpy(&(pkt->data[length]), &crc, sizeof(crc));
  yapi_platform_log_debug("The computed CRC for the following msg is 0x%.4x", crc);
  
  return YAPI_OPS_SUCCESS;
}

// Only blocks if the hardware buffer gets full and returns when the contents of the data buffer are fully enqueued in the outgoing hardware buffer
yapi_ops_status_t yapi_service_send(yapi_packet_t* pkt) {
  uint16_t totalFrameLen = YAPI_HEADER_LENGTH + pkt->length + sizeof(pkt->crc);
  uint8_t* pktCharPtr = (uint8_t *) pkt;

  if (pkt->length > 238) {
    return YAPI_OPS_FAIL;
  }

  yapi_platform_log_debug("\nSending YAPI msg: 0x");
  for (uint16_t i = 0; i < totalFrameLen; i++) {
    yapi_platform_log_debug("%.2x ", pktCharPtr[i]);
  }
  yapi_platform_log_debug("\n\n");

  if (yapi_platform_transmit((uint8_t *) pkt, totalFrameLen) == totalFrameLen) {
    return YAPI_OPS_SUCCESS;
    yapi_platform_log_debug("YAPI SERVICE message sent successfully");
  }
  yapi_platform_log_error("YAPI SERVICE FAILED TO SEND MESSAGE!");

  return YAPI_OPS_FAIL;
}

yapi_ops_status_t yapi_service_build_send(yapi_device_id_enum_t targetId,
  yapi_command_enum_t command,
  yapi_message_type_enum_t messageType,
  yapi_message_priority_enum_t priority,
  uint8_t *options,
  uint8_t *data,
  uint8_t length) {
  yapi_packet_t pkt;

  if (yapi_service_build_pkt(&pkt,
                              _yapiSelfDeviceId,
                              targetId,
                              command,
                              messageType,
                              priority,
                              options,
                              data,
                              length) == YAPI_OPS_SUCCESS) {
                                return yapi_service_send(&pkt);
                              }

  return YAPI_OPS_FAIL;
}

yapi_ops_status_t yapi_service_build_send_ID(yapi_device_id_enum_t senderId,
  yapi_device_id_enum_t targetId,
  yapi_command_enum_t command,
  yapi_message_type_enum_t messageType,
  yapi_message_priority_enum_t priority,
  uint8_t *options,
  uint8_t *data,
  uint8_t length) {
  yapi_packet_t pkt;

  if (yapi_service_build_pkt(&pkt,
                            senderId,
                            targetId,
                            command,
                            messageType,
                            priority,
                            options,
                            data,
                            length) == YAPI_OPS_SUCCESS) {
                              return yapi_service_send(&pkt);
                           }
  return YAPI_OPS_FAIL;
}


v_fp_yapi_ptr_t* _yapi_cmd_to_cb(yapi_command_enum_t cmd) {
  v_fp_yapi_ptr_t* cb = NULL;

  switch (cmd) {
    case YAPI_CMD_HELLO:
      cb = &_yapi_cmd_hello_cb;
      break;
#ifndef BOOTLOADER_BUILD
    case YAPI_CMD_SUMMARY_STATUS:
      cb = &_yapi_cmd_summary_status_cb;
      break;
    case YAPI_CMD_VERBOSE_STATUS:
      cb = &_yapi_cmd_verbose_status_cb;
      break;
    case YAPI_CMD_CONFIG:
      cb = &_yapi_cmd_config_cb;
      break;
    case YAPI_CMD_PCU_OUTPUT_PORTS_STATUS:
      cb = &_yapi_cmd_pcu_output_ports_status_cb;
      break;
    case YAPI_CMD_PCU_INPUT_PORTS_STATUS:
      cb = &_yapi_cmd_pcu_input_ports_status_cb;
      break;
    case YAPI_CMD_PCU_SUMMARY_STATUS_FLAGS:
      cb = &_yapi_cmd_pcu_summary_status_flags_cb;
      break;
    case YAPI_CMD_PCU_SUMMARY_STATUS:
      cb = &_yapi_cmd_pcu_summary_status_cb;
      break;
    case YAPI_CMD_PCU_OUTPUTS_STATUS:
      cb = &_yapi_cmd_pcu_outputs_status_cb;
      break;
    case YAPI_CMD_PCU_INPUTS_STATUS:
      cb = &_yapi_cmd_pcu_inputs_status_cb;
      break;
    case YAPI_CMD_PCU_CONFIGS:
      cb = &_yapi_cmd_pcu_configs_cb;
      break;
    case YAPI_CMD_PCU_PMICS_STATUS:
      cb = &_yapi_cmd_pcu_pmics_status_cb;
      break;
    case YAPI_CMD_PCU_PMICS_ACCUMULATED_ENERGY:
      cb = &_yapi_cmd_pcu_pmics_accumulated_energy_cb;
      break;
    case YAPI_CMD_PCU_DEVICE:
      cb = &_yapi_cmd_pcu_device_cb;
      break;
    case YAPI_CMD_WMU_SUMMARY_STATUS:
      cb = &_yapi_cmd_wmu_summary_status_cb;
      break;
    case YAPI_CMD_WMU_OTA_STATUS:
      cb = &_yapi_cmd_wmu_ota_status_cb;
      break;
    case YAPI_CMD_WMU_CONFIGS:
      cb = &_yapi_cmd_wmu_configs_cb;
      break;
    case YAPI_CMD_WMU_OTA_START:
      cb = &_yapi_cmd_wmu_ota_start_cb;
      break;
    case YAPI_CMD_WMU_POWER:
      cb = &_yapi_cmd_wmu_power_cb;
      break;
    case YAP_CMD_INVERTER_SUMMARY_STATUS:
      cb = &_yapi_cmd_inverter_summary_status_cb;
      break;
    case YAPI_CMD_BMS_SUMMARY_STATUS:
      cb = &_yapi_cmd_bms_summary_status_cb;
      break;
    case YAPI_CMD_TANK_SUMMARY_STATUS:
      cb = &_yapi_cmd_tank_summary_status_cb;
      break;
    case YAPI_CMD_LINK_SUMMARY_STATUS:
      cb = &_yapi_cmd_link_summary_status_cb;
      break;
    case YAPI_CMD_RVES_SUMMARY_STATUS:
      cb = &_yapi_cmd_rves_summary_status_cb;
      break;
    case YAPI_CMD_PRINT_LOG:
      cb = &_yapi_cmd_print_log_cb;
      break;
    case YAPI_CMD_LOG_TO_FILE:
      cb = &_yapi_cmd_log_to_file_cb;
      break;
    case YAPI_CMD_DISPLAY_DIAGNOSTIC_PAGE:
      cb = &_yapi_cmd_display_diagnostic_page_cb;
      break;
    case YAPI_CMD_HEP_PCU_SET_CIRCUIT_STATES:
      cb = &_yapi_cmd_hep_pcu_set_circuit_states_cb;
      break;
    case YAPI_CMD_HEP_PCU_SET_POWER_CONFIGURATION_STATES:
      cb = &_yapi_cmd_hep_pcu_set_power_configuration_states_cb;
      break;
    case YAPI_CMD_HEP_YETI_STATUS:
      cb = &_yapi_cmd_hep_yeti_status_cb;
      break;
    case YAPI_CMD_HEP_YETI_CONTROL:
      cb = &_yapi_cmd_hep_yeti_control_cb;
      break;
    case YAPI_CMD_HEP_YETI_DEMO_MODE:
      cb = &_yapi_cmd_hep_yeti_demo_mode_cb;
      break;
    case YAPI_CMD_HEP_LINE_STATES:
      cb = &_yapi_cmd_hep_line_states_cb;
      break;
    case YAPI_CMD_HEP_YETI_PAIRING_STATES:
      cb = &_yapi_cmd_hep_yeti_pairing_states_cb;
      break;
    case YAPI_CMD_HEP_240V_BREAKER_PRESENCE:
      cb = &_yapi_cmd_hep_240v_breaker_presence_cb;
      break;
    case YAPI_CMD_MODBUS_ENTER_BOOTLOADER:
      cb = &_yapi_cmd_modbus_enter_bootloader;
      break;
    case YAPI_CMD_MODBUS_SILENCE:
      cb = &_yapi_cmd_modbus_silence;
      break;
    case YAPI_CMD_MOBUS_GET_BOOTINFO:
      cb = &_yapi_cmd_modbus_bootinfo;
      break;
    case YAPI_CMD_MOBUS_GET_FW_VN:
      cb = &_yapi_cmd_modbus_get_fw_version;
      break;
    case YAPI_CMD_MODBUS_GET_PAIRED_TANKS:
      cb = &_yapi_cmd_modbus_get_paired_tank;
      break;
    case YAPI_CMD_PORT_AC_TOGGLE:
      cb = &_yapi_cmd_port_ac_toggle;
      break;
    case YAPI_CMD_PORT_AC_UPDATE_MODE:
      cb = &_yapi_cmd_port_ac_update_mode;
      break;
    case YAPI_CMD_PORT_12V_TOGGLE:
      cb = &_yapi_cmd_port_12v_toggle;
      break;
    case YAPI_CMD_PORT_12V_UPDATE_MODE:
      cb = &_yapi_cmd_port_12v_update_mode;
      break;
    case YAPI_CMD_PORT_USB_TOGGLE:
      cb = &_yapi_cmd_port_usb_toggle;
      break;
    case YAPI_CMD_COMBINER_SUMMARY_STATUS:
      cb = &_yapi_cmd_combiner_summary_status_cb;
      break;
    case YAPI_CMD_SERIAL_WRITE:
      cb = &_yap_cmd_serial_write;
      break;
    case YAPI_CMD_SERIAL_READ:
      cb = &_yap_cmd_serial_read;
      break;
    case YAPI_CMD_WMU_SYSTEM_ID:
      cb = &_yapi_cmd_wmu_system_id;
      break;
#endif
    case YAPI_CMD_FLASH_READ:
      cb = &_yapi_cmd_flash_read;
      break;
    case YAPI_CMD_FLASH_WRITE:
      cb = &_yapi_cmd_flash_write;
      break;
    case YAPI_CMD_FLASH_ERASE:
      cb = &_yapi_cmd_flash_erase;
      break;
    case YAPI_CMD_FLASH_VERIFY:
      cb = &_yapi_cmd_flash_verify;
      break;
    default:
      break;
  }

  return cb;
}

void _process_received_packet(void) {
  uint16_t payloadLen;
  uint16_t packetLen;
  v_fp_yapi_ptr_t *cb = NULL;
  uint16_t received_crc;

  // Add 2 to the start of packet index to get the length position.
  payloadLen = _pPacket->length;

  // Now packetLen holds the total number of bytes in the message frame, minus the CRC itself.
  packetLen = payloadLen + YAPI_HEADER_LENGTH;

  received_crc = _processingBuff[packetLen] | (_processingBuff[packetLen+1] << 8);
#ifdef BOOTLOADER_BUILD
  if (received_crc == gz_crc16_size_optimized(&_processingBuff[0], packetLen)) {
#else
  if (received_crc == gz_crc16(&_processingBuff[0], packetLen)) {
#endif
    // Call a specific YAPI callback.
    cb = _yapi_cmd_to_cb((yapi_command_enum_t) _pPacket->command); // Dereference NULL would crash the application if command not found
    if (cb != NULL && *cb != NULL) {
      (*(cb))(_pPacket);
    }
  }
}

void _receive_incoming_byte(uint8_t incomingByte) {
  _receiveBuff[_receiveBuffHead] = incomingByte;
  _receiveBuffHead = _receiveBuffHead == (RECEIVE_BUFFER_LENGTH - 1) ? 0 : _receiveBuffHead + 1;

  // if the head has wrapped and caught up to the tail, the tail needs to also advance one to always stay behind the head
  if (_receiveBuffHead == _receiveBuffTail) {
    _receiveBuffTail = _receiveBuffTail == (RECEIVE_BUFFER_LENGTH - 1) ? 0 : _receiveBuffTail + 1;
  }
}

#ifdef __cplusplus
}
#endif
