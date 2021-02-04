#include "tgcat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tgcat_init() {
  return 0;
}

int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                          char language_code[6]) {
  // choose a text with the longest length
  const char *text = channel_info->title;
  size_t text_len = strlen(channel_info->title);
  if (strlen(channel_info->description) > text_len) {
    text = channel_info->description;
    text_len = strlen(channel_info->description);
  }
  size_t i;
  for (i = 0; i < channel_info->post_count; i++) {
    if (strlen(channel_info->posts[i]) > text_len) {
      text = channel_info->posts[i];
      text_len = strlen(channel_info->posts[i]);
    }
  }

  if (text_len != 0) {
    // guess language based on the first character
    // don't try this at home
    unsigned char c = (unsigned char)text[0];
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) {
      memcpy(language_code, "en", 3);
      return 0;
    }

    unsigned char next_c = (unsigned char)text[1];
    if ((c == 0xD0 && (next_c == 0x81 || next_c >= 0x90)) || (c == 0xD1 && next_c <= 0x91)) {
      memcpy(language_code, "ru", 3);
      return 0;
    }
  }

  memcpy(language_code, "other", 6);
  return 0;
}

int tgcat_detect_category(const struct TelegramChannelInfo *channel_info,
                          double category_probability[TGCAT_CATEGORY_OTHER + 1]) {
  (void)channel_info;
  memset(category_probability, 0, sizeof(double) * (TGCAT_CATEGORY_OTHER + 1));

  int i;
  for (i = 0; i < 10; i++) {
    category_probability[rand() % (TGCAT_CATEGORY_OTHER + 1)] += 0.1;
  }
  return 0;
}
