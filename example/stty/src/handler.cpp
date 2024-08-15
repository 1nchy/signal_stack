#include "handler.hpp"
#include <cstdio>

void sig_chld_handler(int) {
    printf("receive sig_chld.\n");
}