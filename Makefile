target=lib/libmscreate.a

all:$(target)

INC=-I include/ -I /usr/local/include/casacore/
CXXFLAGS=-O3
LDFLAGS= -lcasa_casa -lcasa_ms -lcasa_measures -lcasa_tables

obj/MSCreate.o:src/MSCreate.cc
	mkdir -p obj && g++ -c $< $(CXXFLAGS) $(INC) -g -o $@

lib/libmscreate.a:obj/MSCreate.o
	mkdir -p lib && ar rv $@ $^

clean:
	rm -f `find -iname *.o` `find -iname *~`
