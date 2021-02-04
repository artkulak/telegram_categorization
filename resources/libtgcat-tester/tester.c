#ifdef NDEBUG
#undef NDEBUG
#endif

#include "json.h"
#include "json-builder.h"
#include "tgcat.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include "windows.h"
#endif

#define MAX_POSTS 100
#define MAX_TEST_LENGTH 1000000

static const char *posts[MAX_POSTS];

static char buf[MAX_TEST_LENGTH];

static double category_probability[TGCAT_CATEGORY_OTHER + 1];

static double get_monotonic_time() {
#if defined(_WIN32)
  LARGE_INTEGER frequency;
  LARGE_INTEGER count;
  int success = QueryPerformanceFrequency(&frequency) != 0 && QueryPerformanceCounter(&count) != 0;
  assert(success);
  return (double)count.QuadPart / (double)frequency.QuadPart;
#else
  struct timespec ts;
  int err = clock_gettime(CLOCK_MONOTONIC, &ts);
  assert(err == 0);
  return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#endif
}

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: tester <mode> <input_file> <output_file>\n");
    return 1;
  }

  // read <mode> command-line argument
  enum mode_t { LANGUAGE, CATEGORY } mode;
  if (strcmp(argv[1], "language") == 0) {
    mode = LANGUAGE;
  } else if (strcmp(argv[1], "category") == 0) {
    mode = CATEGORY;
  } else {
    fprintf(stderr, "Unsupported mode \"%s\" specified\n", argv[1]);
    return 1;
  }

  // read <input_file> command-line argument
  FILE *in = fopen(argv[2], "r");
  if (in == NULL) {
    fprintf(stderr, "Failed to open input file %s\n", argv[2]);
    return 1;
  }

  // read <output_file> command-line argument
  FILE *out = fopen(argv[3], "w");
  if (out == NULL) {
    fprintf(stderr, "Failed to open output file %s\n", argv[3]);
    return 1;
  }

  // query
  struct TelegramChannelInfo info;
  info.posts = posts;

  // json serialization options
  json_serialize_opts options;
  options.mode = json_serialize_mode_packed;
  options.opts = 0;
  options.indent_size = 0;

  // statistics
  double execution_time = 0.0;
  int query_count = 0;

  // initialize library
  if (tgcat_init() != 0) {
    fprintf(stderr, "Failed to init tgcat library\n");
    return 0;
  }

  while (!ferror(in) && !feof(in)) {
    char *fgets_result = fgets(buf, sizeof(buf), in);
    if (fgets_result == NULL) {
      break;
    }
    if (ferror(in)) {
      fprintf(stderr, "Failed to read input file %s\n", argv[2]);
      return 1;
    }
    
    size_t len = strlen(buf);
    assert(len > 0);

    if (buf[len - 1] != '\n') {
      fprintf(stderr, "Input file contains too big test string\n");
      return 1;
    }

    buf[--len] = '\0';

    if (len == 0) {
      fputs("\n", out);
      continue;
    }

    json_value *value = json_parse((json_char *)buf, len);
    if (value == NULL || value->type != json_object || value->u.object.length != 3) {
      fprintf(stderr, "Failed to parse input string as JSON object\n");
      return 1;
    }

    int field_mask = 0;
    size_t i;
    for (i = 0; i < value->u.object.length; i++) {
      const char *field_name = value->u.object.values[i].name;
      json_value *field_value = value->u.object.values[i].value;

      if (strcmp(field_name, "title") == 0) {
        field_mask |= 1;
        if (field_value->type != json_string) {
          fprintf(stderr, "Field \"title\" has wrong type\n");
          return 1;
        }
        info.title = field_value->u.string.ptr;
      }
      if (strcmp(field_name, "description") == 0) {
        field_mask |= 2;
        if (field_value->type != json_string) {
          fprintf(stderr, "Field \"description\" has wrong type\n");
          return 1;
        }
        info.description = field_value->u.string.ptr;
      }
      if (strcmp(field_name, "recent_posts") == 0) {
        field_mask |= 4;
        if (field_value->type != json_array) {
          fprintf(stderr, "Field \"recent_posts\" has wrong type\n");
          return 1;
        }
        info.post_count = field_value->u.array.length;
        size_t j;
        for (j = 0; j < info.post_count; j++) {
          json_value *array_value = field_value->u.array.values[j];
          if (array_value->type != json_string) {
            fprintf(stderr, "Recent post has wrong type\n");
            return 1;
          }
          info.posts[j] = array_value->u.string.ptr;
        }      
      }
    }
    if (field_mask != 7) {
      fprintf(stderr, "Some input fields are missing\n");
      return 1;
    }

    json_value *result = json_object_new(2);
    assert(result != NULL);

    query_count++;
    execution_time -= get_monotonic_time();

    if (mode == LANGUAGE || mode == CATEGORY) {
      char language_code[6] = {0};
      if (tgcat_detect_language(&info, language_code) == 0) {
        json_value *push_result = json_object_push(result, "lang_code",
            json_string_new(language_code));
        assert(push_result != NULL);
      }
    }

    if (mode == CATEGORY) {
      memset(category_probability, 0, sizeof(category_probability));
      if (tgcat_detect_category(&info, category_probability) == 0) {
        json_value *category = json_object_new(2);
        assert(category != NULL);

        int index;
        for (index = 0; index <= TGCAT_CATEGORY_OTHER; index++) {
          if (0.0 < category_probability[index] && category_probability[index] <= 1.0) {
            json_value *push_result = json_object_push(category, TGCAT_CATEGORY_NAME[index],
                json_double_new(category_probability[index]));
            assert(push_result != NULL);
          }
        }

        json_value *push_result = json_object_push(result, "category", category);
        assert(push_result != NULL);
      }
    }

    execution_time += get_monotonic_time();

    json_serialize_ex(buf, result, options);

    fputs(buf, out);
    fputs("\n", out);

    json_builder_free(result);

    json_value_free(value);
  }

  printf("Processed %d queries in %.6lf seconds\n", query_count, execution_time);
}
