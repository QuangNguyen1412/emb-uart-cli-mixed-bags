/*
 * yapi_service.h
 *
 *  Created on: Jun 1, 2022
 *      Author: zacharygarrard
 */

#ifndef YETI_HAL_INCLUDES_YAPI_SERVICE_H_
#define YETI_HAL_INCLUDES_YAPI_SERVICE_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief YAPI ?? Yeti Communications API
 * Terms:
 *   - status: read-only data. If a type is named as "status" the data is readonly, reported by the component.
 *   - config: read/write data. If a type is named as "config" the data is read reported, and can be written as desired.
 *
 * @note While there's a lot here, no single part is complete, this is just an example to communicate the concept.
 */

#define YAPI_API_VERSION 2
#define YAPI_START_BYTE 0xAA
#define YAPI_START_SIGNAL 0xAAAA
#define YAPI_RECOMMENDED_SEND_TIMEOUT_MS 25
#define YAPI_DATA_SIZE 238

typedef enum {
  YAPI_OPS_SUCCESS,
  YAPI_OPS_FAIL
} yapi_ops_status_t;

typedef enum {
  __YAPI_DEVICE_UNKNOWN = 0,
  YAPI_DEVICE_WMU = 1,
  YAPI_DEVICE_PCU = 2,
  YAPI_DEVICE_BMS = 3,
  YAPI_DEVICE_INVERTER = 4,
  YAPI_DEVICE_MPPT = 5,
  YAPI_DEVICE_LINK = 6,
  YAPI_DEVICE_INV_COMBINER = 7,
  YAPI_DEVICE_INV_HV = 8,
  YAPI_DEVICE_TANK = 9,
  YAPI_DEVICE_EXTERNAL_PC = 10,
  /* Tanks IDs are dynamically assigned by the PCU. They are always in the range 0x11 - 0x18 */
  YAPI_DEVICE_TANK_1 = 11,
  YAPI_DEVICE_TANK_2 = 12,
  YAPI_DEVICE_TANK_3 = 13,
  YAPI_DEVICE_TANK_4 = 14,
  YAPI_DEVICE_TANK_5 = 15,
  YAPI_DEVICE_TANK_6 = 16,
  YAPI_DEVICE_TANK_7 = 17,
  YAPI_DEVICE_TANK_8 = 18,
  YAPI_DEVICE_RVES = 19,
} yapi_device_id_enum_t;

typedef enum {
  /// [0-9] common high level commands supported by all (most) YAPI devices
  YAPI_CMD_HELLO = 0, // if HELLO request received, send HELLO response, data expected to be empty
  YAPI_CMD_SUMMARY_STATUS = 1, // read-only status (must be able to fit in single packet), GET/SUB, no SET
  YAPI_CMD_VERBOSE_STATUS = 2, // read-only detailed status (may be more than one packet), GET/SUB, no SET
  YAPI_CMD_CONFIG = 3, // read/write config (high-level writable configurations) GET/SUB/SET
  /// [10-19] PCU specific statuses/commands
  YAPI_CMD_PCU_OUTPUT_PORTS_STATUS = 10, // GET/SUB
  YAPI_CMD_PCU_INPUT_PORTS_STATUS = 11, // GET/SUB
  YAPI_CMD_PCU_SUMMARY_STATUS_FLAGS = 12,
  YAPI_CMD_PCU_SUMMARY_STATUS = 13,
  YAPI_CMD_PCU_OUTPUTS_STATUS = 14,
  YAPI_CMD_PCU_INPUTS_STATUS = 15,
  YAPI_CMD_PCU_CONFIGS = 16,
  YAPI_CMD_PCU_PMICS_STATUS = 17,
  YAPI_CMD_PCU_DEVICE = 18,
  YAPI_CMD_PCU_PMICS_ACCUMULATED_ENERGY = 19,
  /// [20-29] WMU specific commands
  YAPI_CMD_WMU_SUMMARY_STATUS = 20,
  YAPI_CMD_WMU_OTA_STATUS = 21,
  YAPI_CMD_WMU_CONFIGS = 22,
  YAPI_CMD_WMU_OTA_START = 23,
  YAPI_CMD_WMU_POWER = 24,
  /// [30-39] INVERTER specific commands
  YAP_CMD_INVERTER_SUMMARY_STATUS = 30,
  /// [40-49] BMS specific commands
  YAPI_CMD_BMS_SUMMARY_STATUS = 40,
  /// [50-59] TANK specific commands
  YAPI_CMD_TANK_SUMMARY_STATUS = 50,
  /// [60-69] LINK specific commands
  YAPI_CMD_LINK_SUMMARY_STATUS = 60,
  YAPI_CMD_RVES_SUMMARY_STATUS = 61,
  /// [70-79] BOOTLOADER specific commands
  YAPI_CMD_FLASH_READ = 70,
  YAPI_CMD_FLASH_WRITE = 71,
  YAPI_CMD_FLASH_ERASE = 72,
  YAPI_CMD_FLASH_VERIFY = 73,
  YAPI_CMD_FLASH_LOCK = 74,

  /// [80-89] Yapi-modbus command
  YAPI_CMD_MODBUS_ENTER_BOOTLOADER = 80,
  YAPI_CMD_MODBUS_SILENCE = 81,
  YAPI_CMD_MOBUS_GET_BOOTINFO = 82,
  YAPI_CMD_MOBUS_GET_FW_VN = 83,
  YAPI_CMD_MODBUS_GET_PAIRED_TANKS = 84,

  /// [90-99] Yapi-port
  YAPI_CMD_PORT_AC_TOGGLE = 90, // Do app_port_ac_toggle()
  YAPI_CMD_PORT_AC_UPDATE_MODE = 91, // Do app_port_ac_update_mode()
  YAPI_CMD_PORT_12V_TOGGLE = 92, // Do app_port_12v_out_toggle()
  YAPI_CMD_PORT_12V_UPDATE_MODE = 93,
  YAPI_CMD_PORT_USB_TOGGLE = 94, // Do app_port_usb_out_toggle()
  /// [100-109]
  YAPI_CMD_PRINT_LOG = 100, // data is string, if device has logging capability, should log the message
  YAPI_CMD_LOG_TO_FILE = 101, // data is string, if device has logging + File System, should log message to file, else
  YAPI_CMD_DISPLAY_DIAGNOSTIC_PAGE = 102, // data is string, if device has display, should display the message
  /// [110-120] HEP controls
  YAPI_CMD_HEP_PCU_SET_CIRCUIT_STATES = 110, // Payload is a app_circuit_manager_circuit_states_t instance.
  YAPI_CMD_HEP_PCU_SET_POWER_CONFIGURATION_STATES = 111, // Payload is a app_power_configuration_manager_states_t instance.
  YAPI_CMD_HEP_YETI_STATUS = 112,
  YAPI_CMD_HEP_YETI_CONTROL = 113,
  YAPI_CMD_HEP_YETI_DEMO_MODE = 114,
  YAPI_CMD_HEP_LINE_STATES = 115, // Payload is a yapi_hep_line_state_t instance
  YAPI_CMD_HEP_YETI_PAIRING_STATES = 116, //Payload is a yapi_hep_yeti_pairing_states_t instance.
  YAPI_CMD_HEP_240V_BREAKER_PRESENCE = 117, //Payload is a yapi_hep_240V_breaker_presence_t instance.
  /// [120-130] Misc commands
  YAPI_CMD_COMBINER_SUMMARY_STATUS = 120,
  /// [200-210] Factory controls
  YAPI_CMD_SERIAL_WRITE = 200, 
  YAPI_CMD_SERIAL_READ = 201, 
  YAPI_CMD_WMU_SYSTEM_ID = 202, // Payload is yapi_system_id_t
} yapi_command_enum_t;

