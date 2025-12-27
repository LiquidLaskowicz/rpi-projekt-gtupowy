#include <stdio.h>
#include "config.h"

int main(int argc, char *argv[])
{
    printf("%s v%s\n", APP_NAME, APP_VERSION);

    DEBUG_PRINT("Program uruchomiony");
    DEBUG_PRINT("Argumenty: %d", argc);

    // TODO: tutaj bedzie logika programu

    return 0;
}
