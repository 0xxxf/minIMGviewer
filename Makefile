OBJS = src/main.cc src/application.h src/application.cc

CC = g++

COMPILER_FLAGS = -Wall -Wextra -Wpedantic -std=c++2a -O

LINKER_FLAGS = -Iinclude -Llib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image  

OBJ_NAME = program

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
