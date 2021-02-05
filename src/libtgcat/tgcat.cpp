#include "tgcat.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "../../resources/fasttext/src/fasttext.h"
using namespace fasttext;


// global instances (not exported)

static FastText ft;

#define DUMP_PREPROCESSED_DATA
#ifdef DUMP_PREPROCESSED_DATA
static std::ofstream preprocessed_file;
#endif

// definitions

// fasttext

bool fasttext_init() {
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

// preprocess data

bool preprocess_init() {
#ifdef DUMP_PREPROCESSED_DATA
  preprocessed_file.open("./preprocessed.txt", std::ios::trunc);
  if (!preprocessed_file.is_open()) {
    std::cerr << "ERROR: Failed to create file to dump preprocessed data!" << std::endl;
    return false;
  }
#endif
  return true;
}

void preprocess_dump(std::string data) {
#ifdef DUMP_PREPROCESSED_DATA
    data += '\n';
    preprocessed_file.write(data.c_str(), data.size());
#endif
}

std::string preprocess(const std::string& input) {
  std::string output;
  output.reserve(input.size());

  for (char c : input) {
    if (isdigit(c) || ispunct(c) ||
        (isspace(c) && c != ' ') ||
        c == '\a' || c == '\b') {
      continue;
    }

    if (isupper(c)) {
      c = tolower(c);
    }

    output += c;
  }

  return output;
}

// libtgcat

int tgcat_init() {
  if (!fasttext_init() || !preprocess_init()) {
    std::cerr << "ERROR: Initialization failed!" << std::endl;
    return -1;
  }
  return 0;
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  std::stringstream ss;
  ss << preprocess(channel_info->title) << preprocess(channel_info->description);
  for (std::size_t i = 0; i < channel_info->post_count; ++i) {
    ss << preprocess(channel_info->posts[i]);
  }

  if (ss) {
    preprocess_dump(ss.str());

    const int32_t k = 1;
    const real threshold = 0.0;

    std::vector<std::pair<real, std::string>> predictions;
    if (ft.predictLine(ss, predictions, k, threshold) && !predictions.empty()) {
      const auto code = predictions.at(0).second.substr(9); // skip __label__

      // support only 2-letter language codes
      // `sh` is not in the Wikipedia ISO 639-1 list so it is not considered
      if (code.length() == 2 && code != "sh")
      {
        memcpy(language_code, code.c_str(), code.length());
        return 0;
      }
    }
  }

  memcpy(language_code, "other", 6);
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
