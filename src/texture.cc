#include "texture.h"

void miv::load_forward(size_t current, size_t step,
                       std::vector<std::string> &file_list,
                       ImageTexture *image_texture_map, Application &app) {
  /* Check if there's fewer images left than our step */
  int move = step;
  if ((current + step) > file_list.size()) {
    move = file_list.size() - (file_list.size() - current);
  } else {
    move = current + step;
  }

  for (size_t i = current; i < move; i++) {
    auto image = create_image(file_list.at(i));
    auto *texture = load_texture(file_list.at(i), app);
    image_texture_map[i].texture = texture;
    image_texture_map[i].image = image;
  }
}

void miv::load_backwards(size_t current, size_t step,
                       std::vector<std::string> &file_list,
                       ImageTexture *image_texture_map, Application &app) {
  /* Check if there's fewer images left than our step */
  int move;
  if ((current - step) < 0 ) {
    move = file_list.size() - (file_list.size() - current);
  } {
    move = current - step;
  }

  for (size_t i = current; i > move; i++) {
    auto image = create_image(file_list.at(i));
    auto *texture = load_texture(file_list.at(i), app);
    image_texture_map[i].texture = texture;
    image_texture_map[i].image = image;
  }
}

void miv::destroy_textures(size_t start, size_t end,
                           ImageTexture *texture_map) {
  for (size_t i = start; i < end; i++) {
    SDL_DestroyTexture(texture_map[i].texture);
    texture_map[i].texture = nullptr;
  }
}

void miv::destroy_all(ImageTexture *texture_map, size_t size) {}