#ifndef PREDICTOR_HPP
#define PREDICTOR_HPP

#include "fasttext.h"
using namespace fasttext;

class Predictor final {
public:
  Predictor(const std::string name, const std::string model_path);

  std::vector<std::pair<real, std::string>>
  predict(const std::string& data, const int32_t k = 1, const real threshold = 0.0) noexcept;

private:
  std::string _name{"Predictor"};
  FastText    _ft;

  bool loadModel(const std::string& path) noexcept;
};

#endif // PREDICTOR_HPP
