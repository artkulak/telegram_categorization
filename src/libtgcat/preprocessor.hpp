#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <string>
#include <fstream>

class Preprocessor final {
public:
  enum class Mode : bool { DEBUG, RELEASE };

  explicit Preprocessor(const Mode mode = Mode::RELEASE) noexcept;
  ~Preprocessor();

  std::string preprocess(std::string data) noexcept;

private:
  Mode            _mode;
  std::ofstream   _file;

  bool is_debug() const noexcept { return (_mode == Mode::DEBUG); }

  bool set_up_file() noexcept;

  void dump(std::string data) noexcept;

  std::string preprocess_email(const std::string& s) const noexcept;
  std::string preprocess_username(const std::string& s) const noexcept;
  std::string preprocess_links(const std::string& s) const noexcept;
  std::string preprocess_emojis_deletion(const std::string& s) const noexcept;
  std::string preprocess_emojis_isolation(const std::string& s) const noexcept;
  std::string preprocess_emojis(std::string s) const noexcept;
  std::string preprocess_whitespace(const std::string& s) const noexcept;
};

#endif // PREPROCESSOR_HPP
