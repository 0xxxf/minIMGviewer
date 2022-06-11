#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace MinIMGView {

struct Application {
  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;
  SDL_Surface *surface = nullptr;
};

struct Image {
  unsigned int x;
  unsigned int y;

  std::string path;
  SDL_Texture *texture;
  SDL_Rect dest;
};

struct ImageList {
  std::vector<Image> list = {};
  int count = 0;
};

void init_sdl(Application &app);

void render_image(Application &app, std::string path);

std::vector<std::string> load_from_wd(std::string dir);

void render(Application &app, Image &img);

void run(Application &app, std::string path);

SDL_Texture *load_texture(std::string filename, Application &app);

extern std::vector<Image> imageList;

std::string get_wd(std::string path);

}
#endif