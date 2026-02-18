#include "Dijkstra.h"

#include <queue>

#include "BlockLinkedList.h"
#include "FibHeap.h"

Dijkstra::Dijkstra(Graph& graph, const Vertex* src) : graph_(graph), source_(src) {}

std::vector<double> Dijkstra::fib_heap_run() const {
    const size_t n = graph_.size();
    auto& vertices = graph_.get_vertices();
    std::vector<DijkstraState> states_(n);
    std::vector<bool> state_exists(n, false);
    for (const auto& v : vertices) {
        const Vertex* vertex = &v;
        states_[vertex->id_] = DijkstraState{std::numeric_limits<double>::infinity(), false, nullptr};
        state_exists[vertex->id_] = true;
    }
    states_[source_->id_].dist_ = 0;
    FibHeap<HeapKey> priority_queue;
    states_[source_->id_].heap_node_ = priority_queue.insert({0, source_});

    while (!priority_queue.empty()) {
        auto [dist_u, u] = priority_queue.extract_min();

        if (states_[u->id_].finalized_ == true)
            continue;

        states_[u->id_].finalized_ = true;
        states_[u->id_].heap_node_ = nullptr;

        for (const auto& [v_id, weight] : u->outgoing_edges_) {
            const Vertex* v = graph_.get_vertex(v_id);
            if (states_[v->id_].finalized_)
                continue;

            const double new_weight = states_[u->id_].dist_ + weight;
            if (new_weight < states_[v->id_].dist_) {
                HeapKey v_key{new_weight, v};
                const auto v_node = states_[v->id_].heap_node_;
                if (v_node == nullptr) {
                    states_[v->id_].heap_node_ = priority_queue.insert(v_key);
                } else {
                    priority_queue.decrease_key(v_node, v_key);
                }
                states_[v->id_].dist_ = new_weight;
            }
        }
    }

    std::vector<double> result(n);
    for (size_t i = 0; i < states_.size(); ++i)
        result[i] = states_[i].dist_;

    return result;
}

std::vector<double> Dijkstra::std_heap_run() {
    states_.clear();

    const size_t n = graph_.size();
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    std::vector<bool> finalized(n, false);

    dist[source_->id_] = 0.0;

    std::priority_queue<Pair, std::vector<Pair>, std::greater<>> pq;
    pq.emplace(source_, 0.0);
    states_.push_back(make_state(EventType::Start, dist, finalized, pq, -1));

    while (!pq.empty()) {
        auto [u, dist_u] = pq.top();
        pq.pop();

        if (finalized[u->id_])
            continue;

        finalized[u->id_] = true;
        states_.push_back(make_state(EventType::Done, dist, finalized, pq, u->id_));

        for (const auto& [v_id, w_uv] : u->outgoing_edges_) {
            if (finalized[v_id]) continue;

            const double cand = dist_u + w_uv;
            if (cand < dist[v_id]) {
                dist[v_id] = cand;
                pq.emplace(graph_.get_vertex(v_id), cand);

                states_.push_back(make_state(EventType::Relax, dist, finalized, pq, u->id_));
            }
        }
    }

    return dist;
}

DijkstraFrame Dijkstra::make_state(EventType type, const std::vector<double> &dist, const std::vector<bool> &finalized, std::priority_queue<Pair, std::vector<Pair>, std::greater<> > pq, const uint64_t current) {
    DijkstraFrame s;
    s.event = type;
    s.dist = dist;
    s.finalized = finalized;
    s.current = current;

    while (!pq.empty()) {
        s.pq_vertices.push_back(pq.top().key_->id_);
        pq.pop();
    }
    return s;
}