typedef enum {
  YAPI_ERR_NONE = 0,
  YAPI_ERR_COMMAND_NOT_SUPPORTED = 1, // command is recognized, but not supported by device
  YAPI_ERR_COMMAND_NOT_RECOGNIZED = 2, // command is not recognized by the device
  YAPI_ERR_CRC_ERROR = 3, // the CRC didn't match
  YAPI_ERR_DATA_ERROR = 4, // there was an error processing the given data
} yapi_options_message_resp_error_enum_t;

typedef enum {
  YAPI_MSG_GET_RQST = 0x00, // sender is asking to get data
  YAPI_MSG_GET_RESP_OK = 0x10, // sender is responding to a get request
  YAPI_MSG_GET_RESP_ERR = 0x20, // sender is responding to a get request, with error (error code in options)
  YAPI_MSG_SET_RQST = 0x01, // sender is asking to set/update/change data
  YAPI_MSG_SET_RESP_OK = 0x11, // sender is responding to a set request
  YAPI_MSG_SET_RESP_ERR = 0x21, // sender is responding to a set request, with error (error code in options)
  YAPI_MSG_SUB_RQST = 0x02, // sender is asking to change subscription
  YAPI_MSG_SUB_RESP_OK = 0x12, // sender is responding to a sub request
  YAPI_MSG_SUB_RESP_ERR = 0x22, // sender is responding to a sub start request, with error (error code in options)
  YAPI_MSG_UNSOLICITED = 0x3F, // sender is sending an unsolicited message, likely urgent
  __YAPI_TX_MAX = 0x3F, // highest valid value is 0x3F (6 bits)
} yapi_message_type_enum_t; // transmission type

typedef enum {
  YAPI_PRIORITY_LOW = 0,
  YAPI_PRIORITY_MID,
  YAPI_PRIORITY_HIGH,
  YAPI_PRIORITY_CRITICAL,
} yapi_message_priority_enum_t;

enum {
  YAPI_LENGTH_IDX = 2, // The packet length is always at index 2
  YAPI_HEADER_LENGTH = 16, // MAKE SURE IF THE yapi_packet_t STRUCT DEFINITION CHANGES THAT THIS IS UPDATED!!!
  RECEIVE_BUFFER_LENGTH = 512,
  PROCESSING_BUFFER_LENGTH = 256,
  YAPI_DIAGNOSTIC_BUFFER_LENGTH = 35
};

typedef enum {
  YAPI_CONFIG_CHARGE_PROFILE = 0,
  YAPI_CONFIG_DISPLAY,
  YAPI_CONFIG_RECOVERY,
  YAPI_CONFIG_IN,
  YAPI_CONFIG_INV,
  YAPI_CONFIG_CHARGE_RATE,
} yapi_options_config_patch_enum_t;

typedef enum {
  YAPI_STATUS_PORTS = 0b00000001,
  YAPI_STATUS_BATT_CYCLES_USER = 0b00000010,
  YAPI_STATUS_WH_IN_USER = 0b00000100,
  YAPI_STATUS_WH_OUT_USER = 0b00001000,
} yapi_options_status_patch_enum_t;

typedef enum {
  YAPI_RVES_SET_TOUT = 0b00000001,
  YAPI_RVES_SET_BRT = 0b00000010,
  YAPI_RVES_SET_R1 = 0b00000100,
  YAPI_RVES_SET_R2 = 0b00001000,
} yapi_rves_patch_enum_t;

typedef enum {
  YAPI_MFG_DATA_RESPONSE_SUCCESS = 0,
  YAPI_MFG_DATA_RESPONSE_FAIL_ALREADY_SET,
  YAPI_MFG_DATA_RESPONSE_FAIL_WMU_NOT_SET,  // If the WMU was not set, the PCU will not retain the serial number either and it must be re-sent
  YAPI_MFG_DATA_RESPONSE_FAIL_INVALID_FORMAT,
  YAPI_MFG_DATA_RESPONSE_OTHER,

  __YAPI_MFG_DATA_RESPONSE_LENGTH = YAPI_MFG_DATA_RESPONSE_OTHER,
  __YAPI_MFG_DATA_RESPONSE_MAX
} yapi_mfg_data_response_code_t;

typedef enum {
  YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE = 25,
  YAPI_MFG_DATA_MAC_ADDRESS_BUFFER_SIZE = 18,
  YAPI_MFG_DATA_NUMERIC_MAC_ADDRESS_BUFFER_SIZE = 6
} yapi_mfg_data_buffer_sizes_t;

typedef enum {
  YAPI_OTA_START_STATUS_UPDATE_UNKNOWN = 0,
  YAPI_OTA_START_STATUS_UPDATE_UNAVAILABLE,
  YAPI_OTA_START_STATUS_UPDATE_AVAILABLE,
  YAPI_OTA_START_STATUS_UPDATE_CONNECTION_ERROR = 0xFF,
  YAPI_OTA_START_STATUS_UPDATE_FILE_ERROR = 0xFE
} Yapi_Ota_Start_Get_Status_t; // enumeration statuses for the YAPI_OTA_START GET RQST

typedef enum {
  YAPI_WMU_PAIRING_BUTTON_PRESSED = 0x01,
  YAPI_WMU_PAIRING_MODE_CHANGED = 0x02,
  YAPI_WMU_FACTORY_MODE_CHANGED = 0x03,
} yapi_options_wmu_config_enum_t;

typedef enum {
  YAPI_FACTORY_MODE_ACTIVE,
  YAPI_FACTORY_MODE_EXIT_BUTTON_HOLD_COMBO,
  YAPI_FACTORY_MODE_EXIT_SOC,
  YAPI_FACTORY_MODE_EXIT_USER_PAIRED,
  YAPI_FACTORY_MODE_EXIT_TIME_SINCE_PROGRAMMING_ELAPSED,
  YAPI_FACTORY_MODE_EXIT_TIME_SINCE_LAST_WIFI_ELAPSED,
  YAPI_FACTORY_MODE_EXIT_RPC_COMMAND,

  __YAPI_FACTORY_MODE_EXIT_LENGTH,
  __YAPI_FACTORY_MODE_EXIT_MAX = YAPI_FACTORY_MODE_EXIT_RPC_COMMAND
} yapi_y6g_pcu_factory_mode_exit_code_t;

