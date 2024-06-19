#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "cli.h"
#include "gz_log.h"
#include "uart.h"
#include "yapi_manager.h"

#define CENTI_SECOND_IN_USEC 1000
#define DEMO_UART 0
int main(int argNum, char **arg) {
#if DEBUG
  gz_log_set_level(GZ_LOG_LEVEL_DEBUG);
#else
  gz_log_set_level(GZ_LOG_LEVEL_INFO);
#endif
  pthread_t cliThreadId = cli_thread_start(NULL);
  yapi_init();
  while (1) {
    usleep(CENTI_SECOND_IN_USEC);
    int fd = uart_get_connected_device();
    uint8_t buffer = 0;
    if (fd != UART_UNCONNECTED) {
      // For faster development, pooling directly from main
      ///\todo could do read blocking for improving cpu usage
      if (uart_read(fd, &buffer, 1)) {
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