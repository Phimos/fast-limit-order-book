#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__

#include <string>
#include <memory>
#include <map>
#include "double_linked_list.hpp"
#include "treap.hpp"

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
    const uint64_t timestamp;
    std::weak_ptr<Limit> limit;

    Order(uint64_t uid, uint64_t price, uint64_t quantity, uint64_t timestamp)
        : uid(uid), price(price), quantity(quantity), timestamp(timestamp) {}
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

template <>
struct Node<Limit>
{
    std::shared_ptr<Limit> value_ptr;
    size_t size;
    size_t priority;
    uint64_t sum_quantity, count_orders;
    std::shared_ptr<Node<Limit>> left, right;
    std::weak_ptr<Node<Limit>> parent;

    Node(std::shared_ptr<Limit> value_ptr)
        : value_ptr(value_ptr), size(1), priority(rand()),
          sum_quantity(0), count_orders(0) {}
    Node(Limit value)
        : value_ptr(std::make_shared<Limit>(value)), size(1), priority(rand()),
          sum_quantity(0), count_orders(0) {}
    void update()
    {
        size = 1;
        sum_quantity = value_ptr->quantity;
        count_orders = value_ptr->orders.size;
        if (left)
        {
            size += left->size;
            sum_quantity += left->sum_quantity;
            count_orders += left->count_orders;
        }
        if (right)
        {
            size += right->size;
            sum_quantity += right->sum_quantity;
            count_orders += right->count_orders;
        }
    }
    Limit &value() { return *value_ptr; }
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

    Quote(uint64_t uid, uint64_t price, uint64_t quantity, uint64_t timestamp, Side side, QuoteType type)
        : uid(uid), price(price), quantity(quantity), timestamp(timestamp), side(side), type(type) {}
};

enum TradingStatus
{
    CallAuction,
    ContinuousTrading,
};

#endif // __STRUCT_HPP__