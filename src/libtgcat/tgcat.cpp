#include "tgcat.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <regex>

#include "preprocessor.hpp"

#include "../../resources/fasttext/src/fasttext.h"
using namespace fasttext;


// global instances (not exported)

static FastText ft;
static Preprocessor pp{Preprocessor::Mode::DEBUG};

// definitions

// fasttext

bool fasttext_init() noexcept {
  const auto path = "../../models/language";
  try {
    ft.loadModel(path);
  } catch (const std::exception& ex) {
    std::cerr << "Exception: Unable to laod model! [" << path << "] "
              << ex.what() << std::endl;
    return false;
  }
  return true;
}

std::string fasttext_predict(const std::string& data) noexcept {
  std::istringstream iss{data};

  const int32_t k = 1;
  const real threshold = 0.0;

  std::vector<std::pair<real, std::string>> predictions;
  if (ft.predictLine(iss, predictions, k, threshold) && !predictions.empty()) {
    const auto code = predictions.at(0).second.substr(9); // skip __label__

    // support only 2-letter language codes
    // `sh` is not in the Wikipedia ISO 639-1 list so it is not considered
    if (code.length() == 2 && code != "sh")
    {
      return code;
    }
  }

  return "other";
}

// libtgcat

int tgcat_init() {
  if (!fasttext_init()) {
    std::cerr << "ERROR: Initialization failed!" << std::endl;
    return -1;
  }
  return 0;
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  std::string raw_data{channel_info->title};
  raw_data += ' ' + std::string{channel_info->description};
  for (std::size_t i = 0; i < channel_info->post_count; ++i) {
    raw_data += ' ' + std::string{channel_info->posts[i]};
  }

  const auto preprocessed_data = pp.preprocess(raw_data);
  const auto predicted_language_code = fasttext_predict(preprocessed_data);

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
