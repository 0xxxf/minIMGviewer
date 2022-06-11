#include "application.h"

void MinIMGView::init_sdl(Application &app) {
  //TODO: error checking
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  app.window = SDL_CreateWindow("minimgview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
  printf("SDL Initialized");
}

void MinIMGView::clean_up(Application &app) {
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  IMG_Quit();
  SDL_Quit();
}

void MinIMGView::render_image(Application &app, std::string path) {
  if(app.renderer != nullptr)
    SDL_DestroyRenderer(app.renderer);
  app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);

  std::string wd = get_wd(path);

  Image img;
  img.path = path;
  img.x = 0;
  img.y = 0;

  img.texture = load_texture(img.path, app);
  img.dest.x = img.x;
  img.dest.y = img.y;

  SDL_QueryTexture(img.texture, nullptr, nullptr, &img.dest.w, &img.dest.h);
  SDL_SetWindowSize(app.window, img.dest.w, img.dest.h);

  SDL_RenderCopy(app.renderer, img.texture, nullptr, &img.dest);
  SDL_RenderPresent(app.renderer);
}

void MinIMGView::render(Application &app, Image &img) {
  SDL_QueryTexture(img.texture, nullptr, nullptr, &img.dest.w, &img.dest.h);
  SDL_SetWindowSize(app.window, img.dest.w, img.dest.h);

  SDL_RenderCopy(app.renderer, img.texture, nullptr, &img.dest);
  SDL_RenderPresent(app.renderer);
}

void MinIMGView::run(Application &app, std::string path) {
  SDL_Event ev;
  int current = 0;
  printf("Loading file..");
  SDL_RenderClear(app.renderer);
  render_image(app, path);
  std::string wd;
  printf("\n PWD: %s", wd.c_str());

  //TODO: check if current path is a file or a directory
  std::vector<std::string> file_list;

  if(path.find(".") != std::string::npos) {
    render_image(app,path);
    wd = get_wd(path);
    file_list = load_from_wd(wd);
  } else {
    file_list = load_from_wd(path);
    render_image(app, file_list.at(0));
  }

  while(1) {
    if(SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
      break;
    if(ev.type == SDL_KEYDOWN) {
      if(current < (int) file_list.size()) {
        SDL_RenderClear(app.renderer);
        render_image(app, file_list.at(current));
        current++;
      } else {
        current = 0;
      }
    }
  }
  clean_up(app);
}

std::vector<std::string> MinIMGView::load_from_wd(std::string dir) {
  std::vector<std::string> files;
	std::string file_aux;
  std::cout << "\nWORKING DIR WHEN CALLING LOAD_FROM_WD " << dir << "\n";
  //TODO: check for only images
  for(const auto & entry : std::filesystem::directory_iterator(dir))
    if(entry.path().string().find(".PNG") != std::string::npos)
      files.push_back(entry.path().string());

  return files;
}

SDL_Texture* MinIMGView::load_texture(std::string filename, Application &app) {
  return IMG_LoadTexture(app.renderer, filename.c_str());
}

std::string MinIMGView::get_wd(std::string path) {
  auto found = path.find_last_of("/\\");
  return(path.substr(0, found));
}