// Sterowanie silnikami

#include "control.h"
#include "config.h"

#include <gpiod.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

static inline void sleep_us(long us)
{
    struct timespec ts;
    ts.tv_sec  = us / 1000000;
    ts.tv_nsec = (us % 1000000) * 1000;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}


static struct gpiod_chip *chip; // wskaznik na strukture reprezentujaca urzadzenie GPIO w razberce

// Kierunek
static struct gpiod_line *dir_x; // line to pojedynczy pin
static struct gpiod_line *dir_y;

// Ruch
static struct gpiod_line *step_x;
static struct gpiod_line *step_y;

static volatile int move_x = 0; // volatile mowi kompilatorowi ze jak bedzie chcial sie odwolac do tej zmiennej to powinien zalozyc ze jej wartosc sie zmienila i ma ja sprawdzic
static volatile int move_y = 0;

static volatile int current_freq_x = STEP_FREQ_START;
static volatile int current_freq_y = STEP_FREQ_START;

static volatile int target_freq_x = STEP_FREQ_MAX;
static volatile int target_freq_y = STEP_FREQ_MAX;


static pthread_t step_thread; // zmienna watku
static int running = 1;

static void *praca_silnikow(void *arg) // poczatek watku obslugi ruchu // void* oznacza ze moze zwrocic cokolwiek albo NULL
{
    (void)arg; // nieuzywany parametr

    while (running)
    {
        int did_step = 0; // zalozenie ze ruchu na poczatku nie ma, jest po to, ze jesli nic sie nie dzieje to zeby petla miala delay przed kolejnym sprawdzeniem zeby nie zamulac mk (mikrokontrolera)

        // Os X
        if (move_x) // przez to ze jest volatile to tutaj sprawdzany jest od razu stan tej zmiennej
        {
            int delay_x = 1000000 / (current_freq_x * 2); // czas trwania impulsu dla sterownika czyli pol calego okresu

            gpiod_line_set_value(step_x, 1);    // ustawienie stanu wysokiego na wartosc kroku
            sleep_us(delay_x);                  // przytrzymanie tego impulsu bo tego wymaga sterownik
            gpiod_line_set_value(step_x, 0);    // ustawienie stanu niskiego
            sleep_us(delay_x);                  // znowu przytrzymanie

            int accel_step_x = ACCEL_HZ_PER_S * delay_x * 2 / 1000000; // przyspieszenie na jeden krok
            if (accel_step_x < 1) accel_step_x = 1; // blokada przed zablokowaniem przyspieszenia w 0

            if (current_freq_x < target_freq_x) // zwiekszanie czestotliwosci wysylania impulsow o ustalone przyspieszenie az do osiagniecia maksymalnej predkosci ustalonej
            {
                current_freq_x += accel_step_x;
                if (current_freq_x > target_freq_x)
                    current_freq_x = target_freq_x;
            }

            did_step = 1;
        }
        else
        {
            current_freq_x = STEP_FREQ_START; // jesli nie ma ruchu to zerowana jest predkosc skumulowana
        }

        // Os Y
        if (move_y)
        {
            int delay_y = 1000000 / (current_freq_y * 2);

            gpiod_line_set_value(step_y, 1);
            sleep_us(delay_y);
            gpiod_line_set_value(step_y, 0);
            sleep_us(delay_y);

            int accel_step_y = ACCEL_HZ_PER_S * delay_y * 2 / 1000000;
            if (accel_step_y < 1) accel_step_y = 1;

            if (current_freq_y < target_freq_y)
            {
                current_freq_y += accel_step_y;
                if (current_freq_y > target_freq_y)
                    current_freq_y = target_freq_y;
            }

            did_step = 1;
        }
        else
        {
            current_freq_y = STEP_FREQ_START;
        }

        // Nic sie nie rusza to procesor nie sprawdza caly czas tego tylko czeka chwile i nie zamula
        if (did_step == 0)
        {
            sleep_us(1000);
        }
    }

    return NULL;
}

// Inicjalizacja
int control_init(void)
{
    chip = gpiod_chip_open_by_name(GPIO_CHIP_NAME);

    dir_x   = gpiod_chip_get_line(chip, PIN_DIR_X);
    step_x  = gpiod_chip_get_line(chip, PIN_STEP_X);

    dir_y  = gpiod_chip_get_line(chip, PIN_DIR_Y);
    step_y = gpiod_chip_get_line(chip, PIN_STEP_Y);

    gpiod_line_request_output(dir_x,  "DIR_X",  0); // ustalamy te piny jako wyjscia z poczatkowa wartoscia zero i nadajemy mu nazwe
    gpiod_line_request_output(step_x, "STEP_X", 0);

    gpiod_line_request_output(dir_y,  "DIR_Y",  0);
    gpiod_line_request_output(step_y, "STEP_Y", 0);

    pthread_create(&step_thread, NULL, praca_silnikow, NULL); // tutaj jest stworzenie nowego watku pod adresem zmiennej utworzonej wyzej, ktory ma wykonywac funkcje praca_silnikow, bez atrybutow i argumentu przekazywanego do watku

    DEBUG_PRINT("GPIO + STEP uruchomione");
    return 0;
}

// Kierunek
void set_dir_x(int dir)
{
    current_freq_x = STEP_FREQ_START;       // zresetowanie predkosci do poczatkowej zeby nie szarpalo przy zmianie kierunku
    gpiod_line_set_value(dir_x, dir > 0);   // ustawienie pinu kierunku x
}

void set_dir_y(int dir)
{
    current_freq_y = STEP_FREQ_START;
    gpiod_line_set_value(dir_y, dir > 0);
}

// Ruch
void set_move_x(int enable) // ustawienie flagi move
{
    move_x = (enable != 0);
}

void set_move_y(int enable)
{
    move_y = (enable != 0);
}

void control_close(void)
{
    running = 0;                        // wyjscie z petli while
    pthread_join(step_thread, NULL);    // czekanie na zamkniecie watku
    gpiod_chip_close(chip);             // zwolnienie pinow
}
