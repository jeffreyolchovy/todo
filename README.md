todo
====

Usage examples
--------------

* List all tasks (quietly by default)

    $ todo -l
    [ ] don't forget the milk
    [x] destroy alderaan
    [ ] (party) piñata
      [ ] candy
      [x] wiffle bat
      [ ] length of rope


* List all tasks (verbosely)

    $ todo -l -v
    (0)
    ============================================
    [ ] don't forget the milk
    ============================================

    (1)
    ============================================
    [ ] destroy alderaan
    ============================================

    (party)
    ============================================
    [ ] piñata
    --------------------------------------------
      (0)
      ------------------------------------------
      [ ] candy
      ------------------------------------------
      (1)
      ------------------------------------------
      [x] wiffle bat
      ------------------------------------------
      (2)
      ------------------------------------------
      [ ] length of rope
      ==========================================

* List a labeled task

    $ todo -l -k party

* List an unlabeled task

    $ todo -l -k 1

* List a subtask

    $ todo -l -k party/1

* Add an unlabeled task

    $ todo 'don't forget the milk'

* Add a labeled task

    $ todo -a -k party piñata

* Add an unlabeled subtask

    $ todo -a -k party/ candy

* Add a labeled subtask

    $ todo -a -k party/planning 'send out invitations!'

* Edit a labeled task

    $ todo -e -k errand 'don't forget the milk!!!'

* Edit a subtask

    $ todo -e -k party/0 individually wrapped candy

* Remove an unlabeled task

    $ todo -r -k 1

* Remove a labeled task

    $ todo -r -k errand

* Move a labeled task

    $ todo -m -k party fiesta

* Move a labeled task (to the next natural index)

    $ todo -m -k errand .

* Move an unlabeled task into a labeled task's subtasks

    $ todo -m -k 4 errand/

* Mark a task as complete 

    $ todo -x -k errand 

* Mark a task as incomplete

    $ todo -o -k errand

* Change the priority of a task

  $ todo -p [+|-|urgent|high|normal|low] -k party/2

