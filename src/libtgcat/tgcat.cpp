#include "tgcat.hpp"

#include "tg.hpp"
#include "utils.hpp"

#include <cstring>
#include <unordered_map>
#include <utility>

// global instances (not exported)
static tgcat_manager_s tg;

// definitions

static
std::string get_valid_language_code(std::string code) noexcept {
  code = code.substr(9); // skip __label___
  // support only 2-letter language codes
  // `sh` is not in the Wikipedia ISO 639-1 list so it is not considered
  return ((code.length() == 2 && code != "sh") ? code : "other");
}

static
std::vector<std::pair<real, std::string>> get_category_predictions() noexcept {
  using namespace Config::Language;
  if (tg.cache.get_code() == Code::English) {
    return tg.cp_en->predict(tg.cache.get_data(), -1);
  }
  if (tg.cache.get_code() == Code::Russian) {
    return tg.cp_ru->predict(tg.cache.get_data(), -1);
  }
  return {};
}

static
void populate_category_probabilites(const std::vector<std::pair<real, std::string>>& predictions,
                                    double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  auto sum = 0.0f;
  for (const auto& [probability, _] : predictions) {
    sum += probability;
  }

  for (auto [probability, label] : predictions) {
    probability /= sum;
    label = label.substr(9); // skip __label__
    const auto index = std::stoi(label);
    category_probability[index] = probability;
  }
}

// Use-cases

// ---- Complete ----

namespace UseCase__Complete {

static
std::string get_channel_data(const TelegramChannelInfo *channel_info,
                             const bool is_unique = false) noexcept {
  auto data = std::string{channel_info->title};
  data += ' ' + std::string{channel_info->description};
  const auto count = channel_info->post_count;
  const auto posts = channel_info->posts;
  if (is_unique) {
    std::set<std::string> unique_posts;
    for (std::size_t i = 0; i != count; ++i) {
      unique_posts.emplace(posts[i]);
    }
    for (const auto post : unique_posts) {
      data += ' ' + post;
    }
  } else {
    for (std::size_t i = 0; i != count; ++i) {
      data += ' ' + std::string{posts[i]};
    }
  }
  return data;
}

static
void detect_language(const TelegramChannelInfo *channel_info,
                     char language_code[6]) {
  const auto data = get_channel_data(channel_info);
  const auto preprocessed_data = tg.pp->preprocess(data);
  const auto predictions = tg.lp->predict(preprocessed_data);
  if (predictions.empty()) {
    tg.cache.reset();
    return;
  }

  const auto [_, label] = predictions.at(0);
  const auto code = get_valid_language_code(label);
  memcpy(language_code, code.c_str(), code.size());
  tg.cache.set(preprocessed_data, code);
}

static
void detect_category(const TelegramChannelInfo *channel_info,
                     double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  const auto predictions = get_category_predictions();
  if (!predictions.empty()) {
    populate_category_probabilites(predictions, category_probability);
    tg.cache.reset();
  }
}

} // UseCase__Complete

// ---- Randomized ----

namespace UseCase__Randomized {

static
std::string get_channel_data(const TelegramChannelInfo *channel_info,
                             const bool is_unique = false) noexcept {
  const auto indices = get_random_indices(channel_info->post_count,
                                          Config::Randomized::posts_threshold);
  auto data = std::string{channel_info->title};
  data += ' ' + std::string{channel_info->description};
  const auto count = channel_info->post_count;
  const auto posts = channel_info->posts;
  if (is_unique) {
    std::set<std::string> unique_posts;
    for (const auto i : indices) {
      unique_posts.emplace(posts[i]);
    }
    for (const auto post : unique_posts) {
      data += ' ' + post;
    }
  } else {
    for (std::size_t i = 0; i != count; ++i) {
      data += ' ' + std::string{posts[i]};
    }
  }
  return data;
}

static
void detect_language(const TelegramChannelInfo *channel_info,
                     char language_code[6]) {
  std::unordered_map<std::string, std::pair<std::string, std::size_t>> lookup_table;
  for (std::size_t i{0}; i != Config::Randomized::no_of_passes; ++i) {
    const auto data = get_channel_data(channel_info);
    const auto preprocessed_data = tg.pp->preprocess(data);
    const auto predictions = tg.lp->predict(preprocessed_data);
    if (!predictions.empty()) {
      const auto [_, label] = predictions.at(0);
      const auto code = get_valid_language_code(label);
      if (auto it = lookup_table.find(code); it != lookup_table.end()) {
        it->second.first = preprocessed_data;
        it->second.second++;
      } else {
        lookup_table[code] = {preprocessed_data, 1};
      }
    }
  }

  std::string code;
  std::string preprocessed_data;
  std::size_t frequency{0};
  for (const auto& [c, p] : lookup_table) {
    if (frequency < p.second) {
      code = c;
      preprocessed_data = p.first;
      frequency = p.second;
    }
  }

  memcpy(language_code, code.c_str(), code.size());
  tg.cache.set(preprocessed_data, code);
}

static
void detect_category(const TelegramChannelInfo *channel_info,
                     double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  const auto predictions = get_category_predictions();
  if (!predictions.empty()) {
    populate_category_probabilites(predictions, category_probability);
    tg.cache.reset();
  }
}

} // UseCase__Randomized

// libtgcat

int tgcat_init() {
  return tg.init();
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  if (channel_info->post_count < Config::Randomized::posts_threshold) {
    UseCase__Complete::detect_language(channel_info, language_code);
  } else {
    UseCase__Randomized::detect_language(channel_info, language_code);
  }
  return 0;
}

int tgcat_detect_category(const struct TelegramChannelInfo *channel_info,
                          double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  if (channel_info->post_count < Config::Randomized::posts_threshold) {
    UseCase__Complete::detect_category(channel_info, category_probability);
  } else {
    UseCase__Randomized::detect_category(channel_info, category_probability);
  }
  return 0;
}
