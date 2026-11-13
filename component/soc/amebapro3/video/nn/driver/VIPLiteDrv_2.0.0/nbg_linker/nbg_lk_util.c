/******************************************************************************\
|* Copyright (c) 2017-2024 by Vivante Corporation.  All Rights Reserved.      *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of Vivante Corporation.  This is proprietary information owned by Vivante  *|
|* Corporation.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of Vivante Corporation.                                 *|
|*                                                                            *|
\******************************************************************************/

#include <nbg_lk_util.h>
#include <vip_hal_os_port.h>
#if vpmdENABLE_MULTIPLE_TASK && vpmdENABLE_CAPTURE
vip_status_e nbglk_lock_recursive_mutex(
    nbglk_recursive_mutex_t *recursive_mutex
    )
{
    vip_status_e error = VIP_SUCCESS;
    vip_uint32_t cur_tid = viphal_os_get_tid();

    if (cur_tid == recursive_mutex->current_tid) {
        recursive_mutex->ref_count += 1;
    }
    else {
        error = viphal_os_lock_mutex(recursive_mutex->mutex);
        recursive_mutex->current_tid = cur_tid;
        recursive_mutex->ref_count += 1;
    }
    return error;
}

vip_status_e nbglk_unlock_recursive_mutex(
    nbglk_recursive_mutex_t *recursive_mutex
    )
{
    vip_status_e error = VIP_SUCCESS;
    vip_uint32_t cur_tid = viphal_os_get_tid();

    if (cur_tid == recursive_mutex->current_tid) {
        recursive_mutex->ref_count -= 1;
    }
    else {
        vip_error("thread %d call unlock before getting the mutex\n", cur_tid);
    }

    if (0 >= recursive_mutex->ref_count) {
        recursive_mutex->current_tid = 0;
        recursive_mutex->ref_count = 0;
        error = viphal_os_unlock_mutex(recursive_mutex->mutex);
    }

    return error;
}
#endif

#if vpmdENABLE_RECORD_INFO
static vip_uint64_t hash(
    vip_uint64_t h
    )
{
    h = (h << 5) - h;
    return (h ^ (h >> 8) ^ (h >> 16));
}

static void left_rotate(
    nbglk_hashmap_t *hashmap,
    vip_uint32_t index,
    nbglk_hashmap_node_t *header
    )
{
    vip_uint32_t parent = hashmap->node_array[index].parent_index;
    vip_uint32_t right = hashmap->node_array[index].right_index;
    /* current node is root or not */
    nbglk_hashmap_node_t *p_parent = (0 == parent) ? header : &hashmap->node_array[parent];
    vip_uint32_t *p = p_parent->left_index == index ?
                      &p_parent->left_index : &p_parent->right_index;

    *p = right;
    hashmap->node_array[index].right_index = hashmap->node_array[right].left_index;
    hashmap->node_array[index].parent_index = right;
    hashmap->node_array[right].left_index = index;
    hashmap->node_array[right].parent_index = parent;
    hashmap->node_array[hashmap->node_array[index].right_index].parent_index = index;
}

static void right_rotate(
    nbglk_hashmap_t *hashmap,
    vip_uint32_t index,
    nbglk_hashmap_node_t *header
    )
{
    vip_uint32_t parent = hashmap->node_array[index].parent_index;
    vip_uint32_t left = hashmap->node_array[index].left_index;
    /* current node is root or not */
    nbglk_hashmap_node_t *p_parent = (0 == parent) ? header : &hashmap->node_array[parent];
    vip_uint32_t *p = p_parent->left_index == index ?
                      &p_parent->left_index : &p_parent->right_index;

    *p = left;
    hashmap->node_array[index].left_index = hashmap->node_array[left].right_index;
    hashmap->node_array[index].parent_index = left;
    hashmap->node_array[left].right_index = index;
    hashmap->node_array[left].parent_index = parent;
    hashmap->node_array[hashmap->node_array[index].left_index].parent_index = index;
}

