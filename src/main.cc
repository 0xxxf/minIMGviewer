#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <sys/stat.h>
#include "application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
  #ifdef _WIN32
  HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle,SW_HIDE);
  #endif
  if(argv[1] == nullptr) {
    printf("Error: No directory/file path provided.");
    return -1;
  }

  struct stat info;

  if(stat(argv[1], &info) == 0) {
    MinIMGView::Application app;
    MinIMGView::init_sdl(app);
    MinIMGView::run(app, argv[1]); 
  }
  else {
    printf("Error: Invalid directory/file path provided, exitting.");
    return -1;
  }
  
  return 0;
}