#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <cassert>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

#include "data_interface.h"


namespace ann
{

template <typename T>
class node {
public:
    node() : weight(), accu_err(0), accu_sig(1), err_radius(0) {}
    node(const my_vector<T>& w) : weight(w), accu_err(0), accu_sig(1), err_radius(0) {}

public:
    my_vector<T> weight; 
    double accu_err;
    double accu_sig;
    double err_radius;
    double get_err_radius() const {
        return accu_err / accu_sig;
    }
    void set_err_radius() {
        assert(accu_sig != 0);
        err_radius = accu_err / accu_sig;
    }
};

class edge {
public:
    edge() : id(-1), age(0) {}
    edge(int i) : id(i), age(0) {}

public:
    int id;
    int age;
};

template <typename T>
class graph {
private:
    int next_idx_;
    std::map<int, node<T>> nodes_;
    std::map<int, std::list<edge>> adj_nebs_;
    using node_itr_type_ = typename std::map<int, node<T>>::iterator;

public:
    graph() : next_idx_(0) {}

    void add_node(const my_vector<T>&);
    int add_node(const node<T>&);
    void remove_node(int);
    node_itr_type_ remove_node(node_itr_type_);
    void add_edge(int, int);
    void remove_edge(int, int);
    size_t num_nebs(int id) {
        return adj_nebs_[id].size();
    }
    std::vector<int> get_nebs(int);
    auto& get_nodes() {
        return nodes_;
    }
    auto& get_edges() {
        return adj_nebs_;
    }
    bool edge_exist(int, int);
};

template <typename T>
void graph<T>::add_node(const my_vector<T>& new_weight) {
    nodes_.emplace(next_idx_++, new_weight);
}

template <typename T>
int graph<T>::add_node(const node<T>& new_node) {
    nodes_.emplace(next_idx_, new_node);
    return next_idx_++;
}

template <typename T>
void graph<T>::remove_node(int id) {
    nodes_.erase(id);
    std::vector<std::pair<int, int>> vp;
    for (const auto& neb : adj_nebs_[id]) {
        vp.emplace_back(id, neb.id);
    }
    for (const auto& id_pair : vp) {
        remove_edge(id_pair.first, id_pair.second);
    }
}

template <typename T>
typename graph<T>::node_itr_type_ graph<T>::remove_node(node_itr_type_ itr) {
    int id = itr++->first;
    remove_node(id);
    return itr;
}

template <typename T>
void graph<T>::add_edge(int from, int to) {
    adj_nebs_[from].push_front(edge(to));
    adj_nebs_[to].push_front(edge(from));
}

template <typename T>
void graph<T>::remove_edge(int from, int to) {
    auto fd = [&](int f, int t) {
        for (auto p = adj_nebs_[f].begin(); p != adj_nebs_[f].end(); ++p) {
            if (p->id == t) {
                adj_nebs_[f].erase(p);
                break;
            }
        }
    };
    fd(from, to);
    fd(to, from);
}

template <typename T>
std::vector<int> graph<T>::get_nebs(int id) {
    std::vector<int> vi;
    for (const auto& each : adj_nebs_[id]) {
        vi.push_back(each.id);
    }
    return vi;
}

template <typename T>
bool graph<T>::edge_exist(int from, int to) {
    auto nebs = get_nebs(from);
    for (auto each : nebs) {
        if (each == to) return true;
    }
    return false;
}

} // namespace ann

#endif