vip_status_e nbglk_hashmap_init(
    IN nbglk_hashmap_t *hashmap,
    IN vip_uint32_t init_capacity,
    IN vip_uint32_t size_per_element,
    IN vip_char_t_ptr name
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_int32_t i = 0;
    vip_uint32_t next_index = 0;
    hashmap->capacity = init_capacity + 1; /* index 0: idle list header */
    hashmap->size_per_element = size_per_element;
    hashmap->name = name;

#if vpmdENABLE_MULTIPLE_TASK
    status = viphal_os_create_mutex(&hashmap->mutex);
    if (status != VIP_SUCCESS) {
        vip_error("fail to create mutex for hashmap %s\n", name);
        vipGoOnError(status);
    }
#endif
    vipOnError(viphal_allocate_memory(sizeof(nbglk_hashmap_node_t) * hashmap->capacity,
                                      (void**)&hashmap->node_array));
    viphal_os_zero_memory(hashmap->node_array, sizeof(nbglk_hashmap_node_t) * hashmap->capacity);

    if (0 < hashmap->size_per_element) {
        vipOnError(viphal_allocate_memory(hashmap->size_per_element * hashmap->capacity,
                                          (void**)&hashmap->container));
        viphal_os_zero_memory(hashmap->container, hashmap->size_per_element * hashmap->capacity);
    }

    for (i = hashmap->capacity - 1; i > 0; i--) {
        hashmap->node_array[i].right_index = next_index;
        hashmap->node_array[i].left_index = 0;
        next_index = i;
    }
    hashmap->idle_list_head = &hashmap->node_array[0];
    hashmap->idle_list_head->right_index = next_index;
    for (i = 0; i < HASH_MAP_CAPABILITY; i++) {
        hashmap->hashmap[i].right_index = 0;
    }
    hashmap->free_pos = 1; /* index 0: idle list header */
    vip_print("HASHMAP %p(%s) INIT SUCCESS\n", hashmap, name);
    return status;

onError:
#if vpmdENABLE_MULTIPLE_TASK
    if (VIP_NULL != hashmap->mutex) {
        viphal_os_destroy_mutex(hashmap->mutex);
        hashmap->mutex = VIP_NULL;
    }
#endif
    if (VIP_NULL != hashmap->node_array) {
        viphal_free_memory(hashmap->node_array);
        hashmap->node_array = VIP_NULL;
    }
    if (VIP_NULL != hashmap->container) {
        viphal_free_memory(hashmap->container);
        hashmap->container = VIP_NULL;
    }
    return status;
}

vip_status_e nbglk_hashmap_expand(
    IN nbglk_hashmap_t *hashmap
    )
{
    vip_uint32_t i = 0;
    vip_uint32_t next_index = 0;
    vip_uint32_t index = 0;
    vip_status_e status = VIP_SUCCESS;
    nbglk_hashmap_node_t *new_node_array = VIP_NULL;
    void *new_container;
#if vpmdENABLE_MULTIPLE_TASK
    status = viphal_os_lock_mutex(hashmap->mutex);
    if (status != VIP_SUCCESS) {
        vip_error("fail to lock mutex when hashmap expand\n");
        return status;
    }
#endif
    if (vip_false_e == nbglk_hashmap_full(hashmap)) {
        vip_debug("hashmap has been expanded\n");
        vipGoOnError(status);
    }
    vipOnError(viphal_allocate_memory(sizeof(nbglk_hashmap_node_t) *
                                       (hashmap->capacity + HASH_MAP_CAPABILITY),
                                       (void**)&new_node_array));
    viphal_os_zero_memory(new_node_array,
                          sizeof(nbglk_hashmap_node_t) * (hashmap->capacity + HASH_MAP_CAPABILITY));

    if (0 < hashmap->size_per_element) {
        status = viphal_allocate_memory(hashmap->size_per_element *
                                          (hashmap->capacity + HASH_MAP_CAPABILITY),
                                          (void**)&new_container);
        if (VIP_SUCCESS != status) {
            viphal_free_memory(new_node_array);
            vipOnError(status);
        }
        viphal_os_zero_memory(new_container, hashmap->size_per_element * (hashmap->capacity + HASH_MAP_CAPABILITY));
        viphal_copy_memory(new_container, hashmap->container,
                          hashmap->size_per_element * hashmap->capacity);
        viphal_free_memory(hashmap->container);
        hashmap->container = new_container;
    }

    viphal_copy_memory(new_node_array, hashmap->node_array,
                      sizeof(nbglk_hashmap_node_t) * hashmap->capacity);
    viphal_free_memory(hashmap->node_array);

    hashmap->node_array = new_node_array;
    hashmap->capacity += HASH_MAP_CAPABILITY;
    for (i = 0; i < HASH_MAP_CAPABILITY; i++) {
        index = hashmap->capacity - 1 - i;
        hashmap->node_array[index].right_index = next_index;
        hashmap->node_array[index].left_index = 0;
        next_index = index;
    }
    hashmap->idle_list_head = &hashmap->node_array[0];
    hashmap->idle_list_head->right_index = next_index;

onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(hashmap->mutex);
#endif
    return status;
}

