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
    TradingStatus status = TradingStatus::ContinuousTrading;
    std::shared_ptr<Treap<Limit>> bid_limits, ask_limits;
    std::unordered_map<uint64_t, std::shared_ptr<Limit>> bid_price_map, ask_price_map;
    std::unordered_map<uint64_t, std::shared_ptr<Order>> uid_order_map;

public:
    LimitOrderBook() : bid_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())), ask_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())) {}
    void clear();
    void write(const Quote &quote);
    void write_limit_order(const Quote &quote);
    void write_market_order(const Quote &quote);
    void write_best_price_order(const Quote &quote);
    void write_cancel_order(const Quote &quote);
    void write_fill_order(const Quote &quote);
    void trade(uint64_t ask_uid, uint64_t bid_uid, uint64_t quantity);
    void match();

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
    case BestPriceOrder:
        write_best_price_order(quote);
        break;
    case CancelOrder:
        write_cancel_order(quote);
        break;
    case FillOrder:
        write_fill_order(quote);
        break;
    }
}

void LimitOrderBook::write_limit_order(const Quote &quote)
{
    assert(quote.type == LimitOrder);
    // create order
    auto order = std::make_shared<Order>(quote.uid, quote.price, quote.quantity, quote.timestamp);
    assert(uid_order_map.find(quote.uid) == uid_order_map.end());
    uid_order_map[quote.uid] = order;
    std::shared_ptr<Treap<Limit>> &limits = quote.side == Bid ? bid_limits : ask_limits;
    std::unordered_map<uint64_t, std::shared_ptr<Limit>> &price_map = quote.side == Bid ? bid_price_map : ask_price_map;
    auto limit = price_map[quote.price];
    if (!limit)
    {
        limit = std::make_shared<Limit>(quote.price, quote.side);
        limits->insert(limit);
        price_map[quote.price] = limit;
    }
    limit->insert(order);
    if (status == TradingStatus::ContinuousTrading)
        match();
}

void LimitOrderBook::write_market_order(const Quote &quote)
{
    assert(quote.type == MarketOrder);
    if ((quote.side == Bid && ask_limits->empty()) || (quote.side == Ask && bid_limits->empty()))
        return;
    uint64_t price = quote.side == Bid ? ask_limits->min().price : bid_limits->max().price;
    write_limit_order(Quote(quote.uid, price, quote.quantity, quote.timestamp, quote.side, LimitOrder));
}

void LimitOrderBook::write_best_price_order(const Quote &quote)
{
    assert(quote.type == BestPriceOrder);
    if ((quote.side == Bid && bid_limits->empty()) || (quote.side == Ask && ask_limits->empty()))
        return;
    uint64_t price = quote.side == Bid ? bid_limits->max().price : ask_limits->min().price;
    write_limit_order(Quote(quote.uid, price, quote.quantity, quote.timestamp, quote.side, LimitOrder));
}

void LimitOrderBook::write_cancel_order(const Quote &quote)
{
    assert(quote.type == CancelOrder);
    auto limits = quote.side == Bid ? bid_limits : ask_limits;
    auto order = uid_order_map[quote.uid];
    auto limit = order->limit.lock();
    assert(limit);
    limit->quantity -= quote.quantity;
    order->quantity -= quote.quantity;
    if (order->quantity == 0)
        uid_order_map.erase(quote.uid);
    while (!limit->orders.empty() && limit->orders.front()->quantity == 0)
        limit->orders.pop_front();
    if (limit->quantity == 0)
        limits->remove(*limit);
}

void LimitOrderBook::write_fill_order(const Quote &quote)
{
    assert(quote.type == FillOrder);
    auto limits = quote.side == Bid ? bid_limits : ask_limits;
    auto order = uid_order_map[quote.uid];
    auto limit = order->limit.lock();
    assert(limit);
    limit->quantity -= quote.quantity;
    order->quantity -= quote.quantity;
    if (order->quantity == 0)
        uid_order_map.erase(quote.uid);
    while (!limit->orders.empty() && limit->orders.front()->quantity == 0)
        limit->orders.pop_front();
    if (limit->quantity == 0)
        limits->remove(*limit);
}

void LimitOrderBook::trade(uint64_t ask_uid, uint64_t bid_uid, uint64_t quantity)
{
    auto ask_order = uid_order_map[ask_uid];
    auto bid_order = uid_order_map[bid_uid];
    uint64_t price = ask_uid < bid_uid ? ask_order->price : bid_order->price;
    uint64_t timestamp = std::max(ask_order->timestamp, bid_order->timestamp);
    write_fill_order(Quote(ask_uid, price, quantity, timestamp, Ask, FillOrder));
    write_fill_order(Quote(bid_uid, price, quantity, timestamp, Bid, FillOrder));
}

void LimitOrderBook::match()
{
    while (!ask_limits->empty() && !bid_limits->empty() && ask_limits->min().price <= bid_limits->max().price)
    {
        Limit &ask_limit = ask_limits->min();
        Limit &bid_limit = bid_limits->max();
        std::shared_ptr<Order> &ask_order = ask_limit.orders.front();
        std::shared_ptr<Order> &bid_order = bid_limit.orders.front();
        uint64_t quantity = std::min(ask_order->quantity, bid_order->quantity);
        trade(ask_order->uid, bid_order->uid, quantity);
    }
}

void LimitOrderBook::show()
{
    std::cout << "Bid:" << std::endl;
    std::cout << *bid_limits << std::endl;

    std::cout << "Ask:" << std::endl;
    std::cout << *ask_limits << std::endl;
}

#endif // __LIMIT_ORDER_BOOK_HPP__