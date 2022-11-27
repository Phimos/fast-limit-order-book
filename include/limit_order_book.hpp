#ifndef __LIMIT_ORDER_BOOK_HPP__
#define __LIMIT_ORDER_BOOK_HPP__

#include "double_linked_list.hpp"
#include "treap.hpp"
#include "struct.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <iostream>

class LimitOrderBook
{
    std::shared_ptr<Treap<Limit>> bid_limits, ask_limits;
    std::unordered_map<uint64_t, std::shared_ptr<Limit>> bid_price_map, ask_price_map;
    std::unordered_map<uint64_t, std::shared_ptr<Order>> uid_order_map;

public:
    LimitOrderBook() : bid_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())), ask_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())) {}
    void clear();
    void write(const Quote &quote);
    void write_limit_order(const Quote &quote);
    void write_market_order(const Quote &quote);
    void write_cancel_order(const Quote &quote);

    void show();
};

void LimitOrderBook::clear()
{
    bid_limits->clear();
    ask_limits->clear();
    uid_order_map.clear();
}

void LimitOrderBook::write(const Quote &quote)
{
    switch (quote.type)
    {
    case LimitOrder:
        write_limit_order(quote);
        break;
    case MarketOrder:
        write_market_order(quote);
        break;
    case CancelOrder:
        write_cancel_order(quote);
        break;
    }
}

void LimitOrderBook::write_limit_order(const Quote &quote)
{
    assert(quote.type == LimitOrder);
    // create order
    auto order = std::make_shared<Order>(quote.uid, quote.price, quote.quantity);
    assert(uid_order_map.find(quote.uid) == uid_order_map.end());
    uid_order_map[quote.uid] = order;
    auto limits = quote.side == Bid ? bid_limits : ask_limits;
    std::unordered_map<uint64_t, std::shared_ptr<Limit>> &price_map = quote.side == Bid ? bid_price_map : ask_price_map;
    auto limit = price_map[quote.price];
    // auto limit = limits->select_by_value(Limit(quote.price, quote.side));
    if (!limit)
    {
        limit = std::make_shared<Limit>(quote.price, quote.side);
        limits->insert(limit);
        price_map[quote.price] = limit;
    }
    limit->insert(order);
    std::cout << limit->quantity << std::endl;
}

void LimitOrderBook::write_market_order(const Quote &quote)
{
    assert(quote.type == MarketOrder);
    auto limits = quote.side == Bid ? bid_limits : ask_limits;
    if ((quote.side == Bid && ask_limits->empty()) || (quote.side == Ask && bid_limits->empty()))
        return;
    uint64_t price = quote.side == Bid ? ask_limits->min().price : bid_limits->max().price;
    write_limit_order(Quote(quote.uid, price, quote.quantity, quote.timestamp, quote.side, LimitOrder));
}

void LimitOrderBook::write_cancel_order(const Quote &quote)
{
    assert(quote.type == CancelOrder);
    auto limits = quote.side == Bid ? bid_limits : ask_limits;
    auto order = uid_order_map[quote.uid];
    auto limit = order->limit.lock();
    std::cout << uid_order_map.size() << std::endl;
    assert(limit != nullptr);
    limit->quantity -= order->quantity;
    order->quantity = 0;
    uid_order_map.erase(quote.uid);
    if (limit->orders.front()->uid == quote.uid)
        limit->orders.pop_front();
    if (limit->quantity == 0)
        limits->remove(*limit);
}

void LimitOrderBook::show()
{
    std::cout << "Bid:" << std::endl;
    std::cout << *bid_limits << std::endl;

    std::cout << "Ask:" << std::endl;
    std::cout << *ask_limits << std::endl;
}

#endif // __LIMIT_ORDER_BOOK_HPP__