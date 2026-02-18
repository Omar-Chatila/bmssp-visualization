#include "Graph.h"

#include <cstdlib>
#define INDEX(y, x, width) ((y) * width + x)

Graph::Graph(const GraphType type) : type_(type) {
    srand(0);
}

Graph::Graph(const int width, const int height) : type_(GraphType::UNDIRECTED) {
    for (int i = 0; i < width * height; ++i) {
        this->add_vertex(i);
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < height; ++x) {
            // North
            if (y - 1 >= 0)
                this->add_edge(INDEX(y, x, width), INDEX(y - 1, x, width), 1.0);
            // South
            if (y + 1 < height)
                this->add_edge(INDEX(y, x, width), INDEX(y + 1, x, width), 1.0);
            // West
            if (x - 1 >= 0)
                this->add_edge(INDEX(y, x, width), INDEX(y, x - 1, width), 1.0);
            // East
            if (x + 1 < width)
                this->add_edge(INDEX(y, x, width), INDEX(y, x + 1, width), 1.0);
        }
    }
}

void Graph::add_vertex(const uint64_t id) {
    if (id < id_map_.size())
        return;
    vertices_.emplace_back(id);
    id_map_.resize(id + 1);
    id_map_[id] = &vertices_.back();
}

void Graph::add_edge(const uint64_t from_id, const uint64_t to_id, const double weight) {
    Vertex* v = id_map_.at(from_id);
    const double dirt =  static_cast<double>(rand() % 10000) / 1E8;
    if (type_ == GraphType::DIRECTED) {
        v->outgoing_edges_.emplace_back(to_id, weight + dirt);
    } else {
        Vertex* u = id_map_.at(to_id);
        v->outgoing_edges_.emplace_back(to_id, weight + dirt);
        u->outgoing_edges_.emplace_back(from_id, weight + dirt);
    }
    ++num_edges_;
}

const std::deque<Vertex> &Graph::get_vertices() const {
    return vertices_;
}

const Vertex *Graph::get_vertex(const uint64_t id) const {
    return id_map_[id];
}

bool Graph::empty() const {
    return vertices_.empty();
}

size_t Graph::size() const {
    return vertices_.size();
}

size_t Graph::edges_size() const {
    return num_edges_;
}