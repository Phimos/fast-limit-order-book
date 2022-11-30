#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <chrono>
#include <string>
#include <tuple>
#include <vector>
#include <utility>
#include <experimental/tuple>

std::string strftime(uint64_t timestamp, const char *format = "%Y-%m-%d %H:%M:%S") // timestamp in ns
{
    auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>(std::chrono::nanoseconds(timestamp));
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm gmt{}; // UTC
    gmtime_r(&tt, &gmt);
    char buffer[32];
    strftime(buffer, sizeof(buffer), format, &gmt);
    std::string ms = std::to_string((timestamp % 1000000000UL) / 1000000);
    return std::string(buffer) + "." + std::string(3 - ms.size(), '0') + ms;
}

template <class... Ts>
class Table
{
    typedef std::tuple<Ts...> DataTuple;
    std::vector<DataTuple> data;
    std::vector<std::string> headers;
    size_t width;

public:
    Table(std::vector<std::string> headers, size_t width = 20) : headers(headers), width(width) {}

    template <typename Stream>
    void print(Stream &stream)
    {
        stream << "|";
        for (size_t i = 0; i < headers.size(); i++)
        {
            stream << headers[i] << std::string(width - headers[i].size(), ' ') << "|";
        }
        stream << std::endl;
        stream << "|";
        for (size_t i = 0; i < headers.size(); i++)
        {
            stream << std::string(width, '-');
            if (i < headers.size() - 1)
                stream << "+";
        }
        stream << "|" << std::endl;
        for (auto &row : data)
        {
            stream << "|";
            std::experimental::apply([&](auto &&...args)
                                     { ((stream << args << std::string(width - std::to_string(args).size(), ' ') << '|'), ...); },
                                     row);
            stream << std::endl;
        }
    }

    void add(Ts... args)
    {
        data.emplace_back(args...);
    }
};

#endif // __UTILS_HPP__