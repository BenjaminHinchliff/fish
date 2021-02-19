#ifndef FISH_FISH_HPP
#define FISH_FISH_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <random>
#include <stdexcept>
#include <ostream>

class Fish {
public:
  Fish(const std::string &source, std::ostream &output);

  using stacks_t = std::vector<std::vector<int>>;

  const std::vector<std::string> &getGrid() const;
  const std::pair<int, int> &getPosition() const;
  const stacks_t &getStacks() const;
  bool isCompleted() const;

  void step();

private:
  char cur_instruction() const noexcept;
  void push(int val);
  int pop();
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

  static const std::map<char, StringMode> modeMap;

private:
  std::vector<std::string> grid;
  stacks_t stacks = {{}};
  std::pair<int, int> size;
  std::pair<int, int> position = {0, 0};
  std::pair<int, int> direction = {1, 0};
  bool completed = false;
  StringMode stringMode = StringMode::OFF;
  std::ostream &output;
  std::mt19937 gen;
};

#endif // !FISH_FISH_HPP
