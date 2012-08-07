#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<limits.h>
#include<unistd.h>
#include "todo.h"

static int DEBUG = 0;

static int VERBOSE = 0;

static int exit_code = 0;

static char* usage = "usage: todo [action] [flags*] [-k key] [-f filename] ...\n\n" 
                     "actions\n"
                     "  -h    show this message\n"
                     "  -l    list tasks\n"
                     "  -a    add a task\n"
                     "  -e    edit an existing task value\n"
                     "  -n    rename an existing task key\n"
                     "  -o    mark task as incomplete\n"
                     "  -x    mark task as complete\n"
                     "  -p    set task priority\n\n"
                     "flags\n"
                     "  -D    debug mode\n"
                     "  -v    verbose mode\n";

static void fatal_error(char* fmt, ...);

static void vfatal_error(char* fmt, va_list argp);

int main(int argc, char** argv) {
  // the current opt to be parsed and the (default) values of the cmd opt
  int opt, cmd_opt = 0;

  // the value of the key (k) opt
  char* key = NULL;

  // the (default) value of the file (f) opt
  char* filename = "TODO.yaml";

  // the value of the remaining non-option args
  char* arg = NULL;

  // parse cli options
  while ((opt = getopt(argc, argv, ":hlarenoxpvDqk:f:")) != -1) {
    switch (opt) {
    case 'h':
      printf("%s", usage);
      return 0;

    case 'l':
    case 'a':
    case 'r':
    case 'e':
    case 'n':
    case 'o':
    case 'x':
    case 'p':
      if (cmd_opt) {
        char* error_fmt = "-%c and -%c are mutually exclusive options\n%s";
        fatal_error(error_fmt, opt, cmd_opt, usage);
      } else {
        cmd_opt = opt;
      }
      break;
    
    case 'v':
      VERBOSE = 1;
      break;

    case 'D':
      DEBUG = 1;
      break;

    case 'k':
      key = optarg;
      break;

    case 'f':
      filename = optarg;
      break;

    case ':':
      fatal_error("Option flag -%c requires an argument\n%s", optopt, usage);
      break;

    case '?':
      fatal_error("Unknown option: -%c\n%s", optopt, usage);
      break;
    }
  }

  // set default action
  if (!cmd_opt) {
    if (arg)
      cmd_opt = 'a';
    else
      cmd_opt = 'l';
  }

  char* tmp = NULL;

  // join remaining args as a space delimited string
  while (optind < argc) {
    if (arg) {
      asprintf(&tmp, "%s %s", arg, argv[optind]);
      free(arg);
      arg = tmp;
    } else {
      arg = malloc(strlen(argv[optind]) + 1);
      arg = strcpy(arg, argv[optind]);
    }

    optind++;
  }

  // debug
  if (DEBUG) {
    printf("file: %s\n", filename);
    printf("action: %c\n", cmd_opt);
    printf("key: %s\n", key);
    printf("non-option arg: %s\n", arg);
    printf("verbose mode: %d\n", VERBOSE);
    printf("\n");
  }

  FILE* file = fopen(filename, "r+");

  // parse todo from existing file
  if (file) {
    todo_t* todo = todo_read(file);

    if (todo) {
      exit_code = execute(todo, cmd_opt, key, arg);
      rewind(file);
      todo_write(todo, file);
      todo_destroy(todo);
      fclose(file);
    } else {
      fatal_error("Requested file '%s' could not be parsed\n", filename);
    }
  // create a new file iff nonexistent and cmd_opt is 'a' (default)
  } else if (cmd_opt == 'a') {
    file = fopen(filename, "w");
    todo_t* todo = todo_create();
    exit_code = execute(todo, cmd_opt, key, arg);
    todo_write(todo, file);
    todo_destroy(todo);
    fclose(file);
  } else {
    fatal_error("Requested file '%s' does not exist\n", filename);
  }

  // cleanup
  if (arg) free(arg);

  return exit_code;
}

void fatal_error(char* fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vfatal_error(fmt, argp);
  va_end(argp);
}

void vfatal_error(char* fmt, va_list argp) {
  char buffer[500];
  vsnprintf(buffer, sizeof(buffer), fmt, argp);
  printf("%s", buffer);
  exit(++exit_code);
}

int execute(todo_t* todo, char cmd, char* key, char* arg) {
  switch(cmd) {
  case 'l':
    return execute_list(todo, key);

  case 'a':
    return execute_add(todo, key, arg);

  case 'e':
    return execute_edit(todo, key, arg);

  case 'n':
    return execute_rename(todo, key, arg);

  case 'r':
    return execute_remove(todo, key);

  case 'o':
    return execute_mark(todo, key, 0);

  case 'x':
    return execute_mark(todo, key, 1);

  case 'p':
    return execute_prioritize(todo, key, arg);

  default:
    fatal_error("Command -%c not implemented", cmd);
  }

  return 0;
}

