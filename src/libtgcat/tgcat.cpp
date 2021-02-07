#include "tgcat.hpp"

#include <string>
#include <cstring>
#include <memory>

#include "preprocessor.hpp"
#include "language_predictor.hpp"

// global instances (not exported)

static std::unique_ptr<Preprocessor> pp{nullptr};
static std::unique_ptr<LanguagePredictor> lp{nullptr};

// definitions

static
std::string get_channel_raw_data(const struct TelegramChannelInfo *channel_info) {
  std::string raw_data{channel_info->title};
  raw_data += ' ' + std::string{channel_info->description};
  for (std::size_t i = 0; i < channel_info->post_count; ++i) {
    raw_data += ' ' + std::string{channel_info->posts[i]};
  }
  return raw_data;
}

// libtgcat

int tgcat_init() {
  try {
    pp = std::make_unique<Preprocessor>(Preprocessor::Mode::DEBUG);
    lp = std::make_unique<LanguagePredictor>();
  } catch (const std::exception& ex) {
    std::cerr << "ERROR: Initialization failed!" << std::endl;
    return -1;
  }

  return 0;
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  const auto raw_data = get_channel_raw_data(channel_info);
  const auto preprocessed_data = pp->preprocess(raw_data);
  const auto predicted_language_code = lp->predict(preprocessed_data);

  memcpy(language_code, predicted_language_code.c_str(), predicted_language_code.size());
  return 0;
}

int tgcat_detect_category(const struct TelegramChannelInfo *channel_info,
                          double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  int i;
  for (i = 0; i < 10; i++) {
    category_probability[rand() % (TGCAT_CATEGORY_OTHER + 1)] += 0.1;
  }
  return 0;
}
