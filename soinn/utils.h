#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <queue>
#include <vector>
#include <chrono>
#include <iomanip>

#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace ann
{

template<typename T>
void format_print(T t, int width = 20, std::string sep = "")
{
    std::cout << std::internal << std::setw(width) << std::setfill(' ') << t;
    std::cout << sep;
}
template<class Head>
void print_args_(Head&& head) {
    format_print(std::forward<Head>(head));
}
template<class Head, class... Tail>
void print_args_(Head&& head, Tail&&... tail) {
    format_print(std::forward<Head>(head));
    print_args_(std::forward<Tail>(tail)...);
}
template<class... Args>
void print_args(Args&&... args) {
    print_args_(std::forward<Args>(args)...);
    std::cout << "\n";
}

#ifdef _debug_
template<class... Args>
void debug_print(Args&&... args) {
    print_args(std::forward<Args>(args)...);
    std::cout << std::endl;
}
#else
template<class... Args>
void debug_print(Args&&... args) {return;}
#endif

template <typename T>
class pair_greater
{
public:
    bool operator()(const std::pair<double, T> lhs, const std::pair<double, T>& rhs)
    {
        return lhs.first > rhs.first;
    }
};

template <typename T>
using pri_queue = std::priority_queue<
    std::pair<double, T>,
    std::vector<std::pair<double, T>>,
    pair_greater<std::pair<double, T>>
    >;

class timer
{
public:
    timer(): _timer(), _count(0) {}
    void start() {
        _time_point = _timer.now();
    }
    void stop() {
        auto duration = 
        std::chrono::duration_cast<std::chrono::microseconds>
            (_timer.now() - _time_point);
        _count += duration.count();
    }
    void reset() {
        _count = 0;
    }
    size_t count() const {
        return _count;
    }

private:
    std::chrono::steady_clock _timer;
    std::chrono::steady_clock::time_point _time_point;
    size_t _count;
};

template <typename T, typename Y>
void assert_eq(const T& lhs, const Y& rhs) {
    if (lhs != rhs) {
        throw std::runtime_error(std::to_string(lhs) + "==" + std::to_string(rhs) + "faild");
    }
}

/// Check File Existence
inline bool file_exists(const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

inline int newdir(const std::string& name) {
    return mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

} // namespace ann

#endif

