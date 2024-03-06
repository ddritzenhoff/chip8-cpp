#include <_types/_uint16_t.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

constexpr size_t MEMORY_SIZE = 4096;
constexpr size_t DISPLAY_WIDTH = 8;
constexpr size_t DISPLAY_HEIGHT = 4;
constexpr size_t REGISTERS = 16;

int main() {
  std::printf("hello world\n");

  // Specification
  std::array<std::byte, MEMORY_SIZE> mem;
  // Display is 64x32 bits
  std::array<std::array<std::byte, DISPLAY_WIDTH>, DISPLAY_HEIGHT> display;
  // Program counter points to the current instruction in memory.
  size_t PC;
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
  std::array<std::byte, REGISTERS> regs;

  std::array<std::byte, 80> font_values = {
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0x90), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), // 0
      static_cast<std::byte>(0x20), static_cast<std::byte>(0x60),
      static_cast<std::byte>(0x20), static_cast<std::byte>(0x20),
      static_cast<std::byte>(0x70), // 1
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), // 2
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0xF0), // 3
      static_cast<std::byte>(0x90), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0x10), // 4
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0xF0), // 5
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), // 6
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0x20), static_cast<std::byte>(0x40),
      static_cast<std::byte>(0x40), // 7
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), // 8
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x10),
      static_cast<std::byte>(0xF0), // 9
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0x90), // A
      static_cast<std::byte>(0xE0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xE0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xE0), // B
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0x80), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), // C
      static_cast<std::byte>(0xE0), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0x90), static_cast<std::byte>(0x90),
      static_cast<std::byte>(0xE0), // D
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), // E
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0xF0), static_cast<std::byte>(0x80),
      static_cast<std::byte>(0x80) // F
  };

  for (int ii = 0; ii < 16; ii++) {
    for (int jj = 0; jj < 5; jj++) {
      mem[0x050 + 5 * ii + jj] = font_values[5 * ii + jj];
    }
  }
}