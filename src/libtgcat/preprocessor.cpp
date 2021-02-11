#include "preprocessor.hpp"

#include <iostream>
#include <regex>
#include <ctime>
#include <cstdlib>

// public interface

Preprocessor::Preprocessor(const Mode mode) : _mode{mode} {
  if (is_debug()) {
    if (!set_up_file()) {
      throw std::runtime_error{"Preprocessor: Initalization failed!"};
    }
  }
}

Preprocessor::~Preprocessor() {
  if (is_debug()) {
    if (_file.is_open()) {
      _file.close();
    }
  }
}

std::string Preprocessor::preprocess(std::string data) noexcept {
  try {
    data = preprocess_email(data);
    data = preprocess_username(data);
    data = preprocess_links(data);
    // data = preprocess_emojis(data);
    data = preprocess_whitespace(data);
  } catch (const std::exception& ex) {
    std::cerr << "Exception: [" << ex.what()
              << "] Returning partially preprocessed data!"
              << std::endl;
    dump(data);
    return data;
  }

  std::string output;
  output.reserve(data.size());

  for (const char ch : data) {
    if (!isdigit(ch)) {
      output += isupper(ch) ? tolower(ch) : ch;
    }
  }

  dump(output);
  return output;
}

// private utility methods

bool Preprocessor::set_up_file() noexcept {
  const auto current_time = std::time(nullptr);
  const auto prefix = "preprocessed_";
  const auto filename = prefix + std::to_string(current_time) + ".txt";

  _file.open(filename);
  if (!_file.is_open()) {
    std::cerr << "ERROR: Unable to create file to dump preprocessed data! "
              << filename << std::endl;
    return false;
  }

  return true;
}

void Preprocessor::dump(std::string data) noexcept {
  if (is_debug()) {
    data += '\n';
    _file.write(data.c_str(), data.size());
    _file.flush();
  }
}

// preprocess variants

std::string Preprocessor::preprocess_email(const std::string& s) const {
  static const auto re = R"re((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)re";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_username(const std::string& s) const {
  static const auto re = R"re(@(\w+))re";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_links(const std::string& s) const {
  static const auto re = "https?://[^ ]+";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_emojis_deletion(const std::string& s) const {
  static const auto re = "["
    "🍕🐵😑😢🐶️😜😎👊😁😍💖💵👎😀😂🔥😄🏻💥😋👏😱🚌ᴵ͞🌟😊😳😧🙀😐😕👍😮😃😘💩💯⛽🚄🏼ஜ😖🚲😟😈💪🙏🎯🌹😇💔😡"
    "👌🙄😠😉😤⛺🙂👮💙😏🍾🎉😞😅😭👻😥😔😓🏽🎆🍻🍽🎶🌺🤔😪🐰🐇🐱🙆😨🙃💕💗💚🐾🐕😆🔗🚽🙈😴🏿🤗🇺🇸⤵🏆🎃😩🌠🐟💫"
    "💰💎🖐🙅⛲🍰🤐👆🙌💛🙁👀🙊🙉🚬🤓😵😒͝🆕👅👥👄🔄🔤👉👤👶👲🔛🎓😣⏺😌🤑🌏😯😲💞🚓🔔📚🏀👐💤🍇🏡❔⁉👠》🇹🇼🌸"
    "🌞🎲😛💋💀🎄💜🤢َِ🗑💃📣👿😰🤣🐝ツ🎅🍺🎵🌎͟🤡🤥😬🤧🚀🤴😝💨🏈😺🌍⏏ệ🍔🐮🍁🍆🍑🌮🌯🤦🍀😫🤤🕺🍸🥂🗽🎇🎊🆘"
    "🤠👩🖒🚪⚭⚆⬭⬯⏖✀╌🇫🇷🇩🇪🇮🇬🇧😷🇨🇦🌐📺🐋💘💓💐🌋🌄🌅👺🐷🚶🤘ͦ💸👂👃🎫🚢🚂🏃👽😙🎾👹⎌🏒⛸🏄🐀🚑🤷🤙"
    "🐒🐈🦄🚗🐳👇👋🦊🐽🎻🎹⛓🏹🍷🦆♾🎸🤕🤒⛑🎁🏝🦁🙋😶🔫👁凸ὰ💲🗯👑🚿💡😦🏐🇰🇵👾ᐣ🐄🎈🔨🐎🤞🐸💟🎰🌝🛳🍭👣っ🏉"
    "ф💭🎥Ξ🐴👨🤳🦍🍩😗𝟐🏂👳🍗🐲🍒🐑⏰💊「」🍊⤏🇳🔹🤚🍎𝑷🐂💅💢🇱♲𝝈↴💒⊘Ȼ🚴🖕🖤🥘📍👈➕🚫🎨🌑🐻🤖🎎😼🕷🇴🇭🇻🇲"
    "👼📉🍟🍦🌈🔭《🐊🐍🐦🐡💳🙇🥜🔼"
  "]+";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_emojis_isolation(const std::string& s) const {
  static const auto re = "["
    "・ω+=”“^–>°<~•≠™ˈʊɒ∞§{}·τα❤☺ɡ|¢→̶`❥━┣┫┗Ｏ►★©―ɪ✔®\x96\x92●£♥➤´¹☕≈÷♡◐║▬′ɔː€۩۞†μ✒➥═☆ˌ◄½ʻπδηλσερνʃ✬"
    "ＳＵＰＥＲＩＴ☻±♍µº¾✓◾؟．⬅℅»Вав❣⋅¿¬♫ＣＭβ█▓▒░⇒⭐›¡₂₃❧▰▔◞▀▂▃▄▅▆▇↙γ̄″☹➡«φ⅓„✋：¥̲̅́∙‛◇✏▷❓❗¶˚˙）сиʿ✨。ɑ"
    "\x80◕！％¯−ﬂﬁ₁²ʌ¼⁴⁄₄⌠♭✘╪▶☭✭♪☔☠♂☃☎✈✌✰❆☙○‣⚓年∎ℒ▪▙☏⅛ｃａｓǀ℮¸ｗ‚∼‖ℳ❄←☼⋆ʒ⊂、⅔¨͡๏⚾⚽Φ×θ￦？（℃⏩☮⚠月✊"
    "❌⭕▸■⇌☐☑⚡☄ǫ╭∩╮，例＞ʕɐ̣Δ₀✞┈╱╲▏▕┃╰▊▋╯┳┊≥☒↑☝ɹ✅☛♩☞ＡＪＢ◔◡↓♀⬆̱ℏ\x91⠀ˤ╚↺⇤∏✾◦♬³の｜／∵∴√Ω¤☜▲↳▫‿⬇✧ｏｖｍ－"
    "２０８＇‰≤∕ˆ⚜☁"
  "]+";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_emojis(std::string s) const {
  s = preprocess_emojis_deletion(s);
  // s = preprocess_emojis_isolation(s);
  return s;
}

std::string Preprocessor::preprocess_whitespace(const std::string& s) const {
  static const auto re1 = "[\t|\n|\r|\a|\b|\f|\v]+";
  static const auto re2 = "^ +| +$|( ) +";
  static const std::regex r1{re1, std::regex::optimize};
  static const std::regex r2{re2, std::regex::optimize};
  return std::regex_replace(std::regex_replace(s, r1, " "), r2, "$1");
}
