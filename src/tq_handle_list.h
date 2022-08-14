
//------------------------------------------------------------------------------

#ifndef TQ_HANDLE_LIST_H_INC
#define TQ_HANDLE_LIST_H_INC

//------------------------------------------------------------------------------

#include "tq_error.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------
// Generated symbols:
// struct <OBJECT>_array;
// <OBJECT>_array_initialize;
// <OBJECT>_array_terminate;
// <OBJECT>_array_check;
// <OBJECT>_array_add;
// <OBJECT>_array_remove;
//------------------------------------------------------------------------------

#define _FLEXIBLE_ARRAY_GEN_CONTROL_STRUCT(Struct) \
    struct Struct##_array \
    { \
        void (*dtor)(struct Struct *item); \
        int *init; \
        int count; \
        struct Struct *data; \
    };

#define _FLEXIBLE_ARRAY_GEN_INIT_FUNC(Struct) \
    static void Struct##_array_initialize(struct Struct##_array *array, \
        int initial_count, \
        void (*dtor)(struct Struct *item)) \
    { \
        array->data = libtq_malloc(initial_count * sizeof(struct Struct)); \
        array->init = libtq_malloc(initial_count * sizeof(int)); \
        \
        if (!array->data || !array->init) { \
            libtq_out_of_memory(); \
        } \
        array->count = initial_count; \
        array->dtor = dtor; \
        \
        for (int i = 0; i < array->count; i++) { \
            array->init[i] = 0; \
        } \
    }

#define _FLEXIBLE_ARRAY_GEN_TERM_FUNC(Struct) \
    static void Struct##_array_terminate(struct Struct##_array *array) \
    { \
        if (array->dtor) { \
            for (int i = 0; i < array->count; i++) { \
                if (array->init[i]) { \
                    array->dtor(&array->data[i]); \
                } \
            } \
        } \
        \
        libtq_free(array->data); \
        libtq_free(array->init); \
    }

#define _FLEXIBLE_ARRAY_GEN_CHECK_FUNC(Struct) \
    static int Struct##_array_check(struct Struct##_array *array, int id) \
    { \
        return (id >= 0) && (id < array->count) && (array->init[id]); \
    }

#define _FLEXIBLE_ARRAY_GEN_ADD_FUNC(Struct) \
    static int Struct##_array_add(struct Struct##_array *array, struct Struct const *item) \
    { \
        int id = -1; \
        \
        for (int i = 0; i < array->count; i++) { \
            if (!array->init[i]) { \
                id = i; \
                break; \
            } \
        } \
        \
        if (id == -1) { \
            int old_count = array->count; \
            array->count *= 2; \
            array->data = libtq_realloc(array->data, sizeof(struct Struct) * array->count); \
            array->init = libtq_realloc(array->init, sizeof(int) * array->count); \
            \
            if (!array->data || !array->init) { \
                libtq_out_of_memory(); \
            } \
            \
            for (int i = old_count; i < array->count; i++) { \
                array->init[i] = 0; \
            } \
            \
            id = old_count; \
        } \
        \
        memcpy(&array->data[id], item, sizeof(struct Struct)); \
        array->init[id] = 1; \
        \
        return id; \
    }

#define _FLEXIBLE_ARRAY_GEN_REMOVE_FUNC(Struct) \
    static void Struct##_array_remove(struct Struct##_array *array, int id) \
    { \
        if (!Struct##_array_check(array, id)) { \
            return; \
        } \
        \
        if (array->dtor) { \
            array->dtor(&array->data[id]); \
        } \
        \
        array->init[id] = 0; \
    }

//------------------------------------------------------------------------------

#define DECLARE_FLEXIBLE_ARRAY(Struct) \
    _FLEXIBLE_ARRAY_GEN_CONTROL_STRUCT(Struct) \
    _FLEXIBLE_ARRAY_GEN_INIT_FUNC(Struct) \
    _FLEXIBLE_ARRAY_GEN_TERM_FUNC(Struct) \
    _FLEXIBLE_ARRAY_GEN_CHECK_FUNC(Struct) \
    _FLEXIBLE_ARRAY_GEN_ADD_FUNC(Struct) \
    _FLEXIBLE_ARRAY_GEN_REMOVE_FUNC(Struct)

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
