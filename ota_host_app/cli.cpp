/**
 * cli.cpp
 *
 * Parse and process commands.
 * Commands could be in string (spaces separated)
 * Commands could be JSON (unimplemented)
 *  
 * Author: Quang Nguyen <quang.nguyen@goalzero.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h> 
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // read/write/close/sleep

#include "cli.h"
#include "uart.h"
#include "gz_log.h"
#include "yapi_flash.h"
#include "yapi_modbus.h"

#define BACK_SPACE              8
#define NEW_LINE                '\n'
#define CARRIAGE_RETURN         '\r'
#define CLI_PROMPT              "> "
#define MAC_DEVICE_PREFIX       "cu."

typedef struct {
  char *command;
  char *command_args[MAX_CMD_ARGUMENTS];
  unsigned char args_count;
} _Cli_Command_Args_t;

typedef bool (*_func_ptr_command_executer)(_Cli_Command_Args_t);

typedef struct {
  const char *command;
  const char *description;
  _func_ptr_command_executer executer;
} _Cli_Command_t;



static bool _isEcho = true;
static bool _isRunning = false;
static int _uartFd;
static pthread_t _threadId;

/**
 * Get input from console
 * This function blocks until receiving either the Carriage or Newline character
 */
static void _cli_get_line(char* line);
/**
 * @brief parsing the command and return Cli_command_Args_t
 * 
 * @return _Cli_Command_Args_t 
 * @arg: line: received command
 * @arg: delimiter: separator
 */
static _Cli_Command_Args_t _cli_string_parser(const char* line, const char* delimiter);
#ifdef USE_JSON
static _Cli_Command_Args_t _cli_json_parser(const char* line);
#endif

static bool _cli_process_command(_Cli_Command_Args_t);

/* Definition of cli command */
static bool _cli_help_executer(_Cli_Command_Args_t);
static bool _cli_connect_serial_port(_Cli_Command_Args_t);
static bool _cli_disconnect_serial_port(_Cli_Command_Args_t);
static bool _cli_write_serial_port(_Cli_Command_Args_t);
static bool _cli_ls_dev(_Cli_Command_Args_t);
static bool _exit(_Cli_Command_Args_t);
static bool _cli_ls(_Cli_Command_Args_t);
static bool _cli_cat_bin(_Cli_Command_Args_t);
static bool _cli_flash_read(_Cli_Command_Args_t);
static bool _cli_flash_write(_Cli_Command_Args_t);
static bool _cli_flash_erase(_Cli_Command_Args_t);
static bool _cli_flash_upload(_Cli_Command_Args_t);
static bool _cli_flash_verify(_Cli_Command_Args_t);
static bool _cli_modbus_silence(_Cli_Command_Args_t);
static bool _cli_modbus_enter_bootloader(_Cli_Command_Args_t);
static bool _cli_modbus_get_boot_info(_Cli_Command_Args_t);

_Cli_Command_t _cli_commands[] = {
  {
    .command = "help",
    .description = "Show all commands with description",
    .executer = _cli_help_executer
  },
  {
    .command = "connect",
    .description = "connect <name> <baud-rate>",
    .executer = _cli_connect_serial_port
  },
  {
    .command = "disconnect",
    .description = "disconnect",
    .executer = _cli_disconnect_serial_port
  },
  {
    .command = "ls_dev",
    .description = "ls_dev",
    .executer = _cli_ls_dev
  },
  {
    .command = "ls",
    .description = "ls",
    .executer = _cli_ls
  },
  {
    .command = "cat_bin",
    .description = "cat_bin <file_name>",
    .executer = _cli_cat_bin
  },
  {
    .command = "write_port",
    .description = "write_port <data> <is_echo 0/1>",
    .executer = _cli_write_serial_port
  },
  {
    .command = "flash_read",
    .description = "flash_read <hex offset> <words>",
    .executer = _cli_flash_read
  },
  {
    .command = "flash_write",
    .description = "flash_write <hex offset> <data>",
    .executer = _cli_flash_write
  },
  {
    .command = "flash_erase",
    .description = "flash_erase <hex offset> <number of page>",
    .executer = _cli_flash_erase
  },
  {
    .command = "flash_upload",
    .description = "flash_upload <hex addressOffset> <filename>",
    .executer = _cli_flash_upload
  },
  {
    .command = "flash_verify",
    .description = "flash_verify <hex startAddressOffset> <hex endAddressOffset> <crc>",
    .executer = _cli_flash_verify
  },
  {
    .command = "modbus_silence",
    .description = "modbus_silence <isSilenced>",
    .executer = _cli_modbus_silence
  },
  {
    .command = "modbus_enter_boot",
    .description = "modbus_enter_boot <deviceId>",
    .executer = _cli_modbus_enter_bootloader
  },
  {
    .command = "modbus_get_boot_info",
    .description = "modbus_get_boot_info <deviceId>",
    .executer = _cli_modbus_get_boot_info
  },
  {
    .command = "exit",
    .description = "exit",
    .executer = _exit
  },  
};

