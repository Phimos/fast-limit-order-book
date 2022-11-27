#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__

#include <string>
#include <memory>
#include <map>
#include "double_linked_list.hpp"

enum Side : bool
{
    Bid = false,
    Ask = true
};

struct Order;

struct Limit
{
    const Side side;
    const uint64_t price;
    uint64_t quantity;
    DoubleLinkedList<Order> orders;

    Limit(uint64_t price, Side side) : side(side), price(price), quantity(0) {}
    bool operator<(const Limit &other) const { return std::make_pair(side, price) < std::make_pair(other.side, other.price); }
    bool operator==(const Limit &other) const { return std::make_pair(side, price) == std::make_pair(other.side, other.price); }
    bool operator>(const Limit &other) const { return std::make_pair(side, price) > std::make_pair(other.side, other.price); }
    friend std::ostream &operator<<(std::ostream &os, const Limit &limit)
    {
        os << "Limit(" << (limit.side == Bid ? "Bid" : "Ask") << ", " << limit.price << ", " << limit.quantity << ")";
        return os;
    }
};

struct Order
{
    const uint64_t uid;
    const uint64_t price;
    uint64_t quantity;
    std::weak_ptr<Limit> limit;

    Order(uint64_t uid, uint64_t price, uint64_t quantity) : uid(uid), price(price), quantity(quantity) {}
};

enum QuoteType
{
    LimitOrder,
    MarketOrder,
    CancelOrder
};

struct Quote
{
    const uint64_t uid;
    const uint64_t price;
    const uint64_t quantity;
    const uint64_t timestamp;
    const Side side;
    const QuoteType type;

    Quote(uint64_t uid, uint64_t price, uint64_t quantity, uint64_t timestamp, Side side, QuoteType type) : uid(uid), price(price), quantity(quantity), timestamp(timestamp), side(side), type(type) {}
};

#endif // __STRUCT_HPP__