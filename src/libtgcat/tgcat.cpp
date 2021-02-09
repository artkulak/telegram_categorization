#include "tgcat.hpp"

#include <string>
#include <cstring>
#include <memory>

#include "config.hpp"
#include "cache.hpp"
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
    pp = std::make_unique<Preprocessor>(Preprocessor::Mode::DEBUG);
    lp = std::make_unique<Predictor>("Language Predictor", Model::Path::Language);
    // cp_en = std::make_unique<Predictor>("Category Predictor (en)", Model::Path::Category_en);
    cp_ru = std::make_unique<Predictor>("Category Predictor (ru)", Model::Path::Category_ru);
  } catch (const std::exception& ex) {
    std::cerr << "ERROR: Initialization failed!" << std::endl;
    return -1;
  }
  return 0;
}

static
std::string get_channel_data(const struct TelegramChannelInfo *channel_info) noexcept {
  auto raw_data = std::string{channel_info->title};
  raw_data += ' ' + std::string{channel_info->description};
  for (std::size_t i = 0; i < channel_info->post_count; ++i) {
    raw_data += ' ' + std::string{channel_info->posts[i]};
  }
  return raw_data;
}

static
std::string get_valid_code(std::string code) noexcept {
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
  if (cache.getCode() == "ru") {
    return cp_ru->predict(cache.getData(), -1);
  }
  return {};
}

// Use-cases

namespace UseCase__CompleteInfo {

void detect_language(const struct TelegramChannelInfo *channel_info,
                     char language_code[6]) {
  const auto data = get_channel_data(channel_info);
  const auto preprocessed_data = pp->preprocess(data);
  const auto predictions = lp->predict(data);
  if (predictions.empty()) {
    cache.reset();
    return;
  }

  const auto [_, label] = predictions.at(0);
  const auto code = get_valid_code(label);
  memcpy(language_code, code.c_str(), code.size());
  cache.set(data, code);
}

void detect_category(const struct TelegramChannelInfo *channel_info,
                     double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  const auto predictions = get_category_predictions();
  if (predictions.empty()) {
    return;
  }

  double sum = 0.0;
  for (const auto [probability, _] : predictions) {
    sum += probability;
  }

  for (auto [probability, label] : predictions) {
    if (sum > 1.0) {
      probability /= sum;
    }
    const auto index = std::stoi(label.substr(9)) + 1;
    category_probability[index] = probability;
  }
}

} // UseCase_FullChannelInfo

namespace UseCase__RandomizedPosts {

} // UseCase__RandomizedPosts

// libtgcat

int tgcat_init() {
  return init();
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  UseCase__CompleteInfo::detect_language(channel_info, language_code);
  return 0;
}

int tgcat_detect_category(const struct TelegramChannelInfo *channel_info,
                          double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  UseCase__CompleteInfo::detect_category(channel_info, category_probability);
  return 0;
}
