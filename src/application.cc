#include "application.h"
#include "file_util.h"
#include "logger.h"

void miv::init_sdl(Application &app, bool trace) {
  if (trace)
#define _TRACEMODE
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      printf("Couldnt initialize SDL, Error: %s\n", SDL_GetError());
      app.quit = true;
    }
  if (IMG_Init(IMG_INIT_PNG) < 0) {
    printf("Couldnt initialize SDL_Image, Error: %s\n", SDL_GetError());
    app.quit = true;
  }
  app.window = SDL_CreateWindow("minimgview", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, 1600, 900,
                                SDL_WINDOW_RESIZABLE);
  if (app.window == nullptr) {
    printf("Window couldn't be created, Error: %s\n", SDL_GetError());
    app.quit = true;
  }
  app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);
}

void miv::clean_up(Application &app) {
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  IMG_Quit();
  SDL_Quit();
}

miv::Image miv::create_image(std::string path) {
  /* @Refactor Although i made this struct when i started to write the program,
   * it may be cleaner without using it and directly rendering files from a
   * given path.
   */
  Image img;
  img.path = path;
  img.x = 0;
  img.y = 0;
  img.dest.x = img.x;
  img.dest.y = img.y;
  return img;
}

void miv::render(Application &app, TextureImageMap &img) {
  /*
    This may look "hacky", but i think it's perfectly fine, however i'll think
    of a better solution. An image will never be modified on it's first load, so
    we are fine to assume SDL_QueryTexture will be called on each image load,
    and the origin SDL_Rect data will be stored in original_val (we store this
    so we can render to the same window size when modifying the image).
  */

  if (img.image.mod) {
    SDL_SetWindowSize(app.window, img.image.original_val.w,
                      img.image.original_val.h);
  } else {
    SDL_QueryTexture(img.texture, nullptr, nullptr, &img.image.dest.w,
                     &img.image.dest.h);
    SDL_SetWindowSize(app.window, img.image.dest.w, img.image.dest.h);
    img.image.original_val = img.image.dest;
  }

  SDL_RenderCopy(app.renderer, img.texture, nullptr, &img.image.dest);
  SDL_RenderPresent(app.renderer);
}

constexpr void miv::zoom_in(Image &img, int x, int y) {
  img.mod = true;
  img.dest.x = x - (img.dest.w * 2 - img.dest.w) / 2;
  img.dest.y = y - (img.dest.h * 2 - img.dest.h) / 2;
  img.dest.w *= 2;
  img.dest.h *= 2;
}

constexpr void miv::zoom_out(Image &img, int x, int y) {
  img.mod = true;
  // TODO: this calculation doesn't work correctly (im bad at math lol)
  img.dest.x = x - (img.dest.w * 2 - img.dest.w) / 2;
  img.dest.y = y - (img.dest.h * 2 - img.dest.h) / 2;
  img.dest.w /= 2;
  img.dest.h /= 2;
}

