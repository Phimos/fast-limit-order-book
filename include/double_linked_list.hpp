#ifndef __DOUBLE_LINKED_LIST_HPP__
#define __DOUBLE_LINKED_LIST_HPP__

#include <memory>
#include <iostream>

template <typename T>
struct ListNode
{
    T value;
    std::shared_ptr<ListNode> next;
    std::weak_ptr<ListNode> prev;

    ListNode(T value) : value(value) {}
};

template <typename T>
class DoubleLinkedList
{
    std::shared_ptr<ListNode<T>> head, tail;

public:
    size_t size;
    DoubleLinkedList() : head(nullptr), tail(nullptr), size(0) {}
    void push_back(const T &value);
    void push_front(const T &value);
    T pop_back();
    T pop_front();
    T &front();
    T &back();
    void show();
};

template <typename T>
void DoubleLinkedList<T>::push_back(const T &value)
{
    if (!head)
    {
        head = std::make_shared<ListNode<T>>(value);
        tail = head;
    }
    else
    {
        tail->next = std::make_shared<ListNode<T>>(value);
        tail->next->prev = tail;
        tail = tail->next;
    }
    ++size;
}

template <typename T>
void DoubleLinkedList<T>::push_front(const T &value)
{
    if (!head)
    {
        head = std::make_shared<ListNode<T>>(value);
        tail = head;
    }
    else
    {
        head->prev = std::make_shared<ListNode<T>>(value);
        head->prev->next = head;
        head = head->prev;
    }
    ++size;
}

template <typename T>
T DoubleLinkedList<T>::pop_back()
{
    if (!head)
        throw std::runtime_error("Empty list");

    T value = tail->value;
    if (head == tail)
    {
        head = nullptr;
        tail = nullptr;
    }
    else
    {
        tail = tail->prev.lock();
        tail->next = nullptr;
    }
    --size;
    return value;
}

template <typename T>
T DoubleLinkedList<T>::pop_front()
{
    if (!head)
        throw std::runtime_error("Empty list");

    T value = head->value;
    if (head == tail)
    {
        head = nullptr;
        tail = nullptr;
    }
    else
    {
        head = head->next;
        head->prev = std::weak_ptr<ListNode<T>>();
    }
    --size;
    return value;
}

template <typename T>
T &DoubleLinkedList<T>::front()
{
    if (!head)
        throw std::runtime_error("Empty list");

    return head->value;
}

template <typename T>
T &DoubleLinkedList<T>::back()
{
    if (!head)
        throw std::runtime_error("Empty list");

    return tail->value;
}

template <typename T>
void DoubleLinkedList<T>::show()
{
    std::shared_ptr<ListNode<T>> current = head;
    while (current)
    {
        std::cout << current->value << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

#endif // __DOUBLE_LINKED_LIST_HPP__