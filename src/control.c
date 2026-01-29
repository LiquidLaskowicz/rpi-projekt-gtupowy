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


static struct gpiod_chip *chip;

// Kierunek
static struct gpiod_line *dir_x;
static struct gpiod_line *dir_y;

// Ruch
static struct gpiod_line *step_x;
static struct gpiod_line *step_y;

static volatile int move_x = 0;
static volatile int move_y = 0;

static volatile int current_freq_x = STEP_FREQ_START;
static volatile int current_freq_y = STEP_FREQ_START;

static volatile int target_freq_x = STEP_FREQ_MAX;
static volatile int target_freq_y = STEP_FREQ_MAX;


static pthread_t step_thread;
static int running = 1;

static void *stepper_thread(void *arg)
{
    (void)arg;

    while (running)
    {
        int did_step = 0;

        // Os X
        if (move_x)
        {
            int delay_x = 1000000 / (current_freq_x * 2);

            gpiod_line_set_value(step_x, 1);
            sleep_us(delay_x);
            gpiod_line_set_value(step_x, 0);
            sleep_us(delay_x);

            int accel_step_x = ACCEL_HZ_PER_S * delay_x * 2 / 1000000;
            if (accel_step_x < 1) accel_step_x = 1;

            if (current_freq_x < target_freq_x)
            {
                current_freq_x += accel_step_x;
                if (current_freq_x > target_freq_x)
                    current_freq_x = target_freq_x;
            }

            did_step = 1;
        }
        else
        {
            current_freq_x = STEP_FREQ_START;
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

        // Nic sie nie rusza
        if (!did_step)
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

    gpiod_line_request_output(dir_x,  "DIR_X",  0);
    gpiod_line_request_output(step_x, "STEP_X", 0);

    gpiod_line_request_output(dir_y,  "DIR_Y",  0);
    gpiod_line_request_output(step_y, "STEP_Y", 0);

    pthread_create(&step_thread, NULL, stepper_thread, NULL);

    DEBUG_PRINT("GPIO + STEP uruchomione");
    return 0;
}

// Kierunek
void set_dir_x(int dir)
{
    current_freq_x = STEP_FREQ_START;
    gpiod_line_set_value(dir_x, dir > 0);
}

void set_dir_y(int dir)
{
    current_freq_y = STEP_FREQ_START;
    gpiod_line_set_value(dir_y, dir > 0);
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
    pthread_join(step_thread, NULL);
    gpiod_chip_close(chip);
}
