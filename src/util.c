#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include "todo.h"

// does key contain '/'
int is_path(char* key) {
  return strchr(key, '/') ? 1 : 0;
}

// is key a valid int?
int is_int_path(char* key) {
  char* err;
  long int i;

  i = strtol(key, &err, 10);

  if (key[0] != '\n' && (*err == '\n' || *err == '\0'))
    return i >= INT_MIN && i <= INT_MAX;
  else
    return 0;
}

// a path that ends with '/'
int is_todo_path(char* key) {
  char* ptr = strrchr(key, '/');
  return ptr && *++ptr == '\0' ? 1 : 0;
}

