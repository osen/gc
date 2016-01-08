#include <gc.h>

#include <stdio.h>
#include <stdlib.h>

struct Employee
{
  char *name;
  struct Employee *employee;
  FILE *file;
};

void gc_fclose(void *file)
{
  printf("fclose\n");
  fclose(file);
}

struct Employee *EmployeeCreate()
{
  struct Employee *rtn = gc_alloc(sizeof(*rtn));

  if(!rtn)
  {
    printf("Failed to allocate Employee\n");
    exit(1);
  }

  rtn->file = fopen("test.txt", "w");
  gc_attach(rtn->file, gc_fclose);

  return rtn;
}

struct Root
{
  int a;
  struct Employee *employee;
  FILE *file;
};

void EmployeeRun(struct Employee *employee, struct Root *root)
{
  int i = 0;
  for(i = 0; i < 500; i++)
  {
    employee->name = gc_alloc(sizeof(char) * 1000);
    //gc_collect();
  }
}

int main(int argc, char* argv[])
{
  printf("GC Test\n");

  struct Root *root = gc_root(sizeof(*root));

  if(!root)
  {
    printf("Failed to allocate GC root\n");
    exit(1);
  }

  root->employee = EmployeeCreate();
  root->employee->employee = EmployeeCreate();
  root->employee->employee->employee = EmployeeCreate();

  root->file = fopen("test.txt", "w");
  gc_attach(root->file, gc_fclose);

  EmployeeRun(root->employee, root);

  //root->employee = NULL;
  root->employee->employee = NULL;

  gc_collect();
  //gc_collect();
  //gc_collect();

  root->file = NULL;

  gc_collect();

  //memset(root, 0, sizeof(*root));
  //gc_collect();
  //free(root);

  return 0;
}

