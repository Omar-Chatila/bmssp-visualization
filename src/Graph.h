#pragma once

#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>

struct Edge {
    uint64_t to_id_;
    double weight_;

    explicit Edge(const uint64_t to, const double weight) : to_id_(to), weight_(weight) {}
};

struct Vertex {
    uint64_t id_;
    std::vector<Edge> outgoing_edges_;

    explicit Vertex(const uint64_t id) : id_(id) {}

    bool operator==(const Vertex &o) const {
        return this->id_ == o.id_;
    }
};

enum class GraphType {DIRECTED, UNDIRECTED};

class Graph {
private:
    GraphType type_;
    std::deque<Vertex> vertices_;
    std::vector<Vertex*> id_map_;
    size_t num_edges_{};

public:
    Graph(GraphType type);
    Graph(int width, int height);
    void add_vertex(uint64_t id);
    void add_edge(uint64_t from_id, uint64_t to_id, double weight);
    [[nodiscard]] const std::deque<Vertex>& get_vertices() const;
    [[nodiscard]] const Vertex* get_vertex(uint64_t id) const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t edges_size() const;
};
