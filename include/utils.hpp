#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <chrono>
#include <string>

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

#endif // __UTILS_HPP__