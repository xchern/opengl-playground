CXXFLAGS=-O2 -g -std=c++11
LDFLAGS=-lGL -lglfw -lGLEW

str_inc=shader.frag.inc shader.vert.inc

default: main shaderChecker

run: main 
	./$<

main: main.o
	g++ $(LDFLAGS) $< -o $@

shaderChecker: shaderChecker.o
	g++ $(LDFLAGS) $< -o $@

objs=main.o shaderChecker.o

clean:
	rm $(str_inc) $(objs)

%.inc: %
	xxd -i < $< > $@

main.o: main.cc $(str_inc) shader.h mesh.h
	clang++ -c $(CXXFLAGS) $< -o $@

%.o: %.cc
	clang++ -c $(CXXFLAGS) $< -o $@