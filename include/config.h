// Plik konfiguracyjny - przechowuje parametry programu oraz obsluguje debugowanie

#ifndef CONFIG_H
#define CONFIG_H

// Nazwa i wersja
#define APP_NAME "projekt_grupowy"
#define APP_VERSION "0.1.0"

// UART
#define UART_DEVICE "/dev/serial0" // urzadzenie UART
#define UART_BAUDRATE 9600         // predkosc transmisji

// Debugowanie [1 - DEBUG ON / 0 - DEBUG OFF]
#define DEBUG 1

#if DEBUG
    #include <stdio.h>
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

#endif
