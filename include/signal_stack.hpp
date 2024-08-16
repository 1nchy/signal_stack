#ifndef _ICY_SIGNAL_STACK_HPP_
#define _ICY_SIGNAL_STACK_HPP_

#include <csignal>
#include <functional>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

namespace icy {

class signal_stack;

class signal_stack {
public:
    signal_stack();
    signal_stack(const signal_stack&) = delete;
    signal_stack& operator=(const signal_stack&) = delete;
    ~signal_stack() = default;
    typedef void(*handler_t)(int);
public: // for signal actions
    /**
     * @brief set handler for specific signal with flag
     * @param _sig POSIX signal
     * @param _h handler
     * @param _flag flag for sigaction (default 0)
     * @return @c true if success
     */
    bool build(unsigned _sig, handler_t _h, int _flags = 0);
    /**
     * @brief set handler for specific signal with flag and masks
     * @param _sig POSIX signal
     * @param _h handler
     * @param _flag flag for sigaction
     * @param _mask_list masks for sigaction
     * @return @c true if success
     */
    bool build(unsigned _sig, handler_t _h, int _flags, std::initializer_list<int> _mask_list);
    /**
     * @brief set handler for specific signal with sigaction object
     * @param _sig POSIX signal
     * @param _nact sigaction object
     * @return @c true if success
     */
    bool build(unsigned _sig, struct sigaction _nact);
    /**
     * @brief get newest sigaction for specific signal
     * @param _sig POSIX signal
     */
    struct sigaction back(unsigned _sig) const;
    /**
     * @brief restore previous handler for specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool restore(unsigned _sig);
    /**
     * @brief set default action for specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool reset(unsigned _sig);
    /**
     * @brief set default action and clear all handlers in stack for specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool clear(unsigned _sig);
    /**
     * @brief ignore specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool ignore(unsigned _sig);
    /**
     * @brief has the specific signal been ignored
     * @param _sig POSIX signal
     */
    bool has_ignored(unsigned _sig) const;
    /**
     * @brief has the specific signal been handled as the default action
     * @param _sig POSIX signal
     */
    bool has_defaulted(unsigned _sig) const;
    /**
     * @brief has the specific signal been handled as any action (exclude default action)
     * @param _sig POSIX signal
     */
    bool has_handled(unsigned _sig) const;
public: // for signal masks
    template <typename... _Args> bool block(unsigned _sig, _Args&&... _sigs);
    template <typename... _Args> bool block_except(unsigned _sig, _Args&&... _sigs);
    template <typename... _Args> bool unblock(unsigned _sig, _Args&&... _sigs);
    template <typename... _Args> bool unblock_except(unsigned _sig, _Args&&... _sigs);
    /**
     * @brief has the specific signal been blocked
     * @param _sig POSIX signal
     */
    bool has_blocked(unsigned _sig) const;
    /**
     * @brief restore the mask of the specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool restore_mask();
    /**
     * @brief clear the mask of the specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool clear_mask();
private: // for signal actions
    bool _M_build(unsigned _sig, handler_t _h, int _flag = 0);
    bool _M_build(unsigned _sig, struct sigaction _nact);
    bool _M_restore(unsigned _sig);
    bool _M_reset(unsigned _sig);
    bool _M_clear(unsigned _sig);
    bool _M_ignore(unsigned _sig);
private: // for signal masks
    using sigset_operator = int(*)(sigset_t*, int);
    template <typename... _Args> void _M_expand_package(sigset_t* const _s, sigset_operator _op, unsigned _sig, _Args&&... _sigs) const;
    void _M_expand_package(sigset_t* const _s, sigset_operator _op, unsigned _sig) const;
    bool _M_block(sigset_t* const);
    bool _M_unblock(sigset_t* const);
    bool _M_restore_mask();
    bool _M_clear_mask();
private:
    /**
     * When _actions[_i].size() == 1, the only element must be initial signal handler.
     * Thus, when building handler, if _actions[_i].empty(), we need to store initial signal handler,
     * which is used to restore handler.
    */
    std::unordered_map<unsigned, std::vector<struct sigaction>> _actions;
    /**
     * 
     */
    std::vector<sigset_t> _masks;
    mutable std::shared_mutex _mutex;
};


/**
 * @brief block the specific signal
 * @param _sigs POSIX signals
 * @return @c true if success
 */
template <typename... _Args> auto signal_stack::block(unsigned _sig, _Args&&... _sigs) -> bool {
    sigset_t _s; sigemptyset(&_s);
    _M_expand_package(&_s, &sigaddset, _sig, std::forward<_Args>(_sigs)...);
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_block(&_s);
};
/**
 * @brief block all signals except the specific signal
 * @param _sigs POSIX signals
 * @return @c true if success
 */
template <typename... _Args> auto signal_stack::block_except(unsigned _sig, _Args&&... _sigs) -> bool {
    sigset_t _s; sigfillset(&_s);
    _M_expand_package(&_s, &sigdelset, _sig, std::forward<_Args>(_sigs)...);
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_block(&_s);
};
/**
 * @brief unblock the specific signal
 * @param _sigs POSIX signals
 * @return @c true if success
 */
template <typename... _Args> auto signal_stack::unblock(unsigned _sig, _Args&&... _sigs) -> bool {
    sigset_t _s; sigemptyset(&_s);
    _M_expand_package(&_s, &sigaddset, _sig, std::forward<_Args>(_sigs)...);
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_unblock(&_s);
};
/**
 * @brief unblock all signals except the specific signal
 * @param _sigs POSIX signals
 * @return @c true if success
 */
template <typename... _Args> auto signal_stack::unblock_except(unsigned _sig, _Args&&... _sigs) -> bool {
    sigset_t _s; sigfillset(&_s);
    _M_expand_package(&_s, &sigdelset, _sig, std::forward<_Args>(_sigs)...);
    std::unique_lock<std::shared_mutex> _lock(_mutex);
    return _M_unblock(&_s);
};

template <typename... _Args> auto signal_stack::_M_expand_package(sigset_t* const _s, sigset_operator _op, unsigned _sig, _Args&&... _sigs) const -> void {
    _op(_s, _sig);
    _M_expand_package(_s, _op, std::forward<_Args>(_sigs)...);
}


};

#endif // _ICY_SIGNAL_STACK_HPP_