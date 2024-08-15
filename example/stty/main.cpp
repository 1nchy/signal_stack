#include "signal_stack.hpp"
#include "handler.hpp"
#include <cstdlib>

icy::signal_stack _ss;

int main(void) {
    _ss.build(SIGCHLD, &sig_chld_handler);
    system("pwd");
    _ss.restore(SIGCHLD);
    system("ls");
    return 0;
}