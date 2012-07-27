#include<stdio.h>
#include<assert.h>
#include "todo.h"

void todo_print(todo_t* todo, int verbose);

void todo_print_quiet(todo_t* todo);

void todo_print_verbose(todo_t* todo);

void task_print_quiet(task_t* task);

void task_print_verbose(task_t* task);

void todo_print(todo_t* todo, int verbose) {
  if (!verbose)
    todo_print_quiet(todo);
  else
    todo_print_verbose(todo);
}

void todo_print_quiet(todo_t* todo) {
  if (todo->map) {
    int i;

    for (i = 0; i < todo->map->size; i++)
      if (todo->map->lists[i]) {
        tasklist_t* head = todo->map->lists[i];

        while (head) {
          task_print_quiet(head->current);
          head = head->next;
        }
      } 
  }

  if (todo->list) {
    tasklist_t* head = todo->list;

    while (head) {
      task_print_quiet(head->current);
      head = head->next;
    }
  }
}

void todo_print_verbose(todo_t* todo) {
}

void task_print_quiet(task_t* task) {
}

/* @deprecated */
void task_print(task_t* task) {
  printf("[");
  if (task->key)
    printf("(%s) ", task->key);
  printf("%s", task->value);
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
