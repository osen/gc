#include "LinkedList.h"

struct LinkedListItem
{
  void *ptr;
  struct LinkedListItem *prev;
  struct LinkedListItem *next;
};

struct LinkedList
{
  struct LinkedListItem *first;
  struct LinkedListItem *last;
  struct GcContext *gc_ctx;
};

struct LinkedListItem *LinkedListItemNext(struct LinkedListItem *ctx)
{
  return ctx->next;
}

void *LinkedListItemGet(struct LinkedListItem *ctx)
{
  return ctx->ptr;
}

struct LinkedList *LinkedListCreate(struct GcContext * gc_ctx)
{
  struct LinkedList *rtn = NULL;

  rtn = gc_alloc(gc_ctx, sizeof(*rtn));
  rtn->gc_ctx = gc_ctx;

  return rtn;
}

struct LinkedListItem *LinkedListFirst(struct LinkedList *ctx)
{
  return ctx->first;
}

struct LinkedListItem *LinkedListAdd(struct LinkedList *ctx, void *ptr)
{
  struct LinkedListItem *newItem = NULL;

  newItem = gc_alloc(ctx->gc_ctx, sizeof(*newItem));
  newItem->ptr = ptr;

  if(ctx->last == NULL)
  {
    ctx->first = newItem;
    ctx->last = newItem;
  }
  else
  {
    newItem->prev = ctx->last;
    ctx->last->next = newItem;
    ctx->last = newItem;
  }

  return newItem;
}