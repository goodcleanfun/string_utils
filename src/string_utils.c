#include <stdio.h>
#include "string_utils.h"
#include "strndup/strndup.h"

#define INVALID_INDEX(i, n) ((i) < 0 || (i) >= (n))

int string_compare_case_insensitive(const char *str1, const char *str2) {
    int c1, c2;

    do {
        c1 =  tolower(*str1++);
        c2 = tolower(*str2++);
    } while (c1 == c2 && c1 != 0);

    return c1 - c2;
}

int string_compare_len_case_insensitive(const char *str1, const char *str2, size_t len) {
    register unsigned char *s1 = (unsigned char *) str1;
    register unsigned char *s2 = (unsigned char *) str2;

    unsigned char c1, c2;

    if (len == 0) return 0;

    do {
        c1 = *s1++;
        c2 = *s2++;
        if (!c1 || !c2)
            break;
        if (c1 == c2)
            continue;
        c1 = tolower(c1);
        c2 = tolower(c2);
        if (c1 != c2)
            break;
    } while (--len);

    return (int)c1 - (int)c2;

}

inline size_t string_common_prefix(const char *str1, const char *str2) {
    size_t common_prefix;
    for (common_prefix = 0; *str1 && *str2 && *str1 == *str2; str1++, str2++)
        common_prefix++;
    return common_prefix;
}

inline size_t string_common_suffix(const char *str1, const char *str2) {
    size_t common_suffix = 0;
    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);
    size_t min_len = (str1_len < str2_len) ? str1_len : str2_len;
    for (int i=1; i <= min_len && str1[str1_len-i] == str2[str2_len-i]; i++)
        common_suffix++;
    return common_suffix;
}

inline bool string_starts_with(const char *str, const char *start) {
    for (; *start; str++, start++)
        if (*str != *start)
            return false;
    return true;
}

inline bool string_ends_with(const char *str, const char *ending) {
    size_t end_len = strlen(ending);
    size_t str_len = strlen(str);

    return str_len < end_len ? false : !strcmp(str + str_len - end_len, ending);
}

inline bool string_equals(const char *s1, const char *s2) {
    if (s1 == NULL || s2  == NULL) return false;
    return strcmp(s1, s2) == 0;
}

inline void string_upper(char *s) {
    for (; *s; ++s) *s = toupper(*s);
}

inline char *string_replace_char(char *str, char c1, char c2) {
    char *repl = strdup(str);
    if (repl == NULL) return NULL;
    char *ptr = repl;
    for (; *ptr; ++ptr) {
        if (*ptr == c1) *ptr = c2;
    }
    return repl;
}

bool string_replace_with_array(char *str, char *replace, char *with, char_array *result) {
    if (str == NULL) return false;
    if (result == NULL) return false;

    if (replace == NULL) {
        replace = "";
    }

    size_t len_replace = strlen(replace);
    if (len_replace == 0) {
        return true;        
    }

    if (with == NULL) {
        with = "";
    }

    size_t len_with = strlen(with);

    char *temp;
    char *start = str;

    for (size_t count = 0; (temp = strstr(start, replace)); count++) {
        char_array_cat_len(result, start, temp - start);
        char_array_cat_len(result, with, len_with);
        start = temp + len_replace;
    }

    char_array_cat(result, start);

    return true;
}

char *string_replace(char *str, char *replace, char *with) {
    if (str == NULL) return NULL;

    char_array *array = char_array_new_size(strlen(str));
    if (!string_replace_with_array(str, replace, with, array)) {
        char_array_destroy(array);
        return NULL;
    }
    return char_array_to_string(array);
}


inline bool string_is_upper(char *s) {
    for (; *s; ++s) {
        if (*s != toupper(*s)) return false;
    }
    return true;
}

inline void string_lower(char *s) {
    for (; *s; ++s) *s = tolower(*s);
}

inline bool string_is_lower(char *s) {
    for (; *s; ++s) {
        if (*s != tolower(*s)) return false;
    }
    return true;
}


inline bool string_contains(char *str, char *sub) {
    return str != NULL && sub != NULL && strstr(str, sub) != NULL;
}

uint32_t string_translate(char *str, size_t len, char *word_chars, char *word_repls, size_t trans_len) {
    uint32_t num_replacements = 0;
    
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < trans_len; j++) {
            if (str[i] == word_chars[j]) {
                str[i] = word_repls[j];
                num_replacements++;
                break;
            }
        }
    }
    return num_replacements;
}


bool string_is_digit(char *str, size_t len) {
    uint8_t *ptr = (uint8_t *)str;
    size_t idx = 0;

    bool ignorable = true;

    while (idx < len) {
        int32_t ch;
        ssize_t char_len = utf8proc_iterate(ptr, len, &ch);

        if (char_len <= 0) break;
        if (ch == 0) break;
        if (!(utf8proc_codepoint_valid(ch))) return false;

        int cat = utf8proc_category(ch);
        if (cat != UTF8PROC_CATEGORY_ND) {
            return false;
        }

        ptr += char_len;
        idx += char_len;
    }

    return true;
}



