#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include <curses.h>
#include <cxxopts.hpp>

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
    const auto &fishStack = stacks[i];
    const auto &stack = fishStack.stack;
    printw("%d: [", i);
    if (stack.size() > 0) {
      std::stringstream ss;
      for (size_t i = 0; i < stack.size() - 1; ++i) {
        ss << stack[i] << ", ";
      }
      ss << stack[stack.size() - 1];
      std::string ssStr = ss.str();
      printw(ssStr.c_str());
    }
    printw("] reg: ");
    const auto &reg = fishStack.reg;
    std::stringstream ss;
    if (reg.has_value()) {
      ss << *reg;
    } else {
      ss << "None";
    }
    std::string ssStr = ss.str();
    printw("%s\n", ssStr.c_str());
  }
}

void renderDebugger(const Fish &fish, const std::stringstream &input, const std::stringstream &output) {
  move(0, 0);
  clear();
  renderGrid(fish);
  renderStacks(fish.getStacks());
  printw("Input:\n");
  std::string inputString = input.str();
  printw("%s\n", inputString.c_str());
  std::string outputString = output.str();
  printw("Output:\n");
  constexpr size_t seg = 500;
  size_t len = outputString.length();
  size_t loops = len != 0 ? ((len - 1) / seg) + 1 : 1;
  for (size_t i = 0; i < loops; ++i) {
    printw(outputString.substr(i * seg, seg).c_str());
  }
  addch('\n');
  refresh();
}

int main(int argc, char **argv) {
  cxxopts::Options options("fishpp",
                           "A Fish Debugger/Interpreter written in c++");

  std::string source;
  std::string sourceFilePath;
  std::string inputStr;
  options.add_options()("h,help", "Print usage")(
      "c,code", "Source code", cxxopts::value<std::string>(source))(
      "s,source", "Source file", cxxopts::value<std::string>(sourceFilePath),
      "SOURCE")("i,inputStr", "text inputStr into the program",
                cxxopts::value<std::string>(inputStr));

  options.parse_positional({"source"});

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << '\n';
    return 0;
  }

  if (!result.count("code") && !result.count("source")) {
    std::cerr << "Missing required argument: (code | source)" << '\n';
    std::cerr << options.help() << '\n';
    return 1;
  }
  if (result.count("source")) {
    std::ifstream sourceFile(sourceFilePath);
    if (!sourceFile) {
      std::cerr << "unable to open source file: \"" << sourceFilePath << "\"\n";
      std::cerr << options.help() << '\n';
      return 1;
    }
    std::stringstream sourceStream;
    sourceStream << sourceFile.rdbuf();
    source = sourceStream.str();
  }

  initscr();
  curs_set(0);
  cbreak();
  noecho();
  nodelay(stdscr, true);
  keypad(stdscr, true);

  std::stringstream input;
  input << inputStr;

  std::stringstream output;
  Fish fish(source, input, output);

  // first render
  renderDebugger(fish, input, output);

  chrono::milliseconds delay(200);
  constexpr chrono::milliseconds deltaDelay(10);
  auto lastTime = Clock::now();
  const auto &grid = fish.getGrid();
  bool shouldntBlock = true;
  bool shouldRender = false;
  bool userEnd = false;
  while (!fish.isCompleted() && !userEnd) {
    auto now = Clock::now();
    auto duration = now - lastTime;
    if ((shouldntBlock && duration > delay) || shouldRender) {
      shouldRender = false;
      lastTime = now;
      renderDebugger(fish, input, output);
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
    case 's':
      shouldntBlock = !shouldntBlock;
      nodelay(stdscr, shouldntBlock);
      break;
    case ' ':
      shouldRender = true;
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
