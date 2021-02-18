#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <functional>

#include <curses.h>

class Fish {
public:
  Fish(const std::string &source);

  const std::vector<std::string> &getGrid() const;
  const std::pair<int, int> &getPosition() const;
  const std::vector<int> &getStack() const;
  bool isCompleted() const;

  void step();

private:
  char current_instruction() const noexcept;
  void move();
  void handle_instruction(char instruction);

  constexpr int pos_modulo(int i, int n) const noexcept {
    return (i % n + n) % n;
  }
  std::vector<std::string> split(const std::string &source,
                                 const std::string &delim);

private:
  using directions_t = std::map<char, std::pair<int, int>>;
  static const directions_t directions;

  using mirrors_t =
      std::map<char,
               std::function<std::pair<int, int>(const std::pair<int, int> &)>>;
  static const mirrors_t mirrors;

  enum class StringMode {
    OFF,
    DOUBLE_QUOTE,
    SINGLE_QUOTE,
  };

  static const std::map<char, StringMode> stringModeMap;

private:
  std::vector<std::string> grid;
  std::vector<int> stack;
  std::pair<int, int> size;
  std::pair<int, int> position = {0, 0};
  std::pair<int, int> direction = {1, 0};
  bool completed = false;
  StringMode stringMode = StringMode::OFF;
  std::mt19937 gen;
};

Fish::Fish(const std::string &source) {
  std::random_device rd;
  gen = std::mt19937(rd());
  grid = split(source, "\n");
  int height = static_cast<int>(grid.size());
  auto largest_row = std::max_element(
      grid.begin(), grid.end(), [](const std::string &a, const std::string &b) {
        return a.length() < b.length();
      });
  int width = static_cast<int>(largest_row->length());
  size = std::make_pair(width, height);
}

const std::vector<std::string> &Fish::getGrid() const { return grid; }

const std::pair<int, int> &Fish::getPosition() const { return position; }

bool Fish::isCompleted() const { return completed; }

const std::vector<int> &Fish::getStack() const { return stack; }

void Fish::step() {
  handle_instruction(current_instruction());
  move();
}

char Fish::current_instruction() const noexcept {
  try {
    return grid.at(position.second).at(position.first);
  } catch (const std::out_of_range &) {
    return ' ';
  }
}

void Fish::move() {
  position.first = pos_modulo(position.first + direction.first, size.first);
  position.second = pos_modulo(position.second + direction.second, size.second);
}

void Fish::handle_instruction(char instruction) {

  if (stringMode != StringMode::OFF) {
    auto str_end_it = stringModeMap.find(instruction);
    if (str_end_it != stringModeMap.end() && stringMode == str_end_it->second) {
      stringMode = StringMode::OFF;
    } else {
      stack.push_back(instruction);
    }
    return;
  }

  auto str_it = stringModeMap.find(instruction);
  if (str_it != stringModeMap.end()) {
    stringMode = str_it->second;
    return;
  }

  auto dir_it = directions.find(instruction);
  if (dir_it != directions.end()) {
    direction = dir_it->second;
    return;
  }
  auto mir_it = mirrors.find(instruction);
  if (mir_it != mirrors.end()) {
    direction = mir_it->second(direction);
    return;
  }
  if (instruction == ';') {
    completed = true;
  } else if (instruction == '.') {
    int y = stack.back();
    stack.pop_back();
    int x = stack.back();
    stack.pop_back();
    position = std::make_pair(x, y);
  } else if (instruction == 'x') {
    std::uniform_int_distribution<> dis(0, 3);
    dir_it = directions.begin();
    std::advance(dir_it, dis(gen));
    direction = dir_it->second;
  } else if (std::isxdigit(instruction)) {
    stack.push_back(std::stoi(std::string(1, instruction), 0, 16));
  } else if (instruction == 'r') {
    std::reverse(stack.begin(), stack.end());
  }
}

std::vector<std::string> Fish::split(const std::string& source,
  const std::string& delim) {
  std::vector<std::string> output;

  size_t last = 0;
  size_t pos = 0;
  while ((pos = source.find(delim, last)) != std::string::npos) {
    output.push_back(source.substr(last, pos - last));
    last = pos + 1;
  }
  output.push_back(source.substr(last));
  return output;
}


const Fish::directions_t Fish::directions = {
    {'^', {0, -1}}, {'>', {1, 0}}, {'v', {0, 1}}, {'<', {-1, 0}}};

const Fish::mirrors_t Fish::mirrors = {
    {'/',
     [](const std::pair<int, int> &dir) {
       return std::make_pair(-dir.second, -dir.first);
     }},
    {'\\',
     [](const std::pair<int, int> &dir) {
       return std::make_pair(dir.second, dir.first);
     }},
    {'|',
     [](const std::pair<int, int> &dir) {
       return std::make_pair(-dir.first, dir.second);
     }},
    {'_',
     [](const std::pair<int, int> &dir) {
       return std::make_pair(dir.first, -dir.second);
     }},
    {'#', [](const std::pair<int, int> &dir) {
       return std::make_pair(-dir.first, -dir.second);
     }}};

const std::map<char, Fish::StringMode> Fish::stringModeMap = {
    {'\'', Fish::StringMode::SINGLE_QUOTE},
    {'"', Fish::StringMode::DOUBLE_QUOTE},
};

int main() {
  initscr();

  std::string test_source =
      R"("hello, world"r\
          o;!?l<)";

  Fish fish(test_source);

  const auto &grid = fish.getGrid();
  while (!fish.isCompleted()) {
    move(0, 0);
    printw("stack: [");
    const auto &stack = fish.getStack();
    if (stack.size() > 0) {
      for (size_t i = 0; i < stack.size() - 1; ++i) {
        printw("%d, ", stack[i]);
      }
      printw("%d", stack[stack.size() - 1]);
    }
    printw("]\n");
    for (size_t y = 0; y < grid.size(); ++y) {
      for (size_t x = 0; x < grid[y].size(); ++x) {
        const auto &pos = fish.getPosition();
        addch(grid[y][x] |
              (x == pos.first && y == pos.second ? A_STANDOUT : A_NORMAL));
      }
      addch('\n');
    }
    refresh();
    getch();
    fish.step();
  }

  endwin();

  return 0;
}