typedef enum {
  YAPI_PORTS_AC_OUT_STATUS_DISABLED,
  YAPI_PORTS_AC_OUT_STATUS_BATTERY_SOURCED,
  YAPI_PORTS_AC_OUT_STATUS_PASSTHROUGH,
  YAPI_PORTS_AC_OUT_STATUS_ERROR,

  __YAPI_PORTS_AC_OUT_STATUS_LENGTH,
  __YAPI_PORTS_AC_OUT_STATUS_MAX = YAPI_PORTS_AC_OUT_STATUS_ERROR
} yapi_y6g_pcu_ports_ac_out_status_codes_t;

typedef enum {
  APP_INVERTER_MODEL_US = 1,
  APP_INVERTER_MODEL_EUROPEAN = 2,

  __APP_INVERTER_MODEL_MAX = APP_INVERTER_MODEL_EUROPEAN
} yapi_y6g_inv_model_t;

typedef enum {
  YAPI_DIAGNOSTIC_MENU_INACTIVE = 0,
  YAPI_DIAGNOSTIC_MENU_ACTIVE = 1
} yapi_6g_diagnostic_options_t;

// Refers to the electrical combined status of two Yetis using the series AC combiner
typedef enum {
  YAPI_COMBINER_WAITING = 0,
  YAPI_COMBINER_SYNCING,
  YAPI_COMBINER_READY,
  YAPI_COMBINER_ERROR_GENERIC = 10,

  __YAPI_COMBINER_MAX = YAPI_COMBINER_ERROR_GENERIC
} yapi_y6g_combiner_status_t;

enum {
  YAPI_THINGNAME_BUFFER_SIZE = 33 // 32 chars + null terminator
};

typedef struct {
  union {
    struct {
      uint8_t powerOn:1; // System power button pressed
      uint8_t powerOnPairingButtonShortPress:1;
      uint8_t powerOnPairingButtonLongPress:1;
      uint8_t powerOnWirelessChange:1;
      uint8_t powerOnUsageHistoryChange:1;
      uint8_t powerOnWarehouseOTAChange:1;
      uint8_t powerOnOtaChange:1;
      uint8_t powerOnAfterPowerCycle:1; // Add new bits after this line and reduve the reserved bits count below accordingly
      uint8_t powerOnDiagnosticMenu:1;
      uint8_t powerOnSystemIdCheck:1;
      uint16_t _reserved_on:6;
      uint8_t powerOffUnknown:1; // IMPORTANT!: this bit must always remain at position 16 in the bitfield, even if new power on bits are added in the future to ensure compatibility across firmware versions
      uint8_t powerOffNow:1; // Set this if you want to bypass the rule to require an affirmative response from the WMU before powering it down. This is useful for cases where the WMU is unresponsive and the PCU needs to power it down to recover.
      uint8_t powerOffWirelessIdle:1;
      uint8_t powerOffUsageHistoryIdle:1;
      uint8_t powerOffWarehouseOTAIdle:1;
      uint8_t powerOffOtaIdle:1;
      uint8_t powerOffSelfTimeout:1; // Add new bits after this line and reduve the reserved bits count below accordingly
      uint16_t _reserved_off:9;
    };
    uint32_t all;
  };
} yapi_y6g_wmu_power_change_request_codes_t;

typedef struct {
  union {
    struct {
      uint8_t unknownDenyChangeRequest:1;
      uint8_t mqttJobInProgress:1;
      uint8_t OTAInProgress:1;
      uint8_t usageHistoryInProgress:1;
      uint8_t BLE_Active:1;
      uint8_t AP_Active:1;
      uint8_t STA_Active:1; // Add new bits after this line and reduve the reserved bits count below accordingly
      uint8_t DiagnosticMenuActive:1;
      uint32_t _reserved_deny_change_requests:16;
      uint8_t unknown_power_change_request_in_progress:1; // IMPORTANT!: this bit must always remain at position 24 in the bitfield, even if new power change denial bits are added in the future to ensure compatibility across firmware versions
      uint8_t power_off_in_progress:1;
      uint8_t power_on_in_progress:1;
      uint8_t power_cycle_in_progress:1;
      uint8_t pcu_reboot_in_progress:1; // Add new bits after this line and reduce the reserved bits count below accordingly
      uint8_t _reserved_change_request_status:3;
    };
    uint32_t all;
  };
} yapi_y6g_wmu_power_change_response_codes_t;

/**
 * @brief This struct defines the contents of the messageData parameter in a @ref yapi_packet_t packet.
 * 
 */
typedef struct {
  union {
    struct {
      uint8_t type:6; /** @brief yapi_message_type_enum_t - get/sub/set request/response */
      uint8_t priority:2; /** @brief yapi_message_priority_enum_t - message priority level, responses should match request priority level */
    };
    uint8_t all;
  };
} yapi_message_data_t;

/**
 * @brief Yeti API Data Packet. Total Packet size is 256 bytes. If bytes are added above "data" ensure other parameters
 * are not shifted and the total size remains 256 bytes.
 */
typedef struct __attribute__ ((packed)) {
  uint16_t start; /** @brief start byte is used to delineate one packet from another */
  uint8_t length; /** @brief length indicates the length of the data in @ref data */
  uint8_t apiVersion; /** @brief the API version used in the packet content */
  uint8_t command; /** @brief the type of the message, i.e. how to interpret the data */
  yapi_message_data_t messageData; /** @brief yapi_message_data_t - get/sub/set request/response & priority */
  uint8_t senderId; /** @brief yapi_device_id_enum_t of the sender device */
  uint8_t targetId; /** @brief yapi_device_id_enum_t of the target device */
  uint8_t options[4]; /** @brief 4 additional metadata bytes that can be used by different commands. i.e. sequence number */
  uint8_t _reserved[4]; /** @brief reserved bytes that can be used in future API revisions */
  uint8_t data[YAPI_DATA_SIZE]; /** @brief the 238-byte payload of the message. May be further modeled by other structs based on the @ref command */
  uint16_t crc; /** @brief computed 16-bit crc of the packet */
} yapi_packet_t;

typedef struct __attribute__ ((packed)) {
  union {
    struct {
      uint8_t factoryModeActive:1;
      uint8_t usbConsoleEnabled:1;
      uint8_t lowTempThreshold1:1;
      uint8_t lowTempThreshold2:1;
      uint8_t highTempThreshold1:1;
      uint8_t highTempThreshold2:1;
      uint8_t highPowerOutThreshold1:1;
      uint8_t highPowerOutThreshold2:1;
      uint8_t lvSolarInputLowVoltage:1;
      uint8_t hvSolarInputLowVoltage:1;
      uint8_t bmsSohThreshold1:1;
      uint8_t bmsSohThreshold2:1;
      uint8_t lcdCommsDisconnected:1;
      uint8_t bmsCommsDisconnected:1;
      uint8_t inverterCommsDisconnected:1;
      uint8_t auxCommsDisconnected:1;
      uint8_t pdCommsDisconnected:1;
      uint8_t mpptCommsDisconnected:1;
      uint8_t poweringDown:1;
      uint8_t inverterAcSyncFailure:1;
      uint8_t factoryResetRequested:1;
    };
    uint32_t all;
  };
  // TODO: this is just an example for now, expected to change rapidly during development
} yapi_y6g_pcu_summary_status_flags_t;

