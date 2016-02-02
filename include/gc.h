#include <string.h>

struct GcContext;

struct GcContext *gc_context();
void *gc_root(struct GcContext *ctx, size_t size);
void *gc_alloc(struct GcContext *ctx, size_t size);
int gc_finalizer(struct GcContext *ctx, void *ptr, void (*deleter)(void*));
void gc_collect(struct GcContext *ctx);
