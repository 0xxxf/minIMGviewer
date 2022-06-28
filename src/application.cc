#include "application.h"
#include "file_util.h"

void miv::init_sdl(Application &app) {
  //TODO: error checking
  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    printf("Couldnt initialize SDL, Error: %s\n",SDL_GetError());
    app.quit = true;
  }
  if(IMG_Init(IMG_INIT_PNG) <0 ) {
    printf("Couldnt initialize SDL_Image, Error: %s\n", SDL_GetError());
    app.quit = true;
  } 
  app.window = SDL_CreateWindow("minimgview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 900, SDL_WINDOW_RESIZABLE);
  if(app.window == nullptr) {
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
  /* @Refactor Although i made this struct when i started to write the program, it may be cleaner without using it and directly rendering files
   * from a given path.
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
    This may look "hacky", but i think it's perfectly fine, however i'll think of a better solution.
    An image will never be modified on it's first load, so we are fine to assume SDL_QueryTexture will be called on each
    image load, and the origin SDL_Rect data will be stored in original_val (we store this so we can render to the same window size when modifying the image).
  */

  if(img.image.mod) {
    SDL_SetWindowSize(app.window, img.image.original_val.w, img.image.original_val.h);
  } else {
    SDL_QueryTexture(img.texture, nullptr, nullptr, &img.image.dest.w, &img.image.dest.h);
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
  //TODO: this calculation doesn't work correctly
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

  /* If we receive a file as an argument, create an image from it and get the rest of the files in that directory,
   * however if we receive a folder, get all the files from the folder and render the first one.
   * Think of a cleaner way to do this */

  if(is_file(path)) {
    img = create_image(path);
    wd = get_wd(path);
    file_list = load_all(wd);
  } else {
    file_list = load_all(path);
    if(!file_list.empty())
      img = create_image(file_list.at(0));
    current++;
  }

  TextureImageMap placeholder; 
  placeholder.image = img;
  placeholder.texture = load_texture(img.path, app);

  if(file_list.empty()) {
    app.quit = true;
    printf("Provided directory doesn't contain any images, exitting.\n");
  }
	
  // Current soluntion i've implemented is to allocate all the needed memory for a provided
  // directory, this results in the program running faster, but our memory consumption is bigger.
  // Maybe change it to have an initial memory load, and increase it if needed, for eg:
  //
  // #define BLOCK_SIZE 25MB
  // Check if enough for next image => Ask for another block if needed
  TextureImageMap texture_map[file_list.size()];
  for(size_t i = 0; i < file_list.size(); i++) {
    std::string curr_file = file_list.at(i);
    auto image = create_image(curr_file);
    SDL_Texture *texture = load_texture(curr_file, app);
    texture_map[i].texture = texture;
    texture_map[i].image = image;
    std::cout << "Loaded file" << file_list[i] << "\n";
  }

  while(!app.quit) {
  while(SDL_PollEvent(&ev)) {
    if(ev.type == SDL_QUIT)
      app.quit = true;
    if(ev.type == SDL_KEYDOWN) {
      switch(ev.key.keysym.sym) {
        // Replace those two (SDLK_RIGHT and SDLK_LEFT), can be handled by the same function.
        case SDLK_RIGHT:
          if(current < (int)file_list.size()) {
            current++;
            if(current == (int)file_list.size() -1)
              current = 0;
          } else {
            current = 0;
            SDL_RenderClear(app.renderer);
          }
          break;
        case SDLK_LEFT:
          if(current == 0)
            current = (int)file_list.size() - 1;
          current--;
          break;
        case SDLK_z:
          SDL_GetMouseState(&mouse_x, &mouse_y);
          zoom_in(texture_map[current].image, mouse_x, mouse_y);
          break;
        case SDLK_x:
	  // Broke after last refactor
          SDL_GetMouseState(&mouse_x, &mouse_y);
          zoom_out(texture_map[current].image, mouse_x, mouse_y);
          break;
        case SDLK_r:
          texture_map[current].image.mod = false;
          texture_map[current].image.dest.x = 0;
          texture_map[current].image.dest.y = 0;
          break;
        case SDLK_q:
          app.quit = true;
          break;
      }
    }
  }
  SDL_RenderClear(app.renderer);
  render(app, texture_map[current]);
  }
  clean_up(app);
}

SDL_Texture* miv::load_texture(std::string filename, Application &app) {
  return IMG_LoadTexture(app.renderer, filename.c_str());
}

miv::TextureImageMap miv::generate_texture_map() {
  
}
