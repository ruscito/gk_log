#ifndef INCLUDE_GK_LOG_H
#define INCLUDE_GK_LOG_H

#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define  TRACE(fmt, ...)     gk_log_output(TRACE,   fmt, ##__VA_ARGS__);
#define  DEBUG(fmt, ...)     gk_log_output(DEBUG,   fmt, ##__VA_ARGS__);
#define  INFO(fmt, ...)      gk_log_output(INFO,    fmt, ##__VA_ARGS__);
#define  WARNING(fmt, ...)   gk_log_output(WARNING, fmt, ##__VA_ARGS__);
#define  ERROR(fmt, ...)     gk_log_output(ERROR,   fmt, ##__VA_ARGS__);
#define  FATAL(fmt, ...)     gk_log_output(FATAL,   fmt, ##__VA_ARGS__);

#define SUPPORTS_ANSI_COLOR (printf("\033[32m") > 0)


typedef enum {
    TRACE   = 0,
    DEBUG   = 1,
    INFO    = 2,
    WARNING = 3,
    ERROR   = 4,
    FATAL   = 5
} gk_log_level;

#ifdef NDEBUG
  static log_level_t log_level = ERROR;
#else
  static gk_log_level log_level = TRACE;
#endif

static char *level_string[6] = {
  "[TRACE] ",
  "[DEBUG] ",
  "[INFO] ",
  "[WARNING]",
  "[ERROR] ",
  "[FATAL] ",
};


#ifdef _WIN32
    #define IS_MSYS2 (strcmp(getenv("MSYSTEM"), "MINGW64") == 0 || strcmp(getenv("MSYSTEM"), "MINGW32") == 0)
#else
    #define IS_MSYS2 0
#endif

static int32_t color_supported = 0;

static void console_write(const char *message, char color){
    // ref: https://stackoverflow.com/questions/33309136/change-color-in-os-x-console-output
    const char *message_color[] = {"\x1b[36m",
        "\x1b[35m",
        "\x1b[32m",
        "\x1b[33m",
        "\x1b[31m",
        "\x1b[37;41m"};
    if (color_supported) {
      printf("%s%s\x1b[0m",message_color[color], message);
    } else {
      printf("%s", message);
    }
}

static int _string_format(char* dest, const char *fmt, void *va_list) {
  char buffer[32000];
  int n = vsnprintf(buffer, 32000, fmt, va_list);
  buffer[n] = 0;
  memcpy(dest, buffer, n+1);
  return n;
}

static int string_format(char* dest, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = _string_format(dest, fmt, args);
  va_end(args);
  return n;
}


void gk_log_init();
void gk_log_output(gk_log_level level, const char *fmt, ...);
void gk_log_set(gk_log_level level);


#endif // INCLUDE_GK_LOG_H

void gk_log_output(gk_log_level level, const char *fmt, ...){
  // calculate time stamp
  if (level < log_level) {
    return;
  }
  char time_stamp[9];
  char msg[4096];

  time_t current_time;
  struct tm* local_time;

  // Get the current time
  current_time = time(NULL);

  // Convert to local time
  local_time = localtime(&current_time);

  // Extract the components of the local time
  int hours = local_time->tm_hour;
  int minutes = local_time->tm_min;
  int seconds = local_time->tm_sec;

  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, sizeof(msg),fmt, args);
  char msg_out[32000];
  // string_format(msg_out, "%s %s%s\n", time_stamp, level_string[level],msg);
  string_format(msg_out, "%02d:%02d:%02d %s%s\n", hours, minutes, seconds, level_string[level],msg);
  va_end(args);
  
  console_write(msg_out, level);
}

void gk_log_init() {
  if (isatty(fileno(stdout))) {
      char* term = getenv("TERM");
      if (term != NULL && strcmp(term, "dumb") != 0) {
          color_supported =  1;
      } else {
          color_supported =  0;
      }
  } else {
      printf("Standard output is not a terminal.\n");
  }
}
void gk_log_set(gk_log_level level) {
  if (level > 5) {
    log_level = 5; 
  } else {
    log_level = level;
  }
}
