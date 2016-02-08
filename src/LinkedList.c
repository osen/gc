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

struct LinkedListItem *LinkedListLast(struct LinkedList *ctx)
{
  return ctx->last;
}

int LinkedListSize(struct LinkedList *ctx)
{
  struct LinkedListItem *current = NULL;
  int rtn = 0;

  current = ctx->first;

  while(current != NULL)
  {
    rtn ++;
    current = current->next;
  }

  return rtn;
}

struct LinkedListItem *LinkedListAt(struct LinkedList *ctx, int index)
{
  struct LinkedListItem *rtn = NULL;
  int i = 0;

  rtn = ctx->first;

  for(i = 0; i < index; i++)
  {
    if(rtn == NULL)
    {
      printf("Index out of bounds\n");
      exit(1);
    }

    rtn = rtn->next;
  }

  return rtn;
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