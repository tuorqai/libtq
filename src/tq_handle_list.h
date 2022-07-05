
//------------------------------------------------------------------------------

#ifndef TQ_HANDLE_LIST_H_INC
#define TQ_HANDLE_LIST_H_INC

//------------------------------------------------------------------------------

#include "tq/tq.h"

//------------------------------------------------------------------------------

typedef void (*handle_list_item_dtor)(void *item);

typedef struct handle_list_node
{
    struct handle_list_node *prev;
    struct handle_list_node *next;
    unsigned char item[];
} handle_list_node_t;

typedef struct handle_list
{
    handle_list_node_t *last;
    size_t item_size;
    handle_list_item_dtor item_dtor;

    size_t map_used;
    size_t map_size;
    handle_list_node_t **map;
} handle_list_t;

handle_list_t *handle_list_create(size_t item_size, handle_list_item_dtor item_dtor);
void handle_list_destroy(handle_list_t *handle_list);

int32_t handle_list_append(handle_list_t *handle_list, void *item);
void handle_list_erase(handle_list_t *handle_list, int32_t handle);
void *handle_list_get(handle_list_t *handle_list, int32_t handle);

//------------------------------------------------------------------------------

#endif // TQ_HANDLE_LIST_H_INC