typedef struct __attribute__ ((packed)) {
  uint8_t batteryFans_pct; // fan duty in [0-100] percent, 0 if disabled
  uint8_t inverterFans_pct; // fan duty in [0-100] percent, 0 if disabled
  uint8_t caseFans_pct; // fan duty in [0-100] percent, 0 if disabled
  uint8_t _reserved;
} yapi_y6g_fans_status_t;

typedef struct __attribute__ ((packed)) {
  union {
    struct {
      uint8_t isOcp:    1;
      uint8_t isOvp:    1;
      uint8_t isOtp:    1;
      uint8_t isUvp:    1;
      uint8_t isShort:  1;
      uint8_t isFault:  1;
      uint8_t reserved: 2;
    };
    uint8_t all;
  };
} yapi_y6g_port_status_flags_t;

typedef struct __attribute__ ((packed)) {
    uint32_t current_dA;
    uint32_t power_dW;
    uint16_t voltage_dV;
    uint8_t status; // Differs slightly per port type. See https://github.com/GoalZero26503/docs-iot-architecture/tree/docs/gen6/latest/yeti-1000-1500/status or similar for a different project
                    // See the enums for status.ports.acOut.s, status.ports.usbOut.s, status.ports.v12Out.s, status.ports.auxOut.s  
    yapi_y6g_port_status_flags_t flags;
} yapi_y6g_pcu_output_status_t;

typedef struct __attribute__ ((packed)) {
  yapi_y6g_pcu_output_status_t outputAc;
  yapi_y6g_pcu_output_status_t outputUsb;
  yapi_y6g_pcu_output_status_t output12v;
  yapi_y6g_pcu_output_status_t outputAux;
} yapi_y6g_pcu_outputs_status_t;

typedef struct __attribute__ ((packed)) {
  uint32_t current_dA;
  uint32_t power_dW;
  uint16_t voltage_dV;
  bool isOcp; // not currently used
  bool isOvp; // not currently used
  bool fast_charge;  //only applies to ac 
  yapi_y6g_port_status_flags_t flags;
} yapi_y6g_pcu_input_status_t;

typedef struct __attribute__ ((packed)) {
  yapi_y6g_pcu_input_status_t inputAc;
  yapi_y6g_pcu_input_status_t inputLvDc;
  yapi_y6g_pcu_input_status_t inputHvDc;
  yapi_y6g_pcu_input_status_t inputAux;
} yapi_y6g_pcu_inputs_status_t;

typedef struct __attribute__ ((packed)) {
  uint8_t inputAc;
  uint8_t inputLvDc;
  uint8_t inputHvDc;
  uint8_t inputAux;
} yapi_y6g_pcu_input_status_codes_t;

// Y6G primary controller summary status
typedef struct __attribute__ ((packed)) {
  uint16_t battery_cycles_lifetime;
  uint16_t battery_cycles_user;
  uint8_t soh;
  uint8_t soc;
  uint16_t capacity_wh;
  uint16_t capacity_remaining_wh;
  uint16_t battery_voltage_mV;
  int16_t average_net_amps_dA;
  int16_t net_amps_dA;
  int16_t battery_temp_C;
  int16_t ttef;
  int16_t average_net_watts_w;
  int16_t net_watts_w;
  uint16_t relative_humidity_pct;
  int16_t temp_sense_C; //Ambient?
  uint32_t wh_in_user; //add DC / AC
  uint32_t wh_out_user;
  uint32_t wh_in_lifetime;
  uint32_t wh_out_lifetime;
  yapi_y6g_pcu_outputs_status_t outputs;
  yapi_y6g_pcu_inputs_status_t inputs;
  yapi_y6g_pcu_summary_status_flags_t flags;
  uint8_t factory_mode_exit_code; // yapi_y6g_pcu_factory_mode_exit_code_t
  int16_t inverter_temp_C;
  int16_t port_12v_temp_C;
  yapi_y6g_pcu_input_status_codes_t input_status_codes;
  uint8_t power_btn_state; // 0: OFF 1: ON
} yapi_y6g_pcu_summary_status_t;

// Y6G primary controller summary status
typedef struct __attribute__ ((packed)) {
  uint16_t pcu_hw_version;
  uint16_t pcu_fw_version;
  uint16_t bms_hw_version;
  uint16_t bms_fw_version;
  uint16_t inv_lv_hw_version;
  uint16_t inv_lv_fw_version;
  uint16_t inv_hv_hw_version;
  uint16_t inv_hv_fw_version;
  uint16_t mppt_hw_version;
  uint16_t mppt_fw_version;
  uint8_t inv_model; // 1 = 120V, 2 = 230V. See yapi_y6g_inv_model_t
} yapi_y6g_pcu_device_t;

typedef struct __attribute__ ((packed)) {
  uint32_t uvpCount;
  uint32_t ovpCount;
  uint32_t otpCount;
  uint32_t ocpCount;
  uint32_t utpCount;
  uint32_t mhtCount;
  uint32_t mltCount;
  uint32_t mlvCount;
  int16_t cTmpMax;
  int16_t cTmpMin;
  char packMfgDate[16]; // currently 11 chars
  char packMfgName[32]; // currently 17 chars
  char packSerial[32]; // currently 29 chars
  uint8_t otp;
  uint8_t utp;
  // TODO: this is just an example for now, expected to change rapidly during development
} yapi_y6g_bms_summary_status_t;

typedef struct __attribute__ ((packed)) {
  uint16_t lvFirmwareVersion;
  uint16_t hvFirmwareVersion;
  uint8_t upsState;
  uint8_t parallelState;
  uint8_t frequency_hz;
  uint32_t powerIn_cW;
  uint16_t currentIn_dW;
  uint32_t voltageIn_mV;
  uint16_t invVoltage_dV;
  uint16_t invCurrent_cA;
  uint32_t invPowerRMS_cW;
  uint16_t invFrequency_dHz;
  uint16_t pfcCurrent_cA;
  uint32_t pfcPowerRMS_cW;
  uint16_t upsPFSR_pct;
  uint16_t gridVoltage_dV;
  uint16_t gridCurrent_cA;
  uint32_t gridPowerRMS_cW;
  uint16_t gridFrequency_dHz;
  uint16_t batteryVoltage_cV;
  // TODO: this is just an example for now, expected to change rapidly during development
} yapi_y6g_inverter_summary_status_t;

typedef struct __attribute__ ((packed)) {
  uint8_t soc;
  int16_t hours;
} yapi_y6g_pcu_config_recovery_t;

