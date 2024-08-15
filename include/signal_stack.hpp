#ifndef _ICY_SIGNAL_STACK_HPP_
#define _ICY_SIGNAL_STACK_HPP_

#include <csignal>
#include <functional>
#include <vector>
#include <unordered_map>

namespace icy {

class signal_stack;

class signal_stack {
public:
    signal_stack() = default;
    signal_stack(const signal_stack&) = delete;
    signal_stack& operator=(const signal_stack&) = delete;
    ~signal_stack() = default;
    typedef void(*handler_t)(int);
    /**
     * @brief return whether the signal handler is set
     * @param _sig POSIX signal
     */
    bool empty(unsigned _sig) const;
    /**
     * @brief set handler for specific signal
     * @param _sig POSIX signal
     * @param _h handler
     * @return @c true if success
     */
    bool build(unsigned _sig, handler_t _h);
    /**
     * @brief set handler for specific signal with flag
     * @param _sig POSIX signal
     * @param _flag flag for sigaction
     * @param _h handler
     * @return @c true if success
     */
    bool build(unsigned _sig, int _flag, handler_t _h);
    /**
     * @brief set handler for specific signal with flag and masks
     * @param _sig POSIX signal
     * @param _flag flag for sigaction
     * @param _h handler
     * @param _mask_list masks for sigaction
     * @return @c true if success
     */
    bool build(unsigned _sig, int _flag, handler_t _h, std::initializer_list<int> _mask_list);
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
     * @brief set initial handler for specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool reset(unsigned _sig);
    /**
     * @brief set initial handler and clear all handlers in stack for specific signal
     * @param _sig POSIX signal
     * @return @c true if success
     */
    bool clear(unsigned _sig);
private:
/**
 * When _data[_i].size() == 1, the only element must be initial signal handler.
 * Thus, when building handler, if _data[_i].empty(), we need to store initial signal handler,
 * which is used to restore handler.
*/
    std::unordered_map<unsigned, std::vector<struct sigaction>> _data;
};
};

#endif // _ICY_SIGNAL_STACK_HPP_