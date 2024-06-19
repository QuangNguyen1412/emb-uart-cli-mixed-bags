/**
 * uart.cpp
 * 
 * Notes: On mac, have to add yourself to 'Wheel' user group then restart in order to access serial port
 * Eg: sudo dseditgroup -o edit -a qnguyen-gz -t user wheel
 * 
 * port name should be the /dev/cu.*
 * Please don't use /dev/tty.* as it blocks the open operation (It is not a Linux thing)
 * 
 * Author Quang Nguyen <quang.nguyen@goalzero.com>
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // read/write/close/sleep
#include <dirent.h> 

#include "uart.h"
#include "gz_array.h"
#include "gz_log.h"


#ifdef __cplusplus
extern "C" {
#endif


#define MAX_DEVICE_NAME_LENGTH            32
#define MAX_SERIAL_PORT_COUNT             1
#define DEVICE_DELIMITER                  "cu."

typedef struct {
  int fd;
  int baud;
  char name[MAX_DEVICE_NAME_LENGTH];
} _Uart_Info_t;

static _Uart_Info_t _uart_port_info[MAX_SERIAL_PORT_COUNT] = { {0} };
static int _uart_getc(int fd, unsigned char *c);
static pthread_mutex_t _lock;

v_fp_u8_t _uart_read_one_byte_cb;
/**
 * Open uart port
 * 
 * @param: device name (todo: device name does not have to be the whole path)
 * @param: baud rate
 * 
 * @return file descriptor
*/
int uart_connect(char* device_name, int baud_rate) {
  int available_port = 0;
  GZ_LOG_INFO("device name: (%s)\n", device_name);
  GZ_LOG_INFO("baud rate: (%d)\n", baud_rate);
  for (available_port = 0; available_port < MAX_SERIAL_PORT_COUNT; available_port++) {
    printf("available port: %d\n", available_port);
    if (_uart_port_info[available_port].fd) {
      continue;
    }
    _uart_port_info[available_port].baud = baud_rate;
    memcpy(_uart_port_info[available_port].name, device_name, MAX_DEVICE_NAME_LENGTH);
    GZ_LOG_INFO("opening devices: (%s)\n", device_name);
    _uart_port_info[available_port].fd = open(device_name, O_RDWR | O_NOCTTY);
    if (_uart_port_info[available_port].fd == -1) {
      GZ_LOG_ERROR("unable to open serial port. Exiting...\n");
      return -1;
    }
    struct termios option;
    GZ_LOG_INFO("getting attribute configuration: fd (%d)\n", _uart_port_info[available_port].fd);
    if (tcgetattr(_uart_port_info[available_port].fd, &option)) {
      close(_uart_port_info[available_port].fd);
      return -1;
    }

    // Turn off any options that might interfere with our ability to send and
    // receive raw binary bytes.
    option.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    option.c_oflag &= ~(ONLCR | OCRNL);
    option.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    
    // Set up timeouts: Calls to read() will return as soon as there is
    // at least one byte available or when 100 ms has passed.
    option.c_cc[VTIME] = 0;
    option.c_cc[VMIN] = 0;

    switch(baud_rate) {
      case 4800: 
        cfsetospeed(&option, B4800);
        break;
      case 9600: 
        cfsetospeed(&option, B9600);
        break;
      case 19200: 
        cfsetospeed(&option, B19200);
        break;
      case 38400: 
        cfsetospeed(&option, B38400);
        break;
      default:
        GZ_LOG_ERROR("Unsupported baudrate (%d): using 115200\n", baud_rate);  // Falling through 
        _uart_port_info[available_port].baud = 115200;
      case 115200:
        cfsetospeed(&option, B115200);
        break;
    }
    cfsetispeed(&option, cfgetospeed(&option));
    if (tcsetattr(_uart_port_info[available_port].fd, TCSANOW, &option)) {
      GZ_LOG_ERROR("tcsetattr failed\n");
      close(_uart_port_info[available_port].fd);
      return -1;
    }
    break; // Break for
  }
  if (available_port == MAX_SERIAL_PORT_COUNT) {
    return -1;
  }
  if (pthread_mutex_init(&_lock, NULL) != 0) {
      GZ_LOG_ERROR("\n mutex init has failed\n");
      return 1;
  }

  return _uart_port_info[available_port].fd;
}

void uart_disconnect(int fd) {
  for(int i = 0; i < MAX_SERIAL_PORT_COUNT; i++) {
    if (fd == _uart_port_info[i].fd) {
      close(fd);
      _uart_port_info[i].fd = 0;
      _uart_port_info[i].baud = 0;
      memset(_uart_port_info[i].name, 0, sizeof(_uart_port_info[i].name));
    }
  }
}

void uart_list(const char* devDelimiter) {
  DIR *fd;
  struct dirent *dir;
  fd = opendir("/dev/");
  if (fd) {
    while ((dir = readdir(fd)) != NULL) {
      if (strstr(dir->d_name, devDelimiter)) {
        printf("%s\n", dir->d_name);
      }
    }
    closedir(fd);
  }
}

/**
 * @brief Read incoming data from host serial device (non blocking)
 * Should not be called from ISR
 *
 * @param buffer stores the data read
 * @param size expected buffer size in bytes
 * @return int number of character read
 */
int uart_read(int fd, unsigned char *buffer, int size) {
  int portId = 0;
  int readCount = 0;
  unsigned char c = 0;
  for (portId = 0; portId < arrayLength(_uart_port_info); portId++) {
    if (_uart_port_info[portId].fd == fd) {
      break;
    }
  }
  if (portId == arrayLength(_uart_port_info)) {
    GZ_LOG_ERROR("Port has not been opened (%d)\n", fd);
    return -1;
  }
  pthread_mutex_lock(&_lock);
  while(readCount < size && _uart_getc(fd, &c)) {
    buffer[readCount++] = c;
  }
  pthread_mutex_unlock(&_lock);
  return readCount;
}

/**
 * Not async
*/
static int _uart_getc(int fd, unsigned char *c) {
  if (read(fd, c, 1)) {
    if (_uart_read_one_byte_cb) {
      _uart_read_one_byte_cb(*c);
    }
    return 1;
  }
  return 0;
}

/**
 * @brief Write data to host (non blocking)
 * Should not be called from ISR
 *
 * @param buffer Data need to be sent
 * @param size Size of data buffer in bytes
 * @return int number of character written
 */
int uart_write(int fd, unsigned char *buffer, int size) {
  int port_id = 0;
  for (port_id = 0; port_id < arrayLength(_uart_port_info); port_id++) {
    if (_uart_port_info[port_id].fd == fd) {
      break;
    }
  }
  if (port_id == arrayLength(_uart_port_info)) {
    GZ_LOG_ERROR("Port has not been opened (%d)\n", fd);
    return -1;
  }
  pthread_mutex_lock(&_lock);
  int readSize = write(fd, buffer, size);
  pthread_mutex_unlock(&_lock);
  return readSize;
}

void uart_register_read_one_byte_callback(v_fp_u8_t cb) {
  _uart_read_one_byte_cb = cb;
}

int uart_get_connected_device() {
  return _uart_port_info[0].fd;
}

#ifdef __cplusplus
}
#endif
