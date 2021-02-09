#ifndef CACHE_HPP
#define CACHE_HPP

#include <string>

class Cache final {
public:
  void set(const std::string& data, const std::string& code) noexcept { setData(data); setCode(code); }
  void setData(const std::string& data) noexcept { _data = data; }
  void setCode(const std::string& code) noexcept { _code = code; }
  std::string getData() const noexcept { return _data; }
  std::string getCode() const noexcept { return _code; }
  void reset() noexcept { _data.clear(); _code.clear(); }

private:
  std::string _data;
  std::string _code;
};

#endif // CACHE_HPP
