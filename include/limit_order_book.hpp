#ifndef __LIMIT_ORDER_BOOK_HPP__
#define __LIMIT_ORDER_BOOK_HPP__

#include "double_linked_list.hpp"
#include "treap.hpp"
#include "struct.hpp"
#include "utils.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>

const std::vector<std::tuple<TradingStatus, uint64_t, uint64_t>> CHINA_A_SHARE_TRADING_SCHEDULE = {
    // Opening Call Auction: 09:15:00 - 09:25:00
    {TradingStatus::CallAuction, 33300000000000UL, 33900000000000UL},
    // Continuous Auction (Morning): 09:30:00 - 11:30:00
    {TradingStatus::ContinuousTrading, 34200000000000UL, 41400000000000},
    // Continuous Auction (Afternoon): 13:00:00 - 14:57:00
    {TradingStatus::ContinuousTrading, 46800000000000UL, 53820000000000UL},
    // Closing Call Auction: 14:57:00 - 15:00:00
    {TradingStatus::CallAuction, 53820000000000UL, 54000000000000UL},
};

class LimitOrderBook
{
    typedef std::tuple<TradingStatus, uint64_t, uint64_t> TradingHour;

    TradingStatus status = TradingStatus::ContinuousTrading;
    std::vector<TradingHour> schedule;
    size_t decimal_places;
    double scale_up, scale_down;

    std::shared_ptr<Treap<Limit>> bid_limits, ask_limits;
    std::shared_ptr<Limit> bid_best_limit, ask_best_limit; // TODO: get best limit in O(1) time
    std::unordered_map<uint64_t, std::shared_ptr<Limit>> bid_price_map, ask_price_map;
    std::unordered_map<uint64_t, std::shared_ptr<Order>> uid_order_map;

    std::deque<Transaction> transactions;
    std::deque<Tick> ticks;
    std::deque<Quote> quotes;

    uint64_t open, high, low, close, volume, amount;
    size_t topk;
    uint64_t start_of_day, snapshot_gap;

    void write_limit_order(const Quote &quote);
    void write_market_order(const Quote &quote);
    void write_best_price_order(const Quote &quote);
    void write_cancel_order(const Quote &quote);
    void write_fill_order(const Quote &quote);
    void write_chinext_limit_order(const Quote &quote); // TODO: Support ChiNext Market
    void write_chinext_cancel_order(const Quote &quote);
    void track_transaction(const Transaction &transaction);

    void on_period_start(TradingStatus status, uint64_t timestamp);
    void on_period_end(TradingStatus status, uint64_t timestamp);
    void execute(std::tuple<TradingStatus, uint64_t, uint64_t> &period);

    inline uint64_t double2int(double value) { return (uint64_t)(value * scale_up); }
    inline double int2double(uint64_t value) { return (double)value * scale_down; }
    inline std::string int2string(uint64_t value)
    {
        uint64_t scale = (uint64_t)pow(10, decimal_places);
        std::string integer_part = std::to_string(value / scale);
        std::string fractional_part = std::to_string(value % scale);
        fractional_part = std::string(decimal_places - fractional_part.size(), '0') + fractional_part;
        return integer_part + "." + fractional_part;
    }
    inline uint64_t shift_timestamp(uint64_t timestamp) { return timestamp < nanoseconds_per_day ? timestamp + start_of_day : timestamp; }

public:
    LimitOrderBook(size_t decimal_places = 2, uint64_t snapshot_gap = 0, size_t topk = 5)
        : decimal_places(decimal_places),
          scale_up(std::pow(10, decimal_places)),
          scale_down(1.0 / scale_up),
          bid_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())),
          ask_limits(std::make_shared<Treap<Limit>>(Treap<Limit>())),
          bid_best_limit(nullptr),
          ask_best_limit(nullptr),
          open(0), high(0), low(0), close(0), volume(0), amount(0),
          topk(topk),
          start_of_day(0), snapshot_gap(snapshot_gap) {}
    void clear();
    void write(const Quote &quote);
    void trade(uint64_t ask_uid, uint64_t bid_uid, uint64_t quantity, uint64_t price = 0, uint64_t timestamp = 0);

    void set_status(TradingStatus status) { this->status = status; }
    void set_status(const std::string &status);
    void set_schedule(const std::vector<TradingHour> &schedule) { this->schedule = schedule; }
    void set_schedule(const std::string &schedule);

    void set_snapshot_gap(uint64_t snapshot_gap) { this->snapshot_gap = snapshot_gap; }

    void match(uint64_t ref_price = 0, uint64_t timestamp = 0);
    void match_call_auction(uint64_t timestamp = 0);

    void show(size_t n = 10);
    void show_transactions(size_t n = 10);

    size_t load(const std::string &filename, bool header = true);
    void until(uint64_t timestamp);
    void run();

    std::vector<Transaction> get_transactions() const { return std::vector<Transaction>(transactions.begin(), transactions.end()); }
    std::vector<Tick> get_ticks() const { return std::vector<Tick>(ticks.begin(), ticks.end()); }

    std::vector<double> get_topk_bid_price(size_t k, bool fill = false);
    std::vector<double> get_topk_ask_price(size_t k, bool fill = false);
    std::vector<uint64_t> get_topk_bid_volume(size_t k, bool fill = false);
    std::vector<uint64_t> get_topk_ask_volume(size_t k, bool fill = false);
    double get_kth_bid_price(size_t k);
    double get_kth_ask_price(size_t k);
    uint64_t get_kth_bid_volume(size_t k);
    uint64_t get_kth_ask_volume(size_t k);
};

