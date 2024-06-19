#ifndef GZ_STRING_H
#define GZ_STRING_H

#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef IS_ALPHA
#define IS_ALPHA(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#endif

#ifndef TO_UPPER
#define TO_UPPER(c) ((c) & 0xDF)
#endif

/**
 * @fn int strend(const char *s, const char *t)
 * @brief Searches the end of string s for string t
 * @param s the string to be searched
 * @param t the substring to locate at the end of string s
 * @return one if the string t occurs at the end of the string s, and zero otherwise
 */
int strend(const char *s, const char *t);

/**
 * @fn int strstart(const char *s, const char *t)
 * @brief Searches the start of string s for string t
 * @param s the string to be searched
 * @param t the substring to locate at the start of string s
 * @return one if the string t occurs at the start of the string s, and zero otherwise
 */
int strstart(const char *s, const char *t);

/**
 * @brief Find the first occurrence of needle in haystack, using
 * case-insensitive comparison. (not provided by string.h in this system for some reason)
 * 
 * @param haystack 
 * @param needle 
 * @return pointer to character of match
 */
char * strcasestr(const char *haystack, const char *needle);

/**
 * @brief returns true if string is non-null and has some length. Only scans length until 
 * a null terminating character is found, or the maxlen has been reached.
 * 
 * @param s the string to be validated
 * @param maxlen the maximum length allowed to scan for the null terminating character
 * @return true if string is non-null and has some length, false otherwise
 */ 
bool strnisset(const char *s, size_t maxlen);

/**
 * @fn int strisset(const char *s)
 * @brief returns true if string is non-null and has some length
 * @param s the string to be validated
 * @return true if string is non-null and has some length
 */ 
bool strisset(const char *s);

/**
 * @brief returns s if non-empty string, "<empty>" otherwise
 * 
 * @param s 
 * @return const char* 
 */
const char * prstr(const char *s, const char *alt);

/**
 * @brief returns *s if non-empty string, *alt otherwise
 * 
 * @param s
 * @param slen 
 * @param alt 
 * @return const char* 
 */
const char * prnstr(const char *s, size_t slen, const char *alt);

/**
 * @brief Converts the given buffer to a json string, does not prettify.
 * If the given buffer is already in JSON '{' or '[', buffer will not be modified.
 * Returns new string length on success, returns same length if not modified, returns -1 on failure.
 * WARNING: modifies the provided *buf!
 * @param buf 
 * @param len 
 * @return int 
 */
int jsonify(char *, size_t);

/**
 * @brief Center justifies text within a string, padded with spaces on either side.
 * 
 * @param buff A pointer to the character buffer to hold the center justified string.
 * @param buffSize The allocated size of @ref buff. The desired size should be 1 greater than
 * the size of the field to be centered in to allow for the null terminator.
 * @param text The null terminated text to be centered within @buff.
 * @return The number of bytes filled in the buffer. Equal to buffSize if operation is successful.
 */
int gz_string_center_justify_text(char* buff, int buffSize, const char* text);

/**
 * @brief Left justifies text within a string, padded with spaces on the right side.
 * 
 * @param buff A pointer to the character buffer to hold the left justified string.
 * @param buffSize The allocated size of @ref buff. The desired size should be 1 greater than
 * the size of the field to be centered in to allow for the null terminator.
 * @param text The null terminated text to be centered within @buff.
 * @return The number of bytes filled in the buffer. Equal to buffSize if operation is successful.
 */
int gz_string_left_justify_text(char* buff, int buffSize, const char* text);

/**
 * return true if the character is a hex digit
 */ 
bool gz_string_is_char_hex(char c);

/**
 * @brief Converts a string to lower case
 * @param str the string to be converted
 * @param strSize the size of the string
 * **/
void gz_string_to_lower(char *str, uint16_t strSize);

/**
 * @brief Converts a string to upper case
 * @param str the string to be converted
 * @param strSize the size of the string
 * **/
void gz_string_to_upper(char *str, uint16_t strSize);

/**
 * @brief Removes any whitespace found after the last non-whitespace character in the string is found
 * in a null-terminated string.
 * 
 * @param str The string to be stripped of trailing whitespace.
 * @param maxlen The maximum length of the string to be scanned for whitespace.
 */
void gz_string_remove_trailing_whitespace(char* str, size_t maxlen);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GZ_STRING_H */
