/** @file gz_observer.h
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

#ifndef GZ_OBSERVER_H
#define GZ_OBSERVER_H

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif /* __cplusplus */

typedef void (*gz_observer_cb_t)(void *);

typedef struct gz_observer_node {
  gz_observer_cb_t callback;
  struct gz_observer_node* nextObserver_ptr;
} gz_observer_node_t;

/**
 * @brief destroy the observer list
 * @param gz_observer_node_t: pointer to the head of the observers
 * **/
void gz_observer_destroy(gz_observer_node_t**);

/**
 * @brief Insert an observer to HEAD of list, this doesn't check for duplicates
 * @param gz_observer_node_t: pointer to the head of the observers
 * @param gz_observer_cb_t: callback to be added
 * @return int: number of observers in the list
 * **/
int gz_observer_add_to_list(gz_observer_node_t**, gz_observer_cb_t cb);

/**
 * @brief Remove the first found observer from the list
 * @param gz_observer_node_t: pointer to the head of the observers
 * @param gz_observer_cb_t: callback to be removed
 * @return int: number of observers in the list
 * **/
int gz_observer_remove_from_list(gz_observer_node_t**, gz_observer_cb_t cb);

/**
 * @brief Notify all observers
 * @param gz_observer_node_t: pointer to the head of the observers
 * @param context: context to be passed to the observers
 * **/
void gz_observer_notify(gz_observer_node_t*, void* context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GZ_OBSERVER_H */
