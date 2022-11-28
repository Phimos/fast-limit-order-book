#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <iostream>
#include <chrono>
#include <string>

std::string strftime(uint64_t timestamp, const char *format = "%Y-%m-%d %H:%M:%S") // timestamp in us
{
    auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>(std::chrono::microseconds(timestamp));
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm gmt{}; // UTC
    gmtime_r(&tt, &gmt);
    char buffer[32];
    strftime(buffer, sizeof(buffer), format, &gmt);
    return std::string(buffer) + "." + std::to_string(timestamp % 1000000);
}

#endif // __UTILS_HPP__