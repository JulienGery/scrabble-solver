#include <chrono>
#include <iostream>

class SimpleTimer
{
public:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::string m_message;

    SimpleTimer(const std::string& message) : m_message(message)
    {
        m_start = std::chrono::high_resolution_clock::now();
    };

    ~SimpleTimer()
    {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);
        std::cout << m_message << ": " << delta.count() << "ms\n";
    }

};
