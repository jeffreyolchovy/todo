#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "todo.h"

void task_print(task_t* task, size_t width, size_t margin);

void task_print_verbose(task_t* task, const char* top_border, const char* bottom_border, size_t width, size_t margin);

void print_times(const char* buffer, size_t n) {
  while (n--) printf("%s", buffer);
}

void print_indent(const char* buffer, size_t n) {
  printf("%-*s", (int) n, buffer);
}

void print_column(const char* buffer, size_t width, size_t margin) {
  size_t count, buflen;
  const char *ptr, *endptr;

  count = 0;
  buflen = strlen(buffer);

  do {
    ptr = buffer + count;

    /* don't set endptr beyond the end of the buffer */
    if (ptr - buffer + width <= buflen)
      endptr = ptr + width;
    else
      endptr = buffer + buflen;

    /* back up EOL to a null terminator or space */
    while (*(endptr) && !isspace(*(endptr))) endptr--;

    count += fwrite(ptr, 1, endptr - ptr, stdout);

    /* print a newline and an indent */
    print_indent("\n", (size_t) buflen != count ? margin : 0);
  } while (*endptr);
}

void tasklist_print(tasklist_t* list, int verbose) {
  while(list) {
    if (verbose)
      task_print_verbose(list->current, "=", "=", 80, 0);
    else
      task_print(list->current, 80, 0);

    list = list->next;
  }
}

void taskmap_print(taskmap_t* map, int verbose) {
  int i;

  for (i = 0; i < map->size; i++) {
    if (map->lists[i])
      tasklist_print(map->lists[i], verbose);
  }
}

void todo_print(todo_t* todo, int verbose) {
  taskmap_print(todo->map, verbose);
  tasklist_print(todo->list, verbose);
}

void task_print(task_t* task, size_t width, size_t margin) {
  size_t inner_margin = 4;

  // print status
  print_indent("", margin);

  if (task->status == COMPLETE)
    printf("[x] ");
  else
    printf("[ ] ");

  // print value
  if (task->value)
    print_column(task->value, width - inner_margin - margin, inner_margin + margin);
}

void task_print_verbose(task_t* task, const char* top_border, const char* bottom_border, size_t width, size_t margin) {
  size_t inner_margin = 4;

  // print key line
  print_indent("", margin); 

  if (task->key)
    printf("(%s)", task->key);
  else
    printf("(?)");

  printf("\n");

  // print top border line
  print_indent("", margin);
  print_times(top_border, width - margin);
  printf("\n");

  // print status and value column
  print_indent("", margin);

  if (task->status == COMPLETE)
    printf("[x] ");
  else
    printf("[ ] ");

  if (task->value)
    print_column(task->value, width - inner_margin - margin, inner_margin + margin);

  // print bottom border line
  print_indent("", margin);
  print_times(bottom_border, width - margin);
  printf("\n");
}
