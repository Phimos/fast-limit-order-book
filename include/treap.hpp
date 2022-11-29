#ifndef __TREAP_HPP__
#define __TREAP_HPP__

#include <memory>
#include <tuple>
#include <vector>
#include <stack>

template <typename T>
struct Node : public std::enable_shared_from_this<Node<T>>
{
    std::shared_ptr<T> value_ptr;
    size_t size;
    size_t priority;
    std::shared_ptr<Node<T>> left, right;
    std::weak_ptr<Node<T>> parent;

    Node(std::shared_ptr<T> value_ptr) : value_ptr(value_ptr), size(1), priority(rand()) {}
    Node(T value) : value_ptr(std::make_shared<T>(value)), size(1), priority(rand()) {}
    void update()
    {
        size = 1;
        if (left)
            size += left->size;
        if (right)
            size += right->size;
    }
    T &value() { return *value_ptr; }
    std::shared_ptr<Node<T>> prev();
    std::shared_ptr<Node<T>> next();
};

template <typename T>
std::shared_ptr<Node<T>> Node<T>::prev()
{
    std::shared_ptr<Node<T>> node;
    if (left)
    {
        node = left;
        while (node->right)
            node = node->right;
        return node;
    }
    else
    {
        node = this->shared_from_this();
        while (node->parent.lock() && node->parent.lock()->left == node)
            node = node->parent.lock();
        return node->parent.lock();
    }
}

template <typename T>
std::shared_ptr<Node<T>> Node<T>::next()
{
    std::shared_ptr<Node<T>> node;
    if (right)
    {
        node = right;
        while (node->left)
            node = node->left;
        return node;
    }
    else
    {
        node = this->shared_from_this();
        while (node->parent.lock() && node->parent.lock()->right == node)
            node = node->parent.lock();
        return node->parent.lock();
    }
}

template <typename T>
class Treap
{
public:
    std::shared_ptr<Node<T>> root;

private:
    std::tuple<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>>
    split_by_value(std::shared_ptr<Node<T>> node, const T &value);
    std::tuple<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>>
    split_by_index(std::shared_ptr<Node<T>> node, size_t index);
    std::shared_ptr<Node<T>> merge(std::shared_ptr<Node<T>> left, std::shared_ptr<Node<T>> right);

public:
    Treap() : root(nullptr) {}
    void clear();
    bool empty();
    size_t size();
    void insert(const T &value);
    void insert(std::shared_ptr<T> value_ptr);
    void remove(const T &value);
    std::shared_ptr<Node<T>> select_by_value(const T &value);
    std::shared_ptr<Node<T>> select_by_index(size_t index);
    std::shared_ptr<Node<T>> min();
    std::shared_ptr<Node<T>> max();
    std::vector<std::shared_ptr<Node<T>>> nlargest(size_t n);
    std::vector<std::shared_ptr<Node<T>>> nsmallest(size_t n);
    std::shared_ptr<Node<T>> kth_largest(size_t k);
    std::shared_ptr<Node<T>> kth_smallest(size_t k);
};

template <typename T>
std::tuple<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>>
Treap<T>::split_by_value(std::shared_ptr<Node<T>> node, const T &value)
{
    if (!node)
        return std::make_tuple(nullptr, nullptr, nullptr);
    if (node->value() < value)
    {
        auto [left, middle, right] = split_by_value(node->right, value);
        node->right = left;
        if (left)
            left->parent = node;
        node->update();
        return std::make_tuple(node, middle, right);
    }
    else if (value < node->value())
    {
        auto [left, middle, right] = split_by_value(node->left, value);
        node->left = right;
        if (right)
            right->parent = node;
        node->update();
        return std::make_tuple(left, middle, node);
    }
    else
    {
        std::shared_ptr<Node<T>> left = node->left;
        std::shared_ptr<Node<T>> right = node->right;
        node->left = nullptr;
        node->right = nullptr;
        if (left)
            left->parent = std::weak_ptr<Node<T>>();
        if (right)
            right->parent = std::weak_ptr<Node<T>>();
        node->update();
        return std::make_tuple(left, node, right);
    }
}

template <typename T>
std::tuple<std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>, std::shared_ptr<Node<T>>>
Treap<T>::split_by_index(std::shared_ptr<Node<T>> node, size_t index)
{
    if (!node)
        return std::make_tuple(nullptr, nullptr, nullptr);
    size_t current_index = (node->left ? node->left->size : 0) + 1;
    if (index < current_index)
    {
        auto [left, middle, right] = split_by_index(node->left, index);
        node->left = right;
        if (right)
            right->parent = node;
        node->update();
        return std::make_tuple(left, middle, node);
    }
    else if (current_index < index)
    {
        auto [left, middle, right] = split_by_index(node->right, index - current_index);
        node->right = left;
        if (left)
            left->parent = node;
        node->update();
        return std::make_tuple(node, middle, right);
    }
    else
    {
        std::shared_ptr<Node<T>> left = node->left;
        std::shared_ptr<Node<T>> right = node->right;
        node->left = nullptr;
        node->right = nullptr;
        if (left)
            left->parent = std::weak_ptr<Node<T>>();
        if (right)
            right->parent = std::weak_ptr<Node<T>>();
        node->update();
        return std::make_tuple(left, node, right);
    }
}

