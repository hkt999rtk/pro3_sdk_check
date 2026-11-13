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
#include "vip_hal_heap.h"

#if vpmdENABLE_SYS_MEMORY_HEAP
#include "vip_hal_os_port.h"

#define STATUS_USED     0xABBAF00D
#define STATUS_FREE     0x5A5A5A5A
#define STATUS_INIT     0xCDCDCDCD

#define SYS_MEM_ALIGN_BYTES    4

static vip_status_e new_node(
    heap_t *heap,
    heap_node_t **node
    )
{
    vip_status_e status = VIP_SUCCESS;
    vip_uint32_t i;

    do {
        if (heap->node_count >= heap->node_capacity) {
            vip_error("sys mem node count=%d, node capacity=%d\n",
                        heap->node_count, heap->node_capacity);
            status = VIP_ERROR_OUT_OF_RESOURCE;
            break;
        }

        for (i = 0; i < heap->node_capacity; i++) {
            if (heap->nodes[i].status == STATUS_INIT) {
                break;
            }
        }

        *node = &heap->nodes[i];
        heap->node_count++;
    } while(0);

    return status;
}

static vip_status_e del_node(
    heap_t *heap,
    heap_node_t *node
    )
{
    vip_status_e status = VIP_SUCCESS;

    if (VIP_NULL == node) {
        vip_error("fail to delete node, node is NULL\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }

    node->status = STATUS_INIT;
    heap->node_count--;

    return status;
}

/* Add the list item in front of "head". */
static void add_list(
    list_head_t *new,
    list_head_t *prev,
    list_head_t *next
    )
{
    /* Link the new item. */
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static void add_list_tail(
    list_head_t *new,
    list_head_t *head
    )
{
    add_list(new, head->prev, head);
}

/* Remove an entry out of the list. */
static void delete_list(
    list_head_t *entry
    )
{
    if (entry->prev != VIP_NULL) {
        entry->prev->next = entry->next;
    }
    if (entry->next != VIP_NULL) {
        entry->next->prev = entry->prev;
    }
}

static vip_status_e split_node(
    heap_t *heap,
    heap_node_t * node,
    vip_uint32_t size
    )
{
    vip_status_e status = VIP_SUCCESS;
    heap_node_t *split;

    do {
        /* Allocate a new node. */
        status = new_node(heap, &split);
        if (status != VIP_SUCCESS) {
            vip_error("fail to split node status=%d\n", status);
            break;
        }

        /* Fill in the data of this node of the remaning size. */
        split->offset = node->offset + size;
        split->size = node->size - size;
        split->status = STATUS_FREE;

        /* Add the new node before the current node. */
        add_list_tail(&split->list, &node->list);

        /* Adjust the size of the current node. */
        node->size = size;
    } while(0);

    return status;
}

static void *find_node(
    heap_t *heap,
    void *memory,
    vip_uint32_t status
    )
{
    heap_node_t *node = VIP_NULL;
    vip_uint32_t offset = 0;
    vip_uint32_t i;

    offset = (vip_uint32_t)((vip_uint8_t *)memory - (vip_uint8_t *)heap->memory);

    for (i = 0; i < heap->node_capacity; i++) {
        if ((heap->nodes[i].offset == offset) &&
            (heap->nodes[i].status == status))
        {
            node = &heap->nodes[i];
            break;
        }
    }

    return node;
}

vip_size_t viphal_heap_max_size(
    heap_t *heap
    )
{
    vip_size_t max_size = 0;
    heap_node_t *pos = VIP_NULL;

    if (0 == heap->node_count) {
        return 0;
    }

    pos = (heap_node_t *)heap->list.next;

    for (; &pos->list != &heap->list; pos = (heap_node_t*)pos->list.next) {
        if (pos->status == STATUS_FREE) {
            if (pos->size > max_size) {
                max_size = pos->size;
            }
        }
    }

    if (max_size > heap->free_bytes) {
        max_size = heap->free_bytes;
    }

    return max_size;
}

void *viphal_heap_alloc(
    heap_t *heap,
    vip_size_t size,
    void **logical
    )
{
    heap_node_t *pos = VIP_NULL;
    vip_uint32_t aligned_size = 0;

    /* align to 4 Bytes */
    aligned_size = VIP_ALIGN(size, SYS_MEM_ALIGN_BYTES);
    /* Check if there is enough free_bytes memory available. */
    if (aligned_size > heap->free_bytes) {
        vip_error("system memory alloc failed. size: 0x%x, free: 0x%x\n",
                     aligned_size, heap->free_bytes);
        return VIP_NULL;
    }

    /* Walk the heap backwards. */
    for (pos = (heap_node_t *)heap->list.next;
                 &pos->list != &heap->list;
                 pos = (heap_node_t*)pos->list.next) {
        /* Check if the current node is free_bytes and is big enough. */
        if ((pos->status == STATUS_FREE) && (pos->size >= aligned_size)) {
            /* See if we the current node is big enough to split. */
            if (pos->size > aligned_size) {
                split_node(heap, pos, aligned_size);
            }

            /* Mark the current node as used. */
            pos->status = STATUS_USED;

            *logical = (vip_uint8_t *)heap->memory + pos->offset;

            /* Update free size. */
            heap->free_bytes -= pos->size;

            /* Return the node pointer as a handle. */
            return pos;
        }
    }

    /* Out of memory. */
    return VIP_NULL;
}

vip_status_e viphal_heap_get_memsize(
    heap_t *heap,
    void *memory,
    vip_size_t *size
    )
{
    vip_status_e status = VIP_SUCCESS;
    heap_node_t *node = VIP_NULL;

    node = find_node(heap, memory, STATUS_USED);
    if (node == VIP_NULL) {
        vip_error("fail to get memory size, node is NULL\n");
        status = VIP_ERROR_OUT_OF_RESOURCE;
    }
    else if (size != VIP_NULL) {
        *size = node->size;
    }
    else {
        vip_error("fail to get memory size, size is NULL\n");
        status = VIP_ERROR_OUT_OF_RESOURCE;
    }

    return status;
}

vip_status_e viphal_heap_free_mem(
    heap_t *heap,
    void *memory
    )
{
    vip_status_e status = VIP_SUCCESS;
    void *node = VIP_NULL;

    do {
        node = find_node(heap, memory, STATUS_USED);
        if (node == VIP_NULL) {
            status = VIP_ERROR_OUT_OF_RESOURCE;
            break;
        }

        status = viphal_heap_free(heap, node);
    } while(0);

    return status;
}

vip_status_e viphal_heap_free(
    heap_t *heap,
    void *handle
    )
{
    heap_node_t *pos = VIP_NULL;
    heap_node_t *node = VIP_NULL;
    vip_status_e status = VIP_SUCCESS;

    if (handle == VIP_NULL) {
        vip_error("fail to free heap\n");
        return VIP_ERROR_INVALID_ARGUMENTS;
    }
    /* Get pointer to node. */
    node = (heap_node_t *)handle;

    if (node->status != STATUS_USED) {
        vip_error("node is used\n");
        return VIP_ERROR_FAILURE;
    }

    /* Mark node as free_bytes. */
    node->status = STATUS_FREE;

    /* Add node size to free_bytes count. */
    heap->free_bytes += node->size;

    pos = (heap_node_t *)heap->list.next;
    for (; &pos->list != &heap->list; pos = (heap_node_t*)pos->list.next) {
        if (pos->offset == (node->offset + node->size)) {
            if (pos->status == STATUS_FREE) {
                node->size += pos->size;
                delete_list(&pos->list);
                del_node(heap, pos);
            }
        }
        else if ((pos->offset + pos->size) == node->offset) {
            if (pos->status == STATUS_FREE) {
                /* Merge the nodes. */
                pos->size += node->size;
                delete_list(&node->list);
                del_node(heap, node);
                node = pos;
            }
        }
    }

    return status;
}

vip_status_e viphal_heap_construct(
    heap_t *heap,
    vip_size_t size,
    void *logical
    )
{
    vip_status_e status = VIP_SUCCESS;
    heap_node_t *node = VIP_NULL;
    vip_size_t nodes_size = 0;
    vip_uint32_t i = 0;
    vip_size_t node_cap = 0;

    if ((VIPPTR_TO_UINT64(logical) % SYS_MEM_ALIGN_BYTES) != 0) {
        vip_error("fail sys memory heap base address=%p not align to %d\n",
                    logical, SYS_MEM_ALIGN_BYTES);
        return VIP_ERROR_FAILURE;
    }

    if (size > 0x1000000) { /* 10M byte */
        node_cap = size / 4096; /* 4k bytes pre-node */
    }
    else if (size > 0x100000) { /* 1M byte */
        node_cap = size / 1024; /* 1k bytes pre-node */
    }
    else if (size > 0x80000) { /* 512K byte */
        node_cap = size / 512; /* 512 bytes pre-node */
    }
    else if (size > 0x19000) { /* 100k byte */
        node_cap = size / 256; /* 256 bytes pre-node */
    }
    else if (size > 0x2800) { /* 10k byte */
        node_cap = size / 125; /* 125 bytes pre-node */
    }
    else {
        node_cap = 60;
    }

    if (size < sizeof(heap_node_t) * node_cap) {
        vip_error("sys_mem_size too small, at least needs to be more than 0x%x bytes.\n",
                sizeof(heap_node_t) * node_cap);
        return VIP_ERROR_FAILURE;
    }

    INIT_LIST_HEAD(&heap->list);
    nodes_size = sizeof(heap_node_t) * node_cap;
    heap->nodes = (heap_node_t *)((vip_uint8_t *)logical + (size - nodes_size));
    heap->node_capacity = (vip_uint32_t)node_cap;
    heap->memory = logical;
    heap->free_bytes = size - nodes_size;
    heap->node_count = 0;
    heap->node_max_count = 0;
    vip_debug("system memory heap total free: 0x%x bytes, node used: 0x%x bytes, node capacity: %d\n",
                heap->free_bytes, nodes_size, node_cap);

    for (i = 0; i < node_cap; i++) {
        heap->nodes[i].status = STATUS_INIT;
    }

    status = new_node(heap, &node);
    if (status != VIP_SUCCESS) {
        vip_error("fail to new node create heap\n");
        return VIP_ERROR_FAILURE;
    }

    node->offset = 0;
    node->size = heap->free_bytes;
    node->status = STATUS_FREE;
    add_list_tail(&node->list, &heap->list);

    return status;
}

vip_status_e viphal_heap_destroy(
    heap_t *heap
    )
{
    heap_node_t *pos = VIP_NULL;
    heap_node_t *n = VIP_NULL;
    list_head_t *hlist = VIP_NULL;

    if ((heap != VIP_NULL) && (heap->node_count != 0)) {
        pos = (heap_node_t *)heap->list.next;
        n = (heap_node_t *)pos->list.next;
        hlist = &heap->list;
        /* Process each node. */
        for (;&pos->list != hlist; n = (heap_node_t *)pos->list.next) {
            /* Remove it from the linked list. */
            delete_list(&pos->list);

            /* Free up the memory. */
            del_node(heap, pos);
            pos = n;
            if (heap->node_count == 0) {
                break;
            }
        }
        /* zero heap */
        viphal_zero_memory(heap, sizeof(heap_t));
    }

    return VIP_SUCCESS;
}
#endif
