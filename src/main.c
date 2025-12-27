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

    DEBUG_PRINT("This metal gear is pretty solid (Program chodzi)");

    int uart_file_desc; // deskryptor pliku UART
    char buffor[64]; // buffor na dane z UART
    kierunek_t kierunek = {0, 0}; // struktura przechowujace kierunek

    int active = 0; // zmienna okreslajaca czy z portu przychodza dane (w teorii nie trzeba bo sygnaly beda caly czas ale dodaje z milosci do gry)

    uart_file_desc = uart_init(UART_DEVICE, UART_BAUDRATE); // inicjalizacja UART (otwarcie urzadzenia i ustawienie predkoscji transmisji)

    if (uart_file_desc < 0)
    {
        DEBUG_PRINT("UART inicjalizacja nieudana");
        return 1;
    } // sprawdzenie czy UART uruchomil sie poprawnie

    DEBUG_PRINT("Oczekiwanie na dane z ESP...");

    while(1) //glowna petla programu
    {
        int length = uart_czytaj_linie(uart_file_desc, buffor, sizeof(buffor)); // czyta jedna linie z UART, zapisuje do bufora i zwraca liczbe odczytanych znakow

        if (length <= 0) // sprawdzenie czy cos odebrano
        {
            continue;
        }

        // Usuniecie znakow nowej linii
        buffor[strcspn(buffor, "\r\n")] = 0;


        // TRYB OCZEKIWANIA
        if (!active)
        {
            if (strcmp(buffor, "START") == 0)
            {
                DEBUG_PRINT("Uruchomienie trybu odbioru danych");
                active = 1;
            }
            continue;
        }

        // TRYB ODBIORU DANYCH
        if (strcmp(buffor, "STOP")  == 0)
        {
            DEBUG_PRINT("Uruchomienie trybu oczekiwania");
            active = 0;
            continue;
        }
    
        int x, y; // zmienne przechowujace dane z ESP

        if (sscanf(buffor, "%d %d", &x, &y) == 2) // proba odczytu dwoch liczb calkowitych z tekstu
        {
            // przypisanie danych do strunktury kierunku
            kierunek.x = x;
            kierunek.y = y;
            
            DEBUG_PRINT("Kierunek: X=%d Y=%d", kierunek.x, kierunek.y); // wypisanie aktualnego kierunku
        }
        else
        {
            DEBUG_PRINT("Nieprawidlowe dane: %s", buffor); // blad jesli format danych jest niepoprawny
        }
        
    }

    uart_close(uart_file_desc); // generalnie to program dziala w petli nieskonczonej
                                // wiec UART sie nie zamknie chyba ze dodac funkcje do tego
    return 0;
}
