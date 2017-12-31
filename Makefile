CXXFLAGS=-O2 -g
LDFLAGS=-lGL -lglfw -lGLEW

objs=main.o
str_inc=shader.frag.inc shader.vert.inc

default: main

run: main
	./main

main: $(objs)
	g++ $(LDFLAGS) $(objs) -o $@

clean:
	rm $(str_inc) $(objs)

%.inc: %
	xxd -i < $< > $@

main.o: main.cc $(str_inc)
	g++ -c $(CXXFLAGS) $< -o $@

%.o: %.cc
	g++ -c $(CXXFLAGS) $< -o $@