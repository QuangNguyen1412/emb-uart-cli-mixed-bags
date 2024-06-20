#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> // for timeval
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // for inet_ntop
#include <errno.h> // for errno

#include "cli.h"
#include "gz_log.h"
#include "uart.h"
// #include "yapi_manager.h"

#define SLEEP_S 1
#define SLEEP_CS (SLEEP_S)*100
#define SLEEP_UC (SLEEP_CS)*1000
#define DEMO_UART 0
int main(int argc, char *argv[]) {
  int option;
  char *sockName = NULL;
  char *port_s = NULL;
  int port_n = 23; // default socket port
  bool isRunning = true;
  int maxFd = 0;
  int connFd = 0; // Connection file descriptor - subject to change as more connections are accepted

  // Process command line options
  while ((option = getopt(argc, argv, "n:p:")) != -1) {
      switch (option) {
          case 'n':
              sockName = optarg;
              break;
          case 'p':
              port_s = optarg;
              break;
          case '?':
              if (optopt == 'n') {
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
  if (sockName == NULL ) {
      fprintf(stderr, "Missing required options.\n");
      return 1;
  }
  if (port_s == NULL) {
    printf("Default port to [%d]\n", port_n);
  }
  // Print the parsed options and their values
  printf("Option -n has value: %s\n", sockName);

#if DEBUG
  gz_log_set_level(GZ_LOG_LEVEL_DEBUG);
#else
  gz_log_set_level(GZ_LOG_LEVEL_INFO);
#endif
  pthread_t cliThreadId = cli_thread_start(NULL);
  int socketDomain = AF_INET; // Address Family IPv4 - UDP, TCP, etc. designate type of addresses that socket can communicate with
  int socketType = SOCK_STREAM; // Socket TYPE - TCP, require an end-to-end connection. Must be connected before sending/receiving data
  /* Alternative UDP */
  // int socketType = SOCK_DGRAM; // Socket TYPE - UDP. recvfrom() and sendto() are used to receive and send data
  /* Alternative UDP Ends */
  int socketProto = 0; // Protocol - IP protocol

  // Create a socket
  int socketFd = socket(socketDomain, socketType, socketProto);
  if (socketFd < 0) {
    GZ_LOG_ERROR("Socket creation failed\n");
    return 1;
  }

  // Create a socket address for binding
  struct sockaddr_in sockaddr_v4;
  sockaddr_v4.sin_family = socketDomain;
  sockaddr_v4.sin_port = htons(port_n); // assign a port to bind the socket to. `htons` translate host byte order to network byte order
  sockaddr_v4.sin_addr.s_addr = INADDR_ANY; // bind to any available interface. can also use inet_addr("0.0.0.0")
  // Bind the socket to the address
  int bindStatus = bind(socketFd, (struct sockaddr *)&sockaddr_v4, sizeof(sockaddr_v4));
  if (bindStatus < 0) {
    GZ_LOG_ERROR("Socket bind failed\n");
    return 1;
  }
  char ipaddress_s[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(AF_INET, &(sockaddr_v4.sin_addr), ipaddress_s, INET_ADDRSTRLEN);
  printf("Socket bind successful ip address[%s], port[%d]\n", ipaddress_s, port_n);

  // Put the socket to a state that it listens for incoming connections
  int listenStatus = listen(socketFd, 5); // 5 is the maximum number of connections that can be queued
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(socketFd, &readfds);
  maxFd = socketFd > maxFd ? socketFd : maxFd;
  timeval timeout = { .tv_sec = 1, .tv_usec = 500000 }; // 1.5s

  while (isRunning) {
    fd_set currFd = readfds;
    int ret = select(maxFd + 1, &currFd, NULL, NULL, &timeout); // Polling the readfds for any activity
    if (ret < 0) { // select Error
      GZ_LOG_ERROR("Errno [%d]\n", errno);
      continue;
    } else if (ret == 0) { // select Timeout
      GZ_LOG_DEBUG("Timeout\n");
      continue;
    } else { // select Success
      GZ_LOG_INFO("We received something\n");
      for (int activeFd = 0; activeFd < FD_SETSIZE; activeFd++) {
        if (!FD_ISSET(activeFd, &currFd)) {
          continue; // not this file descriptor
        }
        // Found the active fd
        if(activeFd == socketFd) {
          GZ_LOG_INFO("Accepting a new connection\n");
          struct sockaddr_in client_address;
          int client_len = sizeof(client_address);
          int newFd = accept(socketFd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
          if (newFd < 0) {
            GZ_LOG_ERROR("Accept failed\n");
            continue;
          }
          struct sockaddr_in tempsockaddr_v4 = (sockaddr_in)client_address;
          char client_ipaddress_s[INET_ADDRSTRLEN] = {'\0'};
          inet_ntop(AF_INET, &(tempsockaddr_v4.sin_addr), client_ipaddress_s, INET_ADDRSTRLEN);
          GZ_LOG_INFO("Connection accepted ip address[%s], port[%d]\n", client_ipaddress_s, ntohs(tempsockaddr_v4.sin_port));
          FD_SET(newFd, &readfds);
          maxFd = newFd > maxFd ? newFd : maxFd;
        } else { // Not socketFD, must be from a newFd
          GZ_LOG_INFO("Data available on one of the newFd [%d]\n", activeFd);
          char buffer[1024] = {'\0'};
          int bytesRead = recv(activeFd, buffer, sizeof(buffer), 0);
          if (bytesRead < 0) {
            GZ_LOG_ERROR("Recv failed\n");
            continue;
          } else if (bytesRead == 0) {
            GZ_LOG_INFO("Connection closed fd[%d]\n", activeFd);
            close(activeFd);
            FD_CLR(activeFd, &readfds);
          } else {
            GZ_LOG_INFO("Received  >> %s\n", buffer);
          }
        } // newFd is active
      } // For activeFd
    } // Select success
    GZ_LOG_INFO("Hello from main\n");
  } // While isRunning
  pthread_join(cliThreadId, NULL);
  return 1;
}