vip_status_e nbglk_hashmap_destroy(
    IN nbglk_hashmap_t *hashmap
    )
{
    vip_int32_t i = 0;
#if vpmdENABLE_MULTIPLE_TASK
    if (VIP_NULL != hashmap->mutex) {
        viphal_os_destroy_mutex(hashmap->mutex);
        hashmap->mutex = VIP_NULL;
    }
#endif
    if (VIP_NULL != hashmap->node_array) {
        viphal_free_memory(hashmap->node_array);
        hashmap->node_array = VIP_NULL;
    }
    if (VIP_NULL != hashmap->container) {
        viphal_free_memory(hashmap->container);
        hashmap->container = VIP_NULL;
    }
    hashmap->idle_list_head = VIP_NULL;
    hashmap->capacity = 0;
    for (i = 0; i < HASH_MAP_CAPABILITY; i++) {
        hashmap->hashmap[i].right_index = 0;
    }
    hashmap->free_pos = 0;

    return VIP_SUCCESS;
}

vip_status_e nbglk_hashmap_insert(
    IN nbglk_hashmap_t *hashmap,
    IN vip_uint64_t handle,
    OUT vip_uint32_t *index
    )
{
    vip_uint32_t cur = 0;
    vip_uint32_t h = 0;
    vip_uint32_t parent = 0;
    vip_uint32_t grand_p = 0;
    vip_uint32_t uncle = 0;
    vip_uint32_t *p = VIP_NULL;
    nbglk_hashmap_node_t *header = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_MULTIPLE_TASK
    status = viphal_os_lock_mutex(hashmap->mutex);
    if (status != VIP_SUCCESS) {
        vip_error("fail to lock mutex when hashmap put\n");
        return status;
    }
#endif
    h = (HASH_MAP_CAPABILITY - 1) & hash(handle);
    header = &hashmap->hashmap[h];

    /* insert into tree */
    p = &header->right_index;
    while (0 != *p) {
        header = &hashmap->node_array[*p];
        parent = *p;
        if (handle > header->handle) {
            p = &header->right_index;
        }
        else if (handle < header->handle) {
            p = &header->left_index;
        }
        else {
            /* handle exist */
            *index = *p;
            vip_debug("HASHMAP %s INSERT: handle 0x%"PRIx64" exist in hashmap\n", hashmap->name, handle);
            vipGoOnError(VIP_ERROR_INVALID_ARGUMENTS);
        }
    }

    /* use the first node in idle list */
    cur = hashmap->idle_list_head->right_index;
    /* no idle node */
    if (0 == cur) {
        *index = 0;
        vip_debug("HASHMAP %s INSERT: hashmap is full\n", hashmap->name);
        vipGoOnError(VIP_ERROR_OUT_OF_RESOURCE);
    }

    hashmap->idle_list_head->right_index = hashmap->node_array[cur].right_index;
    hashmap->node_array[cur].parent_index = parent;
    hashmap->node_array[cur].right_index = 0;
    hashmap->node_array[cur].left_index = 0;
    hashmap->node_array[cur].handle = handle;
    /* link parent node to current node */
    *p = cur;
    /* record ID of current node */
    *index = cur;
    /*vip_debug("hashmap %p: insert index=%d, handle=%p\n", hashmap, *index, handle);*/
    /* reorder red black tree */
    while (0 != cur) {
        parent = hashmap->node_array[cur].parent_index;
        hashmap->node_array[cur].is_black = vip_false_e;
        if (0 == parent) {
            hashmap->node_array[cur].is_black = vip_true_e;
            break;
        }
        else if (vip_true_e == hashmap->node_array[parent].is_black) {
            break;
        }
        else {
            grand_p = hashmap->node_array[parent].parent_index;
            uncle = hashmap->node_array[grand_p].left_index == parent ?
                    hashmap->node_array[grand_p].right_index : hashmap->node_array[grand_p].left_index;
            if (0 != uncle && vip_false_e == hashmap->node_array[uncle].is_black) {
                hashmap->node_array[uncle].is_black = vip_true_e;
                hashmap->node_array[parent].is_black = vip_true_e;
                hashmap->node_array[grand_p].is_black = vip_false_e;
                cur = grand_p;
            }
            else if (parent == hashmap->node_array[grand_p].left_index) {
                if (cur == hashmap->node_array[parent].left_index) {
                    hashmap->node_array[parent].is_black = vip_true_e;
                    hashmap->node_array[grand_p].is_black = vip_false_e;
                    right_rotate(hashmap, grand_p, &hashmap->hashmap[h]);
                }
                else {
                    left_rotate(hashmap, parent, &hashmap->hashmap[h]);
                    cur = parent;
                }
            }
            else {
                if (cur == hashmap->node_array[parent].right_index) {
                    hashmap->node_array[parent].is_black = vip_true_e;
                    hashmap->node_array[grand_p].is_black = vip_false_e;
                    left_rotate(hashmap, grand_p, &hashmap->hashmap[h]);
                }
                else {
                    right_rotate(hashmap, parent, &hashmap->hashmap[h]);
                    cur = parent;
                }
            }
        }
    }

    if (*index >= hashmap->free_pos) {
        hashmap->free_pos = *index + 1;
    }

onError:
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(hashmap->mutex);
#endif
    return status;
}

