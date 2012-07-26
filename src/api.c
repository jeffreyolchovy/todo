#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "todo.h"

task_t* task_create(char* name, char* description) {
  task_t* task = malloc(sizeof(task_t));

  if (!task) return NULL;

  task->name        = NULL;
  task->description = NULL;
  task->status      = INCOMPLETE;
  task->priority    = NORMAL;
  task->tasks       = NULL;

  if (name) {
    task->name = malloc(strlen(name) + 1);
    strcpy(task->name, name);
  }

  if (description) {
    task->description = malloc(strlen(description) + 1);
    strcpy(task->description, description);
  }

  return task;
}

void task_destroy(task_t* task) {
  if (!task) return;

  if(task->tasks) tasktable_destroy(task->tasks);

  free(task->name);
  free(task->description);
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
    if (strcmp(key, list->current->name) == 0)
      return list->current;

  return NULL;
}

void taskmap_insert(taskmap_t* map, task_t* task) {
  unsigned hash = taskmap_hash(map, task->name);
  tasklist_t* list = map->lists[hash];

  if (!list)
    list = tasklist_create(task);
  else {
    tasklist_t* head = list;

    while (head) {
      if (strcmp(head->current->name, task->name) == 0) {
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
    if (strcmp(key, list1->current->name) == 0) {
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

tasktable_t* tasktable_create() {
  tasktable_t* table = malloc(sizeof(tasktable_t));

  if (!table) return NULL;

  table->map  = NULL;
  table->list = NULL;

  return table;
}

void tasktable_destroy(tasktable_t* table) {
  if (!table) return;

  taskmap_destroy(table->map);
  tasklist_destroy(table->list);
  free(table);
}

void tasktable_insert_into_map(tasktable_t* table, task_t* task) {
  if (!table->map)
    table->map = taskmap_create(7);

  if (table->map)
    taskmap_insert(table->map, task);
}

void tasktable_insert_into_list(tasktable_t* table, task_t* task) {
  if (!table->list)
    table->list = tasklist_create(task);
  else
    tasklist_append(&table->list, task);
}

void tasktable_insert(tasktable_t* table, task_t* task) {
  if (task->name) {
    tasktable_insert_into_map(table, task);
  } else {
    tasktable_insert_into_list(table, task);
  }
}

void tasktable_remove_from_map(tasktable_t* table, char* key) {
  if (!table->map) return;
  taskmap_remove(table->map, key);
}

void tasktable_remove_from_list(tasktable_t* table, int i) {
  if (!table->list) return;
  tasklist_remove(&table->list, i);
}

void tasktable_remove(tasktable_t* table, char* key) {
  char* err;
  long int i;

  i = strtol(key, &err, 10);

  if (key[0] != '\n' && (*err == '\n' || *err == '\0')) {
    if (i < INT_MIN && i > INT_MAX) return;
    tasktable_remove_from_list(table, (int) i);
  } else {
    tasktable_remove_from_map(table, key);
  }
}
