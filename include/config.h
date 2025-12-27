#ifndef CONFIG_H
#define CONFIG_H

// ===============================
// Konfiguracja projektu
// ===============================

// Nazwa aplikacji
#define APP_NAME "projekt_grupowy"

// Wersja
#define APP_VERSION "0.1.0"

// Tryb debug
#define DEBUG 1

#if DEBUG
    #include <stdio.h>
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

#endif // CONFIG_H
