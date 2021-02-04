#include "tgcat.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <map>

#include "../../resources/fasttext/src/fasttext.h"
using namespace fasttext;

static FastText ft;

bool fasttext_init() {
  const auto path = "../../models/lid.176.bin";
  try {
    ft.loadModel(path);
  } catch (const std::exception& ex) {
    std::cerr << "Exception: " << ex.what() << std::endl;
    return false;
  }

  return true;
}

std::string escaped(const std::string& input)
{
  std::string output;
  output.reserve(input.size());
  for (const char c : input) {
    switch (c) {
      case '\a': output += "\\a"; break;
      case '\b': output += "\\b"; break;
      case '\f': output += "\\f"; break;
      case '\n': output += "\\n"; break;
      case '\r': output += "\\r"; break;
      case '\t': output += "\\t"; break;
      case '\v': output += "\\v"; break;
      default:   output += c;     break;
    }
  }
  return output;
}

int tgcat_init() {
  if (!fasttext_init()) {
    std::cerr << "ERROR: fasttext initialization failed!" << std::endl;
    return -1;
  }

  return 0;
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  std::stringstream ss;
  ss << escaped(channel_info->title) << escaped(channel_info->description);
  for (std::size_t i = 0; i < channel_info->post_count; ++i) {
    ss << escaped(channel_info->posts[i]);
  }

  if (ss) {
    const int32_t k = 1;
    const real threshold = 0.0;

    std::vector<std::pair<real, std::string>> predictions;
    if (ft.predictLine(ss, predictions, k, threshold) && !predictions.empty()) {
      const auto code = predictions.at(0).second.substr(9); // skip __label__
      memcpy(language_code, code.c_str(), code.length());
      return 0;
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