typedef struct __attribute__ ((packed)) {
  struct __attribute__ ((packed)) {
    uint8_t userColorR;
    uint8_t userColorG;
    uint8_t userColorB;
    uint8_t userBrightness;
    bool blackout;
  } ledConfig;
  struct  __attribute__ ((packed)) {
    uint16_t v12:3;
    uint16_t usb:3;
    uint16_t vac:3;
    uint16_t display:1;
  } enables;
  struct  __attribute__ ((packed)) {
    uint16_t dischargeShutoffSoc;
    uint8_t chargeMaxSoc;
    uint8_t rechargeSoc;
  } chargeProfile;
  struct  __attribute__ ((packed)) {
    uint8_t brightness: 7;
    uint8_t blackout: 1;
  } display;
  struct  __attribute__ ((packed)) {
    yapi_y6g_pcu_config_recovery_t usb;
    yapi_y6g_pcu_config_recovery_t v12;
    yapi_y6g_pcu_config_recovery_t ac;
  } recovery;
  struct  __attribute__ ((packed)) {
    uint8_t mode;
    uint16_t charge_limit;
  } inv;
  struct __attribute__ ((packed)) {
    uint16_t wall_dA; // port limit
    uint16_t plus_dA; // port limit
    uint16_t wall_charge_limit_dA; // charge limit
    uint16_t plus_charge_limit_dA; // charge limit
  } invInputLmt;
} yapi_y6g_pcu_config_t;

/** @brief This structure defines a single PMIC channel state object */
typedef struct __attribute__ ((packed)) {
  uint8_t channelIndex;   // Necessary in case these get packed out of order, or a channel is missing

  struct {
    uint8_t _reserved   :4; // Future expansion - decrement this if you add a new bit
    uint8_t present     :1;
    uint8_t detected    :1;
    uint8_t error       :1;
    uint8_t accumulating:1;
  } channelStatus;

  uint32_t rmsVoltage_mV;
  uint32_t rmsCurrent_mA;
  uint16_t lineFrequency_dHz;
  int16_t activePower_W;
  int16_t reactivePower_W;
  int16_t apparentPower_W;
} yapi_y6g_pcu_pmic_status_t;

/** @brief This structure defines a single PMIC channel accumulated energy object */
typedef struct __attribute__ ((packed)) {
  uint8_t channelIndex;   // Necessary in case these get packed out of order, or a channel is missing

  uint64_t accumulatedActivePower_Wh;
} yapi_y6g_pcu_pmic_accumulated_energy_t;

/** @brief This structure defines a dynamic array of PMIC channel state objects */
typedef struct __attribute__ ((packed)) {
  uint8_t numPmics;

  /* Note that this will not be a 1:1 mapping to the hephal PMIC channels, this is just a holding buffer */
  /* Sorry for the magic number but this will handle up to 12 PMICs at once - change this number as necessary */
  yapi_y6g_pcu_pmic_status_t pmicStatuses[12];
} yapi_y6g_pcu_pmics_status_t;

/** @brief This structure defines a dynamic array of PMIC channel accumulated energy objects */
typedef struct __attribute__ ((packed)) {
  uint8_t numPmics;

  /* Note that this will not be a 1:1 mapping to the hephal PMIC channels, this is just a holding buffer */
  /* Sorry for the magic number but this will handle up to 12 PMICs at once - change this number as necessary */
  yapi_y6g_pcu_pmic_accumulated_energy_t pmicStatuses[12];
} yapi_y6g_pcu_pmics_accumulated_energy_t;

typedef struct __attribute__ ((packed)) {
  uint16_t firmwareVersion;
  uint8_t numericMacAddress[YAPI_MFG_DATA_NUMERIC_MAC_ADDRESS_BUFFER_SIZE];
  uint8_t stringMacAddress[YAPI_MFG_DATA_MAC_ADDRESS_BUFFER_SIZE];
  uint16_t wifiStaTimeout_1s; // how long until an STA search times out
  uint16_t wifiApTimeout_1s; // how long until AP times out (no clients connected)
  uint32_t wifiTimeSinceLastSTAConnect_1s; // how long since last STA connected
  bool newSTAModeIPAcquired; // true if new STA IP address was just acquired (event driven)
  int rssi; // mgos_wifi_sta_get_rssi()
  uint8_t apMode; // gzm_wifi_ap_mode_t
  uint8_t staMode; // gzm_wifi_sta_mode_t
  uint8_t bleMode; // gzm_ble_mode_t
  uint8_t wifiStatus; // gzm_wifi_status_t
  uint8_t cloudStatus; // gzm_device_cloud_status_t
  uint8_t cloudError;  // gzm_device_cloud_err_t
  char ssid[33]; // gzm_wifi_sta_get_connected_ssid()
  bool factoryResetRequested; // true if factory reset requested
  char thingName[YAPI_THINGNAME_BUFFER_SIZE]; // The thing name of this Yeti's WMU
} yapi_y6g_wmu_summary_status_t;

// Use YAPI_MSG_SET_RQST as the YAPI message type
typedef struct {
  char serialString[YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE]; // "AAAAA-BB-CCDEEEEE"
} yapi_mfg_data_serial_number_write_request_t;

typedef struct {
  yapi_mfg_data_response_code_t response;
} yapi_mfg_data_serial_number_write_response_t;

typedef struct __attribute__ ((packed)) {
  union {
    struct {
      uint8_t mac_address_valid :1;
      uint8_t serial_num_valid :1; // Set to true if the string in serialNumberString is valid, false if value should not be used (ie serial never set, etc)
      uint8_t fw_ver_wmu_valid :1;
      uint8_t fw_ver_pcu_valid :1;
      uint8_t fw_ver_bms_valid :1;
      uint8_t fw_ver_mppt_valid :1;
      uint8_t fw_ver_lv_inv_valid :1;
      uint8_t fw_ver_hv_inv_valid :1;
      uint8_t hw_ver_pcu_wmu_valid :1;
      uint8_t hw_ver_bms_valid :1;
      uint8_t hw_ver_mppt_valid :1;
      uint8_t hw_ver_inv_valid :1;
    };
    uint16_t all_flags;
  } fieldEntryValid;
  char macAddress[YAPI_MFG_DATA_MAC_ADDRESS_BUFFER_SIZE]; // example "00:1b:63:84:45:e6"
  char serialNumberString[YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE]; // "AAAAA-BB-CCDEEEEE", sometimes longer
  uint16_t fwVersion_WMU;
  uint16_t fwVersion_PCU;
  uint16_t fwVersion_BMS;
  uint16_t fwVersion_MPPT;
  uint16_t fwVersion_LV_INV;
  uint16_t fwVersion_HV_INV;
  // Tanks are not supported at this time
  uint16_t hwVersion_PCU_WMU; // The PCU and WMU are on the same PCB
  uint16_t hwVersion_BMS;
  uint16_t hwVersion_MPPT;
  uint16_t hwVersion_INV;
} yapi_mfg_data_read_response_t;

typedef struct  __attribute__ ((packed)) {
  uint16_t previousVersion;
  uint16_t currentVersion;
  uint8_t updateProgress;
  uint8_t downloadProgress;
} yapi_y6g_wmu_ota_status_t;

typedef struct  __attribute__ ((packed)) {
  uint8_t totalProgress;
  uint8_t statusCode; //Ota_States_t
  bool isFactoryUpdate;
  uint8_t currentStep;
  uint8_t totalSteps;
  bool updateFailed;
  uint8_t failCode;
  char lcd_row1_buf[9];
  char lcd_row2_buf[9];
  char lcd_row3_buf[9];
} yapi_y6g_wmu_ota_statuses_t;

