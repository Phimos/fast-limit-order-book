#ifndef __LIMIT_ORDER_BOOK_HPP__
#define __LIMIT_ORDER_BOOK_HPP__

#include "double_linked_list.hpp"
#include "treap.hpp"
#include "struct.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

class LimitOrderBook
{
    TradingStatus status = TradingStatus::ContinuousTrading;
    size_t decimal_places; // TODO: use this to round prices (and quantities) (double <-> int64_t)

    std::shared_ptr<Treap<Limit>> bid_limits, ask_limits;
    std::shared_ptr<Limit> bid_best_limit, ask_best_limit; // TODO: get best limit in O(1) time
    std::unordered_map<uint64_t, std::shared_ptr<Limit>> bid_price_map, ask_price_map;
    std::unordered_map<uint64_t, std::shared_ptr<Order>> uid_order_map;

    std::unordered_map<uint64_t, std::shared_ptr<Order>> call_auction_orders;

public:
    LimitOrderBook(size_t decimal_places = 2)
        : decimal_places(decimal_places),
          bid_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())),
          ask_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())),
          bid_best_limit(nullptr),
          ask_best_limit(nullptr) {}
    void clear();
    void write(const Quote &quote);
    void write_limit_order(const Quote &quote);
    void write_market_order(const Quote &quote);
    void write_best_price_order(const Quote &quote);
    void write_cancel_order(const Quote &quote);
    void write_fill_order(const Quote &quote);
    void trade(uint64_t ask_uid, uint64_t bid_uid, uint64_t quantity, uint64_t price = 0);

    void set_status(TradingStatus status) { this->status = status; }
    void set_status(const std::string &status);

    void match();
    void match_call_auction();

    void show();
};

void LimitOrderBook::clear()
{
    bid_limits->clear();
    ask_limits->clear();
    uid_order_map.clear();
}

void LimitOrderBook::set_status(const std::string &status)
{
    assert(status == "CallAuction" || status == "ContinuousTrading");
    if (status == "CallAuction")
        this->status = TradingStatus::CallAuction;
    else
        this->status = TradingStatus::ContinuousTrading;
}

void LimitOrderBook::write(const Quote &quote)
{
    switch (status)
    {
    case TradingStatus::ContinuousTrading:
        switch (quote.type)
        {
        case QuoteType::LimitOrder:
            write_limit_order(quote);
            break;
        case QuoteType::MarketOrder:
            write_market_order(quote);
            break;
        case QuoteType::BestPriceOrder:
            write_best_price_order(quote);
            break;
        case QuoteType::CancelOrder:
            write_cancel_order(quote);
            break;
        case QuoteType::FillOrder:
            write_fill_order(quote);
            break;
        }
        break;
    case TradingStatus::CallAuction:
        // TODO: implement call auction
        switch (quote.type)
        {
        case QuoteType::LimitOrder:
            break;
        case QuoteType::CancelOrder:
            break;
        case QuoteType::FillOrder:
            break;
        }
    }
}

void LimitOrderBook::write_limit_order(const Quote &quote)
{
    assert(quote.type == QuoteType::LimitOrder);
    // create order
    auto order = std::make_shared<Order>(quote.uid, quote.price, quote.quantity, quote.timestamp);
    assert(uid_order_map.find(quote.uid) == uid_order_map.end());
    uid_order_map[quote.uid] = order;
    auto &limits = quote.side == Side::Bid ? bid_limits : ask_limits;
    auto &price_map = quote.side == Side::Bid ? bid_price_map : ask_price_map;
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
    assert(quote.type == QuoteType::MarketOrder);
    if ((quote.side == Side::Bid && ask_limits->empty()) || (quote.side == Side::Ask && bid_limits->empty()))
        return;
    uint64_t price = quote.side == Side::Bid ? ask_limits->min()->value().price : bid_limits->max()->value().price;
    write_limit_order(Quote(quote.uid, price, quote.quantity, quote.timestamp, quote.side, LimitOrder));
}

void LimitOrderBook::write_best_price_order(const Quote &quote)
{
    assert(quote.type == QuoteType::BestPriceOrder);
    if ((quote.side == Side::Bid && bid_limits->empty()) || (quote.side == Side::Ask && ask_limits->empty()))
        return;
    uint64_t price = quote.side == Side::Bid ? bid_limits->max()->value().price : ask_limits->min()->value().price;
    write_limit_order(Quote(quote.uid, price, quote.quantity, quote.timestamp, quote.side, LimitOrder));
}

void LimitOrderBook::write_cancel_order(const Quote &quote)
{
    assert(quote.type == QuoteType::CancelOrder);
    auto &limits = quote.side == Side::Bid ? bid_limits : ask_limits;
    auto &price_map = quote.side == Side::Bid ? bid_price_map : ask_price_map;
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
    {
        limits->remove(*limit);
        price_map.erase(limit->price);
    }
}

void LimitOrderBook::write_fill_order(const Quote &quote)
{
    assert(quote.type == QuoteType::FillOrder);
    auto &limits = quote.side == Side::Bid ? bid_limits : ask_limits;
    auto &price_map = quote.side == Side::Bid ? bid_price_map : ask_price_map;
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
    {
        limits->remove(*limit);
        price_map.erase(limit->price);
    }
}

void LimitOrderBook::trade(uint64_t ask_uid, uint64_t bid_uid, uint64_t quantity, uint64_t price)
{
    auto ask_order = uid_order_map[ask_uid];
    auto bid_order = uid_order_map[bid_uid];
    if (price == 0)
        price = ask_uid < bid_uid ? ask_order->price : bid_order->price;
    uint64_t timestamp = std::max(ask_order->timestamp, bid_order->timestamp);
    write_fill_order(Quote(ask_uid, price, quantity, timestamp, Side::Ask, FillOrder));
    write_fill_order(Quote(bid_uid, price, quantity, timestamp, Side::Bid, FillOrder));
}

void LimitOrderBook::match()
{
    while (!ask_limits->empty() && !bid_limits->empty() && ask_limits->min()->value().price <= bid_limits->max()->value().price)
    {
        Limit &ask_limit = ask_limits->min()->value();
        Limit &bid_limit = bid_limits->max()->value();
        std::shared_ptr<Order> &ask_order = ask_limit.orders.front();
        std::shared_ptr<Order> &bid_order = bid_limit.orders.front();
        uint64_t quantity = std::min(ask_order->quantity, bid_order->quantity);
        trade(ask_order->uid, bid_order->uid, quantity);
    }
}

void LimitOrderBook::match_call_auction()
{
    uint64_t ref_price;
    uint64_t ask_cum_quantity = 0, bid_cum_quantity = 0;
    std::shared_ptr<Node<Limit>> ask_node, bid_node;
    ask_node = ask_limits->min();
    bid_node = bid_limits->max();
    while (ask_node && bid_node && ask_node->value().price < bid_node->value().price)
    {
        if (ask_cum_quantity < bid_cum_quantity)
        {
            ask_cum_quantity += ask_node->value().quantity;
            ask_node = ask_node->next();
        }
        else
        {
            bid_cum_quantity += bid_node->value().quantity;
            bid_node = bid_node->prev();
        }
    }
    ref_price = ask_node ? ask_node->value().price : bid_node->value().price;
}

void LimitOrderBook::show() // TODO: better show function
{
    std::cout << "Side::Bid:" << std::endl;
    std::cout << *bid_limits << std::endl;

    std::cout << "Side::Ask:" << std::endl;
    std::cout << *ask_limits << std::endl;
}

#endif // __LIMIT_ORDER_BOOK_HPP__