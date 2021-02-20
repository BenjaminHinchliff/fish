#ifndef FISH_FISH_HPP
#define FISH_FISH_HPP

#include <cmath>
#include <functional>
#include <map>
#include <optional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

struct FishStack {
  std::vector<double> stack;
  std::optional<double> reg;
};

class Fish {
public:
  Fish(const std::string &source, const std::string &input, const std::string &output);

  using stacks_t = std::vector<FishStack>;

  const std::vector<std::string> &getGrid() const;
  const std::pair<int, int> &getPosition() const;
  const stacks_t &getStacks() const;
  const std::string &getInput() const;
  const std::string &getOutput() const;
  bool isCompleted() const;

  void step();

  static constexpr double ABS_EPSILON = 1e-12;
  static constexpr double REL_EPSILON = 1e-8;

private:
  char cur_instruction() const noexcept;
  char getCell(int x, int y) const noexcept;
  void push(double val);
  double pop();
  std::optional<double> &reg();
  void move();
  void handle_instruction(char instruction);

  constexpr int pos_modulo(int i, int n) const noexcept {
    return (i % n + n) % n;
  }

  bool approximatelyEqual(double a, double b) const noexcept;

  std::vector<std::string> split(const std::string &source,
                                 const std::string &delim);

private:
  using directions_t = std::map<char, std::pair<int, int>>;
  static const directions_t directions;

  using mirrors_t =
      std::map<char,
               std::function<std::pair<int, int>(const std::pair<int, int> &)>>;
  static const mirrors_t mirrors;

  using operators_t = std::map<char, std::function<double(double, double)>>;
  static const operators_t operators;

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
  std::string output;
  std::string input;
  std::mt19937 gen;
};

#endif // !FISH_FISH_HPP
