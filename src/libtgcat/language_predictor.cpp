#include "language_predictor.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>

LanguagePredictor::LanguagePredictor() noexcept {
  if (!loadModel()) {
    exit(EXIT_FAILURE);
  }
}

LanguagePredictor::~LanguagePredictor() {
}

std::string LanguagePredictor::predict(const std::string& data) noexcept {
  std::istringstream iss{data};

  const int32_t k = 1;
  const real threshold = 0.0;

  std::vector<std::pair<real, std::string>> predictions;
  if (_ft.predictLine(iss, predictions, k, threshold) && !predictions.empty()) {
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

bool LanguagePredictor::loadModel() noexcept {
  const auto path = "../../models/language";
  try {
    _ft.loadModel(path);
  } catch (const std::exception& ex) {
    std::cerr << "Exception: Unable to laod model! [" << path << "] "
              << ex.what() << std::endl;
    return false;
  }
  return true;
}
