#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "todo.h"

todo_t* todo_create() {
  todo_t* todo = malloc(sizeof(todo_t));

  if (!todo) return NULL;

  todo->map  = NULL;
  todo->list = NULL;

  return todo;
}

void todo_destroy(todo_t* todo) {
  if (!todo) return;

  taskmap_destroy(todo->map);
  tasklist_destroy(todo->list);
  free(todo);
}

void todo_apply(todo_t* todo, void (*f)(task_t*)) {
  if (todo->map)
    taskmap_apply(todo->map, *f);

  if (todo->list)
    tasklist_apply(todo->list, *f);
}

void todo_insert_into_map(todo_t* todo, task_t* task) {
  if (!todo->map)
    todo->map = taskmap_create(TASKMAP_DEFAULT_SIZE);

  if (todo->map)
    taskmap_insert(todo->map, task);
}

void todo_insert_into_list(todo_t* todo, task_t* task) {
  if (!todo->list)
    todo->list = tasklist_create(task);
  else
    tasklist_append(&todo->list, task);
}

void todo_insert(todo_t* todo, task_t* task) {
  if (task->key)
    todo_insert_into_map(todo, task);
  else
    todo_insert_into_list(todo, task);
}

void todo_remove_from_map(todo_t* todo, char* key) {
  if (!todo->map) return;
  taskmap_remove(todo->map, key);
}

void todo_remove_from_list(todo_t* todo, int i) {
  if (!todo->list) return;
  tasklist_remove(&todo->list, i);
}

void todo_remove(todo_t* todo, char* key) {
  task_t* task = todo_lookup(todo, key);

  if (task->key) {
    todo_remove_from_map(todo, task->key);
  } else {
    int i = (int) strtol(key, (char **) NULL, 10);
    todo_remove_from_list(todo, i);
  }
}

task_t* todo_lookup(todo_t* todo, char* key) {
  char* err = NULL;
  int is_int = 0;
  long int i;

  if (!todo || !key) return NULL;

  i = strtol(key, &err, 10);

  if (key[0] != '\n' && (*err == '\n' || *err == '\0'))
    is_int = i >= INT_MIN && i <= INT_MAX;

  if (is_int && todo->list)
    return tasklist_lookup(todo->list, (int) i);
  else if (!is_int && todo->map)
    return taskmap_lookup(todo->map, key);
  else
    return NULL;
}

task_t* todo_path_insert(todo_t* todo, char* path) {
  char *ptr, *rest, *token;
  task_t* task = NULL;
  task_t* head = NULL;
  todo_t* parent = todo;

  char* tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path);

  ptr = tmp;

  while ((token = strtok_r(ptr, "/", &rest))) {
    // if the current part path is an integer
    if (is_int_path(token)) {
      // cleanup resources
      free(tmp);

      // destroy the first made task alloc.
      // which will destroy any subsequent allocs.
      if (head) task_destroy(head);

      return NULL;
    }

    task = todo_lookup(parent, token);

    // task does not exist for part path, create it
    if (!task) {
      task = task_create(token, "");

      // keep reference if this is first task alloc.
      if (!head) head = task;

      // add to parent's todo
      todo_insert(parent, task);
    }
    
    // if task does not have a todo, create it
    if (!task->todo) {
      todo_t* child = todo_create();
      task->todo = child;
    }

    parent = task->todo;
    ptr = rest;
  }

  free(tmp);

  return task;
}

void todo_path_remove(todo_t* todo, char* path) {
  char *ptr, *rest, *token, *prev;
    
  task_t* task = NULL;
  todo_t* parent = todo;
  todo_t* prev_parent = NULL;

  char* tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path);

  ptr = tmp;
  prev = NULL;

  while ((token = strtok_r(ptr, "/", &rest))) {
    task = todo_lookup(parent, token);
    prev = token;
    prev_parent = parent;

    if(task && task->todo) {
      parent = task->todo;
    } else {
      parent = NULL;
    }

    ptr = rest;
  }

  if (task) {
    if (is_todo_path(path) && task->todo) {
      todo_destroy(task->todo);
      task->todo = NULL;
    } else if (prev_parent) {
      todo_remove(prev_parent, prev);
    }
  }

  free(tmp);
}

task_t* todo_path_lookup(todo_t* todo, char* path) {
  task_t* task = NULL;
  char *ptr, *rest, *token;

  if (!todo || !path) return NULL;

  // copy path because tokenizing will turn '/' to '\0'
  char *tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path);

  ptr = tmp;

  while ((token = strtok_r(ptr, "/", &rest))) {
    // find task in current enclosing todo or in previous task's todo (if exists)
    task = task ? task->todo ? todo_lookup(task->todo, token) : NULL : todo_lookup(todo, token);
    ptr = rest;
  }

  free(tmp);

  return task;
}
