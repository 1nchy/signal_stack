#include "signal_stack.hpp"

#include <thread>

namespace icy {

signal_stack::signal_stack() {
    sigset_t _origin; sigpending(&_origin);
    _masks.push_back(_origin);
}

bool signal_stack::build(unsigned _sig, handler_t _h, int _flags) {
    struct sigaction _nact;
    _nact.sa_flags = _flags;
    sigemptyset(&_nact.sa_mask);
    _nact.sa_handler = _h;
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_build(_sig, _nact);
}
bool signal_stack::build(unsigned _sig, handler_t _h, int _flags, std::initializer_list<int> _mask_list) {
    struct sigaction _nact;
    _nact.sa_flags = _flags;
    sigemptyset(&_nact.sa_mask);
    for (const auto& _mask : _mask_list) {
        sigaddset(&_nact.sa_mask, _mask);
    }
    _nact.sa_handler = _h;
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_build(_sig, _nact);
}
bool signal_stack::build(unsigned _sig, struct sigaction _nact) {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_build(_sig, _nact);
}
auto signal_stack::back(unsigned _sig) const -> struct sigaction {
    std::shared_lock<std::shared_mutex> _lock(_mutex);
    if (_actions.contains(_sig) && !_actions.at(_sig).empty()) {
        return _actions.at(_sig).back();
    }
    struct sigaction _act;
    _act.sa_flags = 0;
    sigemptyset(&_act.sa_mask);
    _act.sa_handler = SIG_DFL;
    return _act;
}
bool signal_stack::restore(unsigned _sig) {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_restore(_sig);
}
bool signal_stack::reset(unsigned _sig) {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_reset(_sig);
}
bool signal_stack::clear(unsigned _sig) {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_clear(_sig);
}
bool signal_stack::ignore(unsigned _sig) {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_ignore(_sig);
}
bool signal_stack::has_ignored(unsigned _sig) const {
    std::shared_lock<std::shared_mutex> _lock(_mutex);
    struct sigaction _oact;
    const int _r = sigaction(_sig, 0, &_oact);
    if (!(_oact.sa_flags & SA_SIGINFO)) {
        return _oact.sa_handler == SIG_IGN;
    }
    return false;
}
bool signal_stack::has_defaulted(unsigned _sig) const {
    std::shared_lock<std::shared_mutex> _lock(_mutex);
    struct sigaction _oact;
    const int _r = sigaction(_sig, 0, &_oact);
    if (!(_oact.sa_flags & SA_SIGINFO)) {
        return _oact.sa_handler == SIG_DFL;
    }
    return false;
}
bool signal_stack::has_handled(unsigned _sig) const {
    std::shared_lock<std::shared_mutex> _lock(_mutex);
    struct sigaction _oact;
    const int _r = sigaction(_sig, 0, &_oact);
    if (!(_oact.sa_flags & SA_SIGINFO)) {
        return _oact.sa_handler != SIG_IGN && _oact.sa_handler != SIG_DFL;
    }
    return false;
}


bool signal_stack::has_blocked(unsigned _sig) const {
    std::shared_lock<std::shared_mutex> _lock(_mutex);
    sigset_t _s; sigpending(&_s);
    return sigismember(&_s, _sig);
}
bool signal_stack::restore_mask() {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_restore_mask();
}
bool signal_stack::clear_mask() {
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_clear_mask();
}



bool signal_stack::_M_build(unsigned _sig, handler_t _h, int _flags) {
    struct sigaction _nact;
    _nact.sa_flags = _flags;
    sigemptyset(&_nact.sa_mask);
    _nact.sa_handler = _h;
    return _M_build(_sig, _nact);
}
bool signal_stack::_M_build(unsigned _sig, struct sigaction _nact) {
    struct sigaction _oact;
    const int _r = sigaction(_sig, &_nact, &_oact);
    if (_r == -1) return false;
    if (_actions[_sig].empty()) {
        // _actions[_sig] = {_oact};
        _actions[_sig].push_back(_oact);
    }
    _actions[_sig].push_back(_nact);
    return true;
}
bool signal_stack::_M_restore(unsigned _sig) {
    if (!_actions.contains(_sig)) return true;
    if (_actions[_sig].size() <= 1) {
        _actions[_sig].clear();
        _actions.erase(_sig);
        return true;
    }
    auto& _act = _actions[_sig].at(_actions[_sig].size() - 2);
    const int _r = sigaction(_sig, &_act, nullptr);
    if (_r == -1) return false;
    _actions[_sig].pop_back();
    return true;
}
bool signal_stack::_M_reset(unsigned _sig) {
    return _M_build(_sig, SIG_DFL);
}
bool signal_stack::_M_clear(unsigned _sig) {
    if (!_M_reset(_sig)) {
        return false;
    }
    _actions[_sig].clear();
    _actions.erase(_sig);
    return true;
}
bool signal_stack::_M_ignore(unsigned _sig) {
    return _M_build(_sig, SIG_IGN);
}


auto signal_stack::_M_expand_package(sigset_t* const _s, sigset_operator _op, unsigned _sig) const -> void {
    _op(_s, _sig);
}
bool signal_stack::_M_block(sigset_t* const _s) {
    const int _r = sigprocmask(SIG_BLOCK, _s, nullptr);
    if (_r == -1) return false;
    _masks.push_back(*_s);
    return true;
}
bool signal_stack::_M_unblock(sigset_t* const _s) {
    const int _r = sigprocmask(SIG_UNBLOCK, _s, nullptr);
    if (_r == -1) return false;
    _masks.push_back(*_s);
    return true;
}
bool signal_stack::_M_restore_mask() {
    if (_masks.size() <= 1) return true;
    auto& _s = _masks.at(_masks.size() - 2);
    const int _r = sigprocmask(SIG_SETMASK, &_s, nullptr);
    if (_r == -1) return false;
    _masks.pop_back();
    return true;
}
bool signal_stack::_M_clear_mask() {
    if (_masks.size() <= 1) return true;
    auto& _s = _masks.at(_masks.size() - 2);
    const int _r = sigprocmask(SIG_SETMASK, &_s, nullptr);
    if (_r == -1) return false;
    _masks.erase(_masks.cbegin() + 1, _masks.cend());
    return true;
}

};