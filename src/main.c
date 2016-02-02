#include <gc.h>

#include <stdio.h>
#include <stdlib.h>

struct GcContext *gc_ctx = NULL;

struct Employee
{
  char *name;
  struct Employee *employee;
  FILE *file;
};

void gc_fclose(void *file)
{
  printf("fclose\n");
  fclose((FILE*)file);
}

struct Employee *EmployeeCreate()
{
  struct Employee *rtn = gc_alloc(gc_ctx, sizeof(*rtn));

  if(!rtn)
  {
    printf("Failed to allocate Employee\n");
    exit(1);
  }

  rtn->file = fopen("test.txt", "w");
  gc_finalizer(gc_ctx, rtn->file, gc_fclose);

  return rtn;
}

struct Root
{
  struct GcContext *gcCtx;
  int a;
  struct Employee *employee;
  FILE *file;
};

  static int test = 0;

void testfinal(void* data)
{
  test++;
}

void EmployeeRun(struct Employee *employee, struct Root *root)
{
  int i = 0;
  for(i = 0; i < 500; i++)
  {
    employee->name = gc_alloc(gc_ctx, sizeof(char) * 1000);
    gc_finalizer(gc_ctx, employee->name, testfinal);
    //employee->name = calloc(1, sizeof(char) * 1000);
    //free(employee->name);
    gc_collect(gc_ctx);
  }

  printf("%i\n", test);
}

int main(int argc, char* argv[])
{
  struct Root *root = NULL;

  printf("GC Test\n");

  gc_ctx = gc_context();
  root = gc_root(gc_ctx, sizeof(*root));

  if(!root)
  {
    printf("Failed to allocate GC root\n");
    exit(1);
  }

  root->gcCtx = gc_ctx;
  root->employee = EmployeeCreate();
  root->employee->employee = EmployeeCreate();
  root->employee->employee->employee = EmployeeCreate();

  root->file = fopen("test.txt", "w");
  gc_finalizer(gc_ctx, root->file, gc_fclose);

  EmployeeRun(root->employee, root);

  //root->employee = NULL;
  root->employee->employee = NULL;
  root->employee->employee = root->employee;

  gc_collect(gc_ctx);
  //gc_collect();
  //gc_collect();

  root->file = NULL;

  gc_collect(gc_ctx);

  //memset(root, 0, sizeof(*root));
  //gc_collect();
  //free(root);

  return 0;
}

