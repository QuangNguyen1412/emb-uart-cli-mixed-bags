#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "provision_cli.h"
#include "gz_log.h"
#include "uart.h"
#include "yapi_manager.h"

#define CENTI_SECOND_IN_USEC 1000
#define DEMO_UART 0
int main(int argc, char *argv[]) {
  int option;
  char *deviceName = NULL;
  char *baudRate = NULL;

  // Process command line options
  while ((option = getopt(argc, argv, "p:b:")) != -1) {
      switch (option) {
          case 'p':
              deviceName = optarg;
              break;
          case 'b':
              baudRate = optarg;
              break;
          case '?':
              if (optopt == 'p' || optopt == 'b') {
                  fprintf(stderr, "Option -%c requires an argument.\n", optopt);
              } else {
                  fprintf(stderr, "Unknown option -%c.\n", optopt);
              }
              return 1;
          default:
              return 1;
      }
  }

  // Check if required options were provided
  if (deviceName == NULL || baudRate == NULL) {
      fprintf(stderr, "Missing required options.\n");
      return 1;
  }

  // Print the parsed options and their values
  printf("Option -p has value: %s\n", deviceName);
  printf("Option -b has value: %s\n", baudRate);
  int _fd = uart_connect(deviceName, atoi(baudRate));
  if (_fd == -1) {
    GZ_LOG_ERROR("cannot open serial port (%s)\n", deviceName);
  } else {
    GZ_LOG_INFO("Serial port opened: (%s)-(%s)\n", deviceName, baudRate);
  }

#if DEBUG
  gz_log_set_level(GZ_LOG_LEVEL_DEBUG);
#else
  gz_log_set_level(GZ_LOG_LEVEL_INFO);
#endif
  pthread_t cliThreadId = cli_thread_start(NULL);
  yapi_init();
  while (1) {
    usleep(CENTI_SECOND_IN_USEC);
    int _fd = uart_get_connected_device();
    uint8_t buffer = 0;
    if (_fd != UART_UNCONNECTED) {
      // For faster development, pooling directly from main
      ///\todo could do read blocking for improving cpu usage
      if (uart_read(_fd, &buffer, 1)) {
#if DEMO_UART
        printf("rev - %02x\t%c\n", buffer, buffer);
#endif
      }
    }
    yapi_task_10ms(NULL);
  }
  pthread_join(cliThreadId, NULL);
  return 1;
}