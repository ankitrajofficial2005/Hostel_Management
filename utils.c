#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void utils_trim(char *str) {
    if (!str) return;

    /* Trim leading whitespace */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    /* Trim trailing whitespace */
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void utils_get_string(const char *prompt, char *buffer, size_t max_len, int allow_empty) {
    if (!buffer || max_len == 0) return;

    while (1) {
        if (prompt && prompt[0] != '\0') {
            printf("%s", prompt);
            fflush(stdout);
        }

        if (fgets(buffer, (int)max_len, stdin) == NULL) {
            /* EOF or error */
            buffer[0] = '\0';
            return;
        }

        /* Check if input was truncated (no newline found and buffer is full) */
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else if (len == max_len - 1) {
            /* Flush remainder of line */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        utils_trim(buffer);

        if (!allow_empty && buffer[0] == '\0') {
            printf("  [!] Input cannot be empty. Please try again.\n");
            continue;
        }

        break;
    }
}

int utils_get_int(const char *prompt, int min_val, int max_val, int *out_val) {
    char line[64];
    long val;
    char *endptr;

    while (1) {
        if (prompt && prompt[0] != '\0') {
            printf("%s", prompt);
            fflush(stdout);
        }

        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        /* Flush remainder if too long */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        } else if (len == sizeof(line) - 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        utils_trim(line);

        if (line[0] == '\0') {
            printf("  [!] Please enter a valid integer.\n");
            continue;
        }

        val = strtol(line, &endptr, 10);
        if (*endptr != '\0') {
            printf("  [!] Invalid number format. Please enter digits only.\n");
            continue;
        }

        if (val < min_val || val > max_val) {
            printf("  [!] Value out of allowed range (%d - %d). Please try again.\n", min_val, max_val);
            continue;
        }

        if (out_val) {
            *out_val = (int)val;
        }
        return 1;
    }
}

int utils_get_double(const char *prompt, double min_val, double max_val, double *out_val) {
    char line[64];
    double val;
    char *endptr;

    while (1) {
        if (prompt && prompt[0] != '\0') {
            printf("%s", prompt);
            fflush(stdout);
        }

        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        } else if (len == sizeof(line) - 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        utils_trim(line);

        if (line[0] == '\0') {
            printf("  [!] Please enter a valid decimal number.\n");
            continue;
        }

        val = strtod(line, &endptr);
        if (*endptr != '\0') {
            printf("  [!] Invalid number format. Please enter a valid amount.\n");
            continue;
        }

        if (val < min_val || val > max_val) {
            printf("  [!] Amount out of allowed range (%.2f - %.2f). Please try again.\n", min_val, max_val);
            continue;
        }

        if (out_val) {
            *out_val = val;
        }
        return 1;
    }
}

int utils_get_confirmation(const char *prompt) {
    char resp[16];
    while (1) {
        printf("%s (y/n): ", prompt);
        fflush(stdout);
        if (fgets(resp, sizeof(resp), stdin) == NULL) {
            return 0;
        }
        utils_trim(resp);
        if (resp[0] == 'y' || resp[0] == 'Y') {
            return 1;
        }
        if (resp[0] == 'n' || resp[0] == 'N') {
            return 0;
        }
        printf("  [!] Please answer with 'y' for Yes or 'n' for No.\n");
    }
}

void utils_pause(void) {
    printf("\nPress [Enter] to return to the menu...");
    fflush(stdout);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

void utils_get_timestamp(char *buffer, size_t max_len) {
    if (!buffer || max_len == 0) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) {
        strftime(buffer, max_len, "%Y-%m-%d %H:%M:%S", t);
    } else {
        snprintf(buffer, max_len, "Unknown");
    }
}

void utils_print_separator(char ch, int length) {
    for (int i = 0; i < length; i++) {
        putchar(ch);
    }
    putchar('\n');
}

void utils_print_banner(const char *title) {
    int len = (int)strlen(title);
    int box_len = len + 10;
    if (box_len < 40) box_len = 40;

    printf("\n");
    utils_print_separator('=', box_len);
    printf("     %s\n", title);
    utils_print_separator('=', box_len);
}

void utils_clear_screen(void) {
    /* Portable approach: print ANSI escape to clear screen, works on macOS & Linux terminals */
    printf("\033[H\033[J");
    fflush(stdout);
}
