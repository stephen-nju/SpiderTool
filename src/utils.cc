
#include "utils.h"

#include "absl/strings/string_view.h"
#include "commom.h"
#include "utf8proc.h"

namespace spider {
bool is_whitespace_char(int c) {
    const char* cat = utf8proc_category_string(c);
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        return true;
    }
    // Unicode中，每个字符都有Category的属性
    return cat[0] == 'Z' && cat[1] == 's';
}

ssize_t utf8proc_iterate_reversed(const uint8_t* str, ssize_t start, int32_t* dst) {
    ssize_t len = 0;

    const uint8_t* ptr = str + start;

    *dst = -1;

    do {
        if (ptr <= str)
            return 0;
        ptr--;
        len++;
    } while ((*ptr & 0xC0) == 0x80);

    int32_t ch = 0;

    ssize_t ret_len = utf8proc_iterate(ptr, len, &ch);
    *dst = ch;
    return ret_len;
}

absl::string_view lstrip(absl::string_view s) {
    const char* text_data = s.data();
    size_t pos = 0;
    int codepoint;
    char dst[4];
    while (pos < std::strlen(text_data)) {
        int len = utf8proc_iterate((const utf8proc_uint8_t*)text_data + pos, -1, &codepoint);
        assert(len > 0);
        if (!is_whitespace_char(codepoint)) {
            break;
        }
        pos += len;
    }
    return s.substr(pos, -1);
};

absl::string_view rstrip(absl::string_view s) {
    const char* str = s.data();
    size_t spaces = 0;
    uint8_t* ptr = (uint8_t*)str;
    int32_t ch = 0;
    ssize_t pos = std::strlen(str);

    while (pos > 0) {
        ssize_t char_len = utf8proc_iterate_reversed(ptr, pos, &ch);
        assert(char_len > 0);
        if (!is_whitespace_char(ch)) {
            break;
        }
        pos -= char_len;
    }

    return s.substr(0, pos);
};

absl::string_view strip(absl::string_view s) {
    absl::string_view s1 = lstrip(s);
    absl::string_view s2 = rstrip(s1);
    return s2;
};
}  // namespace spider
