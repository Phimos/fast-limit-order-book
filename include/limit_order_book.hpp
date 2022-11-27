#ifndef __LIMIT_ORDER_BOOK_HPP__
#define __LIMIT_ORDER_BOOK_HPP__

#include <double_linked_list.hpp>
#include <treap.hpp>
#include <struct.hpp>
#include <memory>
#include <unordered_map>

class LimitOrderBook
{
    Treap<Limit> bid_limits, ask_limits;
    std::unordered_map<uint64_t, std::shared_ptr<Order>> uid_order_map;

public:
    LimitOrderBook() = default;
    void clear();
};

void LimitOrderBook::clear()
{
    bid_limits.clear();
    ask_limits.clear();
    uid_order_map.clear();
}

#endif // __LIMIT_ORDER_BOOK_HPP__