int execute_list(todo_t* todo, char* path) {
  if (!path) {
    // print all
    todo_print(todo, VERBOSE);
  } else {
    task_t* task = todo_path_lookup(todo, path);

    if (!task)
      fatal_error("No task found at key '%s'\n", path);

    if (is_todo_path(path)) {
      if (task->todo) {
        // print child tasks
        todo_print(task->todo, VERBOSE);
      } else {
        // no-op, no child tasks
      }
    } else {
      // print task and all children
      task_print(task, VERBOSE);
    }
  }

  return 0;
}

int execute_add(todo_t* todo, char* path, char* value) {
  if (!value) value = "";

  if (!path) {
    task_t* task = task_create(NULL, value);
    todo_insert(todo, task);
  } else {
    task_t* task = todo_path_insert(todo, path);

    if (!task)
      fatal_error("Can not create task at key '%s'\n", path);

    if (is_todo_path(path)) {
      todo_t* parent;

      if (!task->todo) {
        parent = todo_create();
        task->todo = parent;
      } else {
        parent = task->todo;
      }

      task = task_create(NULL, value);
      todo_insert(parent, task);
    } else {
      task->status = INCOMPLETE;
      task->priority = NORMAL;
      
      if (task->todo) {
        todo_destroy(task->todo);
        task->todo = NULL;
      }

      if (task->value)
        free(task->value);

      task->value = malloc(strlen(value) + 1);
      strcpy(task->value, value);
    }
  }
    
  return 0;
}

int execute_edit(todo_t* todo, char* path, char* value) {
  if (!path) fatal_error("No key specified\n");

  task_t* task = todo_path_lookup(todo, path);

  if (!task)
    fatal_error("No task found at key '%s'\n", path);

  if (is_todo_path(path))
    fatal_error("Key must not end with '/'\n");

  if (task->value)
    free(task->value);

  task->value = malloc(strlen(value) + 1);
  strcpy(task->value, value);

  return 0;
}

int execute_remove(todo_t* todo, char* path) {
  if (!path) fatal_error("No key specified\n");

  if (strcmp("/", path) == 0) {
    todo_destroy(todo);
    todo = todo_create();
  } else { 
    todo_path_remove(todo, path);
  }

  return 0;
}

int execute_rename(todo_t* todo, char* from, char* to) {
  if (!from || !to) fatal_error("Must specify existing and new keys\n");

  task_t* task = todo_path_lookup(todo, from);

  if (!task)
    fatal_error("No task found at key '%s'\n", from);

  if (is_todo_path(from) || is_todo_path(to))
    fatal_error("Key must not end with '/'\n");

  if (!task->key)
    fatal_error("Task must have key in order to be renamed\n");

  task->key = malloc(strlen(to) + 1);
  strcpy(task->key, to);

  return 1;
}

int execute_mark(todo_t* todo, char* path, int is_complete) {
  if (!path) fatal_error("No key specified\n");

  task_t* task = todo_path_lookup(todo, path);

  if (!task) fatal_error("No task found at key '%s'\n", path);

  void mark_task_complete   (task_t*);
  void mark_task_incomplete (task_t*);

  task_apply(task, is_complete ? mark_task_complete : mark_task_incomplete);

  return 0;
}

int execute_prioritize(todo_t* todo, char* path, char* priority) {
  if (!path) fatal_error("No key specified\n");

  task_t* task = todo_path_lookup(todo, path);

  if (!task) fatal_error("No task found at key '%s'\n", path);

  void mark_task_urgent (task_t* task);
  void mark_task_high   (task_t* task);
  void mark_task_normal (task_t* task);
  void mark_task_low    (task_t* task);

  void (*f)(task_t*);

  switch(task_priority_valueof(priority)) {
  case URGENT:
    f = mark_task_urgent;
    break;

  case HIGH:
    f = mark_task_high;
    break;

  case NORMAL:
    f = mark_task_normal;
    break;

  case LOW:
    f = mark_task_low;
    break;
  }

  task_apply(task, f);

  return 0;
}

void mark_task_complete   (task_t* task) { task->status   = COMPLETE;   }
void mark_task_incomplete (task_t* task) { task->status   = INCOMPLETE; }
void mark_task_urgent     (task_t* task) { task->priority = URGENT;     } 
void mark_task_high       (task_t* task) { task->priority = HIGH;       } 
void mark_task_normal     (task_t* task) { task->priority = NORMAL;     } 
void mark_task_low        (task_t* task) { task->priority = LOW;        } 