typedef enum {
  YAPI_Y6G_BLE_OFF = 0,
  YAPI_Y6G_BLE_CONNECTED = 0x01,
  YAPI_Y6G_BLE_BROADCASTING = 0x02,
  YAPI_Y6G_BLE_UPDATING = 0x03,
} yapi_y6g_ble_state_t;

typedef enum {
  YAPI_Y6G_WIFI_OFF = 0,
  YAPI_Y6G_WIFI_CLOUD_CONNECTED = 1,
  YAPI_Y6G_WIFI_BROADCASTING = 2,
  YAPI_Y6G_WIFI_DIRECT = 3,
  YAPI_Y6G_WIFI_WLAN_CONNECTING = 4,
  YAPI_Y6G_WIFI_CLOUD_CONNECTING = 5,
  YAPI_Y6G_WIFI_UPDATING = 6,
} yapi_y6g_wifi_state_t;

typedef struct __attribute__ ((packed)) {
  uint8_t wmuMode;
  uint8_t autoUpdateFrequency; // never, daily, weekly, monthly, quarterly
  uint16_t wifiStaTimeout_1s; // configuration for how long STA search will time out
  uint16_t wifiApTimeout_1s; // configuration for how long AP will time out
  bool pairing_short_press;
  bool pairing_long_press;
  uint32_t bleState;   // yapi_y6g_ble_state_t
  uint32_t wifiState;  // yapi_y6g_wifi_state_t
  uint32_t factoryExitRequest;
} yapi_y6g_wmu_config_t;

typedef struct __attribute__ ((packed)) {
  yapi_y6g_wmu_power_change_request_codes_t request_code;
  yapi_y6g_wmu_power_change_response_codes_t response_code;
  bool powerCycle;
  bool terminateRequest;
  bool rebootPCU;
  int32_t retryLimit; // Number of times to request a power change. Set to -1 to retry forever
  uint32_t timeToPowerOff_s; // Time to either power off or power cycle. Requires affirmation from the WMU to proceed. Set powerOffNow to 1 to bypass this requirement, setting this to 0 will just request immediately
} yapi_y6g_wmu_power_t;

typedef struct __attribute__ ((packed)) {
  bool isLinkConnected;
  bool isTankConnected;
  bool isCombinerConnected;
  uint16_t voltage_dV;
  int32_t current_mA;
  int32_t power_dW;
} yapi_y6g_aux_port_status_t;

typedef struct __attribute__ ((packed)) {
  uint8_t count; // number of tanks
  uint8_t tankID;
  uint16_t statusCode; // i.e. disconnected, stabilizing_voltage, connected_idle, discharging, charging
  uint16_t firmwareVersion;
  uint16_t hardware_version;
  char serial_number[32];
  uint32_t cyc;
  uint8_t soh;
  uint8_t soc;
  uint16_t capacity_wh;
  uint32_t whIn;
  uint32_t whOut;
  uint16_t capacity_remaining_wh;
  uint16_t average_net_amps_dA;
  uint16_t net_amps_dA;
  uint16_t battery_temp_C;
  uint16_t ttef;
  uint16_t average_net_watts_w;
  uint16_t net_watts_w;
  uint16_t relative_humidity_pct;
  uint16_t temp_sense_C;
  uint32_t batteryVoltage_mV;
  uint32_t downstreamVoltage_mV;
  uint32_t upstreamVoltage_mV;
  int32_t batteryCurrent_mA;
} yapi_y6g_tank_status_t;

typedef struct __attribute__ ((packed)) {
  uint8_t statusCode; // i.e. offline
  uint16_t firmwareVersion;
  uint32_t voltage_mV;
  int32_t current_mA;
  int32_t power_dW;
} yapi_y6g_link_status_t;

typedef struct __attribute__ ((packed)) {
  uint16_t firmware_version;
  uint16_t hardware_version;
  uint16_t screen_brightness;
  uint16_t screen_timeout;
  uint16_t statusCode;
  uint8_t relay1_state;
  uint8_t relay2_state;
} yapi_y6g_rves_summary_status_t;

typedef struct __attribute__ ((packed)) {
  uint8_t  yetiModelId; //Yeti model Id number
  uint8_t  normalizedSOC_pct; //Reported battery state of charge %
  uint16_t battCapacity_Wh; //Design capacity of yeti
  uint16_t battRemainingCapacity_Wh; //Wh remaining of Yeti at current SOC%
  uint16_t expCapacity_Wh; 
  uint16_t expRemainingCapacity_Wh;
  uint16_t solarInput_W; //Total watts solar input
  uint16_t acInput_W; //Total watts AC input
  uint16_t dcOutput_W; //Total output from DC ports
  uint16_t acOutput_W; //TOtal output from AC ports
  int16_t  timeToEmptyFull_minutes; //units = minutes, + time to full, - time to empty
} yapi_hep_yeti_status_summary_t;

typedef struct __attribute__ ((packed)) {
  uint8_t minimumDischargeSOC_pct;  // Yeti will disable all outputs when SOC drops to this point
  uint8_t maximumChargeSOC_pct;  // Yeti will stop all charging at this point
  uint8_t inverter_enable;
  uint8_t inverter_charge;
  uint8_t inverter_discharge;
} yapi_hep_yeti_control_t;

typedef struct __attribute__ ((packed)) {
  union {
    uint8_t byte;

    struct {
      uint8_t L1_up:1;
      uint8_t L2_up:1;
      uint8_t Y1_up:1;
      uint8_t Y2_up:1;
      uint8_t HW_line_up:1;
      /* Future expansion */
      uint8_t _rsvd:3;
    } bits;
  };
} yapi_hep_line_state_t;

typedef struct {
  char serialString[YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE]; // "AAAAA-BB-CCDEEEEE"
} yapi_serial_number_write_request_t;

// Serial numbers are of the format AAAAA-BB-CCDEEEEE, where A is the SKU set by Goal Zero for this model, B is the hardware version starting from 01, C is the year code (23 is for 2023), D is the month letter code for the month of manufacture (A for JAN - L for DEC), and E is the unique serial ID of this individual product.
typedef enum {
  YAPI_SERIAL_RESPONSE_SUCCESS = 0,
  YAPI_SERIAL_RESPONSE_FAIL_ALREADY_SET,
  YAPI_SERIAL_RESPONSE_FAIL_WMU_NOT_SET,  // If the WMU was not set, the PCU will not retain the serial number either and it must be re-sent
  YAPI_SERIAL_RESPONSE_FAIL_INVALID_FORMAT,
  YAPI_SERIAL_RESPONSE_OTHER,

  __YAPI_SERIAL_RESPONSE_LENGTH = YAPI_SERIAL_RESPONSE_OTHER,
  __YAPI_SERIAL_RESPONSE_MAX
} YAPI_SERIAL_RESPONSE_CODE_t;

