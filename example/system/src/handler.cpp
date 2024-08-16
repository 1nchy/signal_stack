#include "handler.hpp"
#include <cstdio>

void sig_chld_handler_yes(int) {
    printf("YES! receive sig_chld.\n");
}
void sig_chld_handler_no(int) {
    printf("NO! receive sig_chld.\n");
}