#ifndef CACHE_HPP
#define CACHE_HPP

#include <string>

class Cache final {
public:
  void set(const std::string& data, const std::string& code) noexcept { set_data(data); set_code(code); }
  void set_data(const std::string& data) noexcept { _data = data; }
  void set_code(const std::string& code) noexcept { _code = code; }
  std::string get_data() const noexcept { return _data; }
  std::string get_code() const noexcept { return _code; }
  void reset() noexcept { _data.clear(); _code.clear(); }

private:
  std::string _data;
  std::string _code;
};

#endif // CACHE_HPP