typedef struct __attribute__ ((packed)) {
  union {
    struct {
      uint8_t mac_address_valid :1;
      uint8_t serial_num_valid :1; // Set to true if the string in serialNumberString is valid, false if value should not be used (ie serial never set, etc)
      uint8_t fw_ver_wmu_valid :1;
      uint8_t fw_ver_pcu_valid :1;
      uint8_t fw_ver_bms_valid :1;
      uint8_t fw_ver_mppt_valid :1;
      uint8_t fw_ver_lv_inv_valid :1;
      uint8_t fw_ver_hv_inv_valid :1;
      uint8_t hw_ver_pcu_wmu_valid :1;
      uint8_t hw_ver_bms_valid :1;
      uint8_t hw_ver_mppt_valid :1;
      uint8_t hw_ver_inv_valid :1;
    };
    uint16_t all_flags;
  } fieldEntryValid;
  char macAddress[YAPI_MFG_DATA_MAC_ADDRESS_BUFFER_SIZE]; // example "00:1b:63:84:45:e6"
  char serialNumberString[YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE]; // "AAAAA-BB-CCDEEEEE"
  uint16_t fwVersion_WMU;
  uint16_t fwVersion_PCU;
  uint16_t fwVersion_BMS;
  uint16_t fwVersion_MPPT;
  uint16_t fwVersion_LV_INV;
  uint16_t fwVersion_HV_INV;
  // Tanks are not supported at this time
  uint16_t hwVersion_PCU_WMU; // The PCU and WMU are on the same PCB
  uint16_t hwVersion_BMS;
  uint16_t hwVersion_MPPT;
  uint16_t hwVersion_INV;
} yapi_serial_number_read_response_t;

typedef struct __attribute__ ((packed)) {
  uint8_t Y1_online;
  uint8_t Y1_assignedHcomId;
  uint8_t Y1_mac_numerical[6];
  uint8_t Y2_online;
  uint8_t Y2_assignedHcomId;
  uint8_t Y2_mac_numerical[6];
} yapi_hep_yeti_pairing_states_t;

typedef struct __attribute__ ((packed)) {
  uint8_t installed_9_10;
  uint8_t installed_11_12;
} yapi_hep_240V_breaker_presence_t;

typedef enum {
    OTA_STEP_IDLE = 0,
    OTA_STEP_COMM_CHECK = 1, // CHECK PCU YAPI COMMUNICATION
    OTA_STEP_DOWNLOAD_MANIFEST = 2,
    OTA_STEP_DOWNLOAD_IMAGES = 3,
    OTA_STEP_WMU_UPDATE = 4,
    OTA_STEP_PCU_UPDATE = 5,
    OTA_STEP_BMS_UPDATE = 6,
    OTA_STEP_MPPT_UPDATE = 7,
    OTA_STEP_INV_HV_UPDATE =  8,
    OTA_STEP_INV_LV_UPDATE = 9,
    OTA_STEP_TANK_UPDATE = 10, // TODO tank should be updated after other components when persistent Pairing ID is implemented
    OTA_STEP_RVES_UPDATE = 11,
    OTA_STEP_SUCCESS = 12,
    OTA_STEP_FAIL = 13,
    OTA_STEP_ABORT = 14
} Ota_Step_t; // TODO move this one to yapi service header

typedef struct __attribute__ ((packed)) {
  // Indexed from 1. 0 means no page is selected and to stop displaying the log. When the PCU wants the WMU to start sending messages, it should start with page 1 and send this frame.
  // When the PCU no longer wants the WMU to send messages, it should send a page number of 0. When the PCU wants to change pages, it should send a page number of the page it wants to change to.
  uint8_t currentPage; 
  // Total number of pages the user can cycle through set by the WMU. There will always be at least one diagnostic page, so index 1 is always valid.
  uint8_t totalPages; 
  // The depth of the current page. 0 means we are at the top level. 1 or greater means we are in a sub-menu.
  uint8_t currentDepth;
  // The total depth of the current page. We can go up to this depth, if the user cycles again then we go back to page 1.
  uint8_t totalDepth;
  // The interval in seconds that the WMU should send an updated page automatically. 0 means no automatic updates (PCU is polling).
  uint8_t updateInterval_S; 
  // This is a fully formatted page message string. It is intended to use newline characters to separate lines, and it must be null terminated.
  // If the page message string is longer than the buffer, it will be truncated before being transmitted. 
  // For center justified text, the sender should pad the string with spaces in the desired line according to the LCD width of the product.
  // Note that not all characters are supported by the LCD, and the sender should ensure that the string is formatted correctly.
  char pageMsg[YAPI_DIAGNOSTIC_BUFFER_LENGTH]; // support for 32 displayed characters + 2 newlines + null terminator
} yapi_y6g_display_diagnostic_page_t;

// Status of the combined yeti, as well as the combiner hardware itself
typedef struct __attribute__ ((packed)) {
  uint8_t statusCode; // yapi_y6g_combiner_status_t
  uint16_t powerIn_W; // the other Yeti's total power in including DC input.
  uint16_t powerOut_W; // the other Yeti's total power out including DC output.
  uint16_t line1_W; // the combiner's L1 loading
  uint16_t line2_W; // the combiner's L2 loading
  int32_t ttef_m; // time to empty or full in minutes. Negative values are time to empty, positive values are time to full
  uint8_t soc;
  uint16_t firmwareVersion;
  uint16_t hardwareVersion;
  char thingName[YAPI_THINGNAME_BUFFER_SIZE]; // The thing name of the other connected Yeti's WMU
} yapi_y6g_combiner_summary_status_t;


typedef struct __attribute__ ((packed)) {
  union {
    struct {
      uint8_t noSystemSerialNumber:1; // Un-provisioned system
      uint8_t unknownBmsId:1; // Unkonwn BMS ID
      uint8_t mismatchedBmsId:1; // Mismatched BMS ID
      uint8_t mismatchedInvType:1; // Mismatched Inverter Type
      uint8_t bypassReasonEnv:1; // bypass mismatched error due to the environment not on production
      uint8_t bypassReasonConfigured:1; // bypass mismatched error due to being configured
      uint16_t _reserved:10;
    };
    uint16_t all;
  };
} yapi_system_id_error_t;

typedef struct __attribute__ ((packed)) {
  yapi_system_id_error_t error;
  char systemSerialNumber[YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE]; // "AAAAA-BB-CCDEEEEE"
} yapi_system_id_t;
/*
Application version manifest
*/

typedef void (*v_fp_yapi_ptr_t)(yapi_packet_t* yapi_pkt);

/**
 * @brief generic void function pointer that takes a uint8_t
 */
#ifndef _V_FP_U8_T
typedef void (*v_fp_u8_t)(uint8_t byte);
#define _V_FP_U8_T
#endif

/**
 * @brief A pointer to a function that allows the YAPI service to register
 * its own Receive Byte callback
 * The user application should have a function that registers a callback
 * to call for every byte received on the data channel.
 */
typedef void (*yapi_register_rx_byte_func_t)(v_fp_u8_t);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * NOTE: The application needs to define these functions      *
 * We considered allowing these functions to be injected      *
 * as callbacks from the application, but decided since       *
 * this library is used in Bootloaders and needs to be        *
 * optimized for space, that we will declare the function     *
 * signatures in the header and require the application to    *
 * define the function implementation.                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @brief Buffer Transmit function
 * The Application must implement this function.
 * The function takes a character buffer and buffer length
 * and uses its hardware layer to send the data buffer.
 * The function should be non-blocking
 * @param buffer 
 * @param length 
 * @return uint16_t 
 */
