#include "io.h"

int main(void) {
    char buff[256];

    printf("%d", read_word(buff, 256, stdin));
    return 0;
}