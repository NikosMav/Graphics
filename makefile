CC = g++
INCDIR = includes/
OBJDIR = build
SRCDIR = src
BINDIR = bin
LIBDIR = lib/
LIBS   = -I$(INCDIR) -L$(LIBDIR) -lassimp -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl


SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = build/main.o build/sphere.o build/glad.o

.PHONY: clean

all: main sphere

main: $(OBJ)
	echo compiling $?
	$(CC) -g3 -o $@ $(OBJ) -lm $(LIBS)

sphere: $(OBJ)
	echo compiling $?
	$(CC) -g3 -o $@ $(OBJ) -lm $(LIBS)

$(OBJDIR)/main.o: $(SRCDIR)/Main.cpp
	echo compiling $?
	$(CC) -c  $? -o $@  $(LIBS)

$(OBJDIR)/sphere.o: $(SRCDIR)/Sphere.cpp
	echo compiling $?
	$(CC) -c  $? -o $@  $(LIBS)

$(OBJDIR)/glad.o: $(SRCDIR)/glad.c
	echo compiling $?
	$(CC) -c  $? -o $@ $(LIBS)



clean:
	rm $(OBJDIR)/*.o
	rm main sphere