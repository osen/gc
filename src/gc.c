#include <gc.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct GcBlockReference
{
  struct GcBlock *block;
  struct GcBlockReference *next;
  struct GcBlockReference *prev;
};

struct GcBlock
{
  void *ptr;
  size_t size;
  void (*deleter)(void*);
  struct GcBlockReference *references;
  int generation;
  struct GcBlock *next;
  struct GcBlock *prev;
};

struct GcContext
{
  struct GcBlock *root;
};

int gc_addblock(struct GcBlock *block);
void gc_purgeblocks();

struct GcContext _gc_context = {0};

/******************************************************************************
 * gc_addblock
 *
 * Reallocate the blocks array to make room for a copy of the new block.
 ******************************************************************************/
int gc_addblock(struct GcBlock *block)
{
  struct GcBlock *last = NULL;

  last = _gc_context.root;

  if(last == NULL)
  {
    return 1;
  }

  while(last->next != NULL)
  {
    last = last->next;
  }

  last->next = block;
  block->prev = last;

  return 0;
}

void gc_freereferences(struct GcBlock *ctx)
{
  struct GcBlockReference *reference = NULL;
  struct GcBlockReference *tofree = NULL;

  reference = ctx->references;

  while(reference != NULL)
  {
    tofree = reference;
    reference = reference->next;
    free(tofree); tofree = NULL;
  }

  ctx->references = NULL;
}

/******************************************************************************
 * gc_purgeblocks
 *
 * Iterate backwards through the blocks array to see if each one is marked
 * as to be purged. If so, execute its delete function on its contained pointer
 * and unless it is the last one of the array, move all the other blocks up to
 * fill up the space and replace it.
 ******************************************************************************/
void gc_purgeblocks()
{
  struct GcBlock *tofree = NULL;
  struct GcBlock *block = NULL;

  block = _gc_context.root;

  if(block == NULL)
  {
    return;
  }

  while(block->next != NULL)
  {
    block = block->next;
  }

  while(block != _gc_context.root)
  {
    if(block->generation == -1)
    {
      if(block->deleter != NULL)
      {
        block->deleter(block->ptr);
      }

      if(block->size > 0)
      {
        free(block->ptr); block->ptr = NULL;
      }

      if(block->references != NULL)
      {
        gc_freereferences(block);
      }

      block->prev->next = block->next;

      if(block->next != NULL)
      {
        block->next->prev = block->prev;
      }

      tofree = block;
      block = block->prev;
      free(tofree); tofree = NULL;
    }
    else
    {
      block = block->prev;
    }
  }
}

/******************************************************************************
 * gc_root
 *
 * The data allocated here serves as the start of the garbage collection root
 * and will never be freed. Since this GC implementation does not scan the
 * stack, references to the stack that must be retained should be placed within
 * the allocated space. If blocks is not NULL then this function does
 * nothing and returns NULL. Otherwise it allocates the specified space
 * (filling it with zeros). Then add * a block containing the new data.
 ******************************************************************************/
void *gc_root(size_t size)
{
  void *rtn = NULL;
  struct GcBlock *newBlock = NULL;

  if(_gc_context.root != NULL)
  {
    return NULL;
  }

  rtn = calloc(1, size);

  if(rtn == NULL)
  {
    return NULL;
  }

  newBlock = calloc(1, sizeof(*newBlock));

  if(newBlock == NULL)
  {
    free(rtn); rtn = NULL;
    return NULL;
  }

  newBlock->ptr = rtn;
  newBlock->size = size;

  _gc_context.root = newBlock;

  return rtn;
}

/******************************************************************************
 * gc_alloc
 *
 * Allocate the space specified by size and add block to be later scanned and
 * evaluated for deletion.
 ******************************************************************************/
