#ifndef GRAPH_FACTORY_H
#define GRAPH_FACTORY_H
#include "Graph.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <unordered_set>



inline Graph graph_from_csv(const char* filename, GraphType type) {
    Graph g(type);
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cout << "File not found -> returning empty graph \n";
        return g;
    }

    std::string str;
    while (getline(f, str)){
        uint64_t from_id;
        uint64_t to_id;
        double weight;
        double rest;
        int res = sscanf(str.c_str(),"%lu,%lu,%lf,%lf", &from_id, &to_id, &weight, &rest);
        if (res < 3) 
            sscanf(str.c_str(),"%lu,%lu,%lf", &from_id, &to_id, &weight);
            
        g.add_vertex(from_id);
        g.add_vertex(to_id);
        if (weight < 0.0)
            continue;
        g.add_edge(from_id, to_id, weight);
    }

    f.close();
    return g;
}

inline std::vector<const Vertex*> get_start_vertices(const Graph& g, const int num) {
    auto& vertices = g.get_vertices();
    std::random_device rd;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> distrib(0, static_cast<int>(g.size()) - 1);

    std::vector<const Vertex*> result;
    std::unordered_set<uint64_t> added;

    result.reserve(num);
    for (int i = 0; i < num; ++i) {
        const int rand_i = distrib(gen);
        const Vertex* candidate = &vertices.at(rand_i);
        while (candidate->outgoing_edges_.empty() || added.contains(candidate->id_)) {
            const int index = distrib(gen);
            candidate = &vertices.at(index);
        }
        result.push_back(candidate);
        added.emplace(candidate->id_);
    }
    return result;
}

#endif

