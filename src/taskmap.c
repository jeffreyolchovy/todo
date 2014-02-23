#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "todo.h"

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
  if (task->key == NULL)
    return;

  task_t* current_task = taskmap_lookup(map, task->key);

  if (current_task != NULL)
    taskmap_remove(map, task->key);

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
      return;
    }
}

