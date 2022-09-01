OBJS = src/main.cc src/application.h src/application.cc src/file_util.h src/file_util.cc src/logger.h src/logger.cc

CC = clang++

detected_OS := $(shell uname)

ifeq ($(detected_OS),Windows)
	CFLAGS = -Wall -Wextra -Wpedantic -O2 -std=c++17 
	LFLAGS = -Iinclude -Llib  -lmingw32 -lSDL2main -lSDL2 -lSDL2_image  
endif
ifeq ($(detected_OS),Darwin)
	CFLAGS = -Wall -Wextra -Wpedantic -O2 -std=c++17 -I/opt/homebrew/include/ -D_THREAD_SAFE
	LFLAGS =  -Iinclude -L/opt/homebrew/lib -lSDL2 -lSDL2_image
endif
ifeq ($(detected_OS),Linux)
	CFLAGS = -Wall -Wextra -Wpedantic -O2 -std=c++17 
	LFLAGS = -Iinclude -Llib -lSDL2main -lSDL2 -lSDL2_image  
endif

OBJ_NAME = imgview

all : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS)