void LimitOrderBook::on_period_start(TradingStatus status, uint64_t timestamp)
{
    ;
}

void LimitOrderBook::on_period_end(TradingStatus status, uint64_t timestamp)
{
    switch (status)
    {
    case TradingStatus::CallAuction:
        match_call_auction(timestamp);
        break;

    case TradingStatus::Snapshot:
        ticks.emplace_back(timestamp,
                           open == 0 ? std::nan("") : int2double(open),
                           high == 0 ? std::nan("") : int2double(high),
                           low == 0 ? std::nan("") : int2double(low),
                           close == 0 ? std::nan("") : int2double(close),
                           volume,
                           amount,
                           get_topk_bid_price(topk, true),
                           get_topk_ask_price(topk, true),
                           get_topk_bid_volume(topk, true),
                           get_topk_ask_volume(topk, true));
        open = high = low = volume = amount = 0;
        break;

    default:
        break;
    }
}

void LimitOrderBook::set_schedule(const std::string &schedule)
{
    if (schedule == "AShare")
        this->schedule = CHINA_A_SHARE_TRADING_SCHEDULE;
    else
        throw std::invalid_argument("Unknown trading schedule: " + schedule);
}

void LimitOrderBook::execute(std::tuple<TradingStatus, uint64_t, uint64_t> &period)
{
    on_period_start(std::get<0>(period), shift_timestamp(std::get<1>(period)));
    set_status(std::get<0>(period));
    until(std::get<2>(period));
    on_period_end(std::get<0>(period), shift_timestamp(std::get<2>(period)));
}

std::vector<double> LimitOrderBook::get_topk_bid_price(size_t k, bool fill)
{
    auto nodes = bid_limits->nlargest(k);
    std::vector<double> prices;
    for (auto &node : nodes)
        prices.push_back(int2double(node->value().price));
    if (fill)
        while (prices.size() < k)
            prices.push_back(std::nan(""));
    return prices;
}

std::vector<double> LimitOrderBook::get_topk_ask_price(size_t k, bool fill)
{
    auto nodes = ask_limits->nsmallest(k);
    std::vector<double> prices;
    for (auto &node : nodes)
        prices.push_back(int2double(node->value().price));
    if (fill)
        while (prices.size() < k)
            prices.push_back(std::nan(""));
    return prices;
}

std::vector<uint64_t> LimitOrderBook::get_topk_bid_volume(size_t k, bool fill)
{
    auto nodes = bid_limits->nlargest(k);
    std::vector<uint64_t> quantities;
    for (auto &node : nodes)
        quantities.push_back(node->value().quantity);
    if (fill)
        while (quantities.size() < k)
            quantities.push_back(0);
    return quantities;
}

std::vector<uint64_t> LimitOrderBook::get_topk_ask_volume(size_t k, bool fill)
{
    auto nodes = ask_limits->nsmallest(k);
    std::vector<uint64_t> quantities;
    for (auto &node : nodes)
        quantities.push_back(node->value().quantity);
    if (fill)
        while (quantities.size() < k)
            quantities.push_back(0);
    return quantities;
}

double LimitOrderBook::get_kth_bid_price(size_t k)
{
    auto node = bid_limits->kth_largest(k);
    return node ? int2double(node->value().price) : 0;
}

double LimitOrderBook::get_kth_ask_price(size_t k)
{
    auto node = ask_limits->kth_smallest(k);
    return node ? int2double(node->value().price) : 0;
}

uint64_t LimitOrderBook::get_kth_bid_volume(size_t k)
{
    auto node = bid_limits->kth_largest(k);
    return node ? node->value().quantity : 0;
}

uint64_t LimitOrderBook::get_kth_ask_volume(size_t k)
{
    auto node = ask_limits->kth_smallest(k);
    return node ? node->value().quantity : 0;
}

void LimitOrderBook::clear()
{
    bid_limits->clear();
    ask_limits->clear();
    uid_order_map.clear();
    bid_best_limit = nullptr;
    ask_best_limit = nullptr;
    open = high = low = close = volume = 0;
}

