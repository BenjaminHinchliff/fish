#include "fish.hpp"

Fish::Fish(const std::string &source, std::ostream &output) : output(output) {
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

const Fish::stacks_t &Fish::getStacks() const { return stacks; }

void Fish::step() {
  handle_instruction(cur_instruction());
  move();
}

char Fish::cur_instruction() const noexcept {
  try {
    return grid.at(position.second).at(position.first);
  } catch (const std::out_of_range &) {
    return ' ';
  }
}

void Fish::push(int val) { stacks.back().push_back(val); }

int Fish::pop() {
  auto &stack = stacks.back();
  if (stack.empty()) {
    throw std::runtime_error("unexpected end of stack");
  }
  int ret = stack.back();
  stack.pop_back();
  return ret;
}

void Fish::move() {
  position.first = pos_modulo(position.first + direction.first, size.first);
  position.second = pos_modulo(position.second + direction.second, size.second);
}

void Fish::handle_instruction(char instruction) {
  // string mode handling
  if (stringMode != StringMode::OFF) {
    auto str_end_it = modeMap.find(instruction);
    if (str_end_it != modeMap.end() && stringMode == str_end_it->second) {
      stringMode = StringMode::OFF;
    } else {
      push(instruction);
    }
    return;
  }

  // test if any string delimiters are there
  auto str_it = modeMap.find(instruction);
  if (str_it != modeMap.end()) {
    stringMode = str_it->second;
    return;
  }

  // test for any of the directional instructions
  auto dir_it = directions.find(instruction);
  if (dir_it != directions.end()) {
    direction = dir_it->second;
    return;
  }

  // test for any of the mirror instructions
  auto mir_it = mirrors.find(instruction);
  if (mir_it != mirrors.end()) {
    direction = mir_it->second(direction);
    return;
  }

  // test for (hex) number
  if (std::isxdigit(instruction)) {
    push(std::stoi(std::string(1, instruction), 0, 16));
    return;
  }

  // miscellaneous instructions
  switch (instruction) {
  case ';':
    completed = true;
    break;
  case '.': {
    int y = pop();
    int x = pop();
    position = std::make_pair(x, y);
  } break;
  case 'x': {
    std::uniform_int_distribution<> dis(0, 3);
    dir_it = directions.begin();
    std::advance(dir_it, dis(gen));
    direction = dir_it->second;
  } break;
  case 'r': {
    auto &stack = stacks.back();
    std::reverse(stack.begin(), stack.end());
  } break;
  case 'l':
    push(static_cast<int>(stacks.back().size()));
    break;
  case '!':
    move();
    break;
  case '?': {
    int value = pop();
    if (value == 0) {
      move();
    }
  } break;
  case 'o': {
    int value = pop();
    // TODO: bounds check
    output << static_cast<char>(value);
  } break;
  }
}

std::vector<std::string> Fish::split(const std::string &source,
                                     const std::string &delim) {
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

const std::map<char, Fish::StringMode> Fish::modeMap = {
    {'\'', Fish::StringMode::SINGLE_QUOTE},
    {'"', Fish::StringMode::DOUBLE_QUOTE},
};
