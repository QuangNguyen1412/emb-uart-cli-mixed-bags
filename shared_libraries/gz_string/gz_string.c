/**
 * @file gz_string.c
 * @author Alex Stout <astout@goalzero.com>
 * @brief This module offers various string utility functions.
 * NOTE: It's not a portable library (yet) because there are some mongoose os
 * dependencies.
 * @version 0.1
 * @date 2022-07-09
 * 
 * @copyright Copyright (c) 2022
 */

#include "gz_string.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "gz_memory.h"

// TODO: add safer n len terminated alternatives to each of the functions below

bool strnisset(const char *s, size_t maxlen) {
  if (s) {
    uint32_t len = strnlen(s, maxlen);
    if (len <= maxlen && len > 0) {
      return true;
    }
  }
   
  return false;
}

bool strisset(const char *s) {
  return (s && strlen(s) > 0);
}

int strend(const char *s, const char *t) {
  size_t ls = strlen(s); // find length of s
  size_t lt = strlen(t); // find length of t
  if (ls >= lt) { // check if t can fit in s
    // point s to where t should start and compare the strings from there
    return(0 == memcmp(t, s +(ls - lt), lt));
  }
  return 0; // t was longer than s
}

int strstart(const char *s, const char *t) {
  size_t ls = strlen(s); // find length of s
  size_t lt = strlen(t); // find length of t
  if (ls >= lt) { // check if t can fit in s
    // point s to where t should start and compare the strings from there
    return(0 == memcmp(t, s, lt));
  }
  return 0; // t was longer than s
}

char * strcasestr(const char * str1, const char * str2){
  char *cp = (char *) str1;
  char *s1, *s2;

  if ( !*str2 )
    return((char *)str1);

  while (*cp){
    s1 = cp;
    s2 = (char *) str2;

    while ( *s1 && *s2 && (IS_ALPHA(*s1) && IS_ALPHA(*s2))?!(TO_UPPER(*s1) - TO_UPPER(*s2)):!(*s1-*s2)) {
      ++s1, ++s2;
    }

    if (!*s2) {
      return(cp);
    }

    ++cp;
  }
  return(NULL);
}

const char * prstr(const char *s, const char *alt) {
  if (strisset(s)) {
    return s;
  }
  return alt;
}

const char * prnstr(const char *s, size_t slen, const char *alt) {
  if (strnisset(s, slen)) {
    return s;
  }
  return alt;
}

int jsonify(char *buf, size_t len) {
  if (buf == NULL) {
    return -1;
  }
  if (buf[0] == '{' || buf[0] == '[') {
    return len;
  }  
  char *temp = calloc(len, sizeof(char));
  if (temp == NULL) {
    return -1;
  }
  int n = snprintf(temp, len, "{%s}", buf);
  if (n < 0 || n >= len) { // snprintf docs: "only when this returned value is non-negative and less than n, the string has been completely written."
    free(temp);
    return -1;
  }
  memset(buf, 0, len);
  snprintf(buf, len, "%s", temp);
  free(temp);
  return strnlen(buf, len);
}

int gz_string_center_justify_text(char* buff, int buffSize, const char* text) {
  int leftPad, rightPad;
  
  if (buffSize <= strnlen(text, buffSize)) {
      return 0;
  }
  
  leftPad = rightPad = (buffSize - strnlen(text, buffSize)) / 2;
  if ((strnlen(text, buffSize) + buffSize) % 2 == 0) {
      leftPad--;
  }

  return snprintf(buff, buffSize, "%*s%s%*s", leftPad, "", text, rightPad, "");
}

int gz_string_left_justify_text(char* buff, int buffSize, const char* text) {
  int padding;
  
  if (buffSize <= strnlen(text, buffSize)) {
      return 0;
  }
  
  padding = buffSize - strnlen(text, buffSize);
  
  return snprintf(buff, buffSize, "%s%*s", text, padding, "");
}

bool gz_string_is_char_hex(char c) {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

void gz_string_to_lower(char *str, uint16_t strSize) {
  for (int i = 0; i < strSize; i++) {
    if (str[i] >= 'A' && str[i] <= 'Z') {
      str[i] = str[i] + 32;
    } else if (str[i] == '\0') {
      break;
    }
  }
}

void gz_string_to_upper(char *str, uint16_t strSize) {
  for (int i = 0; i < strSize; i++) {
    if (str[i] >= 'a' && str[i] <= 'z') {
      str[i] = str[i] - 32;
    } else if (str[i] == '\0') {
      break;
    }
  }
}

void gz_string_remove_trailing_whitespace(char* str, size_t maxlen) {
  if (!strnisset(str, maxlen)) {
    return;
  }

  int len = strnlen(str, maxlen);
  if (len == 0) {
    return; // Empty string
  }

  char *endPtr = str + len - 1;

  while (endPtr >= str && isspace(*endPtr)) {
    *endPtr = '\0';
    endPtr--;
  }
}
