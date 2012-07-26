#include<stdio.h>
#include<assert.h>
#include "todo.h"

void tasktable_print(tasktable_t* table, int verbose);

void tasktable_print_quiet(tasktable_t* table);

void tasktable_print_verbose(tasktable_t* table);

void task_print_quiet(task_t* task);

void task_print_verbose(task_t* task);

void tasktable_print(tasktable_t* table, int verbose) {
  if (!verbose)
    tasktable_print_quiet(table);

  else
    tasktable_print_verbose(table);
}

void tasktable_print_quiet(tasktable_t* table) {
  if (table->map) {
    int i;

    for (i = 0; i < table->map->size; i++)
      if (table->map->lists[i]) {
        tasklist_t* head = table->map->lists[i];

        while (head) {
          task_print_quiet(head->current);
          head = head->next;
        }
      } 
  }

  if (table->list) {
    tasklist_t* head = table->list;

    while (head) {
      task_print_quiet(head->current);
      head = head->next;
    }
  }
}

void tasktable_print_verbose(tasktable_t* table) {
}

void task_print_quiet(task_t* task) {
  assert(task->description);
}

/* @deprecated */
void task_print(task_t* task) {
  printf("[");
  if (task->name)
    printf("(%s) ", task->name);
  printf("%s", task->description);
  printf("]");
}

/* @deprecated */
void tasklist_print(tasklist_t* list) {
  while(list) {
    task_print(list->current);
    if (list->next) printf(", ");
    list = list->next;
  }

  printf("\n");
}

/* @deprecated */
void taskmap_print(taskmap_t* map) {
  int i;

  for (i = 0; i < map->size; i++) {
    printf("[%d] -> ", i);

    if(map->lists[i])
      tasklist_print(map->lists[i]);
    else
      printf("NIL\n");
  }
}
