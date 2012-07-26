#include<stdio.h>
#include<assert.h>
#include "yaml.h"
#include "todo.h"

typedef enum cli_opt_actions_e {
  HELP,
  LIST,
  ADD,
  REMOVE,
  EDIT,
  MOVE,
  MARK,
  PRIORITIZE
} cli_opt_actions_t;

typedef enum cli_opt_filter_opts_e {
  ALL,
  ONLY_INCOMPLETE,
  ONLY_COMPLETED
} cli_opt_filter_opts_t;

typedef enum cli_opt_sort_opts_e {
  BY_NATURAL,
  BY_PRIORITY_ASC,
  BY_PRIORITY_DESC
} cli_opt_sort_opts_t;

int main(int argc, char** argv) {
/* TODO
 *
 * parse cli options
 *
 * :a:r:e:o:x:s:q:lmphv
 * 
 *  mutex opts:
 *    a r e o x l m p h
 *
 *  list all tasks (verbosely)
 *  $ todo -lv
 *
 *  Task: errand
 *  ============================================
 *  [ ] don't forget the milk
 *  --------------------------------------------
 *    Task: 0
 *    ------------------------------------------
 *    [ ] and the bread too
 *    ------------------------------------------
 *    Task: 1
 *    ------------------------------------------
 *    [ ] eggs?
 *    ------------------------------------------
 *      Task: 0 
 *      ----------------------------------------
 *      [ ] something
 *      ----------------------------------------
 *      ... more ...
 *  ============================================
 *
 *  Task: 0
 *  ============================================
 *  [x] destroy alderaan 
 *  ============================================
 *
 *  list all tasks (quietly)
 *  $ todo -lq
 *
 *  [ ] (errand) don't forget the milk
 *    [ ] and the bread too
 *    [ ] eggs?
 *      [ ] something
 *        [ ] something else
 *  [x] destroy alderaan
 *
 *  list labeled task
 *  $ todo -l errand
 *
 *  list unlabeled task
 *  $ todo -l 4
 *
 *  list labeled subtask
 *  $ todo -l errand/reminder
 *
 *  list unlabeled subtask
 *  $ todo -l errand/0
 *
 *  list (sort / filter options)
 *  $ todo -ls [p|x|o]
 *
 *  add an unlabeled task
 *  $ todo don't forget the milk
 * 
 *  add a labeled task
 *  $ todo -a errand don't forget the milk
 *
 *  add an unlabeled subtask
 *  $ todo -a errand/ and the bread too
 *  
 *  add a labeled subtask
 *  $ todo -a errand/reminder and the bread too
 *
 *  edit a labeled task
 *  $ todo -e errand don't forget the milk
 *
 *  edit an unlabeled subtask
 *  $ todo -e errand/2 and the bread too
 *  
 *  edit a labeled subtask
 *  $ todo -e errand/reminder and the bread too
 *
 *  rm an unlabeled task
 *  $ todo -r 4
 *
 *  rm a labeled task
 *  $ todo -r errand
 *
 *  mv a labeled task
 *  $ todo -m errand reminder
 *
 *  mv a labeled task (to next natural index)
 *  $ todo -m errand .
 *
 *  mv an unlabeled task into a labeled's subtasks
 *  $ todo -m 4 errand/
 *
 *  mark as complete 
 *  $ todo -x errand 
 *
 *  mark as incomplete
 *  $ todo -o errand
 *
 *  increment priority
 *  $ todo +p errand
 *
 *  decrement priority
 *  $ todo -p errand
 *
 *  change priority
 *  $ todo [+|-]p [urgent|high|normal|low] errand
 */

  if (argc != 3) {
		printf("Not enough arguments.\n");
		printf("Usage: ./todo <infile> <outfile>\n");
		return 0;
	}

  /* currently, all this does is parse the infile and rewrite it to the outfile */

  FILE* infile = fopen(argv[1], "r");

  if (infile) {
    tasktable_t* table = tasktable_read(infile);

    if (table) {
      tasktable_print(table, 0);

      FILE* outfile = fopen(argv[2], "w");

      if (outfile) {
        tasktable_write(table, outfile);
        fclose(outfile);
      }

      tasktable_destroy(table);
    }

    fclose(infile);
  }

  return 0;
}
