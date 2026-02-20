#include "BMSSP.h"

#include <cmath>
#include <functional>
#include <queue>
#include <ranges>

static constexpr double INF = std::numeric_limits<double>::infinity();


BMSSP::BMSSP(Graph &graph, const Vertex* src) : graph_(graph), source_(src) {
    n_ = graph.get_vertices().size();
    k_ = static_cast<size_t>(std::pow(std::log2(n_), 1.0/3.0));
    t_ = static_cast<size_t>(std::pow(std::log2(n_), 2.0/3.0));

    pivot_root_cache_.reserve(n_);
    pivot_tree_sz_cache_.reserve(n_);
    dist_cache_.assign(n_, INF);
    last_complete_level_.resize(n_, -1);
    finalized_.resize(n_, false);
}

BMSSP::BMSSP(Graph &graph, const Vertex* src, const size_t k, const size_t t) : graph_(graph), source_(src), n_(graph.size()), k_(k), t_(t) {
    pivot_root_cache_.reserve(n_);
    pivot_tree_sz_cache_.reserve(n_);
    dist_cache_.assign(n_, INF);
    last_complete_level_.resize(n_, -1);
    finalized_.resize(n_, false);
}

std::pair<VertexSet, VertexSet> BMSSP::find_pivots(const VertexSet& S, const double B) const {

    VertexSet W = S;
    VertexSet W_prev = S;

    pivot_root_cache_.assign(n_, 0);
    pivot_tree_sz_cache_.assign(n_, 0);

    std::vector<bool> pivot_visited_(n_, false);
    for (const auto& [u, du] : S) {
        pivot_root_cache_[u->id_] = u->id_;
        pivot_visited_[u->id_] = true;
    }

    for (size_t i = 1; i <= k_; ++i) {
        VertexSet Wi;
        for (const auto& [u, d_u] : W_prev) {
            for (const auto& [v_id, w_uv] : u->outgoing_edges_) {
                const Vertex *v = graph_.get_vertex(v_id);
                const double cand = d_u + w_uv;
                if (cand < B and cand <= dist_cache_[v->id_]) {
                    dist_cache_[v->id_] = cand;
                    if (dist_cache_[v->id_] < B) {
                        pivot_root_cache_[v->id_] = pivot_root_cache_[u->id_];
                        if (!pivot_visited_[v->id_]) {
                            pivot_visited_[v->id_] = true;
                            Wi.emplace_back(v, cand);
                        }
                    }
                }
            }
        }
        if (Wi.empty()) break;

        W.insert(W.end(), Wi.begin(), Wi.end());
        W_prev = std::move(Wi);

        if (W.size() > k_ * S.size()) {
            return {S, std::move(W)};
        }
    }

    for (const auto& [vtx, _] : W) {
        pivot_tree_sz_cache_[pivot_root_cache_[vtx->id_]]++;
    }

    VertexSet P;
    P.reserve(W.size() / k_);
    for (const auto& [u, du] : S) {
        if (pivot_tree_sz_cache_[u->id_] >= k_)
            P.emplace_back(u, du);
    }

    return {std::move(P), std::move(W)};
}

std::pair<double, VertexSet> BMSSP::base_case(const Pair& S, const double B) {
    const auto& [v_ptr, v_dist] = S;

    std::priority_queue<Pair, VertexSet, std::function<bool(const Pair&, const Pair&)>> H(
        [](const Pair& a, const Pair& b) {
            return b < a;
        });
    H.emplace(v_ptr, v_dist);

    VertexSet U;
    U.reserve(k_ + 1);

    while (!H.empty() and U.size() < k_ + 1) {
        const auto [u, d_u] = H.top();
        H.pop();

        if (d_u > dist_cache_[u->id_]) continue;
        finalized_[u->id_] = true;
        U.emplace_back(u, d_u);
        for (const auto& [v_id, w_uv] : u->outgoing_edges_) {
            const Vertex *v = graph_.get_vertex(v_id);
            const double cand = d_u + w_uv;
            if (cand < B and cand <= dist_cache_[v->id_]) {
                dist_cache_[v->id_] = cand;
                H.emplace(v, cand);
            }
        }
    }

    if (U.size() <= k_) {
        push_state(BMSSP_Event::BaseCase, 0, B, dist_cache_, finalized_, U,{}, S.key_->id_);
        return {B, U};
    }

    double B_new = dist_cache_[U.back().key_->id_];
    U.pop_back();
    push_state(BMSSP_Event::BaseCase, 0,B, dist_cache_, finalized_, U,{}, S.key_->id_);

    return {B_new, std::move(U)};
}

