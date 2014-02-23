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
  -o    mark task as incomplete
  -x    mark task as complete
  -p    set task priority

flags
  -D    debug mode
  -v    verbose mode
```

Installation
------------
```sh
$ make && sudo make install
```

Tests
------------
```sh
$ make test
```

Usage examples
--------------

### List tasks

    $ todo -l
    [ ] (api) 
      [ ] (rest) 
        [x] (0) Refactor Accept header parsing
        [ ] (1) Deprecate XML support
      [ ] (cli) 
      [ ] (bugs) 
        [x] (#137) Return an exit code instead of void
        [ ] (#142) Add null check
    [x] (opts) Parse additional options for running in development, staging, and production environments

    
### List a labeled task

    $ todo -l -k cli/bugs
    [ ] (bugs)
      [x] (#137) Return an exit code instead of void
      [ ] (#142) Add null check

### List child tasks

    $ todo -l -k cli/bugs/
    [x] (#137) Return an exit code instead of void
    [ ] (#142) Add null check

### List an unlabeled task

    $ todo -l -k api/rest/1
    [ ] Deprecate XML support

### Add an unlabeled task

    $ todo -a Schedule call with Jim RE: security audit

### Add a labeled task

    $ todo -a -k cli/bugs/#151 Readline support

### Add an unlabeled subtask

    $ todo -a -k api/rest/ PATCH vs. PUT vs. POST

### Edit a labeled task

    $ todo -e -k cli/bugs/#151 Readline (or Editline?) support

### Edit an unlabeled child task

    $ todo -e -k rest/api/1 Deprecate custom XML format in favor of RSS

### Remove an unlabeled task

    $ todo -r -k rest/api/1

### Remove a labeled task

    $ todo -r -k cli/bugs/#137

### Remove all child tasks

    $ todo -r -k cli/bugs/

### Mark a task as complete 

    $ todo -x -k cli/bugs 

### Mark a task as incomplete

    $ todo -o -k cli/bugs/#151

### Change the priority of a task

    $ todo -p -k cli/bugs/#137 [urgent|high|normal|low]

