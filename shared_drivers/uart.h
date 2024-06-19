#ifndef _LINUX_UART_
#define _LINUX_UART_

#ifdef __cplusplus
extern "C" {
#endif

#define UART_UNCONNECTED        0
#include <stdint.h>

typedef void (*v_fp_u8_t)(uint8_t);

/**
 * Open uart port
 * 
 * @param: device name
 * @param: baud rate
 * 
 * @return file descriptor
 */
int uart_connect(char*, int);

/**
 * @param port descriptor
 */
void uart_disconnect(int);

/**
 * @brief Read incoming data from host serial device
 * Should not be called from ISR
 *
 * @param buffer stores the data read
 * @param size expected buffer size in bytes
 * @return int number of character read
 */
int uart_read(int fd, unsigned char *buffer, int size);

/**
 * @brief Write data to host
 * Should not be called from ISR
 *
 * @param buffer Data need to be sent
 * @param size Size of data buffer in bytes
 * @return int number of character written
 */
int uart_write(int fd, unsigned char *buffer, int size);

/**
 * List devices under /dev/ directory
 * @param: delimiter of device name (tty/cu...)
*/
void uart_list(const char*);

/**
 * @brief: register cb for every byte read
*/
void uart_register_read_one_byte_callback(v_fp_u8_t cb);

/**
 * @brief: get File descriptor of a connected device after calling `uart_connect`
 * Caller need to sanity the return value with UART_UNCONNECTED
 * @return: File descriptor of the connected device
 * 
*/
int uart_get_connected_device();

#ifdef __cplusplus
}
#endif

#endif
