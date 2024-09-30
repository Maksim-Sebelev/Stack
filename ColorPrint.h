#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

#define ANSI_COLOR_WHITE   "\e[0;37m"
#define ANSI_COLOR_BLUE    "\e[0;34m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_VIOLET  "\e[0;35m"
#define ANSI_COLOR_CYAN    "\e[0;36m"
#define ANSI_COLOR_YELLOW  "\e[0;33m"

#define WHITE  ANSI_COLOR_WHITE
#define BLUE   ANSI_COLOR_BLUE
#define RED    ANSI_COLOR_RED
#define GREEN  ANSI_COLOR_GREEN
#define RESET  ANSI_COLOR_RESET
#define VIOLET ANSI_COLOR_VIOLET
#define CYAN   ANSI_COLOR_CYAN
#define YELLOW ANSI_COLOR_YELLOW

#define COLOR_PRINT(COLOR, ...) do  \
{                                    \
    printf(COLOR);                    \
    printf(__VA_ARGS__);               \
    printf(RESET);                        \
} while (0);                             \


#endif
