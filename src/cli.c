#include<stdio.h>
#include<assert.h>
#include<unistd.h>
#include "yaml.h"
#include "todo.h"

void usage(const char* error_buffer) {
  if (error_buffer) printf("%s\n", error_buffer);
  printf("Usage: todo -[h|l|a|r|e|m|o|x|p] -[q|v] [-k key] [-f filename] task\n");
}

int main(int argc, char** argv) {
  int exit_code = 0;

  // the current opt to be parsed and the (default) values of the action and verbose opts
  int opt, action_opt = 0, verbose_opt = 0;

  // the value of the key (k) opt
  char* key = NULL;

  // the (default) value of the file (f) opt
  char* filename = "TODO.yml";

  // the value of the remaining non-option args
  char* arg = NULL;

  char* error_buffer = NULL;

  // the format string to be printed if a mutex error is triggered
  char* mutex_error_f = "-%c and -%c are mutually exclusive options";

  // parse cli options
  while ((opt = getopt(argc, argv, ":hlaremoxpvqk:f:")) != -1) {
    switch (opt) {
      case 'h':
      case 'l':
      case 'a':
      case 'r':
      case 'e':
      case 'm':
      case 'o':
      case 'x':
      case 'p':
        if (action_opt)
          asprintf(&error_buffer, mutex_error_f, opt, action_opt);
        else
          action_opt = opt;
        break;
      
      case 'v':
      case 'q':
        if (verbose_opt)
          asprintf(&error_buffer, mutex_error_f, opt, verbose_opt);
        else
          verbose_opt = opt;
        break;

      case 'k':
        key = optarg;
        break;

      case 'f':
        filename = optarg;
        break;

      case ':':
        asprintf(&error_buffer, "Option flag -%c requires an argument", optopt);
        break;

      case '?':
        asprintf(&error_buffer, "Unknown option: -%c", optopt);
        break;
    }
  }

  if (error_buffer) {
    usage(error_buffer);
    exit_code = 1;
  } else {
    // set default action to 'ADD'
    if (!action_opt)
      action_opt = 'a';

    // set default verbosity to 'QUIET'
    if (!verbose_opt)
      verbose_opt = 'q';

    // join remaining args as a space delimited string
    while (optind < argc) {
      if (arg)
        asprintf(&arg, "%s %s", arg, argv[optind]);
      else
        arg = argv[optind];

      optind++;
    }

    printf("file: %s\n", filename);
    printf("action: %c\n", action_opt);
    printf("key: %s\n", key);
    printf("verbose: %c\n", verbose_opt);
    printf("non-option arg: %s\n", arg);

    FILE* file = fopen(filename, "r+");

    if (file) {
      todo_t* todo = todo_read(file);

      if(todo) {
        todo_write(todo, file);
        todo_destroy(todo);
      } else {
        printf("Requested file '%s' could not be parsed\n", filename);
        exit_code = 3;
      }

      fclose(file);
    } else {
      if (action_opt == 'a') {
        file = fopen(filename, "w");
        todo_t* todo = todo_create();
        task_t* task = task_create(key, arg);
        todo_insert(todo, task);
        todo_write(todo, file);
        todo_destroy(todo);
        fclose(file);
      } else {
        printf("Requested file '%s' does not exist\n", filename);
        exit_code = 2;
      }
    }
  }

  return exit_code;
}
