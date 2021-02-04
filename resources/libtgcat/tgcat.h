#ifndef TGCAT_H
#define TGCAT_H

/**
 * Library for determining topic and main languange of Telegram channels by
 * their recent content.
 */
 
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#  ifdef tgcat_EXPORTS
#    define TGCAT_EXPORT __declspec(dllexport)
#  else
#    define TGCAT_EXPORT __declspec(dllimport)
#  endif
#else
#  define TGCAT_EXPORT __attribute__((visibility("default")))
#endif

/**
 * Initializes the library. Must be called once before any other request.
 * \return 0 on success and a negative value on fail.
 */
TGCAT_EXPORT int tgcat_init();

/**
 * Information about a Telegram channel.
 */
struct TelegramChannelInfo {
  /**
   * Title of the channel. A null-terminated string in UTF-8 encoding.
   */
  const char *title;

  /**
   * Description of the channel. A null-terminated string in UTF-8 encoding.
   */
  const char *description;

  /**
   * Number of available channel posts.
   */
  size_t post_count;

  /**
   * List of post_count channel posts. Posts are null-terminated strings in UTF-8 encoding.
   */
  const char **posts;
};

/**
 * Detects main language of channel posts.
 * \param[in] channel_info Information about the channel.
 * \param[out] language_code Array to be filled with null-terminated ISO 639-1 language code
 *                           of the channel posts, or "other" if the language doesn’t have
 *                           a two-letter code.
 * \return 0 on success and a negative value on fail.
 */
TGCAT_EXPORT int tgcat_detect_language(const struct TelegramChannelInfo *channel_info,
                                       char language_code[6]);

/**
 * List of supported categories.
 */
enum TgcatCategory {
  TGCAT_CATEGORY_ART_AND_DESIGN,
  TGCAT_CATEGORY_BETS_AND_GAMBLING,
  TGCAT_CATEGORY_BOOKS,
  TGCAT_CATEGORY_BUSINESS_AND_ENTREPRENEURSHIP,
  TGCAT_CATEGORY_CARS_AND_OTHER_VEHICLES,
  TGCAT_CATEGORY_CELEBRITIES_AND_LIFESTYLE,
  TGCAT_CATEGORY_CRYPTOCURRENCIES,
  TGCAT_CATEGORY_CULTURE_AND_EVENTS,
  TGCAT_CATEGORY_CURIOUS_FACTS,
  TGCAT_CATEGORY_DIRECTORIES_OF_CHANNELS_AND_BOTS,
  TGCAT_CATEGORY_ECONOMY_AND_FINANCE,
  TGCAT_CATEGORY_EDUCATION,
  TGCAT_CATEGORY_EROTIC_CONTENT,
  TGCAT_CATEGORY_FASHION_AND_BEAUTY,
  TGCAT_CATEGORY_FITNESS,
  TGCAT_CATEGORY_FOOD_AND_COOKING,
  TGCAT_CATEGORY_FOREIGN_LANGUAGE_LEARNING,
  TGCAT_CATEGORY_HEALTH_AND_MEDICINE,
  TGCAT_CATEGORY_HISTORY,
  TGCAT_CATEGORY_HOBBIES_AND_ACTIVITIES,
  TGCAT_CATEGORY_HOME_AND_ARCHITECTURE,
  TGCAT_CATEGORY_HUMOR_AND_MEMES,
  TGCAT_CATEGORY_INVESTMENTS,
  TGCAT_CATEGORY_JOB_LISTINGS,
  TGCAT_CATEGORY_KIDS_AND_PARENTING,
  TGCAT_CATEGORY_MARKETING_AND_PR,
  TGCAT_CATEGORY_MOTIVATION_AND_SELF_DEVELOPMENT,
  TGCAT_CATEGORY_MOVIES,
  TGCAT_CATEGORY_MUSIC,
  TGCAT_CATEGORY_OFFERS_AND_PROMOTIONS,
  TGCAT_CATEGORY_PETS,
  TGCAT_CATEGORY_POLITICS_AND_INCIDENTS,
  TGCAT_CATEGORY_PSYCHOLOGY_AND_RELATIONSHIPS,
  TGCAT_CATEGORY_REAL_ESTATE,
  TGCAT_CATEGORY_RECREATION_AND_ENTERTAINMENT,
  TGCAT_CATEGORY_RELIGION_AND_SPIRITUALITY,
  TGCAT_CATEGORY_SCIENCE,
  TGCAT_CATEGORY_SPORTS,
  TGCAT_CATEGORY_TECHNOLOGY_AND_INTERNET,
  TGCAT_CATEGORY_TRAVEL_AND_TOURISM,
  TGCAT_CATEGORY_VIDEO_GAMES,
  TGCAT_CATEGORY_OTHER
};

/**
 * Names of supported categories.
 */
const char *TGCAT_CATEGORY_NAME[] = {
  "Art & Design",
  "Bets & Gambling",
  "Books",
  "Business & Entrepreneurship",
  "Cars & Other Vehicles",
  "Celebrities & Lifestyle",
  "Cryptocurrencies",
  "Culture & Events",
  "Curious Facts",
  "Directories of Channels & Bots",
  "Economy & Finance",
  "Education",
  "Erotic Content",
  "Fashion & Beauty",
  "Fitness",
  "Food & Cooking",
  "Foreign Language Learning",
  "Health & Medicine",
  "History",
  "Hobbies & Activities",
  "Home & Architecture",
  "Humor & Memes",
  "Investments",
  "Job Listings",
  "Kids & Parenting",
  "Marketing & PR",
  "Motivation & Self-development",
  "Movies",
  "Music",
  "Offers & Promotions",
  "Pets",
  "Politics & Incidents",
  "Psychology & Relationships",
  "Real Estate",
  "Recreation & Entertainment",
  "Religion & Spirituality",
  "Science",
  "Sports",
  "Technology & Internet",
  "Travel & Tourism",
  "Video Games",
  "Other"
};

/**
 * Detects main topic of a channel.
 * \param[in] channel_info Information about the channel.
 * \param[out] category_probability Array to be filled with probabilities that
 *                                  channel belongs to a corresponding category.
 * \return 0 on success and a negative value on fail.
 */
TGCAT_EXPORT int tgcat_detect_category(const struct TelegramChannelInfo *channel_info,
                                       double category_probability[TGCAT_CATEGORY_OTHER + 1]);

#ifdef __cplusplus
}
#endif

#endif
