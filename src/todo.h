#ifndef TODO_H
#define TODO_H

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>

#define TASKMAP_DEFAULT_SIZE 11

typedef enum task_status_e {
  INCOMPLETE,
  COMPLETE
} task_status_t;

typedef enum task_priority_e {
  URGENT   = 04,
  HIGH     = 02,
  NORMAL   = 01,
  LOW      = 00
} task_priority_t;

typedef struct tasklist_s tasklist_t;

typedef struct taskmap_s taskmap_t;

typedef struct todo_s todo_t;

typedef struct task_s {
  char* key;
  char* value;
  task_status_t status;
  task_priority_t priority;
  todo_t* todos;
} task_t;

struct tasklist_s {
  task_t* current;
  tasklist_t* next;
};

struct taskmap_s {
  tasklist_t** lists;
  unsigned size;
};

struct todo_s {
  taskmap_t* map;
  tasklist_t* list;
};

task_t* task_create(char* name, char* description);

void task_destroy(task_t* task);

tasklist_t* tasklist_create();

void tasklist_destroy(tasklist_t* list);

void tasklist_append(tasklist_t** ref, task_t* task);

void tasklist_prepend(tasklist_t** ref, task_t* task);

void tasklist_insert(tasklist_t** ref, task_t* task, int i);

void tasklist_remove(tasklist_t** ref, int i);

task_t* tasklist_lookup(tasklist_t* list, int i);

taskmap_t* taskmap_create(unsigned size);

void taskmap_destroy(taskmap_t* map);

unsigned taskmap_hash(taskmap_t* map, char* key);

task_t* taskmap_lookup(taskmap_t* map, char* key);

void taskmap_insert(taskmap_t* map, task_t* task);

void taskmap_remove(taskmap_t* map, char* key);

todo_t* todo_create();

void todo_destroy(todo_t* todo);

void todo_insert(todo_t* todo, task_t* task);

void todo_remove(todo_t* todo, char* key);

todo_t* todo_read(FILE* file);

void todo_write(todo_t* todo, FILE* file);

void todo_print(todo_t* todo, int verbose);

#ifdef __cplusplus
}
#endif

#endif /* TODO_H */
