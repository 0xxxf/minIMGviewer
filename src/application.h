#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>


namespace miv {

struct Application {
  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;
  SDL_Surface *surface = nullptr;

  bool quit = false;
};

struct Image {
  /* This could be a Class, however it's fine for now */
  unsigned int x;
  unsigned int y;

  /* mod flag allows the struct to always store the original SDL_Rect width and
   * height this should be set to true whenever any of the original values get
   * changed
   */
  bool mod = false;
  std::string path;

  /* Use this SDL_Rect as a surface for rendering the image, keep a copy in
   * original_val in case we want to go back to the original state */
  SDL_Rect dest;
  SDL_Rect original_val;
};

struct TextureImageMap {
  SDL_Texture *texture = nullptr;
  Image image;
};

struct TextureImageMap generate_texture_map();

/* This kind of works like a constructor, maybe think of a better solution or
   directly convert Image to a class, altought this creates some additional
   complexity that doesn't need to be there.*/
Image create_image(std::string path);

void init_sdl(Application &app, bool trace);

void render(Application &app, TextureImageMap &img);

void run(Application &app, std::string path);
void clean_up(Application &app);

constexpr void zoom_in(Image &img, int x, int y);
constexpr void zoom_out(Image &img, int x, int y);

SDL_Texture *load_texture(std::string filename, Application &app);

void allocate_memory(size_t current_file_index, size_t batch,
                     TextureImageMap *texture_map,
                     std::vector<std::string> &file_list, Application &app);

void deallocate_memory(size_t start, size_t end, TextureImageMap *texture_map);

bool check_alloc(TextureImageMap *texture_map, size_t pos);

void destroy_all(TextureImageMap *texture_map, size_t size);
} // namespace miv
#endif
