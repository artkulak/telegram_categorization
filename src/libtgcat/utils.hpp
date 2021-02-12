#ifndef UTILS_HPP
#define UTILS_HPP

#include <random>
#include <set>

using RandomNumbers = std::set<std::size_t>;

RandomNumbers get_random_indices(const std::size_t post_count,
                                 const std::size_t threshold) noexcept {
  RandomNumbers numbers;
  if (post_count < threshold) {
    for (std::size_t i{0}; i != post_count; ++i) {
      numbers.emplace(i);
    }
  } else {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distrib(0, post_count-1);
    while (numbers.size() != threshold) {
      numbers.emplace(distrib(gen));
    }
  }
  return numbers;
}

#endif // UTILS_HPP
