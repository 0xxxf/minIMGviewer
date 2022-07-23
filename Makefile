OBJS = src/main.cc src/application.h src/application.cc src/file_util.h src/file_util.cc src/logger.h src/logger.cc

CC = g++

COMPILER_FLAGS = -Wall -Wextra -Wpedantic -O2 -std=c++2a -static-libstdc++ -DDEBUG

LINKER_FLAGS = -Iinclude -Llib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image  

OBJ_NAME = imgview

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
