#include <string.h>

struct GcBlock;

void *gc_root(size_t size);
void *gc_alloc(size_t size);
int gc_finalizer(void *ptr, void (*deleter)(void*));
void gc_collect();