static void *_cli_thread(void *params) {
  char cmdLine[CLI_BUFFER_SIZE] = {0};
  _isRunning = true;
  while (_isRunning) {
    _cli_get_line(cmdLine);
    _cli_process_command(_cli_string_parser(cmdLine, " "));
  }
  return NULL;
}

pthread_t cli_thread_start(void *params) {
  pthread_create(&_threadId, NULL, _cli_thread, NULL);
  return _threadId;
}

bool cli_get_running_status() {
  return _isRunning;
}

static _Cli_Command_Args_t _cli_string_parser(const char* line, const char* delimiter) {
  _Cli_Command_Args_t command_arg;
  GZ_LOG_DEBUG("DEBUG %s-%d\n", __FILE__,__LINE__);
  command_arg.command = strtok((char*)line, delimiter);
  if (command_arg.command == NULL) {
    return command_arg;
  }
  command_arg.args_count = 0;
  do {
    command_arg.command_args[command_arg.args_count] = strtok(NULL, delimiter);
    GZ_LOG_DEBUG("DEBUG - command_arg[%d]: %s\n", command_arg.args_count, command_arg.command_args[command_arg.args_count]);
  } while(command_arg.command_args[command_arg.args_count] && command_arg.args_count++ < MAX_CMD_ARGUMENTS);
  return command_arg;
}

#ifdef USE_JSON
static _Cli_Command_Args_t _cli_json_parser(const char* line) {
  _Cli_Command_Args_t command_arg = {
    .command = (char*)"example_of_cmd",
    .command_args = { (char*)"1" },
    .args_count = 1
  };
  ///\todo Insert Json parser code here
  return command_arg;
}
#endif

static bool _exit(_Cli_Command_Args_t command_arguments) {
  exit(0);
  return true;
}

static bool _cli_process_command(_Cli_Command_Args_t command_arguments) {
  bool cmd_found = false;
  if (!command_arguments.command) {
    GZ_LOG_DEBUG("() - empty command\n");
    return cmd_found;
  }
  for (unsigned int cmd_index = 0; cmd_index < (sizeof(_cli_commands) / sizeof(_Cli_Command_t)); cmd_index++) {
    GZ_LOG_DEBUG("%s\n", command_arguments.command);
    if (strcmp(_cli_commands[cmd_index].command, command_arguments.command) == 0) { // Found matched command
      cmd_found = true;
      if (_cli_commands[cmd_index].executer) { // Check null
        if (!_cli_commands[cmd_index].executer(command_arguments)) {
          GZ_LOG_ERROR("%s\n", _cli_commands[cmd_index].description); // Fail to execute
        }
      }
      break;
    }
  }
  if (!cmd_found) {
    GZ_LOG_ERROR("(%s) - command not found\n", command_arguments.command);
    _cli_help_executer(command_arguments);
  }
  return cmd_found;
}

static void _cli_get_line(char *line) {
  char cmdLine[CLI_BUFFER_SIZE] = {0};
  int count = 0;
  memset(line, 0, CLI_BUFFER_SIZE);
  printf(CLI_PROMPT);
  while (1) {
    char c = getchar();
    switch (c) {
      case NEW_LINE: // Falling through
      case CARRIAGE_RETURN:
      {
        if (count < CLI_BUFFER_SIZE) {
          cmdLine[count] = '\0'; // Added end of string
        } else {
          cmdLine[CLI_BUFFER_SIZE - 1] = '\0';
        }
        printf("\n");
        memcpy(line, cmdLine, count);
        return;
      }
      case BACK_SPACE: // back space
      {
        if (count > 0) {
          cmdLine[--count] = 0;
        }
        break;
      }
      default:
      {
        if (c >= ' ' && c <= '~') { // Readable characters
          if (count < CLI_BUFFER_SIZE - 1) {
            cmdLine[count++] = c;
            if (_isEcho) {
              printf("%c", c);
            }
          }
        }
        break;
      }
    }
  }
}

