CC=gcc
CFLAGS=-c -g -O -Wall -I../lib/libyaml/build/include -I.
LDFLAGS=-L../lib/libyaml/build/lib -lyaml
SOURCES=$(shell ls *.c)#api.c serialize.c deserialize.c print.c cli.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=todo

all: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -rf tests/*.o 
	rm -rf *.o $(EXECUTABLE)

dependencies:
	cd ../lib/libyaml && ./bootstrap && ./configure --prefix=`pwd`/build && make && make install

clean-dependencies:
	-cd ../lib/libyaml && make distclean
	-cd ../lib/libyaml && rm Makefile.in aclocal* configure config.h* && rm -rf build autom4te*

uninstall-dependencies:
	-cd ../lib/libyaml && make uninstall
	
test: all
	$(CC) -Wall tests/api_test.c api.o -o tests/api_test.o
	- tests/api_test.o
