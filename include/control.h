// Plik naglowkowy obslugi ruchu silnikow

#ifndef CONTROL_H
#define CONTROL_H

typedef struct
{
    int x;
    int y;
    int strzal;
} kierunek_t;

int control_init(void);

void set_dir_x(int dir);
void set_move_x(int enable);

void set_dir_y(int dir);
void set_move_y(int enable);

void control_close(void);

#endif