void LimitOrderBook::set_status(const std::string &status)
{
    assert(status == "CallAuction" || status == "ContinuousTrading" || status == "ClosingAuction");
    if (status == "CallAuction" || status == "ClosingAuction")
        this->status = TradingStatus::CallAuction;
    else
        this->status = TradingStatus::ContinuousTrading;
}

void LimitOrderBook::write(const Quote &quote)
{
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
}

void LimitOrderBook::write_limit_order(const Quote &quote)
{
    assert(quote.type == QuoteType::LimitOrder);
    // create order
    auto order = std::make_shared<Order>(quote.uid, quote.price, quote.quantity, quote.timestamp);

    if (uid_order_map.find(quote.uid) != uid_order_map.end())
        throw std::runtime_error("order already exists");
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
    assert(status == TradingStatus::ContinuousTrading);
    if ((quote.side == Side::Bid && ask_limits->empty()) || (quote.side == Side::Ask && bid_limits->empty()))
        return;
    uint64_t price = quote.side == Side::Bid ? ask_limits->min()->value().price : bid_limits->max()->value().price;
    uint64_t quantity = quote.side == Side::Bid ? ask_limits->min()->value().quantity : bid_limits->max()->value().quantity;
    quantity = std::min(quantity, quote.quantity);
    write_limit_order(Quote(quote.uid, price, quantity, quote.timestamp, quote.side, LimitOrder));
    if (quote.quantity > quantity)
        write_market_order(Quote(quote.uid, 0, quote.quantity - quantity, quote.timestamp, quote.side, MarketOrder));
}

void LimitOrderBook::write_best_price_order(const Quote &quote)
{
    assert(quote.type == QuoteType::BestPriceOrder);
    assert(status == TradingStatus::ContinuousTrading);
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

void LimitOrderBook::track_transaction(const Transaction &transaction)
{
    open = open == 0 ? transaction.price : open;
    high = high == 0 ? transaction.price : std::max(high, transaction.price);
    low = low == 0 ? transaction.price : std::min(low, transaction.price);
    close = transaction.price;
    volume += transaction.quantity;
    amount += transaction.price * transaction.quantity;
}

void LimitOrderBook::trade(uint64_t ask_uid, uint64_t bid_uid, uint64_t quantity, uint64_t price, uint64_t timestamp)
{
    auto ask_order = uid_order_map[ask_uid];
    auto bid_order = uid_order_map[bid_uid];
    if (price == 0)
        price = ask_uid < bid_uid ? ask_order->price : bid_order->price;
    if (timestamp == 0)
        timestamp = std::max(ask_order->timestamp, bid_order->timestamp);
    transactions.emplace_back(bid_uid, ask_uid, price, quantity, timestamp, int2double(price));
    track_transaction(transactions.back());
    write_fill_order(Quote(ask_uid, price, quantity, timestamp, Side::Ask, FillOrder));
    write_fill_order(Quote(bid_uid, price, quantity, timestamp, Side::Bid, FillOrder));
}

void LimitOrderBook::match(uint64_t ref_price, uint64_t timestamp)
{
    while (!ask_limits->empty() && !bid_limits->empty() && ask_limits->min()->value().price <= bid_limits->max()->value().price)
    {
        Limit &ask_limit = ask_limits->min()->value();
        Limit &bid_limit = bid_limits->max()->value();
        std::shared_ptr<Order> &ask_order = ask_limit.orders.front();
        std::shared_ptr<Order> &bid_order = bid_limit.orders.front();
        uint64_t quantity = std::min(ask_order->quantity, bid_order->quantity);
        trade(ask_order->uid, bid_order->uid, quantity, ref_price, timestamp);
    }
}

void LimitOrderBook::match_call_auction(uint64_t timestamp) // TODO: auto parse input timestamp
{
    uint64_t ref_price = 0;
    uint64_t ask_cum_quantity = 0, bid_cum_quantity = 0;
    std::shared_ptr<Node<Limit>> ask_node, bid_node;
    ask_node = ask_limits->min();
    bid_node = bid_limits->max();
    if (!ask_node || !bid_node)
        return;
    while (ask_node && bid_node && (ask_node->value().price <= ref_price || ref_price <= bid_node->value().price))
    {
        if (ask_cum_quantity < bid_cum_quantity)
        {
            ask_cum_quantity += ask_node->value().quantity;
            ref_price = ask_node->value().price;
            ask_node = ask_node->next();
        }
        else
        {
            bid_cum_quantity += bid_node->value().quantity;
            ref_price = bid_node->value().price;
            bid_node = bid_node->prev();
        }
    }
    if (ref_price == 0)
        return;
    match(ref_price, timestamp);
}

void LimitOrderBook::show(size_t n)
{
    auto table = Table<std::string, uint64_t>({"Price", "Quantity"});

    auto topk_ask_price = get_topk_ask_price(n);
    auto topk_ask_volume = get_topk_ask_volume(n);
    for (size_t i = 0; i < n - topk_ask_price.size(); i++)
        table.add_empty_row();
    for (size_t i = topk_ask_price.size(); i > 0; --i)
        table.add_row(int2string(double2int(topk_ask_price[i - 1])), topk_ask_volume[i - 1]);

    table.add_divider();
    table.add_divider();

    auto topk_bid_price = get_topk_bid_price(n);
    auto topk_bid_volume = get_topk_bid_volume(n);
    for (size_t i = 0; i < topk_bid_price.size(); i++)
        table.add_row(int2string(double2int(topk_bid_price[i])), topk_bid_volume[i]);
    for (size_t i = 0; i < n - topk_bid_price.size(); i++)
        table.add_empty_row();

    table.print(std::cout);
}

void LimitOrderBook::show_transactions(size_t n)
{
    auto table = Table<std::string, std::string, uint64_t>({"Timestamp", "Price", "Quantity"});
    for (size_t i = std::max(0, (int)transactions.size() - (int)n); i < transactions.size(); ++i)
        table.add_row(strftime(transactions[i].timestamp, "%H:%M:%S"), int2string(transactions[i].price), transactions[i].quantity);
    table.print(std::cout);
}

size_t LimitOrderBook::load(const std::string &filename, bool header)
{
    std::cout << "Loading " << filename << "..." << std::endl;
    // check it is a csv file
    if (filename.substr(filename.find_last_of(".") + 1) != "csv")
        throw std::runtime_error("file is not a csv file");

    // open file
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("file is not open");

    // read file
    std::string line;
    if (header)
        std::getline(file, line);
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> record;
        while (std::getline(ss, item, ','))
            record.push_back(item);
        if (record.size() != 6)
            throw std::runtime_error("record size is not 6");
        uint64_t timestamp = std::stoull(record[0]);
        uint64_t uid = std::stoull(record[1]);
        uint64_t price = double2int(std::stod(record[2]));
        uint64_t quantity = std::stoull(record[3]);
        Side side = static_cast<Side>(std::stoi(record[4]));
        QuoteType type = static_cast<QuoteType>(std::stoi(record[5]));

        quotes.emplace_back(uid, price, quantity, timestamp, side, type);
    }
    start_of_day = quotes.empty() ? 0 : quotes.front().timestamp - quotes.front().timestamp % nanoseconds_per_day;
    std::cout << "read " << quotes.size() << " quotes" << std::endl;
    return quotes.size();
}

