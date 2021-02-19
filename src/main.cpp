#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include <curses.h>

#include "fish.hpp"

namespace chrono = std::chrono;
using Clock = std::chrono::high_resolution_clock;

void renderGrid(const Fish &fish) {
  const auto &grid = fish.getGrid();
  printw("Source:\n");
  for (size_t y = 0; y < grid.size(); ++y) {
    for (size_t x = 0; x < grid[y].size(); ++x) {
      const auto &pos = fish.getPosition();
      addch(grid[y][x] |
            (x == pos.first && y == pos.second ? A_STANDOUT : A_NORMAL));
    }
    addch('\n');
  }
}

void renderStacks(const Fish::stacks_t &stacks) {
  printw("Stacks:\n");
  for (size_t i = 0; i < stacks.size(); ++i) {
    const auto &stack = stacks[i];
    printw("%d: [", i);
    if (stack.size() > 0) {
      for (size_t i = 0; i < stack.size() - 1; ++i) {
        printw("%d, ", stack[i]);
      }
      printw("%d", stack[stack.size() - 1]);
    }
    printw("]\n");
  }
}

void renderDebugger(const Fish &fish, std::stringstream &output) {
  move(0, 0);
  renderGrid(fish);
  renderStacks(fish.getStacks());
  std::string outputString = output.str();
  printw("Output:\n");
  printw("%s\n", outputString.c_str());
  refresh();
}

int main() {
  initscr();
  curs_set(0);
  cbreak();
  noecho();
  nodelay(stdscr, true);
  keypad(stdscr, true);

  std::string test_source =
      R"("hello, world"r\
          o;!?l<)";

  std::stringstream output;
  Fish fish(test_source, output);

  // first render
  renderDebugger(fish, output);

  chrono::milliseconds delay(200);
  constexpr chrono::milliseconds deltaDelay(10);
  auto lastTime = Clock::now();
  const auto &grid = fish.getGrid();
  bool userEnd = false;
  while (!fish.isCompleted() && !userEnd) {
    auto now = Clock::now();
    auto duration = now - lastTime;
    if (duration > delay) {
      lastTime = now;
      renderDebugger(fish, output);
      fish.step();
    }

    int y = getcury(stdscr);
    clrtoeol();
    mvprintw(y, 0, "Delay: %dms", delay.count());

    int ch = getch();
    switch (ch) {
    case KEY_UP:
    case 'k':
      delay += deltaDelay;
      break;
    case KEY_DOWN:
    case 'j':
      delay -= deltaDelay;
      break;
    case KEY_EXIT:
    case 'q':
      userEnd = true;
      break;
    }
  }

  int y = getcury(stdscr);
  move(y, 0);
  clrtoeol();
  if (userEnd) {
    printw("User Exit.");
  } else {
    printw("Completed.");
  }

  nodelay(stdscr, false);
  getch();

  endwin();
  return 0;
}