void cli_set_echo(bool is_echo) {
  _isEcho = is_echo;
}

static bool _cli_help_executer(_Cli_Command_Args_t command_arguments) {
  for (unsigned int i = 0; i < (sizeof(_cli_commands) / sizeof(_Cli_Command_t)); i++) {
    GZ_LOG_INFO("%s - (%s)\n", _cli_commands[i].command, _cli_commands[i].description);
  }
  return true;
}

static bool _cli_connect_serial_port(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("Device name and baudrate should not be empty!\n");
    return false;
  }
  char deviceName[32] = "/dev/";
  if (strstr(command_arguments.command_args[0], "/dev/") == NULL) {
    strcat(deviceName, (const char*)command_arguments.command_args[0]);
  }
  _uartFd = uart_connect(deviceName, atoi(command_arguments.command_args[1]));
  if (_uartFd == -1) {
    GZ_LOG_ERROR("cannot open serial port (%s)\n", deviceName);
  } else {
    GZ_LOG_INFO("Serial port opened: (%s)-(%s)\n", deviceName, command_arguments.command_args[1]);
  }
  return true;
}

static bool _cli_disconnect_serial_port(_Cli_Command_Args_t command_arguments) {
  uart_disconnect(_uartFd);
  _uartFd = 0;
  return true;
}

static bool _cli_write_serial_port(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("data and echo option should not be empty!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }
  uart_write(_uartFd, (unsigned char*)command_arguments.command_args[0], sizeof(command_arguments.command_args[0]));
  return true;
}

static bool _cli_cat_bin(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0]) {
    GZ_LOG_ERROR("file to read is empty\n");
    return false;
  }
  int fd = open(command_arguments.command_args[0], O_RDONLY);
  if (fd < 0) {
    GZ_LOG_ERROR("Could not open file: %s\n", command_arguments.command_args[0])
    return false;
  }
  uint8_t buffer[32] = { 0 };
  int chunkSizeByte = sizeof(buffer) / 2;
  while(read(fd, buffer, chunkSizeByte)) {
    memcpy(buffer + chunkSizeByte, buffer, chunkSizeByte);
    for (int i = 0; i < sizeof(buffer); i += 2) {
      if (i < chunkSizeByte) {
        printf("%02x%02x ", buffer[i], buffer[i+1]);
        if (i == chunkSizeByte - 2) {
          printf("  ");
        }
      } else {
        char firstByte = buffer[i];
        char secondByte = buffer[i+1];
        if (buffer[i] < ' ' || buffer[i] > '~') {
          firstByte = '.';
        }
        if (buffer[i+1] < ' ' || buffer[i+1] > '~') {
          secondByte = '.';
        }
        printf("%c%c ", firstByte, secondByte);
      }
    }
    printf("\n");
  }
  close(fd);
  return true;
}

static bool _cli_ls(_Cli_Command_Args_t command_arguments) {
  char dirName[32] = "./";
  if (command_arguments.command_args[0]) {
    strcpy(dirName, command_arguments.command_args[0]);
  }
  DIR *fd;
  struct dirent *pDir;
  fd = opendir(dirName);
  if (fd) {
    while ((pDir = readdir(fd)) != NULL) {
      printf("%s\n", pDir->d_name);
    }
    closedir(fd);
  }

  return true;
}

static bool _cli_ls_dev(_Cli_Command_Args_t command_arguments) {
  uart_list(MAC_DEVICE_PREFIX);
  return true;
}

