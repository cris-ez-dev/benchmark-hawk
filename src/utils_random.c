#include "utils_random.h"
#include <stdlib.h>
#include <time.h>

void generate_random_message(uint8_t *buffer, size_t length) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (uint8_t)(rand() % 256);
    }
}
