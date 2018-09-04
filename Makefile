
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -framework OpenGL
EXT =
CPPFLAGS = `pkg-config --cflags glfw3`

CC = g++
EXE = assign2
OBJS = model-view.o Table.o Shader.o Viewer.o Sphere.o Bouncer.o

.PHONY: all clean

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

Bouncer.o: Bouncer.h Bouncer.cpp
	$(CC) $(CPPFLAGS) -c Bouncer.cpp

model-view.o: model-view.cpp InputState.h
	$(CC) $(CPPFLAGS) -c model-view.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Viewer.o: Viewer.h Viewer.cpp InputState.h
	$(CC) $(CPPFLAGS) -c Viewer.cpp

Table.o: Table.h Table.cpp
	$(CC) $(CPPFLAGS) -c Table.cpp

Sphere.o: Sphere.hpp Sphere.cpp
	$(CC) $(CPPFLAGS) -c Sphere.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
