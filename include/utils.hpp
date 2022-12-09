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

constexpr uint64_t nanoseconds_per_millisecond = 1000000UL;
constexpr uint64_t nanoseconds_per_second = nanoseconds_per_millisecond * 1000UL;
constexpr uint64_t nanoseconds_per_minute = nanoseconds_per_second * 60UL;
constexpr uint64_t nanoseconds_per_hour = nanoseconds_per_minute * 60UL;
constexpr uint64_t nanoseconds_per_day = nanoseconds_per_hour * 24UL;

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
    std::vector<std::pair<size_t, std::string>> special_lines;

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
    void print_header(Stream &stream)
    {
        stream << "│";
        for (size_t i = 0; i < headers.size(); i++)
        {
            stream << std::string((width - headers[i].size()) / 2, ' ')
                   << headers[i]
                   << std::string((width - headers[i].size() + 1) / 2, ' ')
                   << "│";
        }
        stream << std::endl;
        stream << line("╞", "╡", "╪", "═") << std::endl;
    }

    template <typename Stream>
    void print(Stream &stream)
    {
        stream << line("┌", "┐", "┬", "─") << std::endl;
        if (headers.size() > 0)
            print_header<Stream>(stream);

        size_t i, j;
        for (i = 0, j = 0; i < data.size(); ++i)
        {
            while (j < special_lines.size() && i == special_lines[j].first)
                stream << special_lines[j++].second << std::endl;
            stream << "│";
            std::experimental::apply([&](auto &&...args)
                                     { ((stream << std::setw(width) << args << "│"), ...); },
                                     data[i]);
            stream << std::endl;
        }
        while (j < special_lines.size() && i == special_lines[j].first)
            stream << special_lines[j++].second << std::endl;
        stream << line("└", "┘", "┴", "─") << std::endl;
    }

    void add_row(Ts... args) { data.emplace_back(args...); }
    void add_divider() { special_lines.emplace_back(data.size(), line("├", "┤", "┼", "─")); }
    void add_empty_row() { special_lines.emplace_back(data.size(), line("│", "│", "│", " ")); }
};

#endif // __UTILS_HPP__