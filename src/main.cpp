#include "../include/double_linked_list.hpp"
#include "../include/treap.hpp"
#include "../include/limit_order_book.hpp"

#include <iostream>
#include <string>

int main()
{
    std::cout << "Hello World!" << std::endl;
    DoubleLinkedList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    std::cout << list << std::endl;
    list.pop_back();
    list.pop_front();
    std::cout << list << std::endl;

    Treap<int> treap;
    treap.insert(1);
    treap.insert(2);
    treap.insert(3);
    treap.insert(7);
    treap.insert(4);
    std::cout << treap << std::endl;
    std::cout << treap.min() << ' ' << treap.max() << std::endl;
    treap.remove(3);
    std::cout << treap << std::endl;
    treap.remove(1);
    std::cout << treap.min() << ' ' << treap.max() << std::endl;
    treap.clear();
    std::cout << "treap cleared" << std::endl;

    LimitOrderBook lob;
    lob.write(Quote(1, 1, 1, 1, Bid, LimitOrder));
    lob.write(Quote(2, 2, 2, 2, Bid, LimitOrder));
    lob.show();
    lob.write(Quote(4, 2, 2, 2, Ask, MarketOrder));
    lob.show();
    lob.write(Quote(1, 1, 1, 1, Bid, CancelOrder));
    lob.show();
    lob.write(Quote(3, 2, 2, 2, Ask, MarketOrder));
    lob.show();
    std::cout << "lob written" << std::endl;
}