// Plik konfiguracyjny - przechowuje parametry programu oraz obsluguje debugowanie

#ifndef CONFIG_H
#define CONFIG_H

// Nazwa i wersja
#define APP_NAME "projekt_grupowy"
#define APP_VERSION "0.3"

// UART
#define UART_DEVICE "/dev/ttyAMA0" // urzadzenie UART
#define UART_BAUDRATE B9600          // predkosc transmisji
#define BAUDRATE_SPEED_VALUE 9600    // predkosc do wyswietlania w debugu

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
#define STEP_FREQ_START 50
#define STEP_FREQ_MAX   400
#define ACCEL_HZ_PER_S  200


#define PIN_DIR_X 17
#define PIN_STEP_X  27

#define PIN_DIR_Y 5
#define PIN_STEP_Y 6

#endif
