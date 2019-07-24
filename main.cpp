#include <iostream>
#include <future>

using namespace std::chrono_literals;

// https://stackoverflow.com/a/10917945/2794395
template<typename R>
bool is_ready(std::future<R> const &f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void resolve()
{
    std::cout << "RESOLVE" << std::endl;
}

void reject()
{
    std::cout << "REJECT" << std::endl;
}

void detect(float frac)
{
    auto timeout = 5s;
    std::promise<void> p;

    std::thread th([&p, &timeout]
                   {
                       auto future = p.get_future();
                       future.wait_until(std::chrono::steady_clock::now() + timeout);
                       if (is_ready(future))
                       {
                           resolve();
                       }
                       else
                       {
                           reject();
                       }
                   });

    std::this_thread::sleep_for(frac*timeout);

    // if an exception is raised before std::promise::set_value() is called, the program will deadlock.
    p.set_value();
    th.join();
}

int main()
{
    // this first call resolve correctly
    detect(0.5f);
    // this second call reject due to timeout
    detect(1.5f);
    return 0;
}