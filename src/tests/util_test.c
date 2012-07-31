#include<stdio.h>
#include "minunit.h"
#include "../todo.h"

int tests_run = 0;

static char* test_is_path() {
  char* valid_path = "a/b/c";
  mu_assert(is_path(valid_path) == 1, "error, a/b/c is a valid path");

  char* invalid_path = "foo";
  mu_assert(is_path(invalid_path) == 0, "error, foo is not a valid path");

  return 0;
}

static char* test_is_int_path() {
  char* valid_path = "5";
  mu_assert(is_int_path(valid_path) == 1, "error, 5 is an int");

  char* invalid_path = "3a";
  mu_assert(is_int_path(invalid_path) == 0, "error, 3a is not an int");

  return 0;
}

static char* test_is_todo_path() {
  char* valid_path = "foo/";
  mu_assert(is_todo_path(valid_path) == 1, "error, foo/ is a todo path");

  char* invalid_path = "foo/bar";
  mu_assert(is_todo_path(invalid_path) == 0, "error, foo/bar is not a todo path");

  return 0;
}

static char* all_tests() {
  mu_run_test(test_is_path);
  mu_run_test(test_is_int_path);
  mu_run_test(test_is_todo_path);
  return 0;
}

int main() {
  char *result = all_tests();

  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }

  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
