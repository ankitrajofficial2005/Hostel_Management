#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/* Safe string input: reads up to max_len - 1 characters, strips newline, and trims trailing whitespace */
void utils_get_string(const char *prompt, char *buffer, size_t max_len, int allow_empty);

/* Safe integer input with range validation */
int utils_get_int(const char *prompt, int min_val, int max_val, int *out_val);

/* Safe floating point input with range validation */
int utils_get_double(const char *prompt, double min_val, double max_val, double *out_val);

/* Confirmation prompt returning 1 for Yes, 0 for No */
int utils_get_confirmation(const char *prompt);

/* Wait for user to press Enter */
void utils_pause(void);

/* Generates current timestamp string (e.g., "YYYY-MM-DD HH:MM:SS") */
void utils_get_timestamp(char *buffer, size_t max_len);

/* UI formatting helpers */
void utils_print_banner(const char *title);
void utils_print_separator(char ch, int length);
void utils_clear_screen(void);

/* String trimming helper */
void utils_trim(char *str);

#endif /* UTILS_H */
