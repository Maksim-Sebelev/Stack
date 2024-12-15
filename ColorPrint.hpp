#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

#include <stdio.h>

#define WHITE   "\e[0;37m"
#define BLUE    "\e[0;34m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define VIOLET  "\e[0;35m"
#define CYAN    "\e[0;36m"
#define YELLOW  "\e[0;33m"
#define RESET   "\x1b[0m"

#define COLOR_PRINT(COLOR, ...) do  \
{                                    \
    printf(COLOR);                    \
    printf(__VA_ARGS__);               \
    printf(RESET);                      \
} while (0)                              \

#endif
