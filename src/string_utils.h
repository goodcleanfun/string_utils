/*
string_utils.h
--------------

Utilities for manipulating strings in C.
*/
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

#include "char_array/char_array.h"

// ASCII string methods
int string_compare_case_insensitive(const char *str1, const char *str2);
int string_compare_len_case_insensitive(const char *str1, const char *str2, size_t len);
size_t string_common_prefix(const char *str1, const char *str2);
size_t string_common_suffix(const char *str1, const char *str2);

bool string_is_lower(char *s);
void string_lower(char *s);
bool string_is_upper(char *s);
void string_upper(char *s);

char *string_replace_char(char *str, char c1, char c2);
bool string_replace_with_array(char *str, char *replace, char *with, char_array *result);
char *string_replace(char *str, char *replace, char *with);

bool string_starts_with(const char *str, const char *start);
bool string_ends_with(const char *str, const char *ending);

bool string_equals(const char *s1, const char *s2);
bool string_contains(const char *str, const char *sub);

uint32_t string_translate(char *str, size_t len, char *word_chars, char *word_repls, size_t trans_len);

#endif
