#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <__format/format_error.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>

constexpr size_t MEMORY_SIZE = 4096;
constexpr size_t DISPLAY_WIDTH = 8;
constexpr size_t DISPLAY_HEIGHT = 4;
constexpr size_t REGISTERS = 16;

int main(int argc, char *argv[]) {
  std::printf("hello world\n");

  if (argc < 2) {
    std::cerr << "Please provide a filename as an argument.\n";
    return 1;
  }

  // Specification
  std::array<uint8_t, MEMORY_SIZE> mem;
  // Program counter points to the current instruction in memory. Programs are
  // loaded in at address 0x200.
  size_t PC = 0x200;
  // Index register points to locations in memory.
  uint16_t I;
  // stack for 16-bit addresses.
  std::vector<uint16_t> stack;
  // delay timer decremented at 60hz until it reaches 0.
  uint8_t delay_timer;
  // sound timer decremented at 60hz and gives off beeping sound as long as it's
  // not 0.
  uint8_t sound_timer;
  // 16 general purpose registers.
  std::array<uint8_t, REGISTERS> regs;

  std::array<uint8_t, 80> font_values = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
  for (int ii = 0; ii < 16; ii++) {
    for (int jj = 0; jj < 5; jj++) {
      mem[0x050 + 5 * ii + jj] = font_values[5 * ii + jj];
    }
  }

  // file operations
  std::ifstream file(argv[1], std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open file " << argv[1] << '\n';
    return 1;
  }

  // Get the size of the file
  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Make sure the file isn't bigger than the memory array
  if (size > mem.size()) {
    std::cerr << "File is too big to fit in memory.\n";
    return 1;
  }

  std::printf("File size is %td bytes", size);
  // Read the file into the memory array
  file.read(reinterpret_cast<char *>(mem.data()), size);

  // Screen dimension constants
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow("chip8-cpp", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                        SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    std::printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Create a software surface
  SDL_Surface *surface =
      SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 8, 0, 0, 0, 0);
  if (surface == nullptr) {
    std::printf("Surface could not be created! SDL_Error: %s\n",
                SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Set the palette to black and white
  SDL_Color colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
  SDL_SetPaletteColors(surface->format->palette, colors, 0, 2);

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    // fetch
    uint16_t instr = (static_cast<uint16_t>(mem[PC]) << 8) |
                     static_cast<uint16_t>(mem[PC + 1]);
    PC += 2;

    // decode
    uint16_t opcode = (instr & 0xF000) >> 12;
    // second nibble. Used to look up one of the 16 registers (VX).
    uint16_t X = (instr & 0x0F00) >> 8;
    // third nibble. Used to look up one of the 16 registers (VY).
    uint16_t Y = (instr & 0x00F0) >> 4;
    uint16_t N = (instr & 0x000F);
    uint16_t NN = (instr & 0x00FF);
    uint16_t NNN = (instr & 0x0FFF);

    // execute
    switch (opcode) {
    case 0x0:
      if (instr == 0x00E0) {
        // Clear screen
        // Set the color to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // Update the screen
        SDL_RenderPresent(renderer);
      }
      break;
    case 0x1:
      // jump
      PC = NNN;
      break;
    case 0x6:
      regs[X] = NN;
      break;
    case 0x7:
      regs[X] = regs[X] + NN;
      break;
    case 0xA:
      I = NNN;
      break;
    case 0xD: {
      // draw
      uint8_t x_coord = regs[X] % 64;
      uint8_t y_coord = regs[Y] % 32;
      regs[0xF] = 0;

      for (int ii = 0; ii < N; ii++) {
        uint8_t pixel_row = mem[I + ii];
        Uint8 *pixels = (Uint8 *)surface->pixels;
        for (int jj = 0; jj < 8; jj++) {
          uint8_t mask = 1 << (7 - jj);
          if ((mask & pixel_row)) {
            if (pixels[y_coord * surface->w + x_coord]) {
              pixels[y_coord * surface->w + x_coord] = 0;
              regs[0xF] = 1;
            } else {
              pixels[y_coord * surface->w + x_coord] = 1;
            }
          }

          x_coord++;
          if (x_coord == 64) {
            // stop drawing this row if you hit the edge.
            break;
          }
        }
        y_coord++;
        if (y_coord == 32) {
          // stop if the edge of the screen was reached.
          break;
        }
      }

      // Create a texture from the surface
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      if (texture == nullptr) {
        std::printf("Texture could not be created! SDL_Error: %s\n",
                    SDL_GetError());
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
      }

      // Render the texture
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
      break;
    }
    default:
      break;
    }
  }

  // Clean up SDL components
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}
