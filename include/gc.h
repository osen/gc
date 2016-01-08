#include <string.h>

struct GcBlock;

/******************************************************************************
 * gc_addblock
 *
 * Reallocate the gc_blocks array to make room for a copy of the new block.
 ******************************************************************************/
int gc_addblock(struct GcBlock *block);

/******************************************************************************
 * gc_purgeblocks
 *
 * Iterate backwards through the gc_blocks array to see if each one is marked
 * as to be purged. If so, execute its delete function on its contained pointer
 * and unless it is the last one of the array, move all the other blocks up to
 * fill up the space and replace it.
 ******************************************************************************/
void gc_purgeblocks();

/******************************************************************************
 * gc_root
 *
 * The data allocated here serves as the start of the garbage collection root
 * and will never be freed. Since this GC implementation does not scan the
 * stack, references to the stack that must be retained should be placed within
 * the allocated space. If gc_blocks is not NULL then this function does
 * nothing and returns NULL. Otherwise it allocates the specified space
 * (filling it with zeros). Then add * a block containing the new data.
 ******************************************************************************/
void *gc_root(size_t size);

/******************************************************************************
 * gc_alloc
 *
 * Allocate the space specified by size and add block to be later scanned and
 * evaluated for deletion.
 ******************************************************************************/
void *gc_alloc(size_t size);

/******************************************************************************
 * gc_attach
 *
 * Using an existing pointer, add a block with the specified deleter so that
 * the garbage collector can manage it.
 ******************************************************************************/
int gc_attach(void *ptr, void (*deleter)(void*));

/******************************************************************************
 * gc_collect
 *
 * For each block (other than the root), go through all the other blocks and
 * scan through the data pointed to by ptr to look for references to the prior
 * block. If nothing is found, mark it as eligible for purge and then trigger
 * a purge of any blocks.
 ******************************************************************************/
void gc_collect();
