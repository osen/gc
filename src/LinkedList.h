#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#include <gc.h>

struct LinkedListItem;

struct LinkedListItem *LinkedListItemNext(struct LinkedListItem *ctx);
void *LinkedListItemGet(struct LinkedListItem *ctx);

struct LinkedList;

struct LinkedList *LinkedListCreate(struct GcContext* gc_ctx);
struct LinkedListItem *LinkedListFirst(struct LinkedList *ctx);
struct LinkedListItem *LinkedListLast(struct LinkedList *ctx);
int LinkedListSize(struct LinkedList *ctx);
struct LinkedListItem *LinkedListAt(struct LinkedList *ctx, int index);
struct LinkedListItem *LinkedListAdd(struct LinkedList *ctx, void *ptr);

#endif