template <typename T>
std::shared_ptr<Node<T>>
Treap<T>::merge(std::shared_ptr<Node<T>> left, std::shared_ptr<Node<T>> right)
{
    if (!left || !right)
        return left ? left : right;
    if (left->priority > right->priority)
    {
        left->right = merge(left->right, right);
        if (left->right)
            left->right->parent = left;
        left->update();
        return left;
    }
    else
    {
        right->left = merge(left, right->left);
        if (right->left)
            right->left->parent = right;
        right->update();
        return right;
    }
}

template <typename T>
void Treap<T>::clear()
{
    root = nullptr;
}

template <typename T>
bool Treap<T>::empty()
{
    return !root;
}

template <typename T>
size_t Treap<T>::size()
{
    return root ? root->size : 0;
}

template <typename T>
void Treap<T>::insert(const T &value)
{
    insert(std::make_shared<T>(value));
}

template <typename T>
void Treap<T>::insert(std::shared_ptr<T> value_ptr)
{
    auto [left, middle, right] = split_by_value(root, *value_ptr);
    if (middle)
        return;
    std::shared_ptr<Node<T>> node = std::make_shared<Node<T>>(value_ptr);
    if (left)
        left->parent = node;
    if (right)
        right->parent = node;
    node->left = left;
    node->right = right;
    node->update();
    root = node;
}

template <typename T>
void Treap<T>::remove(const T &value)
{
    auto [left, middle, right] = split_by_value(root, value);
    root = merge(left, right);
}

template <typename T>
std::shared_ptr<Node<T>> Treap<T>::select_by_value(const T &value)
{
    auto [left, middle, right] = split_by_value(root, value);
    root = merge(merge(left, middle), right);
    return middle;
}

template <typename T>
std::shared_ptr<Node<T>> Treap<T>::select_by_index(size_t index)
{
    auto [left, middle, right] = split_by_index(root, index);
    root = merge(merge(left, middle), right);
    return middle;
}

template <typename T>
std::shared_ptr<Node<T>> Treap<T>::min()
{
    std::shared_ptr<Node<T>> node = root;
    while (node->left)
        node = node->left;
    return node;
}

template <typename T>
std::shared_ptr<Node<T>> Treap<T>::max()
{
    std::shared_ptr<Node<T>> node = root;
    while (node->right)
        node = node->right;
    return node;
}

template <typename T>
std::shared_ptr<Node<T>> Treap<T>::kth_largest(size_t k)
{
    return select_by_index(size() - k + 1);
}

template <typename T>
std::shared_ptr<Node<T>> Treap<T>::kth_smallest(size_t k)
{
    return select_by_index(k);
}

template <typename T>
std::vector<std::shared_ptr<Node<T>>> Treap<T>::nlargest(size_t n)
{
    std::vector<std::shared_ptr<Node<T>>> nodes;
    std::stack<std::shared_ptr<Node<T>>> stack;
    std::shared_ptr<Node<T>> node = root;
    while (node || !stack.empty())
    {
        while (node)
        {
            stack.push(node);
            node = node->right;
        }
        node = stack.top();
        stack.pop();
        nodes.push_back(node);
        if (nodes.size() == n)
            break;
        node = node->left;
    }
    return nodes;
}

template <typename T>
std::vector<std::shared_ptr<Node<T>>> Treap<T>::nsmallest(size_t n)
{
    std::vector<std::shared_ptr<Node<T>>> nodes;
    std::stack<std::shared_ptr<Node<T>>> stack;
    std::shared_ptr<Node<T>> node = root;
    while (node || !stack.empty())
    {
        while (node)
        {
            stack.push(node);
            node = node->left;
        }
        node = stack.top();
        stack.pop();
        nodes.push_back(node);
        if (nodes.size() == n)
            break;
        node = node->right;
    }
    return nodes;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Treap<T> &treap)
{
    std::stack<std::shared_ptr<Node<T>>> stack;
    std::shared_ptr<Node<T>> node = treap.root;
    while (node || !stack.empty())
    {
        while (node)
        {
            stack.push(node);
            node = node->left;
        }
        node = stack.top();
        stack.pop();
        os << node->value() << " ";
        node = node->right;
    }
    return os;
}

#endif // __TREAP_HPP__