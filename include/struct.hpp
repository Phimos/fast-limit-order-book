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

struct Limit;

struct Order
{
    const uint64_t uid;
    const uint64_t price;
    uint64_t quantity;
    std::weak_ptr<Limit> limit;

    Order(uint64_t uid, uint64_t price, uint64_t quantity) : uid(uid), price(price), quantity(quantity) {}
};

struct Limit : public std::enable_shared_from_this<Limit>
{
    const Side side;
    const uint64_t price;
    uint64_t quantity;
    DoubleLinkedList<std::shared_ptr<Order>> orders;

    Limit(uint64_t price, Side side) : side(side), price(price), quantity(0) {}
    bool operator<(const Limit &other) const { return std::make_pair(side, price) < std::make_pair(other.side, other.price); }
    bool operator==(const Limit &other) const { return std::make_pair(side, price) == std::make_pair(other.side, other.price); }
    bool operator>(const Limit &other) const { return std::make_pair(side, price) > std::make_pair(other.side, other.price); }
    void insert(std::shared_ptr<Order> &order)
    {
        orders.push_back(order);
        quantity += order->quantity;
        order->limit = weak_from_this();
    }
    friend std::ostream &operator<<(std::ostream &os, const Limit &limit)
    {
        os << "Limit(" << (limit.side == Bid ? "Bid" : "Ask") << ", " << limit.price << ", " << limit.quantity << ")";
        return os;
    }
};

enum QuoteType
{
    LimitOrder,
    MarketOrder,
    BestPriceOrder,
    CancelOrder,
    FillOrder
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

enum TradingStatus
{
    CallAuction,
    ContinuousTrading,
};

#endif // __STRUCT_HPP__