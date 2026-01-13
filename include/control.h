// Struktura przechowujaca zmienne kierunkowe

#ifndef CONTROL_H
#define CONTROL_H

typedef struct
{
    int x;
    int y;
    int strzal;  // 0 = brak, 1 = strzelamy
} kierunek_t;

#endif