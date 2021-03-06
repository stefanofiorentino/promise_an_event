#include <iostream>
#include <future>

using namespace std::chrono_literals;

// https://stackoverflow.com/a/10917945/2794395

template <typename F, typename... Ts>
inline auto reallyAsync(F&& f, Ts&&... params) {
    return std::async(std::launch::async, std::forward<F>(f), std::forward<Ts>(params)...);
}

template<typename R>
bool is_ready(std::future<R> const &f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool resolve_func()
{
    std::cout << "RESOLVE" << std::endl;
    return true;
}

bool reject_func()
{
    std::cout << "REJECT" << std::endl;
    return false;
}

void detect(float frac)
{
    auto timeout = 5s;
    std::promise<void> p;

    auto fs = reallyAsync([&p, &timeout](auto resolve, auto reject)
                   {
                       auto future = p.get_future();
                       future.wait_until(std::chrono::steady_clock::now() + timeout);
                       if (is_ready(future))
                       {
                           return resolve();
                       }
                       else
                       {
                           return reject();
                       }
                   }, resolve_func, reject_func);

    // additional tasks can be run here, but cannot throw (really).

    std::this_thread::sleep_for(frac*timeout);

    // if an exception is raised before std::promise::set_value() is called, the program will deadlock (forever).
    p.set_value();

    // the destructor of fs (RAII) call join() for us.
}

int main()
{
    // this first call resolve correctly
    detect(0.5f);
    // this second call reject due to timeout
    detect(1.5f);
    return 0;
}