void miv::run(Application &app, std::string path) {
  SDL_Event ev;
  int current = 0;
  int mouse_x, mouse_y;

  std::string wd;
  std::vector<std::string> file_list;
  Image img;

  /* If we receive a file as an argument, create an image from it and get the
   * rest of the files in that directory, however if we receive a folder, get
   * all the files from the folder and render the first one.
   * Think of a cleaner way to do this */

  if (is_file(path)) {
    img = create_image(path);
    wd = get_wd(path);
    file_list = load_all(wd);
  } else {
    file_list = load_all(path);
    if (!file_list.empty())
      img = create_image(file_list.at(0));
    current++;
  }
  if (file_list.empty()) {
    app.quit = true;
    printf("Provided directory doesn't contain any images, exitting.\n");
  }

  TextureImageMap *texture_map = new TextureImageMap[file_list.size()];
  size_t dir_size = file_list.size();

  // Load memory in chuncks of 5 files
  size_t batch = 5;
  size_t current_file_index = 0;

  if (dir_size <= batch)
    batch = dir_size;

  allocate_memory(current_file_index, batch, texture_map, file_list, app);
  current_file_index += batch;

  while (!app.quit) {
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT)
        app.quit = true;
      if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
        case SDLK_RIGHT:
          if (current < (int)file_list.size()) {
            current++;
            if (current == (int)file_list.size() - 1)
              current = 0;
          } else {
            current = 0;
            SDL_RenderClear(app.renderer);
          }
          if (current % 5 == 0) { // Carefull
            if(!check_alloc(texture_map, current)) {
              miv::log_stdout("Calling memory alloc", RESOURCE);
              allocate_memory(current, batch, texture_map, file_list, app);
              miv::log_stdout("Calling memory dealloc", RESOURCE);
              deallocate_memory(current - batch, batch, texture_map);
            }
          }
          if ((dir_size / 2) == current) {
            miv::log_stdout("Deallocating half of the loaded files", RESOURCE);
            deallocate_memory(0, current, texture_map);
          }
          break;
        case SDLK_LEFT:
          if (current == 0) {
            current = (int)file_list.size() - 1;
            allocate_memory(current - batch, batch, texture_map, file_list,
                            app);
          }
          if (current % 5 == 0) {
            allocate_memory(current - batch, batch, texture_map, file_list,
                            app);
          }
          current--;
          break;
        case SDLK_z:
          SDL_GetMouseState(&mouse_x, &mouse_y);
          zoom_in(texture_map[current].image, mouse_x, mouse_y);
          break;
        case SDLK_x:
          SDL_GetMouseState(&mouse_x, &mouse_y);
          zoom_out(texture_map[current].image, mouse_x, mouse_y);
          break;
        case SDLK_r:
          // Reset to original values
          texture_map[current].image.mod = false;
          texture_map[current].image.dest.x = 0;
          texture_map[current].image.dest.y = 0;
          break;
        case SDLK_q:
          app.quit = true;
          break;
        case SDLK_p:
          destroy_all(texture_map, file_list.size());
          break;
        }
      }
    }
    SDL_RenderClear(app.renderer);
    render(app, texture_map[current]);
  }
  delete texture_map;
  clean_up(app);
}

SDL_Texture *miv::load_texture(std::string filename, Application &app) {
  return IMG_LoadTexture(app.renderer, filename.c_str());
}

miv::TextureImageMap miv::generate_texture_map() {}

void miv::allocate_memory(size_t current_file_index, size_t batch,
                          TextureImageMap *texture_map,
                          std::vector<std::string> &file_list,
                          Application &app) {
#ifdef _TRACEMODE
  miv::log_stdout<size_t>("Allocate texture memory at index", current_file_index, RESOURCE);
#endif
  size_t target_batch;
  if ((current_file_index + batch) > file_list.size()) {
    target_batch = file_list.size() - (file_list.size() - current_file_index);
  } else {
    target_batch = current_file_index + batch;
  }
  for (; current_file_index < target_batch; current_file_index++) {
    std::string curr_file = file_list.at(current_file_index);
    auto image = create_image(curr_file);
    SDL_Texture *texture = load_texture(curr_file, app);
    texture_map[current_file_index].texture = texture;
    texture_map[current_file_index].image = image;
#ifdef _TRACEMODE
    miv::log_stdout<std::string>("Loaded file", file_list[current_file_index], FS);
#endif
  }
}

//FIXME: texture memory is not being freed for some reason
void miv::deallocate_memory(size_t start, size_t end,
                            TextureImageMap *texture_map) {
  for (size_t current = start - 1; current < end; current++) {
    //std::cout << "Destroying texture at map index:" << current << "\n";
    miv::log_stdout<size_t>("Destroying texture at index", current, RESOURCE);
    SDL_DestroyTexture(texture_map[current].texture);
    //std::cout << "Size: " << sizeof(texture_map[current]) << "\n";
  }
} 

bool miv::check_alloc(TextureImageMap *texture_map, size_t pos) {
  if(texture_map[pos].texture == nullptr) {
   return false; 
  }
  return true;
}

void miv::destroy_all(TextureImageMap *texture_map, size_t size) {
  for(size_t i = 0; i < size; i++) {
    SDL_DestroyTexture(texture_map[i].texture);
  }
}