void LimitOrderBook::until(uint64_t timestamp)
{
    const uint64_t oneday = 24UL * 60UL * 60UL * 1000000000UL; // unit: nanosecond
    if (quotes.empty())
        return;
    timestamp = quotes.front().timestamp - (quotes.front().timestamp % oneday) + timestamp;
    while (!quotes.empty() && quotes.front().timestamp <= timestamp)
    {
        write(quotes.front());
        quotes.pop_front();
    }
}

void LimitOrderBook::run()
{
    if (snapshot_gap != 0)
    {
        std::vector<TradingHour> tmp(schedule.begin(), schedule.end());
        schedule.clear();
        for (auto &period : tmp)
        {
            if (std::get<0>(period) == TradingStatus::CallAuction)
            {
                schedule.emplace_back(TradingStatus::CallAuction, std::get<1>(period), std::get<2>(period));
                schedule.emplace_back(TradingStatus::Snapshot, std::get<2>(period), std::get<2>(period));
            }
            else if (std::get<0>(period) == TradingStatus::ContinuousTrading)
            {
                for (uint64_t t = std::get<1>(period); t < std::get<2>(period); t += snapshot_gap)
                {
                    schedule.emplace_back(TradingStatus::ContinuousTrading, t, t + snapshot_gap);
                    schedule.emplace_back(TradingStatus::Snapshot, t + snapshot_gap, t + snapshot_gap);
                }
                if ((std::get<2>(period) - std::get<1>(period)) % snapshot_gap != 0)
                {
                    schedule.emplace_back(TradingStatus::ContinuousTrading, std::get<2>(period) - (std::get<2>(period) - std::get<1>(period)) % snapshot_gap, std::get<2>(period));
                    schedule.emplace_back(TradingStatus::Snapshot, std::get<2>(period), std::get<2>(period));
                }
            }
            else
            {
                schedule.emplace_back(std::get<0>(period), std::get<1>(period), std::get<2>(period));
            }
        }
    }

    for (auto &period : schedule)
        execute(period);
}

#endif // __LIMIT_ORDER_BOOK_HPP__