void *gc_alloc(size_t size)
{
  void *rtn = NULL;
  struct GcBlock *newBlock = NULL;

  if(_gc_context.root == NULL)
  {
    return NULL;
  }

  rtn = calloc(1, size);

  if(rtn == NULL)
  {
    return NULL;
  }

  newBlock = calloc(1, sizeof(*newBlock));

  if(newBlock == NULL)
  {
    free(rtn); rtn = NULL;
    return NULL;
  }

  newBlock->ptr = rtn;
  newBlock->size = size;

  if(gc_addblock(newBlock) != 0)
  {
    free(newBlock); newBlock = NULL;
    free(rtn); rtn = NULL;
    return NULL;
  }

  return rtn;
}

int gc_finalizer(void *ptr, void (*deleter)(void*))
{
  struct GcBlock *block = NULL;
  struct GcBlock *newBlock = NULL;

  if(ptr == NULL)
  {
    return 1;
  }

  if(_gc_context.root == NULL)
  {
    return 1;
  }

  block = _gc_context.root;

  while(block != NULL)
  {
    if(block->ptr == ptr)
    {
      block->deleter = deleter;
      return 0;
    }

    block = block->next;
  }

  newBlock = calloc(1, sizeof(*newBlock));

  if(newBlock == NULL)
  {
    return 1;
  }

  newBlock->ptr = ptr;
  newBlock->deleter = deleter;

  if(gc_addblock(newBlock) != 0)
  {
    free(newBlock); newBlock = NULL;
    return 1;
  }

  return 0;
}

int gc_block_add_reference(struct GcBlock *ctx, struct GcBlock *reference)
{
  struct GcBlockReference *lastReference = NULL;

  if(ctx->references == NULL)
  {
    ctx->references = calloc(1, sizeof(*ctx->references));

    if(ctx->references == NULL)
    {
      return 1;
    }

    lastReference = ctx->references;
  }
  else
  {
    lastReference = ctx->references;

    while(lastReference->next != NULL)
    {
      lastReference = lastReference->next;
    }

    lastReference->next = calloc(1, sizeof(*lastReference));

    if(lastReference->next == NULL)
    {
      return 1;
    }

    lastReference->next->prev = lastReference;
    lastReference = lastReference->next;
  }

  lastReference->block = reference;

  return 0;
}

void gc_assign_generation(struct GcBlock *ctx, int generation)
{
  struct GcBlockReference *reference = NULL;

  // Generation already assigned
  if(ctx->generation != -1)
  {
    return;
  }

  ctx->generation = generation;
  reference = ctx->references;

  while(reference != NULL)
  {
    gc_assign_generation(reference->block, generation + 1);
    reference = reference->next;
  }

  gc_freereferences(ctx);
}

/******************************************************************************
 * gc_collect
 *
 * For each block (other than the root), go through all the other blocks and
 * scan through the data pointed to by ptr to look for references to the prior
 * block. If nothing is found, mark it as eligible for purge and then trigger
 * a purge of any blocks.
 ******************************************************************************/
void gc_collect()
{
  uintptr_t current = 0;
  uintptr_t end = 0;
  struct GcBlock *currentBlock = NULL;
  struct GcBlock *findBlock = NULL;

  findBlock = _gc_context.root;

  while(findBlock != NULL)
  {
    findBlock->generation = -1;
    currentBlock = _gc_context.root;

    while(currentBlock != NULL)
    {
      if(currentBlock->size <= 0)
      {
        currentBlock = currentBlock->next;
        continue;
      }

      // We will let self references pass
      //if(findBlock == currentBlock)
      //{
      //  currentBlock = currentBlock->next;
      //  continue;
      //}

      current = (uintptr_t)currentBlock->ptr;
      end = current + currentBlock->size;
      end -= sizeof(currentBlock->ptr);

      while(current <= end)
      {
        void **test = (void**)current;

        if(*test == findBlock->ptr)
        {
          gc_block_add_reference(currentBlock, findBlock);
          break;
        }

        current++;
      }

      currentBlock = currentBlock->next;
    }

    findBlock = findBlock->next;
  }

  gc_assign_generation(_gc_context.root, 0);

  gc_purgeblocks();
}

