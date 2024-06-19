#ifndef _H_CLI
#define _H_CLI

#include <pthread.h>
#define MAX_CMD_ARGUMENTS       4
#define CLI_BUFFER_SIZE         64


///\todo cli_init taking in an iostream perhaps
pthread_t cli_thread_start(void* param);
void cli_set_echo(bool);
bool cli_get_running_status();
#endif //_H_CLI
