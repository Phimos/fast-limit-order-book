#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <chrono>
#include <string>
#include <tuple>
#include <vector>
#include <iomanip>
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
    size_t width, num_columns;

public:
    Table(std::vector<std::string> headers = {}, size_t width = 20)
        : headers(headers), width(width), num_columns(sizeof...(Ts)) {}

    std::string line(std::string left, std::string right, std::string sep, std::string fill)
    {
        assert(fill.size() == 1);
        std::string cell;
        for (size_t i = 0; i < width; ++i)
            cell += fill;
        std::string line = left + cell;
        for (size_t i = 1; i < num_columns; ++i)
            line += sep + cell;
        line += right;
        return line;
    }

    template <typename Stream>
    void print(Stream &stream)
    {
        std::string top_border = line("┌", "┐", "┬", "─");
        std::string middle_border = line("╞", "╡", "╪", "═");
        std::string bottom_border = line("└", "┘", "┴", "─");

        stream << top_border << std::endl;

        stream << "│";
        for (size_t i = 0; i < headers.size(); i++)
        {
            stream << std::string((width - headers[i].size()) / 2, ' ')
                   << headers[i]
                   << std::string((width - headers[i].size() + 1) / 2, ' ')
                   << "│";
        }
        stream << std::endl;

        stream << middle_border << std::endl;

        for (auto &row : data)
        {
            stream << "│";
            std::experimental::apply([&](auto &&...args)
                                     { ((stream << std::setw(width) << args << "│"), ...); },
                                     row);
            stream << std::endl;
        }

        stream << bottom_border << std::endl;
    }

    void add(Ts... args) { data.emplace_back(args...); }
};

#endif // __UTILS_HPP__