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

task_t* task_copy(task_t* src) {
  if (!src) return NULL;

  task_t* copy = malloc(sizeof(task_t));

  if (!copy) return NULL;

  copy->key       = NULL;
  copy->value     = NULL;
  copy->status    = src->status;
  copy->priority  = src->priority;
  copy->todo      = NULL;

  if (src->key) {
    copy->key = malloc(strlen(src->key) + 1);
    strcpy(copy->key, src->key);
  }

  if (src->value) {
    copy->value = malloc(strlen(src->value) + 1);
    strcpy(copy->value, src->value);
  }

  return copy;
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
