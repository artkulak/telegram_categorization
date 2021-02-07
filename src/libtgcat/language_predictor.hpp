#ifndef LANGUAGE_PREDICTOR_HPP
#define LANGUAGE_PREDICTOR_HPP

#include "../../resources/fasttext/src/fasttext.h"
using namespace fasttext;

class LanguagePredictor final {
public:
    LanguagePredictor();
    ~LanguagePredictor();

    std::string predict(const std::string& data) noexcept;

private:
    FastText    _ft;

    bool loadModel() noexcept;
};

#endif // LANGUAGE_PREDICTOR_HPP
