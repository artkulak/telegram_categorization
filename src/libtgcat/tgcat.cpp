#include "tgcat.hpp"

#include <string>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <utility>

#include "config.hpp"
#include "cache.hpp"
#include "utils.hpp"
#include "preprocessor.hpp"
#include "predictor.hpp"

// global instances (not exported)
static Cache cache;

static std::unique_ptr<Preprocessor> pp{nullptr};
static std::unique_ptr<Predictor> lp{nullptr};
// static std::unique_ptr<Predictor> cp_en{nullptr};
static std::unique_ptr<Predictor> cp_ru{nullptr};

// definitions

static
int init() noexcept {
  using namespace Config;
  try {
    pp = std::make_unique<Preprocessor>();
    lp = std::make_unique<Predictor>("Language Predictor", Model::Path::language);
    // cp_en = std::make_unique<Predictor>("Category Predictor (en)", Model::Path::category_en);
    cp_ru = std::make_unique<Predictor>("Category Predictor (ru)", Model::Path::category_ru);
  } catch (const std::exception& ex) {
    std::cerr << "ERROR: Initialization failed!" << std::endl;
    return -1;
  }
  return 0;
}

static
std::string get_valid_language_code(std::string code) noexcept {
  code = code.substr(9); // skip __label___
  // support only 2-letter language codes
  // `sh` is not in the Wikipedia ISO 639-1 list so it is not considered
  return ((code.length() == 2 && code != "sh") ? code : "other");
}

static
std::vector<std::pair<real, std::string>> get_category_predictions() noexcept {
  // if (previous_code == "en") {
  //   return cp_en->predict(previous_data);
  // }
  if (cache.get_code() == "ru") {
    return cp_ru->predict(cache.get_data(), -1); // get all predictions
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
std::string get_channel_data(const TelegramChannelInfo *channel_info) noexcept {
  auto data = std::string{channel_info->title};
  data += ' ' + std::string{channel_info->description};
  for (std::size_t i = 0; i < channel_info->post_count; ++i) {
    data += ' ' + std::string{channel_info->posts[i]};
  }
  return data;
}

static
void detect_language(const TelegramChannelInfo *channel_info,
                     char language_code[6]) {
  const auto data = get_channel_data(channel_info);
  const auto preprocessed_data = pp->preprocess(data);
  const auto predictions = lp->predict(preprocessed_data);
  if (predictions.empty()) {
    cache.reset();
    return;
  }

  const auto [_, label] = predictions.at(0);
  const auto code = get_valid_language_code(label);
  memcpy(language_code, code.c_str(), code.size());
  cache.set(preprocessed_data, code);
}

static
void detect_category(const TelegramChannelInfo *channel_info,
                     double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  const auto predictions = get_category_predictions();
  if (!predictions.empty()) {
    populate_category_probabilites(predictions, category_probability);
    cache.reset();
  }
}

} // UseCase__Complete

// ---- Randomized ----

namespace UseCase__Randomized {

static
std::string get_channel_data(const TelegramChannelInfo *channel_info) noexcept {
  const auto indices = get_random_indices(channel_info->post_count,
                                          Config::Randomized::posts_threshold);
  auto data = std::string{channel_info->title};
  data += ' ' + std::string{channel_info->description};
  for (const auto i : indices) {
    data += ' ' + std::string{channel_info->posts[i]};
  }
  return data;
}

static
void detect_language(const TelegramChannelInfo *channel_info,
                     char language_code[6]) {
  std::unordered_map<std::string, std::pair<std::string, std::size_t>> lookup_table;
  for (std::size_t i{0}; i != Config::Randomized::no_of_passes; ++i) {
    const auto data = get_channel_data(channel_info);
    const auto preprocessed_data = pp->preprocess(data);
    const auto predictions = lp->predict(preprocessed_data);
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
  cache.set(preprocessed_data, code);
}

static
void detect_category(const TelegramChannelInfo *channel_info,
                     double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  const auto predictions = get_category_predictions();
  if (!predictions.empty()) {
    populate_category_probabilites(predictions, category_probability);
    cache.reset();
  }
}

} // UseCase__Randomized

// libtgcat

int tgcat_init() {
  return init();
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