vip_status_e nbglk_hashmap_get_by_index(
    IN nbglk_hashmap_t *hashmap,
    IN vip_uint32_t index,
    OUT void **element
    )
{
    vip_status_e status = VIP_SUCCESS;
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_lock_mutex(hashmap->mutex);
#endif
    if (0 < index && index < hashmap->capacity) {
        if (VIP_NULL != element) {
            if (0 == hashmap->size_per_element) {
                *element = VIPUINT64_TO_PTR(hashmap->node_array[index].handle);
            }
            else {
                *element = (void*)((vip_uint8_t*)hashmap->container + index * hashmap->size_per_element);
            }
        }
    }
    else {
        vip_debug("HASHMAP %s GET BY INDEX: index %d out of range\n", hashmap->name, index);
        status = VIP_ERROR_INVALID_ARGUMENTS;
    }
#if vpmdENABLE_MULTIPLE_TASK
    viphal_os_unlock_mutex(hashmap->mutex);
#endif
    return status;
}

vip_uint32_t nbglk_hashmap_free_pos(
    nbglk_hashmap_t *hashmap
    )
{
    return hashmap->free_pos;
}

vip_bool_e nbglk_hashmap_full(
    nbglk_hashmap_t *hashmap
    )
{
    return (0 == hashmap->idle_list_head->right_index) ? vip_true_e : vip_false_e;
}
#endif
