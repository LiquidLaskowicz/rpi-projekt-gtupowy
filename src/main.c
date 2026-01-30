// Glowna petla programu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "control.h"
#include "uart.h"

int main(void)
{
    printf("%s v%s\n", APP_NAME, APP_VERSION);

    DEBUG_PRINT("This metal gear is pretty solid");

    int uart_file_desc; // deskryptor pliku UART
    char buffor[64]; // buffor na dane z UART  // 64 bajty to w sumie overkill int ma 2 to wystarczy dla pewnosci 16 ale zostawie na razie
    kierunek_vector kierunek = {0, 0, 0}; // struktura przechowujace kierunek i zmienną strzał

    uart_file_desc = uart_init(UART_DEVICE); // inicjalizacja UART (otwarcie urzadzenia i ustawienie predkosci transmisji)

    if (uart_file_desc < 0)
    {
        DEBUG_PRINT("Blad inicjalizacji UART");
        return 1;
    } // sprawdzenie czy UART uruchomil sie poprawnie

    if (control_init() < 0)
    {
    DEBUG_PRINT("Blad inicjalizacji GPIO");
    return 1;
    } // sprawdzenie czy uruchomil sie onterfejs GPIO

    DEBUG_PRINT("Oczekiwanie na dane z ESP");

    while(1) //glowna petla programu
    {

        int length = uart_czytaj_linie(uart_file_desc, buffor, sizeof(buffor)); // czyta jedna linie z UART, zapisuje do bufora i zwraca liczbe odczytanych znakow

        if (length <= 0) // sprawdzenie czy cos odebrano
        {
            continue;
        }

        // Usuniecie znakow nowej linii
        buffor[strcspn(buffor, "\r\n")] = 0;
    
        int x, y, strzal;
        
        if (sscanf(buffor, "%d,%d,%d", &x, &y, &strzal) == 3)
        {
            kierunek.x = x;
            kierunek.y = y;
            kierunek.strzal = strzal;

            set_dir_x(kierunek.x);
            set_dir_y(kierunek.y);

            set_move_x(kierunek.x);
            set_move_y(kierunek.y);

            DEBUG_PRINT("Kierunek: X=%d Y=%d Strzal=%d", kierunek.x, kierunek.y, kierunek.strzal);
        }
        else
        {
            DEBUG_PRINT("Nieprawidlowe dane: %s", buffor);
        }
        
    }

    uart_close(uart_file_desc); // generalnie to program dziala w petli nieskonczonej
                                // wiec UART sie nie zamknie chyba ze dodac funkcje do tego
    control_close()             // tutaj to samo moze dla picu dodac cos takiego ze jak jakis blad wyrzuci to ma byc wyjsice z glownej petli i wywolanie tych funkcji
    return 0;
}
