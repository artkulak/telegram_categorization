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
    // data = preprocess_stop_words(data);
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
    if (!isdigit(ch) && ch != '\\') {
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

std::string Preprocessor::preprocess_stop_words_en(const std::string& s) const {
  static const auto re =
    "'ll|'tis|'twas|'ve|10|39|a|a's|able|ableabout|about|above|abroad|abst|"
    "accordance|according|accordingly|across|act|actually|ad|added|adj|adopted|"
    "ae|af|affected|affecting|affects|after|afterwards|ag|again|against|ago|ah|"
    "ahead|ai|ain't|aint|al|all|allow|allows|almost|alone|along|alongside|already|"
    "also|although|always|am|amid|amidst|among|amongst|amoungst|amount|an|and|"
    "announce|another|any|anybody|anyhow|anymore|anyone|anything|anyway|anyways|"
    "anywhere|ao|apart|apparently|appear|appreciate|appropriate|approximately|aq|"
    "ar|are|area|areas|aren|aren't|arent|arise|around|arpa|as|aside|ask|asked|"
    "asking|asks|associated|at|au|auth|available|aw|away|awfully|az|b|ba|back|"
    "backed|backing|backs|backward|backwards|bb|bd|be|became|because|become|"
    "becomes|becoming|been|before|beforehand|began|begin|beginning|beginnings|"
    "begins|behind|being|beings|believe|below|beside|besides|best|better|between|"
    "beyond|bf|bg|bh|bi|big|bill|billion|biol|bj|bm|bn|bo|both|bottom|br|brief|"
    "briefly|bs|bt|but|buy|bv|bw|by|bz|c|c'mon|c's|ca|call|came|can|can't|cannot|"
    "cant|caption|case|cases|cause|causes|cc|cd|certain|certainly|cf|cg|ch|changes|"
    "ci|ck|cl|clear|clearly|click|cm|cmon|cn|co|co.|com|come|comes|computer|con|"
    "concerning|consequently|consider|considering|contain|containing|contains|copy|"
    "corresponding|could|could've|couldn|couldn't|couldnt|course|cr|cry|cs|cu|"
    "currently|cv|cx|cy|cz|d|dare|daren't|darent|date|de|dear|definitely|describe|"
    "described|despite|detail|did|didn|didn't|didnt|differ|different|differently|"
    "directly|dj|dk|dm|do|does|doesn|doesn't|doesnt|doing|don|don't|done|dont|"
    "doubtful|down|downed|downing|downs|downwards|due|during|dz|e|each|early|ec|ed|"
    "edu|ee|effect|eg|eh|eight|eighty|either|eleven|else|elsewhere|empty|end|ended|"
    "ending|ends|enough|entirely|er|es|especially|et|et-al|etc|even|evenly|ever|"
    "evermore|every|everybody|everyone|everything|everywhere|ex|exactly|example|"
    "except|f|face|faces|fact|facts|fairly|far|farther|felt|few|fewer|ff|fi|fifteen|"
    "fifth|fifty|fify|fill|find|finds|fire|first|five|fix|fj|fk|fm|fo|followed|"
    "following|follows|for|forever|former|formerly|forth|forty|forward|found|four|"
    "fr|free|from|front|full|fully|further|furthered|furthering|furthermore|furthers|"
    "fx|g|ga|gave|gb|gd|ge|general|generally|get|gets|getting|gf|gg|gh|gi|give|given|"
    "gives|giving|gl|gm|gmt|gn|go|goes|going|gone|good|goods|got|gotten|gov|gp|gq|gr|"
    "great|greater|greatest|greetings|group|grouped|grouping|groups|gs|gt|gu|gw|gy|h|"
    "had|hadn't|hadnt|half|happens|hardly|has|hasn|hasn't|hasnt|have|haven|haven't|"
    "havent|having|he|he'd|he'll|he's|hed|hell|hello|help|hence|her|here|here's|"
    "hereafter|hereby|herein|heres|hereupon|hers|herself|herse”|hes|hi|hid|high|"
    "higher|highest|him|himself|himse”|his|hither|hk|hm|hn|home|homepage|hopefully|"
    "how|how'd|how'll|how's|howbeit|however|hr|ht|htm|html|http|hu|hundred|i|i'd|i'll|"
    "i'm|i've|i.e.|id|ie|if|ignored|ii|il|ill|im|immediate|immediately|importance|"
    "important|in|inasmuch|inc|inc.|indeed|index|indicate|indicated|indicates|"
    "information|inner|inside|insofar|instead|int|interest|interested|interesting|"
    "interests|into|invention|inward|io|iq|ir|is|isn|isn't|isnt|it|it'd|it'll|it's|"
    "itd|itll|its|itself|itse”|ive|j|je|jm|jo|join|jp|just|k|ke|keep|keeps|kept|keys|"
    "kg|kh|ki|kind|km|kn|knew|know|known|knows|kp|kr|kw|ky|kz|l|la|large|largely|last|"
    "lately|later|latest|latter|latterly|lb|lc|least|length|less|lest|let|let's|lets|"
    "li|like|liked|likely|likewise|line|little|lk|ll|long|longer|longest|look|looking|"
    "looks|low|lower|lr|ls|lt|ltd|lu|lv|ly|m|ma|made|mainly|make|makes|making|man|many|"
    "may|maybe|mayn't|maynt|mc|md|me|mean|means|meantime|meanwhile|member|members|men|"
    "merely|mg|mh|microsoft|might|might've|mightn't|mightnt|mil|mill|million|mine|minus|"
    "miss|mk|ml|mm|mn|mo|more|moreover|most|mostly|move|mp|mq|mr|mrs|ms|msie|mt|mu|much|"
    "mug|must|must've|mustn't|mustnt|mv|mw|mx|my|myself|myse”|mz|n|na|name|namely|nay|nc|"
    "nd|ne|near|nearly|necessarily|necessary|need|needed|needing|needn't|neednt|needs|"
    "neither|net|netscape|never|neverf|neverless|nevertheless|new|newer|newest|next|nf|"
    "ng|ni|nine|ninety|nl|no|no-one|nobody|non|none|nonetheless|noone|nor|normally|nos|"
    "not|noted|nothing|notwithstanding|novel|now|nowhere|np|nr|nu|null|number|numbers|nz|"
    "o|obtain|obtained|obviously|of|off|often|oh|ok|okay|old|older|oldest|om|omitted|on|"
    "once|one|one's|ones|only|onto|open|opened|opening|opens|opposite|or|ord|order|"
    "ordered|ordering|orders|org|other|others|otherwise|ought|oughtn't|oughtnt|our|ours|"
    "ourselves|out|outside|over|overall|owing|own|p|pa|page|pages|part|parted|particular|"
    "particularly|parting|parts|past|pe|per|perhaps|pf|pg|ph|pk|pl|place|placed|places|"
    "please|plus|pm|pmid|pn|point|pointed|pointing|points|poorly|possible|possibly|"
    "potentially|pp|pr|predominantly|present|presented|presenting|presents|presumably|"
    "previously|primarily|probably|problem|problems|promptly|proud|provided|provides|pt|"
    "put|puts|pw|py|q|qa|que|quickly|quite|qv|r|ran|rather|rd|re|readily|really|"
    "reasonably|recent|recently|ref|refs|regarding|regardless|regards|related|relatively|"
    "research|reserved|respectively|resulted|resulting|results|right|ring|ro|room|rooms|"
    "round|ru|run|rw|s|sa|said|same|saw|say|saying|says|sb|sc|sd|se|sec|second|secondly|"
    "seconds|section|see|seeing|seem|seemed|seeming|seems|seen|sees|self|selves|sensible|"
    "sent|serious|seriously|seven|seventy|several|sg|sh|shall|shan't|shant|she|she'd|"
    "she'll|she's|shed|shell|shes|should|should've|shouldn|shouldn't|shouldnt|show|"
    "showed|showing|shown|showns|shows|si|side|sides|significant|significantly|similar|"
    "similarly|since|sincere|site|six|sixty|sj|sk|sl|slightly|sm|small|smaller|smallest|"
    "sn|so|some|somebody|someday|somehow|someone|somethan|something|sometime|sometimes|"
    "somewhat|somewhere|soon|sorry|specifically|specified|specify|specifying|sr|st|state|"
    "states|still|stop|strongly|su|sub|substantially|successfully|such|sufficiently|"
    "suggest|sup|sure|sv|sy|system|sz|t|t's|take|taken|taking|tc|td|tell|ten|tends|test|"
    "text|tf|tg|th|than|thank|thanks|thanx|that|that'll|that's|that've|thatll|thats|"
    "thatve|the|their|theirs|them|themselves|then|thence|there|there'd|there'll|there're|"
    "there's|there've|thereafter|thereby|thered|therefore|therein|therell|thereof|therere|"
    "theres|thereto|thereupon|thereve|these|they|they'd|they'll|they're|they've|theyd|"
    "theyll|theyre|theyve|thick|thin|thing|things|think|thinks|third|thirty|this|thorough|"
    "thoroughly|those|thou|though|thoughh|thought|thoughts|thousand|three|throug|through|"
    "throughout|thru|thus|til|till|tip|tis|tj|tk|tm|tn|to|today|together|too|took|top|"
    "toward|towards|tp|tr|tried|tries|trillion|truly|try|trying|ts|tt|turn|turned|turning|"
    "turns|tv|tw|twas|twelve|twenty|twice|two|tz|u|ua|ug|uk|um|un|under|underneath|undoing|"
    "unfortunately|unless|unlike|unlikely|until|unto|up|upon|ups|upwards|us|use|used|"
    "useful|usefully|usefulness|uses|using|usually|uucp|uy|uz|v|va|value|various|vc|ve|"
    "versus|very|vg|vi|via|viz|vn|vol|vols|vs|vu|w|want|wanted|wanting|wants|was|wasn|"
    "wasn't|wasnt|way|ways|we|we'd|we'll|we're|we've|web|webpage|website|wed|welcome|"
    "well|wells|went|were|weren|weren't|werent|weve|wf|what|what'd|what'll|what's|what've|"
    "whatever|whatll|whats|whatve|when|when'd|when'll|when's|whence|whenever|where|"
    "where'd|where'll|where's|whereafter|whereas|whereby|wherein|wheres|whereupon|"
    "wherever|whether|which|whichever|while|whilst|whim|whither|who|who'd|who'll|who's|"
    "whod|whoever|whole|wholl|whom|whomever|whos|whose|why|why'd|why'll|why's|widely|"
    "width|will|willing|wish|with|within|without|won|won't|wonder|wont|words|work|worked|"
    "working|works|world|would|would've|wouldn|wouldn't|wouldnt|ws|www|x|y|ye|year|years|"
    "yes|yet|you|you'd|you'll|you're|you've|youd|youll|young|younger|youngest|your|youre|"
    "yours|yourself|yourselves|youve|yt|yu|z|za|zero|zm|zr";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_stop_words_ru(const std::string& s) const {
  static const auto re =
    "а|е|и|ж|м|о|на|не|ни|об|но|он|мне|мои|мож|она|они|оно|мной|много|многочисленное|"
    "многочисленная|многочисленные|многочисленный|мною|мой|мог|могут|можно|может|можхо|"
    "мор|моя|моё|мочь|над|нее|оба|нам|нем|нами|ними|мимо|немного|одной|одного|менее|"
    "однажды|однако|меня|нему|меньше|ней|наверху|него|ниже|мало|надо|один|одиннадцать|"
    "одиннадцатый|назад|наиболее|недавно|миллионов|недалеко|между|низко|меля|нельзя|"
    "нибудь|непрерывно|наконец|никогда|никуда|нас|наш|нет|нею|неё|них|мира|наша|наше|"
    "наши|ничего|начала|нередко|несколько|обычно|опять|около|мы|ну|нх|от|отовсюду|"
    "особенно|нужно|очень|отсюда|в|во|вон|вниз|внизу|вокруг|вот|восемнадцать|"
    "восемнадцатый|восемь|восьмой|вверх|вам|вами|важное|важная|важные|важный|вдали|"
    "везде|ведь|вас|ваш|ваша|ваше|ваши|впрочем|весь|вдруг|вы|все|второй|всем|всеми|"
    "времени|время|всему|всего|всегда|всех|всею|всю|вся|всё|всюду|г|год|говорил|"
    "говорит|года|году|где|да|ее|за|из|ли|же|им|до|по|ими|под|иногда|довольно|именно|"
    "долго|позже|более|должно|пожалуйста|значит|иметь|больше|пока|ему|имя|пор|пора|"
    "потом|потому|после|почему|почти|посреди|ей|два|две|двенадцать|двенадцатый|двадцать|"
    "двадцатый|двух|его|дел|или|без|день|занят|занята|занято|заняты|действительно|давно|"
    "девятнадцать|девятнадцатый|девять|девятый|даже|алло|жизнь|далеко|близко|здесь|"
    "дальше|для|лет|зато|даром|первый|перед|затем|зачем|лишь|десять|десятый|ею|её|их|бы|"
    "еще|при|был|про|процентов|против|просто|бывает|бывь|если|люди|была|были|было|будем|"
    "будет|будете|будешь|прекрасно|буду|будь|будто|будут|ещё|пятнадцать|пятнадцатый|"
    "друго|другое|другой|другие|другая|других|есть|пять|быть|лучше|пятый|к|ком|конечно|"
    "кому|кого|когда|которой|которого|которая|которые|который|которых|кем|каждое|каждая|"
    "каждые|каждый|кажется|как|какой|какая|кто|кроме|куда|кругом|с|т|у|я|та|те|уж|со|то|"
    "том|снова|тому|совсем|того|тогда|тоже|собой|тобой|собою|тобою|сначала|только|уметь|"
    "тот|тою|хорошо|хотеть|хочешь|хоть|хотя|свое|свои|твой|своей|своего|своих|свою|твоя|"
    "твоё|раз|уже|сам|там|тем|чем|сама|сами|теми|само|рано|самом|самому|самой|самого|"
    "семнадцать|семнадцатый|самим|самими|самих|саму|семь|чему|раньше|сейчас|чего|сегодня|"
    "себе|тебе|сеаой|человек|разве|теперь|себя|тебя|седьмой|спасибо|слишком|так|такое|"
    "такой|такие|также|такая|сих|тех|чаще|четвертый|через|часто|шестой|шестнадцать|"
    "шестнадцатый|шесть|четыре|четырнадцать|четырнадцатый|сколько|сказал|сказала|сказать|"
    "ту|ты|три|эта|эти|что|это|чтоб|этом|этому|этой|этого|чтобы|этот|стал|туда|этим|"
    "этими|рядом|тринадцать|тринадцатый|этих|третий|тут|эту|суть|чуть|тысяч";
  static const std::regex r{re, std::regex::optimize};
  return std::regex_replace(s, r, "");
}

std::string Preprocessor::preprocess_stop_words(std::string s) const {
  s = preprocess_stop_words_en(s);
  s = preprocess_stop_words_ru(s);
  return s;
}

std::string Preprocessor::preprocess_whitespace(const std::string& s) const {
  static const auto re1 = "[\t|\n|\r|\a|\b|\f|\v]+";
  static const auto re2 = "^ +| +$|( ) +";
  static const std::regex r1{re1, std::regex::optimize};
  static const std::regex r2{re2, std::regex::optimize};
  return std::regex_replace(std::regex_replace(s, r1, " "), r2, "$1");
}
