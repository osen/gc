#include <gc.h>

#include <stdio.h>
#include <stdlib.h>

struct ListData
{
  void **data;
  size_t count;
};

#define ENABLE_LIST \
  struct ListData *LIST_data; \

#define LIST_ADD(A, N) \
  if(*A == NULL) \
  { \
    *A = N; \
    (*A)->LIST_data = gc_alloc(sizeof(struct ListData)); \
    (*A)->LIST_data->data = gc_alloc(sizeof(void*)); \
    (*A)->LIST_data->data[0] = N; \
    (*A)->LIST_data->count = 1; \
  } \
  else \
  { \
  }

struct Card
{
  ENABLE_LIST

  int suit;
  int number;
  int value;
};

struct Root
{
  struct Card **cards;
};

int main(int argc, char* argv[])
{
  struct Root *root = gc_root(sizeof(*root));

  root->cards = gc_alloc(sizeof(*root->cards));

  struct Card *newCard = gc_alloc(sizeof(*newCard));

  LIST_ADD(root->cards, newCard);
  printf("done\n");

  gc_collect();

  return 0;
}
