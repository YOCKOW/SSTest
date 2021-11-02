/* *************************************************************************************************
 SSTestTime.c
  © 2021 YOCKOW.
    Licensed under MIT License.
    See "LICENSE.txt" for more information.
 **************************************************************************************************/

// Because built-in `time` and `date` are inconvenience, this exists.
// To be honest, I want to use Swift. 🥺
// See https://git.io/JiLoF

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __MACH__
#include <mach/mach.h>
#include <mach/clock.h>
#include <mach/mach_time.h>
#endif

#ifdef __linux__
#include <time.h>
#endif

typedef struct {
  int64_t seconds;
  int32_t nanoseconds;
} y_timespec_t;

typedef enum {
  monotonicSSTTimeMode,
  calendarSSTTimeMode,
} SSTTimeMode;

typedef enum  {
  monotonicTimeSSTOutputMode = monotonicSSTTimeMode,
  calendarTimeSSTOutputMode = calendarSSTTimeMode,
  differenceOutputMode,
  helpSSTOutputMode,
} SSTOutputMode;

int y_get_time(SSTTimeMode mode, y_timespec_t *nanoseconds) {
  int result = 0;

  #ifdef __MACH__
  mach_timespec_t ts = {0, 0};
  clock_serv_t clock_name = 0;
  result = (int)host_get_clock_service(
    mach_host_self(),
    (mode == monotonicSSTTimeMode) ? SYSTEM_CLOCK : CALENDAR_CLOCK,
    &clock_name
  );
  if (result != 0) return 1;
  result = (int)clock_get_time(clock_name, &ts);
  if (result != 0) return 1;
  result = (int)mach_port_deallocate(mach_task_self(), clock_name);
  if (result != 0) return 1;
  #endif

  #ifdef __linux__
  struct timespec ts = {0, 0};
  result = (int)clock_gettime(
    (mode == monotonicSSTTimeMode) ? CLOCK_MONOTONIC : CLOCK_REALTIME,
    &ts
  );
  if (result != 0) return 1;
  #endif

  nanoseconds->seconds = (int64_t)ts.tv_sec;
  nanoseconds->nanoseconds = (int32_t)ts.tv_nsec;

  return result;
}

void y_print_time(y_timespec_t * const time) {
  printf("%" PRId64 ".%09" PRId32 "\n", time->seconds, time->nanoseconds);
}

int y_parse_time(const char * string, y_timespec_t *result) {
  // Including validation.

  int64_t seconds = 0;
  int32_t nanoseconds = 0;

  #define isNumber(character) ((character) >= 0x30 && (character) < 0x40)

  char * p = (char *)string;
  while (true) {
    char cc = *p;
    if (cc == '.' || cc == 0x00) break;
    if (!isNumber(cc)) return 1;

    int64_t value = (int64_t)(cc - 0x30);
    // guard: seconds * 10 + value <= INT64_MAX
    if (seconds > (INT64_MAX - value) / 10) return 1;
    seconds = seconds * 10 + value;
    p++;
  }
  if (*p != '.') return 1;
  p++;

  while (true) {
    char cc = *p;
    if (cc == 0x00) break;
    if (!isNumber(cc)) return 1;

    int64_t value = (int32_t)(cc - 0x30);
    // guard: nanoseconds * 10 + value < 1000000000
    if (nanoseconds > (1000000000 - value) / 10) return 1;
    nanoseconds = nanoseconds * 10 + value;
    p++;
  }
  if (*p != 0x00) return 1;

  #undef isNumber

  result->seconds = seconds;
  result->nanoseconds = nanoseconds;

  return 0;
}

void y_time_difference(y_timespec_t * const lhs, y_timespec_t * const rhs, y_timespec_t *result) {
  int64_t sec_diff = lhs->seconds - rhs->seconds;
  int32_t ns_diff = lhs->nanoseconds - rhs->nanoseconds;
  if (ns_diff < 0) {
    sec_diff--;
    ns_diff += 1000000000;
  }
  result->seconds = sec_diff;
  result->nanoseconds = ns_diff;
}

int main(int argc, char *argv[]) {
  int result = 0;

  SSTOutputMode outputMode = monotonicTimeSSTOutputMode;

  if (argc > 1) {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      outputMode = helpSSTOutputMode;
    } else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--calendar") == 0) {
      outputMode = calendarTimeSSTOutputMode;
    } else if(strcmp(argv[1], "--since") == 0) {
      if (argc < 3) {
        fprintf(stderr, "Missing past time.\n");
        return 1;
      }
      outputMode = differenceOutputMode;
    } else {
      outputMode = helpSSTOutputMode;
    }
  }

  switch (outputMode) {
  case monotonicTimeSSTOutputMode:
  case calendarTimeSSTOutputMode:
    {
      y_timespec_t time = {0, 0};
      result = y_get_time((SSTTimeMode)outputMode, &time);
      if (result != 0) return 1;
      y_print_time(&time);
    };
    break;
  case differenceOutputMode:
    {
      y_timespec_t time = {0, 0};
      result = y_get_time(monotonicSSTTimeMode, &time);
      if (result != 0) return 1;

      y_timespec_t past = {0, 0};
      result = y_parse_time(argv[2], &past);
      if (result != 0) {
        fprintf(stderr, "Invalid time format.\n");
        return 1;
      }

      y_timespec_t diff = {0, 0};
      y_time_difference(&time, &past, &diff);

      y_print_time(&diff);
    };
    break;
  case helpSSTOutputMode:
    printf(
      "OVERVIEW: Time-related utility for internal use of SSTest.\n"
      "\n"
      "USAGE: %s [options]\n"
      "\n"
      "OPTIONS:\n"
      "  --help     View this message.\n"
      "  --calendar View \"UNIX Time\".\n"
      "  --iso8601  View the time of now with ISO8601 Foramat.\n"
      "  --since <past time (monotonic)>\n"
      "             View the difference between now and the given time.\n"
      , basename(argv[0])
    );
  }

  return result;
}
