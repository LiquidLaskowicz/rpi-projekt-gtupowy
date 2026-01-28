// Obsluga portu szeregowego

#define _POSIX_C_SOURCE 199309L
#include "uart.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

// Konwersja predkosci transmisji na flage do konfiguracji portu szeregowego
static speed_t baud_to_flag(int baudrate)
{
    switch (baudrate)
    {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        default: return B9600;
    }
}

// Inicjalizacja UART (otwiera port, konfiguruje go, zwraca deskryptor pliku)

int uart_init(const char *device, int baudrate)
{
    int file_desc = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    // O_RDWR - odczyt i zapis
    // O_NOCTTY - UART nie stanie sie terminalem
    // O_NDELAY - nie blokuj przy otwieraniu

    if (file_desc < 0)
    {
        DEBUG_PRINT("UART otwarcie nie powiodlo sie: %s", strerror(errno));
        return -1;
    } // obsluga bledu systemowego

    struct termios opcje; // konfiguracja termios czyli ustawienia portu szeregowego

    tcgetattr(file_desc, &opcje);                // pobiera aktualne ustawienia UART
    cfsetispeed(&opcje, baud_to_flag(baudrate)); // ustawia predkosc wejsciowa
    cfsetospeed(&opcje, baud_to_flag(baudrate)); // ustawia predkosc wyjsciowa

    opcje.c_cflag |= (CLOCAL | CREAD);
    // CLOCAL - ignoruj sygnaly modemu
    // CREAD - wlacz odbiornik
    
    // standard 8N1
    opcje.c_cflag &= ~PARENB; // brak parzystosci
    opcje.c_cflag &= ~CSTOPB; //  bit stopu
    opcje.c_cflag &= ~CSIZE;
    opcje.c_cflag |= CS8;     // 8 bitow danych

    opcje.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // wylaczenie: trybu kanonicznego, echa i sygnalow terminala
    opcje.c_iflag &= ~(IXON | IXOFF | IXANY);
    // wylacza programowa kontrole przeplywu
    opcje.c_oflag &= ~OPOST;
    // brak obrobki wyjscia

    tcsetattr(file_desc, TCSANOW, &opcje);
    // zastosowanie ustawien natychmiast

    DEBUG_PRINT("UART zainicjalizowany (%s, %d baud)", device, baudrate);
    return file_desc;
}

// Funkcja czyta jedna linie tekstu z UART znak po znaku do \n
int uart_czytaj_linie(int file_desc, char *buffor, int max_length)
{
    // licznik znakow i bufor na 1 bajt
    int i = 0;
    char c;
    
    while (i < max_length - 1) // -1 bo jeszcze znak \0 na koncu
    {
        int n = read(file_desc, &c, 1); // proba odczytu 1 znaku
        if (n > 0) // jesli cos przyszlo \n jest koncem linii, jesli nie to dopisz do bufora
        {
            if (c == '\n')
                break;
            buffor[i++] = c;

        }
        else
        {
            usleep(1000);
        } // gdy nic nie przyszlo poczekaj 1 ms
    }

    // zakonczenie stringa i zwrot dlugosci
    buffor[i] = '\0';
    return i;
}

// funkcja zamyka port uart
void uart_close(int file_desc)
{
    close(file_desc);
}