#include "signal_stack.hpp"
#include "handler.hpp"
#include <cstdlib>

icy::signal_stack _ss;

int main(void) {
    _ss.build(SIGCHLD, &sig_chld_handler);
    system("pwd");
    const auto _act = _ss.back(SIGCHLD);
    _ss.restore(SIGCHLD);
    system("ls");
    _ss.build(SIGCHLD, _act);
    system("pwd");
    _ss.ignore(SIGCHLD);
    system("ls");
    _ss.restore(SIGCHLD);
    system("pwd");
    return 0;
}