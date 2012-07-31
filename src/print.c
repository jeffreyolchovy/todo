#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/ioctl.h>
#include "todo.h"

static const char* top_border = "=";

static const char* bottom_border = "-";

// print buffer n times
static void print_times(const char* buffer, size_t n);

// print buffer after indent of n chars
static void print_indent(const char* buffer, size_t n);

// print buffer with lines constraining to width and indented by margin
static void print_column(const char* buffer, size_t width, size_t margin, int is_hanging_indent);

// width of terminal
static size_t tty_width();

void task_print(task_t* task, int verbose);

void task_print_compact(task_t* task, size_t width, size_t margin);

void task_print_verbose(task_t* task, size_t width, size_t margin);

void tasklist_print(tasklist_t* task, size_t width, size_t margin, int verbose);

void taskmap_print(taskmap_t* map, size_t width, size_t margin, int verbose);

void todo_print(todo_t* todo, int verbose);

void todo_print_compact(todo_t* todo, size_t width, size_t margin);

void todo_print_verbose(todo_t* todo, size_t width, size_t margin);

void task_print(task_t* task, int verbose) {
  if (verbose)
    task_print_verbose(task, tty_width(), 0);
  else
    task_print_compact(task, tty_width(), 0);
}

void task_print_compact(task_t* task, size_t width, size_t margin) {
  size_t inner_margin = 4;
  size_t outer_margin = margin + inner_margin;
  size_t inner_width  = width - margin - inner_margin;

  char* text = NULL;

  // print status box
  print_indent("", margin);
  printf("[%c] ", task->status == COMPLETE ? 'x' : ' ');

  // print text
  if (task->value) {
    if (task->key)
      asprintf(&text, "(%s) %s", task->key, task->value);

    print_column(text ? text : task->value, inner_width, outer_margin, 1);
  }

  // print child tasks
  if (task->todo)
    todo_print_compact(task->todo, width, margin + 2);

  free(text);
}

void task_print_verbose(task_t* task, size_t width, size_t margin) {
  size_t inner_margin = 4;

  // print heading
  print_indent("", margin); 
  printf("(%s)\n", task->key ? task->key : "?");

  // print top border
  print_indent("", margin);
  print_times(top_border, width - margin);
  printf("\n");

  // print status box
  print_indent("", margin);
  printf("[%c] ", task->status == COMPLETE ? 'x' : ' ');

  // print text
  if (task->value)
    print_column(task->value, width - inner_margin - margin, inner_margin + margin, 1);

  // print bottom border
  print_indent("", margin);
  print_times(bottom_border, width - margin);
  printf("\n");

  // print child tasks
  if (task->todo)
    todo_print_verbose(task->todo, width, margin + 2);
}

void tasklist_print(tasklist_t* list, size_t width, size_t margin, int verbose) {
  int i = 0, is_faux_key = 0;
  task_t* task;
  char* buffer;

  while(list) {
    task = list->current;

    if (!task->key) {
      asprintf(&buffer, "%d", i);
      task->key = buffer;
      is_faux_key = 1;
    }

    if (verbose)
      task_print_verbose(task, width, margin);
    else
      task_print_compact(task, width, margin);

    if (is_faux_key) {
      task->key = NULL;
      free(buffer);
    }

    list = list->next;
    i++;
  }
}

void taskmap_print(taskmap_t* map, size_t width, size_t margin, int verbose) {
  int i;

  for (i = 0; i < map->lists_size; i++)
    if (map->lists[i])
      tasklist_print(map->lists[i], width, margin, verbose);
}

void todo_print(todo_t* todo, int verbose) {
  if (verbose)
    todo_print_verbose(todo, tty_width(), 0);
  else
    todo_print_compact(todo, tty_width(), 0);
}

void todo_print_compact(todo_t* todo, size_t width, size_t margin) {
  if (todo->map)
    taskmap_print(todo->map, width, margin, 0);
  if (todo->list)
    tasklist_print(todo->list, width, margin, 0);
}

void todo_print_verbose(todo_t* todo, size_t width, size_t margin) {
  if (todo->map)
    taskmap_print(todo->map, width, margin, 1);
  if (todo->list)
    tasklist_print(todo->list, width, margin, 1);
}

void print_times(const char* buffer, size_t n) {
  while (n--) printf("%s", buffer);
}

void print_indent(const char* buffer, size_t n) {
  printf("%-*s", (int) n, buffer);
}

void print_column(const char* buffer, size_t width, size_t margin, int is_hanging_indent) {
  size_t count, bufsize;
  const char *ptr, *rest;

  count = 0;
  bufsize = strlen(buffer);

  do {
    ptr = buffer + count;

    // don't set rest beyond the end of the buffer
    if (ptr - buffer + width <= bufsize)
      rest = ptr + width;
    else
      rest = buffer + bufsize;

    // back up EOL to a null terminator or space
    while (*(rest) && !isspace(*(rest))) rest--;

    count += fwrite(ptr, 1, rest - ptr, stdout);

    // print a newline and an indent
    print_indent("\n", is_hanging_indent ? bufsize != count ? margin : 0 : margin);
  } while (*rest);
}

static size_t tty_width() {
  struct ttysize ttys; 
  ioctl(0, TIOCGWINSZ, &ttys);
  return ttys.ts_cols;
}
