
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>

#include "tq_handle_list.h"

//------------------------------------------------------------------------------

static int32_t map_node(handle_list_t *handle_list, handle_list_node_t *node)
{
    if (handle_list->map_used == handle_list->map_size) {
        size_t next_size = handle_list->map_size + (handle_list->map_size / 2);
        if (next_size == 0) {
            next_size = 256;
        }

        handle_list_node_t **next_map = realloc(handle_list->map, sizeof(handle_list_node_t *) * next_size);
        if (!next_map) {
            return -1;
        }

        handle_list->map_size = next_size;
        handle_list->map = next_map;
    }

    int32_t handle = handle_list->map_used++;
    handle_list->map[handle] = node;

    return handle;
}

//------------------------------------------------------------------------------

handle_list_t *handle_list_create(size_t item_size, handle_list_item_dtor item_dtor)
{
    handle_list_t *handle_list = calloc(1, sizeof(handle_list_t));
    if (!handle_list) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_create(): Failed to allocate list.\n");
        return NULL;
    }

    handle_list->item_size = item_size;
    handle_list->item_dtor = item_dtor;

    return handle_list;
}

void handle_list_destroy(handle_list_t *handle_list)
{
    while (handle_list->last) {
        handle_list_node_t *node = handle_list->last;
        handle_list->last = node->prev;
        if (handle_list->item_dtor) {
            handle_list->item_dtor(node->item);
        }
        free(node);
    }

    free(handle_list);
}

int32_t handle_list_append(handle_list_t *handle_list, void *item)
{
    handle_list_node_t *node = malloc(sizeof(handle_list_node_t) + handle_list->item_size);

    if (!node) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_append(): Failed to allocate list node.\n");
        return -1;
    }

    int32_t handle = map_node(handle_list, node);

    if (handle == -1) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_append(): Failed to map list node.\n");

        if (handle_list->item_dtor) {
            handle_list->item_dtor(item);
        }

        free(node);
        return -1;
    }

    node->prev = NULL;
    node->next = NULL;
    memcpy(node->item, item, handle_list->item_size);

    if (handle_list->last) {
        handle_list->last->next = node;
        node->prev = handle_list->last;
    }

    handle_list->last = node;

    return handle;
}

void handle_list_erase(handle_list_t *handle_list, int32_t handle)
{
    if (handle < 0 || handle >= (int64_t) handle_list->map_used) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_erase(): Handle is out of bounds.\n");
        return;
    }

    if (!handle_list->map[handle]) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_erase(): Item is already erased.\n");
        return;
    }

    handle_list_node_t *node = handle_list->map[handle];

    if (node == handle_list->last) {
        handle_list->last = node->prev;
    }

    if (node->prev) {
        node->prev->next = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    }

    if (handle_list->item_dtor) {
        handle_list->item_dtor(node->item);
    }

    free(node);
    handle_list->map[handle] = NULL;
}

void *handle_list_get(handle_list_t *handle_list, int32_t handle)
{
    if (handle < 0 || handle >= (int64_t) handle_list->map_used) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_get(): Handle is out of bounds.\n");
        return NULL;
    }

    if (!handle_list->map[handle]) {
        // libtq_log(LIBTQ_LOG_WARNING, "handle_list_get(): Item is erased.\n");
        return NULL;
    }

    return (void *)handle_list->map[handle]->item;
}

//------------------------------------------------------------------------------
