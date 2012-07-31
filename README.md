todo
====

A command line interface for project TODO files.


Usage
-----
```
todo [action] [flag*] [-k key] [-f filename] <args...>

actions
  -h    show this message
  -l    list tasks
  -a    add a task
  -e    edit an existing task value
  -n    rename an existing task key
  -o    mark task as incomplete
  -x    mark task as complete
  -p    set task priority

flags
  -D    debug mode
  -v    verbose mode
  
```

Installation
------------
```
$ make all && sudo make install
```

Tests
------------
```
$ make test
```

Usage examples
--------------

* List all tasks
```sh
$ todo -l
```

* List all tasks (verbosely)
```sh
$ todo -l -v
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
$ todo 'don\'t forget the milk'
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
$ todo -e -k errand 'don\'t forget the milk!!!'
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

* Rename a labeled task
```sh
$ todo -n -k party fiesta
```

* Rename a labeled task (to the next natural index)
```sh
$ todo -n -k errand .
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
$ todo -p -k party [urgent|high|normal|low]
```