bool string_is_ignorable(char *str, size_t len) {
    uint8_t *ptr = (uint8_t *)str;
    size_t idx = 0;

    bool ignorable = true;

    while (idx < len) {
        int32_t ch;
        ssize_t char_len = utf8proc_iterate(ptr, len, &ch);

        if (char_len <= 0) break;
        if (ch == 0) break;
        if (!(utf8proc_codepoint_valid(ch))) return false;

        int cat = utf8proc_category(ch);
        if (!utf8_is_separator(cat) && !utf8_is_hyphen(ch)) {
            return false;
        }

        ptr += char_len;
        idx += char_len;
    }

    return true;
}


ssize_t string_next_hyphen_index(char *str, size_t len) {
    uint8_t *ptr = (uint8_t *)str;
    int32_t codepoint;
    ssize_t idx = 0;

    while (idx < len) {
        ssize_t char_len = utf8proc_iterate(ptr, len, &codepoint);
        
        if (char_len <= 0 || codepoint == 0) break;

        if (utf8_is_hyphen(codepoint)) return idx;
        ptr += char_len;
        idx += char_len;
    }
    return -1;
}


inline bool string_contains_hyphen_len(char *str, size_t len) {
    return string_next_hyphen_index(str, len) >= 0;
}

inline bool string_contains_hyphen(char *str) {
    return string_next_hyphen_index(str, strlen(str)) >= 0;
}

ssize_t string_next_codepoint_len(char *str, uint32_t codepoint, size_t len) {
    uint8_t *ptr = (uint8_t *)str;
    int32_t ch;
    ssize_t idx = 0;

    while (idx < len) {
        ssize_t char_len = utf8proc_iterate(ptr, len, &ch);

        if (char_len <= 0 || ch == 0) break;

        if ((uint32_t)ch == codepoint) return idx;
        ptr += char_len;
        idx += char_len;
    }
    return -1;
}

ssize_t string_next_codepoint(char *str, uint32_t codepoint) {
    return string_next_codepoint_len(str, codepoint, strlen(str));
}

ssize_t string_next_period_len(char *str, size_t len) {
    return string_next_codepoint_len(str, PERIOD_CODEPOINT, len);
}

ssize_t string_next_period(char *str) {
    return string_next_codepoint(str, PERIOD_CODEPOINT);
}

inline bool string_contains_period_len(char *str, size_t len) {
    return string_next_codepoint_len(str, PERIOD_CODEPOINT, len) >= 0;
}

inline bool string_contains_period(char *str) {
    return string_next_codepoint(str, string_next_codepoint(str, PERIOD_CODEPOINT)) >= 0;
}

ssize_t string_next_whitespace_len(char *str, size_t len) {
    uint8_t *ptr = (uint8_t *)str;
    int32_t ch;
    ssize_t idx = 0;

    while (idx < len) {
        ssize_t char_len = utf8proc_iterate(ptr, len, &ch);

        if (char_len <= 0 || ch == 0) break;

        if (utf8_is_whitespace(ch)) return idx;
        ptr += char_len;
        idx += char_len;
    }
    return -1;
}

ssize_t string_next_whitespace(char *str) {
    return string_next_whitespace_len(str, strlen(str));
}


size_t string_right_spaces_len(char *str, size_t len) {
    size_t spaces = 0;

    uint8_t *ptr = (uint8_t *)str;
    int32_t ch = 0;
    ssize_t index = len;

    while (1) {
        ssize_t char_len = utf8proc_iterate_reversed(ptr, index, &ch);

        if (ch <= 0) break;

        if (!utf8_is_whitespace(ch)) {
            break;
        }

        index -= char_len;
        spaces += char_len;
    }

    return spaces;

}

inline size_t string_hyphen_prefix_len(char *str, size_t len) {
    // Strip beginning hyphens
    int32_t unichr;
    uint8_t *ptr = (uint8_t *)str;
    ssize_t char_len = utf8proc_iterate(ptr, len, &unichr);
    if (utf8_is_hyphen(unichr)) {
        return (size_t)char_len;
    }
    return 0;
}

inline size_t string_hyphen_suffix_len(char *str, size_t len) {
    // Strip ending hyphens
    int32_t unichr;
    uint8_t *ptr = (uint8_t *)str;
    ssize_t char_len = utf8proc_iterate_reversed(ptr, len, &unichr);
    if (utf8_is_hyphen(unichr)) {
        return (size_t)char_len;
    }
    return 0;
}

size_t string_left_spaces_len(char *str, size_t len) {
    size_t spaces = 0;

    uint8_t *ptr = (uint8_t *)str;
    int32_t ch = 0;
    ssize_t index = 0;

    while (1) {
        ssize_t char_len = utf8proc_iterate(ptr, len, &ch);

        if (ch <= 0) break;

        if (!utf8_is_whitespace(ch)) {
            break;
        }
        index += char_len;
        ptr += char_len;
        spaces += char_len;
    }

    return spaces;
}

char *string_trim(char *str) {
    size_t len = strlen(str);
    size_t left_spaces = string_left_spaces_len(str, len);
    size_t right_spaces = string_right_spaces_len(str, len);
    char *ret = strndup(str + left_spaces, len - left_spaces - right_spaces);
    return ret;
}
