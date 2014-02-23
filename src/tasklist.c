#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "todo.h"

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
    head->next = NULL;
    tasklist_destroy(head);
    return;
  }

  while (head->next) {
    if (i == ++j) {
      tasklist_t* temp = head->next;
      head->next = temp->next;
      task_destroy(temp->current);
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
