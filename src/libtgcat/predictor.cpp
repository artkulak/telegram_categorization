#include "predictor.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <exception>

Predictor::Predictor(const std::string name, const std::string model_path) : _name{name} {
  if (!loadModel(model_path)) {
    throw std::runtime_error{_name + " | Initalization failed!"};
  }
}

std::vector<std::pair<real, std::string>>
Predictor::predict(const std::string& data, const int32_t k, const real threshold) noexcept {
  std::istringstream iss{data};
  std::vector<std::pair<real, std::string>> predictions;
  _ft.predictLine(iss, predictions, k, threshold);
  return predictions;
}

bool Predictor::loadModel(const std::string& path) noexcept {
  try {
    _ft.loadModel(path);
  } catch (const std::exception& ex) {
    std::cerr << _name
              << " | Exception: Unable to laod model! [" << path << "] "
              << ex.what() << std::endl;
    return false;
  }
  return true;
}
