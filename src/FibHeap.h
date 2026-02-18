//
// Created by omar on 21.12.25.
//

#pragma once

template<typename T>
struct Node {
    Node* parent_ = nullptr;
    Node* right_ = nullptr;
    Node* left_ = nullptr;
    Node* child_ = nullptr;
    bool mark_ = false;
    int degree_ = 0;
    T key_{};

    explicit Node(const T& k) : key_(k) {
        left_ = right_ = this;
    }

    ~Node() = default;
};

template<typename T>
class FibHeap {
private:
    Node<T>* min_root_;
    size_t n_;

    static void link(Node<T>* y, Node<T>* x);
    void consolidate();
    void cut(Node<T>* x);
    void cascading_cut(Node<T>* y);
    void delete_subtree(Node<T>* x);

public:
    explicit FibHeap() : min_root_(nullptr), n_(0) {}
    FibHeap(const FibHeap& other) = delete;
    FibHeap& operator=(const FibHeap& other) = delete;

    ~FibHeap() {
        if (min_root_) {
            delete_subtree(min_root_);
        }
    }

    const T& min() const;
    Node<T>* insert(const T& key);
    void merge(FibHeap& other);
    [[nodiscard]] bool empty() const;
    T extract_min();
    void decrease_key(Node<T>* x, const T& new_key);
};

#include "../src/FibHeap.tpp"
