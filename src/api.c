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
  task->todos     = NULL;

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

  if(task->todos) todo_destroy(task->todos);

  free(task->key);
  free(task->value);
  free(task);
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

  if (i == 0)
    return tasklist_prepend(ref, task);

  while (head) {
    if (i == ++j) {
      tail->next = head->next;
      head->next = tail;
      return;
    }

    head = head->next;
  }

  if(i > j)
    printf("requested index is out of bounds\n");
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

  if (i > j)
    printf("requested index is out of bounds\n");
}

task_t* tasklist_lookup(tasklist_t* list, int i) {
  int j = 0;

  while(list) {
    if (i == j)
      return list->current;

    if (list->next)
      list = list->next;

    j++;
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

taskmap_t* taskmap_create(unsigned size) {
  taskmap_t* map = malloc(sizeof(taskmap_t));

  if (!map) return NULL;

  map->size   = size;
  map->lists  = malloc(sizeof(tasklist_t*) * size);

  int i;

  for (i = 0; i < map->size; i++)
    map->lists[i] = NULL;

  return map;
}

void taskmap_destroy(taskmap_t* map) {
  if (!map) return;

  int i;

  for (i = 0; i < map->size; i++)
    if (map->lists[i])
      tasklist_destroy(map->lists[i]);

  free(map->lists);
  free(map);
}

unsigned taskmap_hash(taskmap_t* map, char* key) {
  unsigned hash;

  for (hash = 0; *key != '\0'; key++)
    hash = *key + 31 * hash;

  return hash % map->size;
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
      /* found a match */
      free(list1->current);

      if (list2 == NULL) 
        /* at the beginning? */
        map->lists[hash] = list1->next;
      else 
        /* in the middle or at the end? */
        list2->next = list1->next;

      free(list1);
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

void todo_insert_into_map(todo_t* todo, task_t* task) {
  if (!todo->map)
    todo->map = taskmap_create(7);

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
  char* err;
  long int i;

  i = strtol(key, &err, 10);

  if (key[0] != '\n' && (*err == '\n' || *err == '\0')) {
    if (i < INT_MIN && i > INT_MAX) return;
    todo_remove_from_list(todo, (int) i);
  } else {
    todo_remove_from_map(todo, key);
  }
}
