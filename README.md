todo
====

Usage examples
--------------

* List all tasks (quietly by default)
```sh
$ todo -l
[ ] don't forget the milk
[x] destroy alderaan
[ ] (party) piñata
  [ ] candy
  [x] wiffle bat
  [ ] length of rope
```

* List all tasks (verbosely)

```sh
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
```

* List a labeled task
```sh
$ todo -l -k party
```

* List an unlabeled task
```sh
$ todo -l -k 1
```

* List a subtask
```sh
$ todo -l -k party/1
```

* Add an unlabeled task
```sh
$ todo 'don't forget the milk'
```

* Add a labeled task
```sh
$ todo -a -k party piñata
```

* Add an unlabeled subtask
```sh
$ todo -a -k party/ candy
```

* Add a labeled subtask
```sh
$ todo -a -k party/planning 'send out invitations!'
```

* Edit a labeled task
```sh
$ todo -e -k errand 'don't forget the milk!!!'
```

* Edit a subtask
```sh
$ todo -e -k party/0 individually wrapped candy
```

* Remove an unlabeled task
```sh
$ todo -r -k 1
```

* Remove a labeled task
```sh
$ todo -r -k errand
```

* Move a labeled task
```sh
$ todo -m -k party fiesta
```

* Move a labeled task (to the next natural index)
```sh
$ todo -m -k errand .
```

* Move an unlabeled task into a labeled task's subtasks
```sh
$ todo -m -k 4 errand/
```

* Mark a task as complete 
```sh
$ todo -x -k errand 
```

* Mark a task as incomplete
```sh
$ todo -o -k errand
```

* Change the priority of a task
```sh
$ todo -p [+|-|urgent|high|normal|low] -k party/2
```

