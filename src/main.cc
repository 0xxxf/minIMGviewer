#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[]) {
  #ifdef _WIN32
  HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle,SW_HIDE);
  #endif

  MinIMGView::Application app;
  MinIMGView::init_sdl(app);
  MinIMGView::run(app, argv[1]);

  return 0;
}