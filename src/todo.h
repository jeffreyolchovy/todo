#ifndef TODO_H
#define TODO_H

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>

#define TASKMAP_DEFAULT_SIZE 11

//========================================
// Data types and structures
//========================================

// task states
typedef enum task_status_e {
  INCOMPLETE,
  COMPLETE
} task_status_t;

// task priority levels
typedef enum task_priority_e {
  URGENT   = 04,
  HIGH     = 02,
  NORMAL   = 01,
  LOW      = 00
} task_priority_t;

// linked list of tasks
typedef struct tasklist_s tasklist_t;

// hashmap of tasks
typedef struct taskmap_s taskmap_t;

// a structure for storing tasks by key or by natural insertion order index
typedef struct todo_s todo_t;

typedef struct task_s {
  char* key;
  char* value;
  task_status_t status;
  task_priority_t priority;
  todo_t* todo;
} task_t;

struct tasklist_s {
  task_t* current;
  tasklist_t* next;
};

struct taskmap_s {
  tasklist_t** lists;
  unsigned lists_size;
};

struct todo_s {
  tasklist_t* list;
  taskmap_t* map;
};

//========================================
// Data types and structures API
//========================================

task_t* task_create(char* name, char* description);

void task_destroy(task_t* task);

task_t* task_copy(task_t* src);

void task_apply(task_t* task, void (*f)(task_t*));

task_status_t task_status_valueof(char* str);

task_priority_t task_priority_valueof(char* str);

tasklist_t* tasklist_create();

void tasklist_destroy(tasklist_t* list);

void tasklist_apply(tasklist_t* list, void (*f)(task_t*));

void tasklist_append(tasklist_t** ref, task_t* task);

void tasklist_prepend(tasklist_t** ref, task_t* task);

void tasklist_insert(tasklist_t** ref, task_t* task, int i);

void tasklist_remove(tasklist_t** ref, int i);

task_t* tasklist_lookup(tasklist_t* list, int i);

size_t tasklist_size(tasklist_t* list);

taskmap_t* taskmap_create(unsigned size);

void taskmap_destroy(taskmap_t* map);

void taskmap_apply(taskmap_t* map, void (*f)(task_t*));

unsigned taskmap_hash(taskmap_t* map, char* key);

task_t* taskmap_lookup(taskmap_t* map, char* key);

void taskmap_insert(taskmap_t* map, task_t* task);

void taskmap_remove(taskmap_t* map, char* key);

todo_t* todo_create();

void todo_destroy(todo_t* todo);

void todo_apply(todo_t* todo, void (*f)(task_t*));

void todo_insert(todo_t* todo, task_t* task);

void todo_remove(todo_t* todo, char* key);

task_t* todo_lookup(todo_t* todo, char* key);

task_t* todo_path_insert(todo_t* todo, char* path);

void todo_path_remove(todo_t* todo, char* path);

task_t* todo_path_lookup(todo_t* todo, char* path);

todo_t* todo_read(FILE* file);

void todo_write(todo_t* todo, FILE* file);

void todo_print(todo_t* todo, int verbose);

void task_print(task_t* task, int verbose);

//========================================
// CLI API
//========================================

int execute(todo_t* todo, char cmd, char* path, char* arg);

int execute_list(todo_t* todo, char* path);

int execute_add(todo_t* todo, char* path, char* value);

int execute_edit(todo_t* todo, char* path, char* value);

int execute_remove(todo_t* todo, char* path);

int execute_mark(todo_t* todo, char* path, int is_complete);

int execute_prioritize(todo_t* todo, char* path, char* priority);

//========================================
// Misc. and utility
//========================================

int is_path(char* str);

int is_int_path(char* str);

int is_todo_path(char* str);

#ifdef __cplusplus
}
#endif

#endif /* TODO_H */
