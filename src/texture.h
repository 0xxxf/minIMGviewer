#ifndef MEMORY_H
#define MEMORY_H

#include "application.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace miv {

struct ImageTexture {
  SDL_Texture *texture = nullptr;
  Image image;
};

void create_textures(size_t current_file_index, size_t batch,
                      ImageTexture *texture_map,
                      std::vector<std::string> &file_list, Application &app);
void destroy_textures(size_t start, size_t end, ImageTexture *texture_map);

/* Load forward/backward textures */
void load_forward(size_t current, size_t step, std::vector<std::string> &file_list, ImageTexture *image_texture_map, Application &app);
void load_backwards(size_t current, size_t step, std::vector<std::string> &file_list, ImageTexture *image_texture_map, Application &app);

void check_texture(ImageTexture *texture_map, size_t pos);
void destroy_all(ImageTexture *texture_map, size_t size);

} // namespace miv
#endif
