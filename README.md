todo
====

A command line interface for reading and writing TODO files.

Usage
-----
```
todo [action] [flags*] [-k key] [-f filename] ...

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
$ make && sudo make install
```

Tests
------------
```
$ make test
```

Usage examples
--------------

* List tasks
```sh
$ todo -l
```
    [ ] (api) 
      [ ] (rest) 
        [x] (0) Refactor Accept header parsing
        [ ] (1) Deprecate XML support
    [ ] (cli) 
      [ ] (bugs) 
        [x] (#137) Return an exit code instead of void
        [ ] (#142) Add null check
    [x] (opts) Parse additional options for running in development, staging, and production environments

* List a labeled task
```sh
$ todo -l -k cli/bugs
```
    [ ] (bugs)
      [x] (#137) Return an exit code instead of void
      [ ] (#142) Add null check

* List child tasks
```sh
$ todo -l -k cli/bugs/
```
    [x] (#137) Return an exit code instead of void
    [ ] (#142) Add null check

* List an unlabeled task
```sh
$ todo -l -k api/rest/1
```
    [ ] Deprecate XML support

* Add an unlabeled task
```sh
$ todo -a Schedule call with Jim RE: security audit
```

* Add a labeled task
```sh
$ todo -a -k cli/bugs/#151 Readline support
```

* Add an unlabeled subtask
```sh
$ todo -a -k api/rest/ PATCH vs. PUT vs. POST
```

* Edit a labeled task
```sh
$ todo -e -k cli/bugs/#151 Readline (or Editline?) support
```

* Edit an unlabeled child task
```sh
$ todo -e -k rest/api/1 Deprecate custom XML format in favor of RSS
```

* Remove an unlabeled task
```sh
$ todo -r -k rest/api/1
```

* Remove a labeled task
```sh
$ todo -r -k cli/bugs/#137
```

* Remove all child tasks
```sh
$ todo -r -k cli/bugs/
```

* Mark a task as complete 
```sh
$ todo -x -k cli/bugs 
```

* Mark a task as incomplete
```sh
$ todo -o -k cli/bugs/#151
```

* Change the priority of a task
```sh
$ todo -p -k cli/bugs/#137 [urgent|high|normal|low]
```

