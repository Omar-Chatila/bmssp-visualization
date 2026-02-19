#ifndef ALGO_SEMINAR_BMSSP_H
#define ALGO_SEMINAR_BMSSP_H
#include "BlockLinkedList.h"
#include "Graph.h"

using VertexSet = std::vector<Pair>;

enum class BMSSP_Event {
    Start,
    RecurseEnter,
    Pivots,
    Pull,
    RecurseExit,
    Frontier,
    BaseCase,
    Done
};

struct BMSSP_Frame {
    BMSSP_Event event;
    int level;
    double B;
    std::vector<double> dist;
    std::vector<bool> finalized;
    std::vector<uint64_t> frontier;
    std::vector<uint64_t> pivots;
    uint64_t current = -1;
};

class BMSSP {
    Graph& graph_;
    const Vertex* source_;

    size_t n_;
    size_t k_;
    size_t t_;

    std::vector<BMSSP_Frame> frames_;
    std::vector<bool> finalized_;

    mutable std::vector<uint64_t> pivot_root_cache_;
    mutable std::vector<size_t> pivot_tree_sz_cache_;
    mutable std::vector<double> dist_cache_;
    mutable std::vector<int> last_complete_level_;

    void push_state(BMSSP_Event type, int level, double B,
                            const std::vector<double>& dist,
                            const std::vector<bool>& finalized,
                            VertexSet frontier, VertexSet pivots,
                            uint64_t current);

    [[nodiscard]]
    std::pair<VertexSet, VertexSet> find_pivots(const VertexSet& S, double B) const;

    std::pair<double, VertexSet> base_case(const Pair& S, double B) const;

    std::pair<double, VertexSet> bmssp(int l, double B, const VertexSet& S);
public:
    BMSSP(Graph& graph, const Vertex* src);

    BMSSP(Graph& graph, const Vertex* src, size_t k, size_t t);

    std::vector<double> run();

    std::vector<BMSSP_Frame> frames() const {
        return frames_;
    }
};


#endif //ALGO_SEMINAR_BMSSP_H