//
// Created by omar on 21.12.25.
//
#pragma once

#include <algorithm>

#include "FibHeap.h"
#include <cmath>
#include <stdexcept>
#include <vector>

template<typename T>
void FibHeap<T>::delete_subtree(Node<T>* x) {
    if (!x) return;

    Node<T>* start = x;
    Node<T>* cur = x;

    do {
        Node<T>* next = cur->right_;
        if (cur->child_) {
            delete_subtree(cur->child_);
        }
        delete cur;
        cur = next;
    } while (cur != start);
}


template<typename T>
void FibHeap<T>::consolidate() {
    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    size_t D = static_cast<size_t>(std::log(n_) / std::log(phi)) + 2;

    std::vector<Node<T>*> A(D, nullptr);

    std::vector<Node<T>*> roots;
    Node<T>* start = min_root_;
    Node<T>* w = start;
    do {
        roots.push_back(w);
        w = w->right_;
    } while (w != start);

    for (Node<T>* x : roots) {
        size_t d = x->degree_;
        while (A[d]) {
            Node<T>* y = A[d];
            if (y->key_ < x->key_) std::swap(x, y);
            link(y, x);
            A[d] = nullptr;
            ++d;
        }
        A[d] = x;
    }

    min_root_ = nullptr;
    for (Node<T>* x : A) {
        if (x) {
            if (!min_root_) {
                x->left_ = x->right_ = x;
                min_root_ = x;
            } else {
                x->left_ = min_root_;
                x->right_ = min_root_->right_;
                min_root_->right_->left_ = x;
                min_root_->right_ = x;
                if (x->key_ < min_root_->key_) {
                    min_root_ = x;
                }
            }
        }
    }
}



template<typename T>
void FibHeap<T>::link(Node<T>* y, Node<T>* x) {
    y->left_->right_ = y->right_;
    y->right_->left_ = y->left_;
    y->parent_ = x;
    y->mark_ = false;

    if (!x->child_) {
        x->child_ = y;
        y->left_ = y->right_ = y;
    } else {
        y->right_ = x->child_->right_;
        y->left_  = x->child_;
        x->child_->right_->left_ = y;
        x->child_->right_ = y;
    }
    ++x->degree_;
}

template<typename T>
void FibHeap<T>::cut(Node<T>* x) {
    Node<T>* y = x->parent_;

    if (x->right_ == x) {
        y->child_ = nullptr;
    } else {
        x->right_->left_ = x->left_;
        x->left_->right_ = x->right_;
        if (y->child_ == x) {
            y->child_ = x->right_;
        }
    }
    --y->degree_;

    x->parent_ = nullptr;
    x->left_ = min_root_;
    x->right_ = min_root_->right_;
    min_root_->right_->left_ = x;
    min_root_->right_ = x;
    x->mark_ = false;
}

template<typename T>
void FibHeap<T>::cascading_cut(Node<T> *y) {
    Node<T>* z = y->parent_;
    if (z != nullptr) {
        if (y->mark_ == false) {
            y->mark_ = true;
        } else {
            cut(y);
            cascading_cut(z);
        }
    }
}

template<typename T>
const T& FibHeap<T>::min() const {
    return min_root_->key_;
}

template<typename T>
Node<T>* FibHeap<T>::insert(const T& key) {
    auto* x = new Node<T>(key);
    if (!min_root_) {
        min_root_ = x;
    } else {
        x->right_ = min_root_->right_;
        x->left_  = min_root_;
        min_root_->right_->left_ = x;
        min_root_->right_ = x;

        if (x->key_ < min_root_->key_) {
            min_root_ = x;
        }
    }
    ++n_;
    return x;
}

template<typename T>
void FibHeap<T>::merge(FibHeap& other) {
    if (other.empty()) return;

    if (empty()) {
        min_root_ = other.min_root_;
        n_ = other.n_;
    } else {
        Node<T>* a = min_root_;
        Node<T>* b = other.min_root_;

        Node<T>* a_right = a->right_;
        Node<T>* b_left  = b->left_;

        a->right_ = b;
        b->left_  = a;

        a_right->left_ = b_left;
        b_left->right_ = a_right;

        if (b->key_ < a->key_) {
            min_root_ = b;
        }

        n_ += other.n_;
    }

    other.min_root_ = nullptr;
    other.n_ = 0;
}

template<typename T>
bool FibHeap<T>::empty() const {
    return this->n_ == 0;
}

template<typename T>
T FibHeap<T>::extract_min() {
    Node<T>* z = min_root_;
    if (!z) {
        throw std::runtime_error("extract_min on empty heap");
    }

    // add children of z to the root list
    if (z->child_) {
        Node<T>* c = z->child_;
        Node<T>* start = c;
        do {
            Node<T>* next = c->right_;
            c->parent_ = nullptr;

            // insert c into the root list
            c->left_ = min_root_;
            c->right_ = min_root_->right_;
            min_root_->right_->left_ = c;
            min_root_->right_ = c;

            c = next;
        } while (c != start);
    }

    // remove z from root list
    if (z->right_ == z) {
        min_root_ = nullptr;
    } else {
        z->left_->right_ = z->right_;
        z->right_->left_ = z->left_;
        min_root_ = z->right_;
        consolidate();
    }

    --n_;
    T min_key = z->key_;
    delete z;
    return min_key;
}


template<typename T>
void FibHeap<T>::decrease_key(Node<T> *x, const T &new_key) {
    x->key_ = new_key;
    Node<T>* y = x->parent_;
    if (y != nullptr and x->key_ < y->key_) {
        cut(x);
        cascading_cut(y);
    }
    if (new_key < min_root_->key_) {
        min_root_ = x;
    }
}
