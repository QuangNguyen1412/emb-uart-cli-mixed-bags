/** @file gz_observer.c
* 
* @brief helpers to implement observer pattern using singled linked list
*
* Authors:
* - Quang Nguyen <quang.nguyen@goalzero.com>
*
* @par
* COPYRIGHT NOTICE: (c) 2024 Goal Zero.
* All rights reserved.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "gz_observer.h"

static int _gz_observer_count(gz_observer_node_t* observerHead_ptr) {
  int count = 0;
  gz_observer_node_t *currentObserver_ptr = observerHead_ptr;
  while (currentObserver_ptr != NULL) {
    count++;
    currentObserver_ptr = currentObserver_ptr->nextObserver_ptr;
  }
  return count;
}

void gz_observer_destroy(gz_observer_node_t **observerHead_ptr) {
  if (observerHead_ptr == NULL) {
    return;
  }
  gz_observer_node_t *currentObserver_ptr = *(observerHead_ptr);
  while (currentObserver_ptr != NULL) {
    gz_observer_node_t *nextObserver_ptr = currentObserver_ptr->nextObserver_ptr;
    free(currentObserver_ptr);
    currentObserver_ptr = nextObserver_ptr;
  }
  *observerHead_ptr = NULL;
}

int gz_observer_add_to_list(gz_observer_node_t **observerHead_ptr, gz_observer_cb_t cb) {
  if (observerHead_ptr == NULL) {
    return -1;
  }
  if (cb == NULL) {
    return _gz_observer_count(*(observerHead_ptr));
  }
  gz_observer_node_t *newObserver_ptr = (gz_observer_node_t*)calloc(1, sizeof(gz_observer_node_t));
  if (newObserver_ptr == NULL) {
    // Failed to allocate memory
    return -1;
  }
  newObserver_ptr->callback = cb;
  newObserver_ptr->nextObserver_ptr = *(observerHead_ptr);
  *(observerHead_ptr) = newObserver_ptr;

  return _gz_observer_count(newObserver_ptr);
}

int gz_observer_remove_from_list(gz_observer_node_t **observerHead_ptr, gz_observer_cb_t cb) {
  if (observerHead_ptr == NULL) {
    return -1;
  }
  if (cb == NULL) {
    return _gz_observer_count(*(observerHead_ptr));
  }
  gz_observer_node_t *currentObserver_ptr = *(observerHead_ptr);
  gz_observer_node_t *prevObserver_ptr = NULL;
  while (currentObserver_ptr != NULL) {
    if (currentObserver_ptr->callback == cb) {
      if (prevObserver_ptr == NULL) {
        // First observer, update the head
        *(observerHead_ptr) = currentObserver_ptr->nextObserver_ptr;
      } else {
        // Link the previous observer to the next observer
        prevObserver_ptr->nextObserver_ptr = currentObserver_ptr->nextObserver_ptr;
      }
      // Remove the observer
      free(currentObserver_ptr);
      uint8_t count = _gz_observer_count(*(observerHead_ptr));
      if (count == 0) {
        // No more observer, set the head to NULL
        *(observerHead_ptr) = NULL;
      }
      return count;
    }
    // Not found, move to the next observer
    prevObserver_ptr = currentObserver_ptr;
    currentObserver_ptr = currentObserver_ptr->nextObserver_ptr;
  }
  return _gz_observer_count(*(observerHead_ptr));
}

void gz_observer_notify(gz_observer_node_t* observerHead_ptr, void *context_ptr) {
  gz_observer_node_t *currentObserver_ptr = observerHead_ptr;
  while (currentObserver_ptr != NULL) {
    currentObserver_ptr->callback(context_ptr);
    currentObserver_ptr = currentObserver_ptr->nextObserver_ptr;
  }
}
