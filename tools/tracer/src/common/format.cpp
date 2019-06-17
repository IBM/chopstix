/*
#
# ----------------------------------------------------------------------------
#
# Copyright 2019 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ----------------------------------------------------------------------------
#
*/
#include "format.h"

#include "safestring.h"

namespace {
char int_chars[] = "0123456789abcdef";

// constexpr size_t print_max = 128;
// char print_buf[print_max];
// constexpr char *print_end = print_buf + print_max;
// char *print_begin = print_end;
// #define print_size (print_end - print_begin)

#define BUFSIZE 128

template <typename T> char* print_int(char *ptr, T num, T base) {
    bool set_minus = false;
    if (num < 0) {
        num * -1;
        set_minus = true;
    }
    do {
        // assert(print_size < print_max && "Buffer overflow");
        *(--ptr) = int_chars[num % base];
        num /= base;
    } while (num > 0);
    if (set_minus) *(--ptr) = '-';
    return ptr;
}
}

namespace cxtrace {
namespace fmt {
namespace impl {
int next_fmt(const char *fmt, char *&fmt_str, size_t &fmt_size) {
    char *ptr = safe_strchr(fmt, '%');
    // TODO Proper parsing
    if (ptr) {
        fmt_str = ptr;
        fmt_size = 2;
        return fmt_str[1];
    } else {
        fmt_str = nullptr;
        fmt_size = 0;
        return 0;
    }
}

// Pseudo-template for different int types
#define DEF_WRITE_INT(TYPE, UNSIGN)                                            \
    size_t write_arg(char *str, size_t size, const char *fmt_str,              \
                     size_t fmt_size, TYPE arg) {                              \
        int fmt_char = fmt_str[fmt_size - 1];                                  \
        char buf[BUFSIZE]; \
        char *ptr; \
        switch (fmt_char) {                                                    \
        case 'c':                                                              \
            assert(1 < size && "Buffer overflow");                             \
            safe_strncpy(str, (char *)&arg, 1);                                \
            return 1;                                                          \
        case 'o':                                                              \
            ptr = print_int(buf + BUFSIZE, (UNSIGN TYPE)arg, (UNSIGN TYPE)8);                       \
            safe_strncpy(str, ptr, buf + BUFSIZE - ptr);                        \
            return buf + BUFSIZE - ptr;                                                 \
        case 'd':                                                              \
            ptr = print_int(buf + BUFSIZE, arg, (TYPE)10);                                          \
            safe_strncpy(str, ptr, buf + BUFSIZE - ptr);                        \
            return buf + BUFSIZE - ptr;                                                 \
        case 'x':                                                              \
            ptr = print_int(buf + BUFSIZE, (UNSIGN TYPE)arg, (UNSIGN TYPE)16);                      \
            safe_strncpy(str, ptr, buf + BUFSIZE - ptr);                        \
            return buf + BUFSIZE - ptr;                                                 \
        case '\0':                                                             \
        case '%':                                                              \
            assert(1 < size && "Buffer overflow");                             \
            safe_strncpy(str, "%", 1);                                         \
            return 1;                                                          \
        case 'p':                                                              \
        case 's':                                                              \
        case 'g':                                                              \
        case 'e':                                                              \
        case 'f':                                                              \
        default: assert(0 && "Bad format"); break;                             \
        }                                                                      \
        return 0;                                                              \
    }

// Format integers
DEF_WRITE_INT(char, unsigned)
DEF_WRITE_INT(unsigned char, )
DEF_WRITE_INT(int, unsigned)
DEF_WRITE_INT(unsigned int, )
DEF_WRITE_INT(long, unsigned)
DEF_WRITE_INT(unsigned long, )
DEF_WRITE_INT(long long, unsigned)
DEF_WRITE_INT(unsigned long long, )

// Format strings
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 const char *arg) {
    int fmt_char = fmt_str[fmt_size - 1];
    assert(fmt_char == 's' && "Bad format");
    if (!arg) arg = "(null)";
    size_t len = safe_strlen(arg);
    assert(len < size && "Buffer overflow");
    safe_strncpy(str, arg, len);
    return len;
}

// Format pointers
size_t write_arg(char *str, size_t size, const char *fmt_str, size_t fmt_size,
                 void *arg) {
    int fmt_char = fmt_str[fmt_size - 1];
    assert(fmt_char == 'p' && "Bad format");
    char buf[BUFSIZE];
    char *ptr = print_int(buf + BUFSIZE, (unsigned long)arg, (unsigned long)16);
    // assert(print_size + 2 < size && "Buffer overflow");
    safe_strncpy(str, "0x", 2);
    safe_strncpy(str + 2, ptr, buf + BUFSIZE - ptr);
    return 2 + buf + BUFSIZE - ptr;
}

// Format without arguments
size_t write_fmt(char *str, size_t size, const char *&fmt) {
    // Find next format string %...
    char *fmt_str;
    size_t fmt_size;
    char fmt_char = next_fmt(fmt, fmt_str, fmt_size);
    if (fmt_char) {
        assert((fmt_char == '%') && "Expected format argument");
        // Write part before %..
        size_t len = fmt_str - fmt;
        assert(len < size && "Buffer overflow");
        safe_strncpy(str, fmt, len);
        fmt += len + fmt_size;
        str += len;
        size -= len;
        // Write % and call again
        assert(size > 1 && "Buffer overflow");
        str[0] = '%';
        return len + 1 + write_fmt(str + 1, size - 1, fmt);
    } else {
        // Copy fmt
        size_t len = safe_strlen(fmt);
        assert(len < size && "Buffer overflow");
        safe_strncpy(str, fmt, len);
        str[len] = '\0';
        fmt += len;
        return len;
    }
}
}
}
}
