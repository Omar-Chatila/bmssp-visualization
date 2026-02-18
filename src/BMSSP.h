#ifndef ALGO_SEMINAR_BMSSP_H
#define ALGO_SEMINAR_BMSSP_H
#include "BlockLinkedList.h"
#include "Graph.h"

using VertexSet = std::vector<Pair>;

class BMSSP {
    Graph& graph_;
    const Vertex* source_;

    size_t n_;
    size_t k_;
    size_t t_;

    mutable std::vector<uint64_t> pivot_root_cache_;
    mutable std::vector<size_t> pivot_tree_sz_cache_;
    mutable std::vector<double> dist_cache_;
    mutable std::vector<int> last_complete_level_;

    [[nodiscard]]
    std::pair<VertexSet, VertexSet> find_pivots(const VertexSet& S, double B) const;

    std::pair<double, VertexSet> base_case(const Pair& S, double B) const;

    std::pair<double, VertexSet> bmssp(int l, double B, const VertexSet& S);
public:
    BMSSP(Graph& graph, const Vertex* src);

    BMSSP(Graph& graph, const Vertex* src, size_t k, size_t t);

    std::vector<double> run();
};


#endif //ALGO_SEMINAR_BMSSP_H