CC=gcc
CFLAGS=-c -g -O -Wall -I../lib/libyaml/build/include -I.
LDFLAGS=-L../lib/libyaml/build/lib -lyaml
SOURCES=$(shell ls *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=todo
PREFIX=/usr/local

# if a yaml.h file exists in both the include and build dirs of the libyaml dir
# assume our single dependency has been built and alter the 'all' target accordingly
HEADERS_COUNT := $(shell find .. -name 'yaml.h' | wc -l | awk '{print $$1}')

ifeq ("$(HEADERS_COUNT)", "2")
all: local
else
all: deps local
endif

local: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

deps:
	-cd ../lib/libyaml && ./bootstrap && ./configure --prefix=`pwd`/build && make && make install

deps-clean:
	-cd ../lib/libyaml && make distclean
	-cd ../lib/libyaml && rm Makefile.in aclocal* configure config.h* && rm -rf build autom4te*

clean:
	-rm -rf tests/*_test
	-rm -rf *.o $(EXECUTABLE)

distclean: deps-clean clean

test: all
	$(CC) -Wall tests/api_test.c api.o util.o -o tests/api_test
	@tests/api_test
	$(CC) -Wall tests/util_test.c util.o -o tests/util_test
	@tests/util_test

install: all
	mkdir -p $(PREFIX)/bin $(PREFIX)/lib $(PREFIX)/include
	install -s -c -m 0555 $(EXECUTABLE) $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/$(EXECUTABLE)
