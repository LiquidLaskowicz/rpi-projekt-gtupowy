// Sterowanie silnikami

#define _POSIX_C_SOURCE 199309L
#include "control.h"
#include "config.h"

#include <gpiod.h>
#include <pthread.h>
#include <unistd.h>

static struct gpiod_chip *chip;

// Kierunek
static struct gpiod_line *dir_x;
static struct gpiod_line *dir_y1;
static struct gpiod_line *dir_y2;

// Ruch
static struct gpiod_line *step_x;
static struct gpiod_line *step_y1;
static struct gpiod_line *step_y2;

static volatile int move_x = 0;
static volatile int move_y = 0;

static pthread_t step_thread;
static int running = 1;

// Generowanie krokow
static void *stepper_thread(void *arg)
{
    (void)arg;
    int delay_us = 1000000 / (STEP_FREQ_HZ * 2);

    while (running)
    {
        // STEP HIGH
        if (move_x)
            gpiod_line_set_value(step_x, 1);

        if (move_y) {
            gpiod_line_set_value(step_y1, 1);
            gpiod_line_set_value(step_y2, 1);
        }

        usleep(delay_us);

        // STEP LOW
        if (move_x)
            gpiod_line_set_value(step_x, 0);

        if (move_y) {
            gpiod_line_set_value(step_y1, 0);
            gpiod_line_set_value(step_y2, 0);
        }

        usleep(delay_us);
    }
    return NULL;
}


// Inicjalizacja
int control_init(void)
{
    chip = gpiod_chip_open_by_name(GPIO_CHIP_NAME);

    dir_x   = gpiod_chip_get_line(chip, PIN_DIR_X);
    step_x  = gpiod_chip_get_line(chip, PIN_STEP_X);

    dir_y1  = gpiod_chip_get_line(chip, PIN_DIR_Y1);
    step_y1 = gpiod_chip_get_line(chip, PIN_STEP_Y1);

    dir_y2  = gpiod_chip_get_line(chip, PIN_DIR_Y2);
    step_y2 = gpiod_chip_get_line(chip, PIN_STEP_Y2);

    gpiod_line_request_output(dir_x,  "DIR_X",  0);
    gpiod_line_request_output(step_x, "STEP_X", 0);

    gpiod_line_request_output(dir_y1,  "DIR_Y1",  0);
    gpiod_line_request_output(step_y1, "STEP_Y1", 0);

    gpiod_line_request_output(dir_y2,  "DIR_Y2",  0);
    gpiod_line_request_output(step_y2, "STEP_Y2", 0);

    pthread_create(&step_thread, NULL, stepper_thread, NULL);

    DEBUG_PRINT("GPIO + STEP uruchomione");
    return 0;
}

// Kierunek
void set_dir_x(int dir)
{
    gpiod_line_set_value(dir_x, dir > 0);
}

void set_dir_y(int dir)
{
    int v = (dir > 0);
    gpiod_line_set_value(dir_y1, v);
    gpiod_line_set_value(dir_y2, v);
}

// Ruch
void set_move_x(int enable)
{
    move_x = (enable != 0);
}

void set_move_y(int enable)
{
    move_y = (enable != 0);
}

void control_close(void)
{
    running = 0;
    gpiod_chip_close(chip);
}
