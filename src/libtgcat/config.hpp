#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config {
namespace Model::Path {
static constexpr auto language = "../../models/language";
static constexpr auto category_en = "../../models/category_en";
static constexpr auto category_ru = "../../models/category_ru";
} // Model::Path

namespace Randomized {
static constexpr auto posts_threshold = 10UL;
static constexpr auto no_of_passes = 5UL;
} // Prediction

} // Config

#endif // CONFIG_HPP
