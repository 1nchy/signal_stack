#include "signal_stack.hpp"
#include "handler.hpp"
#include <cstdlib>
#include <cstdio>

icy::signal_stack _ss;

int main(void) {
    _ss.build(SIGCHLD, &sig_chld_handler_yes);
    _ss.block(SIGCHLD);
    system("pwd");
    system("ls");
    _ss.restore_mask();


    // _ss.build(SIGCHLD, &sig_chld_handler_yes);
    // if (_ss.has_ignored(SIGCHLD)) {
    //     printf("NO! SIGCHLD has been ignored.\n");
    // }
    // system("pwd >> /dev/null");
    // const auto _act = _ss.back(SIGCHLD);
    // _ss.restore(SIGCHLD);
    // system("ls >> /dev/null");
    // _ss.build(SIGCHLD, _act);
    // system("pwd >> /dev/null");
    // _ss.build(SIGCHLD, &sig_chld_handler_no);
    // _ss.ignore(SIGCHLD);
    // if (_ss.has_ignored(SIGCHLD)) {
    //     printf("YES! SIGCHLD has been ignored.\n");
    // }
    // system("ls >> /dev/null");
    // _ss.restore(SIGCHLD);
    // system("ls >> /dev/null");
    // _ss.restore(SIGCHLD);
    // system("pwd >> /dev/null");
    return 0;
}