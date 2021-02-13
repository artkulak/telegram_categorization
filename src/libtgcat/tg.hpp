#ifndef TG_HPP
#define TG_HPP

#include "config.hpp"
#include "cache.hpp"
#include "preprocessor.hpp"
#include "predictor.hpp"
#include <memory>

struct tgcat_manager_s {
  using Preprocessor_t = std::unique_ptr<Preprocessor>;
  using Predictor_t = std::unique_ptr<Predictor>;

  Cache           cache;
  Preprocessor_t  pp{nullptr};
  Predictor_t     lp{nullptr};
  Predictor_t     cp_en{nullptr};
  Predictor_t     cp_ru{nullptr};

  int init() noexcept {
    using namespace Config::Language;
    try {
      pp = std::make_unique<Preprocessor>();
      lp = std::make_unique<Predictor>("Language Predictor", Model::language);
      cp_en = std::make_unique<Predictor>("Category Predictor (en)", Model::category_en);
      cp_ru = std::make_unique<Predictor>("Category Predictor (ru)", Model::category_ru);
    } catch (const std::exception& ex) {
      std::cerr << "ERROR: Initialization failed!" << std::endl;
      return -1;
    }
    return 0;
  }
};

#endif // TG_HPP
