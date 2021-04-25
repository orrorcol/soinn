#ifndef __SOINN_H__
#define __SOINN_H__

#include <iostream>
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <fstream>
#include <vector>
#include <string>
#include <utility>

#include "data_interface.h"
#include "graph.h"

namespace ann
{

template<typename T>
class soinn {
public:
    soinn<T>(int layer, int dead_age = 50, int lamda = 100, double c_noise = 0.5);
    ~soinn<T>();

    void learn(const std::vector<my_vector<T>>&);
    void save_layer(int, std::string) const;
    std::pair<float*, size_t> get_cendroids() const;
    
private:
    int edge_dead_age_;
    int lamda_;
    double c_noise_;
    double alpha1_;
    double alpha2_;
    double alpha3_;
    double beta_;
    double gamma_;

    graph<T>* layers_;
    int cur_layer_;
    int max_layer_;
    double simi_thresh_c_;

    void _init(const std::vector<my_vector<T>>&);
    void _learn(const my_vector<T>&, size_t i);
    std::pair<int, int> _get2nearest(const my_vector<T>&) const;
    double _get_similar_thresh(int);
    void _calc_const_simi_thresh();
    node<T>& _get_node(int);
    void _set_age_zero(int, int);
    void _inc_neb_age(int);
    void _update_weight(int, const my_vector<T>&);
    void _remove_old_edges();
};

template<typename T>
soinn<T>::soinn(int layer, int dead_age, int lamda, double c_noise)
    :layers_(nullptr) {
    assert(layer > 0);
    layers_ = new graph<T>[layer];
    cur_layer_ = 0;
    max_layer_ = layer - 1;

    edge_dead_age_ = dead_age;
    lamda_ = lamda;
    c_noise_ = c_noise;
    alpha1_ = 1.0 / 6;
    alpha2_ = 0.25;
    alpha3_ = 0.25;
    beta_ = 2.0 / 3;
    gamma_ = 3.0 / 4;
}

template<typename T>
soinn<T>::~soinn<T>() {
    if (layers_ != nullptr) {
        delete [] layers_;
    }
}

template<typename T>
void soinn<T>::learn(const std::vector<my_vector<T>>& weights) {
    //int c = 10;
    //while (c--) {
    //    const auto& vec = weights[c];
    //    float* pdata = (float*)(vec.get_data());
    //    std::cout << *pdata++ << "," << *pdata << "\n";
    //}

    std::cout << "Learning layer " << cur_layer_ << std::endl;
    _init({weights[0], weights[1]});
    for (size_t i = 2; i < weights.size(); ++i) {
        _learn(weights[i], i - 1);
        std::cout << "Learning data " << i  << " , learned node " << 
            layers_[cur_layer_].get_nodes().size() << "\t\t\t\t\r";
    }
    std::cout << std::endl;
    while (++cur_layer_ <= max_layer_) {
        std::cout << "Learning layer " << cur_layer_ << std::endl;
        _calc_const_simi_thresh();
        auto pnode = layers_[cur_layer_ - 1].get_nodes().begin();
        auto pend = layers_[cur_layer_ - 1].get_nodes().end();
        _init({pnode++->second.weight, pnode->second.weight});
        int i = 0;
        while (pnode != pend) {
            _learn(pnode++->second.weight, ++i);
            std::cout << "Learned node " <<
                layers_[cur_layer_].get_nodes().size() << "\t\t\t\t\r";
        }
        std::cout << std::endl;
    }
}

template<typename T>
void soinn<T>::_init(const std::vector<my_vector<T>>& init_weights) {
    assert(init_weights.size() >= 2);
    for (const auto& each : init_weights) {
        layers_[cur_layer_].add_node(each);
    }
}

template<typename T>
void soinn<T>::_learn(const my_vector<T>& new_weight, size_t i) {
    int nearest1 = -1;
    int nearest2 = -1;

    auto n_pair = _get2nearest(new_weight);
    nearest1 = n_pair.first;
    nearest2 = n_pair.second;
    assert(nearest1 >= 0);
    assert(nearest2 >= 0);
    auto& node1 = _get_node(nearest1);
    auto& node2 = _get_node(nearest2);
    double dist1 = dist(node1.weight, new_weight);
    double dist2 = dist(node2.weight, new_weight);
    double simi_thresh1 = _get_similar_thresh(nearest1);
    double simi_thresh2 = _get_similar_thresh(nearest2);
    if (dist1 > simi_thresh1 || dist2 > simi_thresh2) {
        layers_[cur_layer_].add_node(new_weight);
    }
    else {
        if (!layers_[cur_layer_].edge_exist(nearest1, nearest2)) {
            layers_[cur_layer_].add_edge(nearest1, nearest2);
        }
        _set_age_zero(nearest1, nearest2);
        _inc_neb_age(nearest1);
        node1.accu_err += dist1;
        node1.accu_sig += 1;
        _update_weight(nearest1, new_weight);
        _remove_old_edges();

    } // end else for step 3
    /*
    * if the current number of node is a multiple of lamda_
    * a new node may be inserted
    */
    auto& nodes = layers_[cur_layer_].get_nodes();
    auto& edges = layers_[cur_layer_].get_edges();
    if (i % lamda_ == 0) {
        int max_err_node_id = -1;
        int neb_id = -1;
        double max_err = -1;
        for (const auto& each : nodes) {
            if (each.second.accu_err > max_err) {
                max_err_node_id = each.first;
                max_err = each.second.accu_err;
            }
        }
        max_err = -1;
        auto neb_ids = layers_[cur_layer_].get_nebs(max_err_node_id);
        if (neb_ids.empty()) return;
        for (auto id : neb_ids) {
            assert(nodes.find(id) != nodes.end());
            if (nodes[id].accu_err > max_err) {
                neb_id = id;
                max_err = nodes[id].accu_err;
            }
        }
        assert(nodes.find(max_err_node_id) != nodes.end());
        assert(nodes.find(neb_id) != nodes.end());
    
        node<T> node_q(nodes[max_err_node_id]);
        node<T> node_f(nodes[neb_id]);
        node_q.set_err_radius();
        node_f.set_err_radius();
        node<T> node_r;
        node_r.weight = (node_q.weight + node_f.weight) * 0.5;
        node_r.accu_err = alpha1_* (node_q.accu_err + node_f.accu_err);
        node_r.accu_sig = alpha2_ * (node_q.accu_sig + node_f.accu_sig);
        node_r.err_radius = alpha3_ * (node_q.err_radius + node_f.err_radius);
        node_q.accu_err *= beta_;
        node_q.accu_sig *= gamma_;
        node_f.accu_err *= beta_;
        node_f.accu_sig *= gamma_;
    
        bool suc = true;
        for (const auto& each : {node_r, node_f, node_q}) {
            if (each.get_err_radius() > each.err_radius) {
                suc = false;
                break;
            }
        }
        if (suc) {
            nodes[max_err_node_id] = node_q;
            nodes[neb_id] = node_f;
            int id = layers_[cur_layer_].add_node(node_r);
            layers_[cur_layer_].remove_edge(max_err_node_id, neb_id);
            layers_[cur_layer_].add_edge(max_err_node_id, id);
            layers_[cur_layer_].add_edge(neb_id, id);
        }
    
        double avg_signal = 0.0;
        for (const auto& each : nodes) {
            avg_signal += each.second.accu_sig;
        }
        avg_signal /= nodes.size();
        avg_signal *= c_noise_;
        for (auto pnode = nodes.begin(); pnode != nodes.end();) {
            size_t neb_count = edges[pnode->first].size();
            if (neb_count == 1 && pnode->second.accu_sig < avg_signal) {
                pnode = layers_[cur_layer_].remove_node(pnode);
            }
            else {
                ++pnode;
            }
        }
    
        for (auto p = nodes.begin(); p != nodes.end();) {
            if (layers_[cur_layer_].num_nebs(p->first) == 0) {
                p = nodes.erase(p);
            }
            else {
                ++p;
            }
        }
    } // end else for noise remove
}

template<typename T>
void soinn<T>::save_layer(int layer, std::string fname) const {
    std::ofstream ofs(fname, std::ios::binary);
    auto all_nodes = layers_[layer].get_nodes();
    for (auto node : all_nodes) {
        ofs.write(node.second.weight.get_data(), node.second.weight.size());
    }
    ofs.close();
}

template<typename T>
std::pair<float*, size_t> soinn<T>::get_cendroids() const {
    auto layer = max_layer_;
    auto all_nodes = layers_[layer].get_nodes();
    assert(!all_nodes.empty());

    size_t dim = all_nodes.begin()->second.weight.get_dim();
    size_t num = all_nodes.size() * dim;
    float* buf = new T[num];

    char* ptr = (char*)(buf);

    for (auto node : all_nodes) {
        memcpy(ptr, node.second.weight.get_data(), node.second.weight.size());
        ptr += node.second.weight.size();
    }

    return std::make_pair(buf, num);
}

template<typename T>
std::pair<int, int> soinn<T>::_get2nearest(const my_vector<T>& weight) const {
    int n1 = -1;
    int n2 = -1;
    double dist1_min = std::numeric_limits<double>::infinity();
    double dist2_min = std::numeric_limits<double>::infinity();
    for (const auto& each : layers_[cur_layer_].get_nodes()) {
        double d = dist(each.second.weight, weight);
        if (d < dist1_min) {
            dist2_min = dist1_min;
            n2 = n1;
            dist1_min = d;
            n1 = each.first;
        }
        else if (d < dist2_min) {
            dist2_min = d;
            n2 = each.first;
        }
    }
    return std::make_pair(n1, n2);
}

template<typename T>
double soinn<T>::_get_similar_thresh(int id) {
    if (true/*cur_layer_ == 0*/) {
        if (layers_[cur_layer_].num_nebs(id) > 0) {
            double max_dist = 0.0;
            for (auto neb_id : layers_[cur_layer_].get_nebs(id)) {
                double cur_dist = dist(_get_node(neb_id).weight, _get_node(id).weight);
                max_dist = std::max(max_dist, cur_dist);
            }
            return max_dist;
        }
        else {
            double min_dist = std::numeric_limits<double>::infinity();
            for (const auto& each : layers_[cur_layer_].get_nodes()) {
                if (each.first == id) continue;
                double cur_dist = dist(each.second.weight, _get_node(id).weight);
                min_dist = std::min(min_dist, cur_dist);
            }
            return min_dist;
        }
    }
    else {
        return simi_thresh_c_;
    }
}

template<typename T>
void soinn<T>::_calc_const_simi_thresh() {
    simi_thresh_c_ = 0.01;
}

template<typename T>
node<T>& soinn<T>::_get_node(int id) {
    return layers_[cur_layer_].get_nodes()[id];
}

template<typename T>
void soinn<T>::_set_age_zero(int n1, int n2) {
    auto& es = layers_[cur_layer_].get_edges();
    for (auto& neb : es[n1]) {
        if (neb.id == n2) neb.age = 0;
    } 
    for (auto& neb : es[n2]) {
        if (neb.id == n1) neb.age = 0;
    } 
}

template<typename T>
void soinn<T>::_inc_neb_age(int id) {
    auto& es = layers_[cur_layer_].get_edges();
    for (auto& neb : es[id]) {
        ++neb.age;
        auto p = es[neb.id].begin();
        while (p != es[neb.id].end()) {
            if (p->id == id) break;
            else ++p;
        }
        assert(p != es[neb.id].end());
        ++p->age;
    } 
}

template<typename T>
void soinn<T>::_update_weight(int id, const my_vector<T>& new_weight) {
    node<T>& winner = _get_node(id);
    double lrate_winner = 1.0 / winner.accu_sig;
    double lrate_neb = 1.0 / (100.0 * winner.accu_sig);
    winner.weight +=
        lrate_winner * (new_weight - winner.weight);
    auto nebs = layers_[cur_layer_].get_nebs(id);
    for (auto neb_id : nebs) {
        _get_node(neb_id).weight += lrate_neb * (new_weight - winner.weight);
    }
        
}

template<typename T>
void soinn<T>::_remove_old_edges() {
    for (auto& nebs : layers_[cur_layer_].get_edges()) {
        int from = nebs.first;
        for (auto p = nebs.second.begin(); p != nebs.second.end();) {
            if (p->age > edge_dead_age_) {
                p = nebs.second.erase(p);
            }
            else {
                ++p;
            }
        }
    }
}

} // namespace ann

#endif // __SOINN_H__