uint16_t yapi_platform_transmit(uint8_t *buffer, uint16_t length);

/**
 * @brief Blocking Buffer Transmit function
 * The Application must implement this function.
 * The function takes a character buffer and buffer length
 * and uses its hardware layer to send the data buffer.
 * This function should be blocking.
 * @param buffer 
 * @param length 
 * @return uint16_t 
 */
uint16_t yapi_platform_transmit_blocking(uint8_t *buffer, uint16_t length);

/**
 * @brief Platform debug logging function
 * The application can define a debug logger for the YAPI service
 * to log debug information.
 * 
 * @param fmt 
 * @param ... 
 */
void yapi_platform_log_debug(const char *fmt, ...);

/**
 * @brief Platform info logging function
 * The application can define a debug logger for the YAPI service
 * to log normal operation information.
 * 
 * @param fmt 
 * @param ... 
 */
void yapi_platform_log_info(const char *fmt, ...);

/**
 * @brief Platform warn logging function
 * The application can define a debug logger for the YAPI service
 * to log warning information.
 * 
 * @param fmt 
 * @param ... 
 */
void yapi_platform_log_warn(const char *fmt, ...);

/**
 * @brief Platform error logging function
 * The application can define a debug logger for the YAPI service
 * to log error information.
 * 
 * @param fmt 
 * @param ... 
 */
void yapi_platform_log_error(const char *fmt, ...);

/**
 * @brief Initializes the yapi service prior to operation
 * 
 * @param deviceId - The ID for the application device
 * @param registerRxByteFunc - Application specific byte receive callback register function
 * (see @ref yapi_register_rx_byte_func_t)
 */
void yapi_service_init(yapi_device_id_enum_t deviceId, yapi_register_rx_byte_func_t registerRxByteFunc);

/**
 * @brief 10ms function loop that should be called every 10ms
 * for buffer processing
 * @param param user params (currently unused)
 */
void yapi_service_task_10ms(void* param);

/**
 * @brief Registers a callback function of type @ref v_fp_yapi_ptr_t for a given @ref yapi_command_enum_t
 * command recieved. These registrations should be made when @ref yapi_service_init() is called.
 * 
 * @param cb A callback function of type @ref v_fp_yapi_ptr_t to be called when a yapi command of type @ref cmd is received.
 * @param cmd A @ref yapi_command_enum_t command to register the @ref cb function to be called for upon reception.
 * @return yapi_ops_status_t Returns YAPI_OPS_SUCCESS if the operation was successful, and YAPI_OPS_FAIL otherwise.
 */
yapi_ops_status_t yapi_service_register_cmd_cb(v_fp_yapi_ptr_t cb, yapi_command_enum_t cmd);

/**
 * @brief Given thre prescribed set of parameters, constructs a @ref yapi_packet_t packet and stores it in pkt.
 * 
 * @param pkt A pointer to a user allocated @ref yapi_packet_t packet to populate with the prescribed data
 * @param senderId The @ref yapi_device_id_enum_t id of the sender device from which the message is being sent
 * @param targetId The @ref yapi_device_id_enum_t id of the target device for which the message is intended
 * @param command The @ref yapi_command_enum_t command endpoint for the target device
 * @param messageType The @ref yapi_message_type_enum_t message type
 * @param priority The @ref yapi_message_priority_enum_t priority of the yapi message
 * @param options Additional options describing the packet. Not currently defined.
 * @param data The payload of the yapi packet. Max length is currently 238 bytes.
 * @param length The number of bytes in the @ref data buffer.
 * @return yapi_ops_status_t Returns YAPI_OPS_SUCCESS if the operation was successful, and YAPI_OPS_FAIL otherwise.
 */
yapi_ops_status_t yapi_service_build_pkt(yapi_packet_t* pkt,
  yapi_device_id_enum_t senderId,
  yapi_device_id_enum_t targetId,
  yapi_command_enum_t command,
  yapi_message_type_enum_t messageType,
  yapi_message_priority_enum_t priority,
  uint8_t *options,
  uint8_t *data,
  uint8_t length);

/**
 * @brief Sends a previously constructed yapi packet pointed to by @ref pkt to the device prescribed by
 * in the @ref yapi_packet_t packet's targetId.
 * 
 * @param pkt A pointer to a user allocated and populated @ref yapi_packet_t packet to be sent.
 * @return yapi_ops_status_t Returns YAPI_OPS_SUCCESS if the operation was successful, and YAPI_OPS_FAIL otherwise.
 */
yapi_ops_status_t yapi_service_send(yapi_packet_t* pkt);

/**
 * @brief Given thre prescribed set of parameters, constructs a @ref yapi_packet_t packet and then sends it 
 * to the device prescribed by in the @ref yapi_packet_t packet's targetId.
 * 
 * @param targetId The @ref yapi_device_id_enum_t id of the target device for which the message is intended
 * @param command The @ref yapi_command_enum_t command endpoint for the target device
 * @param messageType The @ref yapi_message_type_enum_t message type
 * @param priority The @ref yapi_message_priority_enum_t priority of the yapi message
 * @param options Additional options describing the packet. Not currently defined.
 * @param data The payload of the yapi packet. Max length is currently 238 bytes.
 * @param length The number of bytes in the @ref data buffer.
 * @return yapi_ops_status_t Returns YAPI_OPS_SUCCESS if the operation was successful, and YAPI_OPS_FAIL otherwise.
 */
yapi_ops_status_t yapi_service_build_send(yapi_device_id_enum_t targetId,
  yapi_command_enum_t command,
  yapi_message_type_enum_t messageType,
  yapi_message_priority_enum_t priority,
  uint8_t *options,
  uint8_t *data,
  uint8_t length);

/**
 * @brief Given thre prescribed set of parameters, constructs a @ref yapi_packet_t packet and then sends it 
 * to the device prescribed by in the @ref yapi_packet_t packet's targetId.
 * 
 * @param senderId The @ref yapi_device_id_enum_t id of the sender device from which the message is being sent 
 * @param targetId The @ref yapi_device_id_enum_t id of the target device for which the message is intended
 * @param command The @ref yapi_command_enum_t command endpoint for the target device
 * @param messageType The @ref yapi_message_type_enum_t message type
 * @param priority The @ref yapi_message_priority_enum_t priority of the yapi message
 * @param options Additional options describing the packet. Not currently defined.
 * @param data The payload of the yapi packet. Max length is currently 238 bytes.
 * @param length The number of bytes in the @ref data buffer.
 * @return yapi_ops_status_t Returns YAPI_OPS_SUCCESS if the operation was successful, and YAPI_OPS_FAIL otherwise.
 */
yapi_ops_status_t yapi_service_build_send_ID(yapi_device_id_enum_t senderId,
  yapi_device_id_enum_t targetId,
  yapi_command_enum_t command,
  yapi_message_type_enum_t messageType,
  yapi_message_priority_enum_t priority,
  uint8_t *options,
  uint8_t *data,
  uint8_t length);
  
#ifdef __cplusplus
}
#endif


#endif /* YETI_HAL_INCLUDES_YAPI_SERVICE_H_ */