std::pair<double, VertexSet> BMSSP::bmssp(const int l, const double B, const VertexSet& S) {
    push_state(BMSSP_Event::RecurseEnter, l, B, dist_cache_, finalized_, S, {}, -1);
    if (l == 0) {
        return base_case(S[0], B);
    }

    auto [P, W] = find_pivots(S, B);
    push_state(BMSSP_Event::Pivots, l, B, dist_cache_, finalized_, S,P,-1);

    const auto M = static_cast<size_t>(std::pow(2, (l - 1) * t_));
    DequeueBlocks D(n_, M, B);
    double B_prime = B;
    for (const auto& [vtx, dist_v] : P) {
        D.insert(vtx, dist_v);
        B_prime = std::min(B_prime, dist_v);
    }
    push_state(BMSSP_Event::Frontier, l, B_prime,dist_cache_, finalized_,P,P,-1);

    VertexSet U;
    const auto cap = static_cast<size_t>(std::pow(2, l * t_));
    U.reserve(W.size() + cap);

    while (U.size() < cap and not D.empty()) {
        auto [Si, Bi] = D.pull();
        if (Si.empty()) break;
        push_state(BMSSP_Event::Pull, l, Bi, dist_cache_, finalized_,Si,{},-1);

        auto [Bi_prime, Ui] = bmssp(l - 1, Bi, Si);
        push_state(BMSSP_Event::RecurseExit,l - 1, Bi_prime, dist_cache_, finalized_,Ui,{},-1);
        U.insert(U.end(), Ui.begin(), Ui.end());
        push_state(BMSSP_Event::Frontier, l, B_prime, dist_cache_, finalized_,U,{},-1);

        VertexSet K;
        for (const auto& [u, du] : Ui) {
            D.erase(u);
            last_complete_level_[u->id_] = l;
            finalized_[u->id_] = true;
            for (const auto& [v_id, w_uv] : u->outgoing_edges_) {
                const Vertex *v = graph_.get_vertex(v_id);
                const double cand = du + w_uv;
                if (cand <= dist_cache_[v->id_]) {
                    dist_cache_[v->id_] = cand;
                    if (cand >= Bi and cand < B) {
                        D.insert(v, cand);
                    } else if (cand >= Bi_prime and cand < Bi) {
                        K.emplace_back(v, cand);
                    }
                }
            }
        }
        for (const auto& [x, dx] : Si) {
            if (dx >= Bi_prime and dx < Bi) {
                K.emplace_back(x, dx);
            }
        }
        D.batch_prepend(K, Bi_prime);
        B_prime = Bi_prime;
    }

    const double resB = D.empty() ? B : B_prime;

    for (const auto& [vtx, dv] : W) {
        if (last_complete_level_[vtx->id_] != l and dist_cache_[vtx->id_] < resB) {
            last_complete_level_[vtx->id_] = l;
            finalized_[vtx->id_] = true;
            U.emplace_back(vtx, dist_cache_[vtx->id_]);
        }
    }
    push_state(BMSSP_Event::Done, l, resB, dist_cache_, finalized_, U,{},-1);
    return {resB, std::move(U)};
}

std::vector<double> BMSSP::run() {
    const int l = std::ceil(std::log2(n_) / static_cast<double>(t_));

    const VertexSet S = {{source_, 0.0}};
    constexpr double B = INF;
    dist_cache_[source_->id_] = 0;

    push_state(BMSSP_Event::Start, l, B, dist_cache_, finalized_,{S}, {}, source_->id_);

    bmssp(l, B, S);

    return std::move(dist_cache_);
}

void BMSSP::push_state(BMSSP_Event type, int level, double B,
                            const std::vector<double>& dist,
                            const std::vector<bool>& finalized,
                            VertexSet frontier,
                            VertexSet pivots,
                            const uint64_t current) {
    BMSSP_Frame f;
    f.event = type;
    f.level = level;
    f.B = B;
    f.dist = dist;
    f.finalized = finalized;
    f.frontier = frontier
             | std::views::transform([](const Pair& p){ return p.key_->id_; })
             | std::ranges::to<std::vector<uint64_t>>();
    f.pivots = pivots
            | std::views::transform([](const Pair& p) { return p.key_->id_; })
            | std::ranges::to<std::vector<uint64_t>>();
    f.current = current;
    frames_.push_back(f);
}