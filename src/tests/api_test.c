#include<stdio.h>
#include<assert.h>
#include "minunit.h"
#include "../todo.h"

int tests_run = 0;

static char* test_tasklist() {
  task_t* a = task_create(NULL, "a");
  tasklist_t* list = tasklist_create(a);

  mu_assert(list->current == a, "error, list->current != a");

  task_t* b = task_create(NULL, "b");
  tasklist_prepend(&list, b);

  mu_assert(list->current == b, "error, list->current != b");

  tasklist_remove(&list, 0);

  mu_assert(list->current == a, "error, list->current != a");

  task_t* c = task_create(NULL, "c");
  tasklist_append(&list, c);

  mu_assert(list->next->current == c, "error, list->next->current != c");

  task_t* d = tasklist_lookup(list, 0);
  mu_assert(d == a, "error, list[0] != a");

  task_t* e = tasklist_lookup(list, 1);
  mu_assert(e == c, "error, list[1] != c");

  tasklist_destroy(list);

  return 0;
}

static char* test_taskmap() {
  taskmap_t* map = taskmap_create(7);
  task_t* a = task_create("A", "a");
  taskmap_insert(map, a);

  mu_assert(taskmap_lookup(map, "A") == a, "error, map[A] != a");

  task_t* b = task_create("A", "b");
  taskmap_insert(map, b);

  mu_assert(taskmap_lookup(map, "A") == b, "error, map[A] != b");

  taskmap_remove(map, "A");

  mu_assert(taskmap_lookup(map, "A") == NULL, "error, map[A] != NULL");

  taskmap_destroy(map);

  return 0;
}

static char* test_tasktable() {
  tasktable_t* table = tasktable_create();
  task_t* a = task_create("A", "a");
  task_t* b = task_create(NULL, "b");
  task_t* c = task_create(NULL, "c");
  task_t* d = task_create("A", "d");
  task_t* e = task_create("B", "e");

  tasktable_insert(table, a);
  mu_assert(taskmap_lookup(table->map, "A") == a, "error table->map[A] != a");

  tasktable_insert(table, b);
  tasktable_insert(table, c);

  mu_assert(table->list->current == b, "error table->list->current != b");
  mu_assert(tasklist_lookup(table->list, 1) == c, "error table->list[1] != c");

  tasktable_remove(table, "0");
  tasktable_remove(table, "A");

  mu_assert(table->list->current == c, "error table->list->current != c");

  tasktable_insert(table, d);
  tasktable_insert(table, e);

  mu_assert(taskmap_lookup(table->map, "A") == d, "error table->map[A] != d");
  mu_assert(taskmap_lookup(table->map, "B") == e, "error table->map[B] != e");

  tasktable_destroy(table);

  return 0;
}
 
static char* all_tests() {
  mu_run_test(test_tasklist);
  mu_run_test(test_taskmap);
  mu_run_test(test_tasktable);
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
