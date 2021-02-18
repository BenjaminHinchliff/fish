#include <iostream>
#include <sstream>

#include <curses.h>

#include "fish.hpp"

int main() {
  initscr();

  std::string test_source =
      R"("hello, world"r\
          o;!?l<)";

  std::stringstream output;
  Fish fish(test_source, output);

  const auto &grid = fish.getGrid();
  while (!fish.isCompleted()) {
    move(0, 0);
    printw("Source:\n");
    for (size_t y = 0; y < grid.size(); ++y) {
      for (size_t x = 0; x < grid[y].size(); ++x) {
        const auto &pos = fish.getPosition();
        addch(grid[y][x] |
              (x == pos.first && y == pos.second ? A_STANDOUT : A_NORMAL));
      }
      addch('\n');
    }
    printw("Stacks:");
    for (const auto &stack : fish.getStacks()) {
      printw("[");
      if (stack.size() > 0) {
        for (size_t i = 0; i < stack.size() - 1; ++i) {
          printw("%d, ", stack[i]);
        }
        printw("%d", stack[stack.size() - 1]);
      }
      printw("]\n");
    }
    std::string outputString = output.str();
    printw("Output:\n");
    printw(outputString.c_str());
    refresh();
    getch();
    fish.step();
  }

  endwin();

  return 0;
}