static bool _cli_flash_read(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("Missing Offset and size!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }
  if (command_arguments.command_args[0][0] != '0' && command_arguments.command_args[0][0] != 'x') {
    GZ_LOG_ERROR("address format should be hex\n");
    return false;
  }
  int addressOffset = strtol(command_arguments.command_args[0], NULL, 16);
  int sizeInWords = atoi(command_arguments.command_args[1]);
  GZ_LOG_INFO("Yapi read request at addressOffset [%d] - size in words[%d]\n", addressOffset, sizeInWords);

  yapi_flash_read_request(addressOffset, sizeInWords);
  return true;
}

static bool _cli_flash_write(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("Missing Offset and size!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }
  int wordsLength = strlen(command_arguments.command_args[1]) / sizeof(uint32_t);
  if (strlen(command_arguments.command_args[1]) % sizeof(uint32_t)) {
    wordsLength ++; 
  }
  if (command_arguments.command_args[0][0] != '0' && command_arguments.command_args[0][0] != 'x') {
    GZ_LOG_ERROR("address format should be hex\n");
    return false;
  }
  int addressOffset = strtol(command_arguments.command_args[0], NULL, 16);
  GZ_LOG_INFO("Yapi write request at addressOffset [%d] - size in words[%d]\n", addressOffset, wordsLength);

  GZ_LOG_INFO("Yapi write request [%s]\n", command_arguments.command_args[1]);
  yapi_flash_write_request(addressOffset, command_arguments.command_args[1], wordsLength);
  return true;
}

static bool _cli_flash_erase(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("Missing Offset and size!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }
  if (command_arguments.command_args[0][0] != '0' && command_arguments.command_args[0][0] != 'x') {
    GZ_LOG_ERROR("address format should be hex\n");
    return false;
  }
  int addressOffset = strtol(command_arguments.command_args[0], NULL, 16);
  int pagesCount = atoi(command_arguments.command_args[1]);
  GZ_LOG_INFO("Yapi erase request at addressOffset [%d] - number of Pages[%d]\n", addressOffset, pagesCount);
  yapi_flash_erase_request(addressOffset, pagesCount);
  return true;
}

static bool _cli_flash_upload(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("flash_upload <addressOffset> <filename>\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }
  if (command_arguments.command_args[0][0] != '0' && command_arguments.command_args[0][0] != 'x') {
    GZ_LOG_ERROR("address format should be hex\n");
    return false;
  }
  int addressOffset = strtol(command_arguments.command_args[0], NULL, 16);
  GZ_LOG_INFO("Yapi upload request at addressOffset [%d] - file[%s]\n", addressOffset, command_arguments.command_args[1]);
  yapi_flash_upload_request(addressOffset, command_arguments.command_args[1]);
  return true;
}

static bool _cli_flash_verify(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0] || !command_arguments.command_args[1]) {
    GZ_LOG_ERROR("flash_verify <startAddressOffset> <endAddressOffset> <crc>\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }
  if (command_arguments.command_args[0][0] != '0' && command_arguments.command_args[0][0] != 'x') {
    GZ_LOG_ERROR("address format should be hex\n");
    return false;
  }
  int startAddressOffset = strtol(command_arguments.command_args[0], NULL, 16);
  int endAddressOffset = strtol(command_arguments.command_args[1], NULL, 16);
  uint16_t crc16 = atoi(command_arguments.command_args[2]);
  GZ_LOG_INFO("Yapi verify request at addressOffset [%x] - file[%x]\n", startAddressOffset, endAddressOffset);
  yapi_flash_verify_request(startAddressOffset, endAddressOffset, &crc16 );
  return true;
}

static bool _cli_modbus_silence(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0]) {
    GZ_LOG_ERROR("Missing argument!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }

  yapi_modbus_silence(atoi(command_arguments.command_args[0]));
  return true;
}

static bool _cli_modbus_enter_bootloader(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0]) {
    GZ_LOG_ERROR("Missing device Id!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }

  yapi_modbus_enter_bootloader((yapi_device_id_enum_t)atoi(command_arguments.command_args[0]));
  return true;
}

static bool _cli_modbus_get_boot_info(_Cli_Command_Args_t command_arguments) {
  if (!command_arguments.command_args[0]) {
    GZ_LOG_ERROR("Missing device Id!\n");
    return false;
  }
  if (!_uartFd) {
    GZ_LOG_ERROR("Port has not been opened\n");
    return false;
  }

  yapi_modbus_get_boot_info((yapi_device_id_enum_t)atoi(command_arguments.command_args[0]));
  return true;
}

#undef _Cli_Command_t
