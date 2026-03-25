#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

inline const char *RESET = "\x1b[0m";
inline const char *BOLD = "\x1b[1m";
inline const char *DIM = "\x1b[2m";
inline const char *BLUE = "\x1b[34m";
inline const char *YELLOW = "\x1b[33m";
inline const char *RED = "\x1b[91m";
inline const char *GREEN = "\x1b[32m";

namespace logger {
    inline void timestamp(char *buf, size_t len) {
        time_t secs = time(NULL);
        uint32_t s = secs % 60;
        uint32_t m = (secs / 60) % 60;
        uint32_t h = (secs / 3600) % 24;
        uint32_t days = (int64_t)secs / 86400 + 719468;
        uint32_t era = days / 146097;
        uint32_t doe = days - era * 146097;
        uint32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        uint32_t y = yoe + era * 400;
        uint32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        uint32_t mp = (5 * doy + 2) / 153;
        uint32_t d = doy - (153 * mp + 2) / 5 + 1;
        uint32_t mo = (mp < 10) ? (mp + 3) : (mp - 9);
        y = (mo <= 2) ? (y + 1) : y;

        snprintf(buf, len, "%04u-%02u-%02u %02u:%02u:%02u", y, mo, d, h, m, s);
    }

    inline void log_msg(const char *level, const char *fmt, va_list args) {
        // clang-format off
        const char *level_color;
        const char *msg_color;

        if      (level[0] == 'I') { level_color = BLUE;   msg_color = RESET;  }
        else if (level[0] == 'W') { level_color = YELLOW; msg_color = YELLOW; }
        else if (level[0] == 'E') { level_color = RED;    msg_color = RED;    }
        else if (level[0] == 'R') { level_color = GREEN;  msg_color = RESET;  }
        else                      { level_color = RESET;  msg_color = RESET;  }

        char ts[64];
        timestamp(ts, sizeof(ts));

        char msg[1024];
        vsnprintf(msg, sizeof(msg), fmt, args);

        fprintf(stderr, "%s[%s]%s %s%s[%s]%s: %s%s%s\n",
                DIM, ts, RESET,
                level_color, BOLD, level, RESET,
                msg_color, msg, RESET);
        // clang-format on
    }

    __attribute__((format(printf, 1, 2))) inline void info(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log_msg("INFO", fmt, args);
        va_end(args);
    }

    __attribute__((format(printf, 1, 2))) inline void warn(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log_msg("WARN", fmt, args);
        va_end(args);
    }

    __attribute__((format(printf, 1, 2))) inline void error(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log_msg("ERRO", fmt, args);
        va_end(args);
    }

    __attribute__((format(printf, 1, 2))) inline void rcmd(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log_msg("RCMD", fmt, args);
        va_end(args);
    }

} // namespace logger

#endif // LOGGER_H_
