#include <gc.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct GcBlock
{
  void *ptr;
  size_t size;
  void (*deleter)(void*);
  struct GcBlock **references;
  int referencesCount;
  int generation;
};

struct GcContext
{
  struct GcBlock *blocks;
  int blocksCount;
};

struct GcContext _gc_context = {0};

/******************************************************************************
 * gc_addblock
 *
 * Reallocate the blocks array to make room for a copy of the new block.
 ******************************************************************************/
int gc_addblock(struct GcBlock *block)
{
  struct GcBlock *tmpBlocks = realloc(_gc_context.blocks,
    sizeof(*_gc_context.blocks) * (_gc_context.blocksCount + 1));

  if(tmpBlocks == NULL)
  {
    return 1;
  }

  _gc_context.blocks = tmpBlocks;
  _gc_context.blocks[_gc_context.blocksCount] = *block;
  _gc_context.blocksCount++;

  return 0;
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
  int b; for(b = _gc_context.blocksCount - 1; b >= 1; b--)
  {
    if(_gc_context.blocks[b].generation == -1)
    {
      _gc_context.blocks[b].deleter(_gc_context.blocks[b].ptr);

      if(_gc_context.blocks[b].references != NULL)
      {
          free(_gc_context.blocks[b].references);
      }

      if(b != _gc_context.blocksCount - 1)
      {
        memmove(&_gc_context.blocks[b], &_gc_context.blocks[b] + 1,
          sizeof(*_gc_context.blocks) * (_gc_context.blocksCount - b - 1));
      }

      _gc_context.blocksCount--;
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
  if(_gc_context.blocks != NULL)
  {
    return NULL;
  }

  void *rtn = calloc(1, size);

  if(rtn == NULL)
  {
    return NULL;
  }

  struct GcBlock newBlock = {0};
  newBlock.ptr = rtn;
  newBlock.size = size;

  if(gc_addblock(&newBlock) != 0)
  {
    free(rtn); rtn = NULL;
    return rtn;
  }

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
  if(_gc_context.blocks == NULL)
  {
    return NULL;
  }

  void *rtn = calloc(1, size);

  if(rtn == NULL)
  {
    return rtn;
  }

  struct GcBlock newBlock = {0};
  newBlock.ptr = rtn;
  newBlock.size = size;
  newBlock.deleter = free;

  if(gc_addblock(&newBlock) != 0)
  {
    free(rtn); rtn = NULL;
    return rtn;
  }

  return rtn;
}

/******************************************************************************
 * gc_attach
 *
 * Using an existing pointer, add a block with the specified deleter so that
 * the garbage collector can manage it.
 ******************************************************************************/
int gc_attach(void *ptr, void (*deleter)(void*))
{
  if(_gc_context.blocks == NULL)
  {
    return 1;
  }

  struct GcBlock newBlock = {0};
  newBlock.ptr = ptr;
  newBlock.deleter = deleter;

  if(gc_addblock(&newBlock) != 0)
  {
    return 1;
  }

  return 0;
}

int gc_block_add_reference(struct GcBlock *ctx, struct GcBlock *reference)
{
  struct GcBlock **tmpBlocks = realloc(ctx->references,
    sizeof(*ctx->references) * (ctx->referencesCount + 1));

  if(tmpBlocks == NULL)
  {
    return 1;
  }

  ctx->references = tmpBlocks;
  ctx->references[ctx->referencesCount] = reference;
  ctx->referencesCount ++;

  return 0;
}

void gc_assign_generation(struct GcBlock *ctx, int generation)
{
  ctx->generation = generation;

  int i; for(i = 0; i < ctx->referencesCount; i++)
  {
    gc_assign_generation(ctx->references[i], generation + 1);
  }

  ctx->referencesCount = 0;
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
  int i; for(i = 0; i < _gc_context.blocksCount; i++)
  {
    struct GcBlock *findBlock = &_gc_context.blocks[i];
    findBlock->generation = -1;

    int b; for(b = 0; b < _gc_context.blocksCount; b++)
    {
      struct GcBlock *currentBlock = &_gc_context.blocks[b];

      if(currentBlock->size <= 0)
      {
        continue;
      }

      uintptr_t current = (uintptr_t)currentBlock->ptr;
      uintptr_t end = current + currentBlock->size;
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
    }
  }

  gc_assign_generation(&_gc_context.blocks[0], 0);

  gc_purgeblocks();
}
