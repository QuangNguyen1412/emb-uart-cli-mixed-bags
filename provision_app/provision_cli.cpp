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

#include "provision_cli.h"
#include "uart.h"
#include "gz_log.h"
#include "yapi_provision.h"

#define BACK_SPACE              8
#define NEW_LINE                '\n'
#define CARRIAGE_RETURN         '\r'
#define CLI_PROMPT              "> "
#define YAPI_RESP_TIMEOUT_S     5
#define YAPI_RESP_1S_US         1000000    
#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_YELLOW       "\x1b[33m"
#define ANSI_COLOR_BLUE         "\x1b[34m"
#define ANSI_COLOR_MAGENTA      "\x1b[35m"
#define ANSI_COLOR_CYAN         "\x1b[36m"
#define ANSI_COLOR_RESET        "\x1b[0m"

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



static bool _isEcho = false;
static bool _isRunning = false;
// static int _uartFd;
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
static bool _cli_start_provision(_Cli_Command_Args_t);
static bool _cli_read_id(_Cli_Command_Args_t);
static bool _exit(_Cli_Command_Args_t);

/**
 * @brief verify the scanned serial format
 * @note format should be `SKU-Rev-YYMxxxxx`
 * @return false when 'M' is not in the range [A:L]
 * @return false when the format does not follow `SKU-Rev-YYMxxxxx`
 * @return false when the serial string is more than 25
 * **/
static bool _serial_number_is_valid(char*);

_Cli_Command_t _cli_commands[] = {
  {
    .command = "help",
    .description = "Show all commands with description",
    .executer = _cli_help_executer
  },
  {
    .command = "1",
    .description = "Start Provision - Scan Serial Number",
    .executer = _cli_start_provision
  },
  {
    .command = "2",
    .description = "Read IDs",
    .executer = _cli_read_id
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
          GZ_LOG_ERROR(ANSI_COLOR_RED "Failed to execute [%s]\n" ANSI_COLOR_RESET, _cli_commands[cmd_index].description); // Fail to execute
          _cli_help_executer(command_arguments);
        }
      }
      break;
    }
  }
  if (!cmd_found) {
    GZ_LOG_ERROR(ANSI_COLOR_RED "(%s) - command not found\n" ANSI_COLOR_RESET, command_arguments.command);
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

static bool _cli_help_executer(_Cli_Command_Args_t command_arguments) {
  printf("\n\r\n\r");
  printf("******* PROVISIONING APP OPTIONS *******\n\r");
  for (unsigned int i = 0; i < (sizeof(_cli_commands) / sizeof(_Cli_Command_t)); i++) {
    printf("%s - (%s)\n", _cli_commands[i].command, _cli_commands[i].description);
  }
  printf("******* PROVISIONING APP OPTIONS ends *******\n\r");
  return true;
}

static bool _cli_start_provision(_Cli_Command_Args_t command_arguments) {
  printf(ANSI_COLOR_GREEN "******* Provision starts *******\n\r" ANSI_COLOR_RESET);
  printf("Please scan a serial number: \n\r");
  char serialNumber[YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE] = {0};
  scanf("%s", serialNumber);
  if ( !_serial_number_is_valid(serialNumber) ) {
    return false;
  }
  printf("serial number scanned: [%s]\n\r", serialNumber);
  yapi_provision_send_serial_number(serialNumber, sizeof(serialNumber));
  uint16_t timeout_s = 0;
  while(timeout_s++ < YAPI_RESP_TIMEOUT_S) {
    if (yapi_provision_get_serial_write_respond_status() == YAPI_SERIAL_RESPONSE_OK) {
      break;
    }
    usleep(YAPI_RESP_1S_US);
  }
  if (yapi_provision_get_serial_write_respond_status() == YAPI_SERIAL_RESPONSE_OK) {
    _Cli_Command_Args_t command_arguments;
    _cli_read_id(command_arguments);
  } else {
    GZ_LOG_ERROR(ANSI_COLOR_RED "Serial write error [%s]" ANSI_COLOR_RESET, yapi_provision_serial_response_status_string(yapi_provision_get_serial_write_respond_status()));
    return false;
  }
  return true;
}

static bool _cli_read_id(_Cli_Command_Args_t command_arguments) {
  yapi_provision_send_serial_read_request();
  uint16_t timeout_s = 0;
  while(timeout_s++ < YAPI_RESP_TIMEOUT_S) {
    if (yapi_provision_get_serial_read_respond_status() == YAPI_SERIAL_RESPONSE_OK) {
      break;
    }
    usleep(YAPI_RESP_1S_US);
  }
  if (yapi_provision_get_serial_read_respond_status() != YAPI_SERIAL_RESPONSE_OK) {
    GZ_LOG_ERROR(ANSI_COLOR_RED "Serial read error [%d]" ANSI_COLOR_RESET, yapi_provision_serial_response_status_string(yapi_provision_get_serial_read_respond_status()));
    return false;
  }
  printf(ANSI_COLOR_GREEN "\r\n******* Provision SUCCESS *******" ANSI_COLOR_RESET);
  _cli_help_executer(command_arguments);
  return true;
}

static bool _serial_number_is_valid(char* serialNumber) {
  bool isValid = true;
  char *token = NULL;
  char serialNumberLocal[18] = {0};
  int tokenCount = 0;
  if (strlen(serialNumber) > YAPI_MFG_DATA_SERIAL_NUMBER_BUFFER_SIZE) {
    GZ_LOG_ERROR(ANSI_COLOR_RED "??? - Serial Number format should be 'SKU-Rev-YYMxxxxx'\n\r" ANSI_COLOR_RESET);
    return false;
  }

  memcpy(serialNumberLocal, (const char*)serialNumber, strlen(serialNumber));
  token = strtok(serialNumberLocal, "-");
  if (token) {
    tokenCount++;
    while ((token = strtok(NULL, "-"))) {
      if (++tokenCount == 3) {
        if (token[2] < 'A' || token[2] > 'L') { // Check the month letter
          GZ_LOG_ERROR(ANSI_COLOR_RED "(ERR) - '%s' format should be 'YYMxxxxx', where M should be in range A->L\n\r" ANSI_COLOR_RESET);
          return false;
        }
      }
    }
  }
  if (tokenCount != 3) {
    GZ_LOG_ERROR(ANSI_COLOR_RED "(ERR) - Serial Number format should be 'SKU-Rev-YYMxxxxx'\n\r" ANSI_COLOR_RESET);
    return false;
  }
  return isValid;
}

void cli_set_echo(bool is_echo) {
  _isEcho = is_echo;
}

pthread_t cli_thread_start(void *params) {
  pthread_create(&_threadId, NULL, _cli_thread, NULL);
  _Cli_Command_Args_t arg;
  _cli_help_executer(arg);
  return _threadId;
}

bool cli_get_running_status() {
  return _isRunning;
}

#undef _Cli_Command_t
