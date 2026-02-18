//
// Created by omar on 28.12.25.
//

#ifndef ALGO_SEMINAR_DIJKSTRA_H
#define ALGO_SEMINAR_DIJKSTRA_H

#include <queue>

#include "Graph.h"
#include "FibHeap.h"
#include "BlockLinkedList.h"


struct HeapKey {
    double dist;
    const Vertex* v;

    bool operator<(const HeapKey& key) const {
        return this->dist < key.dist;
    }
};

struct DijkstraState {
    double dist_ = std::numeric_limits<double>::infinity();
    bool finalized_ = false;
    Node<HeapKey>* heap_node_ = nullptr;

    bool operator<(const DijkstraState& ds) const {
        return this->dist_ < ds.dist_;
    }
    bool operator==(const DijkstraState& ds) = delete;
    bool operator>(const DijkstraState& ds) = delete;
};

enum class EventType {
    Start,
    ExtractMin,
    Relax,
    PushPQ,
    Done
};

struct DijkstraFrame {
    EventType event;
    std::vector<double> dist;
    std::vector<bool> finalized;
    std::vector<uint64_t> pq_vertices;
    uint64_t current = -1;
};

class Dijkstra {
private:
    Graph& graph_;
    const Vertex *source_;
    std::vector<DijkstraFrame> states_;

    static DijkstraFrame make_state(EventType type, const std::vector<double>& dist, const std::vector<bool>& finalized, std::priority_queue<Pair, std::vector<Pair>, std::greater<>> pq, uint64_t current);

public:
    explicit Dijkstra(Graph& graph, const Vertex* src);

    [[nodiscard]] std::vector<double> fib_heap_run() const;

    [[nodiscard]] std::vector<double> std_heap_run();

    [[nodiscard]] std::vector<DijkstraFrame> frames() const {
        return states_;
    }
};


#endif //ALGO_SEMINAR_DIJKSTRA_H