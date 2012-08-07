#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "todo.h"

task_t* task_create(char* key, char* value) {
  task_t* task = malloc(sizeof(task_t));

  if (!task) return NULL;

  task->key       = NULL;
  task->value     = NULL;
  task->status    = INCOMPLETE;
  task->priority  = NORMAL;
  task->todo      = NULL;

  if (key) {
    task->key = malloc(strlen(key) + 1);
    strcpy(task->key, key);
  }

  if (value) {
    task->value = malloc(strlen(value) + 1);
    strcpy(task->value, value);
  }

  return task;
}

void task_destroy(task_t* task) {
  if (!task) return;

  if(task->todo) todo_destroy(task->todo);

  free(task->key);
  free(task->value);
  free(task);
}

void task_apply(task_t* task, void (*f)(task_t*)) {
  if (task->todo) todo_apply(task->todo, *f);
  (*f)(task);
}

task_status_t task_status_valueof(char* str) {
  if (strcmp(str, "complete") == 0)
    return COMPLETE;

  if (strcmp(str, "incomplete") == 0)
    return INCOMPLETE;

  // default
  return INCOMPLETE;
}

task_priority_t task_priority_valueof(char* str) {
  if (strcmp(str, "urgent") == 0)
    return URGENT;

  if (strcmp(str, "high") == 0)
    return HIGH;

  if (strcmp(str, "normal") == 0)
    return NORMAL;

  if (strcmp(str, "low") == 0)
    return LOW;

  // default
  return NORMAL;
}

tasklist_t* tasklist_create(task_t* task) {
  tasklist_t* list = malloc(sizeof(tasklist_t));

  if (!list) return NULL;

  list->current = task;
  list->next    = NULL;

  return list;
}

void tasklist_destroy(tasklist_t* list) {
  if (!list) return;

  if (list->next)
    tasklist_destroy(list->next);

  if (list->current)
    task_destroy(list->current);

  free(list);
}

void tasklist_apply(tasklist_t* list, void (*f)(task_t*)) {
  while(list) {
    task_apply(list->current, *f);
    list = list->next;
  }
}

void tasklist_append(tasklist_t** ref, task_t* task) {
  tasklist_t* head = *ref;
  tasklist_t* tail = tasklist_create(task);

  if (!tail) return;

  while (head->next) head = head->next;

  head->next = tail;
}

void tasklist_prepend(tasklist_t** ref, task_t* task) {
  tasklist_t* head = tasklist_create(task);
  head->next = *ref;
  *ref = head;
}

void tasklist_insert(tasklist_t** ref, task_t* task, int i) {
  tasklist_t* head = *ref;
  tasklist_t* tail = tasklist_create(task);
  int j = 0;

  if (i == 0) {
    tasklist_prepend(ref, task);
    return;
  }

  while (head) {
    if (i == ++j) {
      tail->next = head->next;
      head->next = tail;
      return;
    }

    head = head->next;
  }
}

void tasklist_remove(tasklist_t** ref, int i) {
  tasklist_t* head = *ref;
  int j = 0;

  if (i == 0) {
    *ref = head->next;
    return;
  }

  while (head->next) {
    if (i == ++j) {
      tasklist_t* temp = head->next;
      head->next = temp->next;
      free(temp->current);
      free(temp);
      return;
    }

    head = head->next;
  }
}

task_t* tasklist_lookup(tasklist_t* list, int i) {
  int j = 0;

  while(list) {
    if (i == j++)
      return list->current;

    list = list->next;
  }

  return NULL;
}

size_t tasklist_size(tasklist_t* list) {
  int i = 0;

  do {
    i++;
    list = list->next;
  } while(list);

  return i;
}

taskmap_t* taskmap_create(unsigned lists_size) {
  taskmap_t* map = malloc(sizeof(taskmap_t));

  if (!map) return NULL;

  map->lists = malloc(sizeof(tasklist_t*) * lists_size);
  map->lists_size = lists_size;

  size_t i;

  for (i = 0; i < map->lists_size; i++)
    map->lists[i] = NULL;

  return map;
}

void taskmap_destroy(taskmap_t* map) {
  if (!map) return;

  size_t i;

  for (i = 0; i < map->lists_size; i++)
    if (map->lists[i])
      tasklist_destroy(map->lists[i]);

  free(map->lists);
  free(map);
}

void taskmap_apply(taskmap_t* map, void (*f)(task_t*)) {
  size_t i;

  for (i = 0; i < map->lists_size; i++)
    if (map->lists[i])
      tasklist_apply(map->lists[i], *f);
}

unsigned taskmap_hash(taskmap_t* map, char* key) {
  unsigned hash;

  for (hash = 0; *key != '\0'; key++)
    hash = *key + 31 * hash;

  return hash % map->lists_size;
}

task_t* taskmap_lookup(taskmap_t* map, char* key) {
  unsigned hash = taskmap_hash(map, key);
  tasklist_t* list;

  for (list = map->lists[hash]; list != NULL; list = list->next)
    if (strcmp(key, list->current->key) == 0)
      return list->current;

  return NULL;
}

void taskmap_insert(taskmap_t* map, task_t* task) {
  unsigned hash = taskmap_hash(map, task->key);
  tasklist_t* list = map->lists[hash];

  if (!list)
    list = tasklist_create(task);
  else {
    tasklist_t* head = list;

    while (head) {
      if (strcmp(head->current->key, task->key) == 0) {
        free(head->current);
        head->current = task;
        return;
      }

      head = head->next;
    }

    tasklist_prepend(&list, task);
  }

  if (!list) return;

  list->next = map->lists[hash];
  map->lists[hash] = list;
}

void taskmap_remove(taskmap_t* map, char* key) {
  tasklist_t* list1;
  tasklist_t* list2;

  unsigned hash = taskmap_hash(map, key);

  for (list1 = map->lists[hash], list2 = NULL; list1 != NULL;
      list2 = list1, list1 = list1->next) 
    if (strcmp(key, list1->current->key) == 0) {
      // found a match
      task_destroy(list1->current);

      if (list2 == NULL) 
        // is it at the beginning?
        map->lists[hash] = list1->next;
      else 
        // is it in the middle or at the end?
        list2->next = list1->next;

      list1->current = NULL;
      free(list1);
      //tasklist_destroy(list1);
      return;
    }
}

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

