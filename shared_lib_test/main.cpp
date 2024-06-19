#include <stdlib.h>
#include <stdio.h>
#include "gz_observer.h"

static gz_observer_node_t* _observer = NULL;
static void _cb_1(void *data) {
  printf("Notify 1 [%s]\n", (char*)data);
}
static void _cb_2(void *data) {
  printf("Notify 2 [%s]\n", (char*)data);
}
static void _cb_3(void *data) {
  printf("Notify 3 [%s]\n", (char*)data);
}
static void _cb_4(void *data) {
  printf("Notify 4 [%s]\n", (char*)data);
}
uint8_t register_cb(void (*cb)(void *), const char *name) {
  printf("Registering callback [%s]\n\r", name);
  return gz_observer_add_to_list(&_observer, cb);
}

uint8_t unregister_cb(void (*cb)(void *), const char *name) {
  printf("Unregistering callback [%s]\n\r", name);
  return gz_observer_remove_from_list(&_observer, cb);
}

int main(int argc, char** argv) {
  printf("## CB registration and notify test ##\n\r");
  gz_observer_notify(_observer, NULL);
  register_cb(_cb_1, "1");
  register_cb(_cb_2, "2");
  register_cb(_cb_3, "3");
  gz_observer_add_to_list(&_observer, _cb_1);
  printf("Total callbacks: %d\n", register_cb(_cb_4, "4"));
  gz_observer_notify(_observer, NULL);

  printf("## CB unregistration and notify test ##\n\r");
  unregister_cb(_cb_1, "1");
  gz_observer_notify(_observer, NULL);
  unregister_cb(_cb_3, "3");
  gz_observer_notify(_observer, NULL);
  unregister_cb(_cb_2, "2");
  gz_observer_notify(_observer, NULL);
  unregister_cb(_cb_4, "4");
  gz_observer_notify(_observer, NULL);

  printf("## CB notification with input test ##\n\r");
  register_cb(_cb_1, "1");
  register_cb(_cb_2, "2");
  gz_observer_notify(_observer, (void*)"data");
  printf("## CBs deinit test ##\n\r");
  gz_observer_destroy(&_observer);
  printf("Deinit the observer, test notify after destroy\r\n");
  gz_observer_notify(_observer, NULL);

  printf("## Test NULL observer\r\n");
  gz_observer_add_to_list(NULL, NULL);
  printf("Test NULL observer: didn't crash\r\n");
  return 0;
}