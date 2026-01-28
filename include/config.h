// Plik konfiguracyjny - przechowuje parametry programu oraz obsluguje debugowanie

#ifndef CONFIG_H
#define CONFIG_H

// Nazwa i wersja
#define APP_NAME "projekt_grupowy"
#define APP_VERSION "0.3"

// UART
#define UART_DEVICE "/dev/ttyAMA0" // urzadzenie UART
#define UART_BAUDRATE 9600          // predkosc transmisji

// Debugowanie [1 - DEBUG ON / 0 - DEBUG OFF]
#define DEBUG 1

#if DEBUG
    #include <stdio.h>
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

// Sterownaie silnikami
#define GPIO_CHIP_NAME "gpiochip0"
#define STEP_FREQ_HZ 400

#define PIN_DIR_X 17
#define PIN_STEP_X  27

#define PIN_DIR_Y1 23
#define PIN_STEP_Y1 24

#define PIN_DIR_Y2 5
#define PIN_STEP_Y2 6

#endif
