target=lib/libmscreate.a

all:$(target)

INC=-I include/ -I /usr/include/casacore/
CXXFLAGS=-O3 -std=c++11
LDFLAGS= -lcasa_casa -lcasa_ms -lcasa_measures -lcasa_tables

obj/mscreate.o:src/mscreate.cpp
	mkdir -p obj && g++ -c $< $(CXXFLAGS) $(INC) -g -o $@

lib/libmscreate.a:obj/mscreate.o
	mkdir -p lib && ar rv $@ $^

clean:
	rm -f `find -iname *.o` `find -iname *~` lib/lib*
