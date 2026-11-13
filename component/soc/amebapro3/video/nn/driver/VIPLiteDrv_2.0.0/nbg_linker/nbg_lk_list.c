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

#include <nbg_lk_list.h>

void NBGLK_INIT_LIST_HEAD(struct _nbglk_list_head *list)
{
    list->next = list;
    list->prev = list;
}

void _nbglk_list_add(nbglk_list_head_t *new, nbglk_list_head_t *prev, nbglk_list_head_t *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void nbglk_list_add_tail(nbglk_list_head_t *new, nbglk_list_head_t *head)
{
    _nbglk_list_add(new, head->prev, head);
}

void _nbglk_list_del(nbglk_list_head_t *prev, nbglk_list_head_t *next)
{
    next->prev = prev;
    prev->next = next;
}

void _nbglk_list_del_entry(nbglk_list_head_t *entry)
{
    _nbglk_list_del(entry->prev, entry->next);
}

void nbglk_list_del(nbglk_list_head_t *entry)
{
    _nbglk_list_del_entry(entry);
    entry->next = VIP_NULL;
    entry->prev = VIP_NULL;
}

vip_uint32_t nbglk_list_empty(nbglk_list_head_t *head)
{
    return head->next == head;
}

vip_bool_e nbglk_list_is_last(nbglk_list_head_t *list, nbglk_list_head_t *head)
{
    return list->next == head;
}

void nbglk_list_add(nbglk_list_head_t *new, nbglk_list_head_t *head)
{
    _nbglk_list_add(new, head, head->next);
}

void nbglk_list_replace(nbglk_list_head_t *old, nbglk_list_head_t *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

vip_bool_e nbglk_list_is_first(nbglk_list_head_t *list, nbglk_list_head_t *head)
{
    return list->prev == head;
}

vip_bool_e nbglk_list_is_init(nbglk_list_head_t *head)
{
    return (!(head->next == VIP_NULL && head->prev == VIP_NULL));
}
