#ifndef __DATA_INTERFACE__
#define __DATA_INTERFACE__

#include <cmath>
#include <cstring>
#include <assert.h>

#include "utils.h"

namespace ann
{

class one_data {
public:
    virtual size_t data_size() const = 0;
    virtual double dist(const void*, const void*) const = 0;
    virtual ~one_data() {}
};

template <typename data_tp>
class euclidean_distance: public ann::one_data {
private:
    const int dim;
    const int dim_size;

public:
    euclidean_distance(int m): one_data(), dim(m), dim_size(sizeof(data_tp)) {}
    virtual size_t data_size() const {
        return dim * dim_size;
    }
    virtual double dist(const void* p1, const void* p2) const {
        const data_tp* pa = (data_tp*)p1;
        const data_tp* pb = (data_tp*)p2;
        double sum = 0;
        for (int i = 0; i < dim; ++i) {
            double diff = *pa++ - *pb++;
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
};

template <typename T>
class my_vector {
private:
    char* data;
    size_t dim;

public:
    my_vector() : data(nullptr), dim(0) {}
    my_vector(int d) : data(nullptr), dim(0) {
        assert(d > 0);
        dim = d;
        data = new char[sizeof(T) * dim];
        std::memset(data, 0, sizeof(T) * dim);
    }
    my_vector(const my_vector& rhs) : data(nullptr), dim(rhs.dim) {
        data = new char[sizeof(T) * dim];
        std::memcpy(data, rhs.data, sizeof(T) * dim);
    }
    my_vector(my_vector&& rhs) : data(nullptr), dim(rhs.dim) {
        data = rhs.data;
        rhs.data = nullptr;
        rhs.dim = 0;
    }
    ~my_vector() {
        if (data) delete [] data;
    }
    my_vector& operator=(const my_vector& rhs) {
        if (&rhs == this) return *this;
        if (dim != rhs.dim) {
            if (data) delete [] data;
            dim = rhs.dim;
            data = new char[sizeof(T) * dim];
        }
        std::memcpy(data, rhs.data, sizeof(T) * dim);
        return *this;
    }
    my_vector& operator=(my_vector&& rhs) {
        if (&rhs == this) return *this;
        if (data) delete [] data;
        dim = rhs.dim;
        data = rhs.data;
        rhs.data = nullptr;
        rhs.dim = 0;
        return *this;
    }

public:
    char* get_data() const {
        return data;
    }
    size_t get_dim() const {
        return dim;
    }
    size_t size() const {
        return dim * sizeof(T);
    }
    my_vector& operator+=(const my_vector& rhs) {
        assert(dim == rhs.dim);
        for (int i = 0; i < dim; ++i) {
            *((T*)data + i) += *((T*)rhs.data + i);
        }
        return *this;
    }
    my_vector& operator-=(const my_vector& rhs) {
        assert(dim == rhs.dim);
        for (int i = 0; i < dim; ++i) {
            *((T*)data + i) -= *((T*)rhs.data + i);
        }
        return *this;
    }
    my_vector& operator*=(const double x) {
        for (int i = 0; i < dim; ++i) {
            *((T*)data + i) *= x;
        }
        return *this;
    }
};

template <typename T>
my_vector<T> operator-(const my_vector<T>& lhs, const my_vector<T>& rhs) {
    assert(lhs.get_dim() == rhs.get_dim());
    my_vector<T> ret(lhs);
    ret -= rhs;
    return ret;
}

template <typename T>
my_vector<T> operator+(const my_vector<T>& lhs, const my_vector<T>& rhs) {
    assert(lhs.get_dim() == rhs.get_dim());
    my_vector<T> ret(lhs);
    ret += rhs;
    return ret;
}

template <typename T, typename Y>
my_vector<T> operator*(const my_vector<T>& lhs, Y y) {
    my_vector<T> ret(lhs);
    ret *= (double)y;
    return ret;
}

template <typename T, typename Y>
my_vector<T> operator*(Y y, const my_vector<T>& rhs) {
    my_vector<T> ret(rhs);
    ret *= (double)y;
    return ret;
}

template <typename T >
bool operator==(const my_vector<T> lhs, const my_vector<T>& rhs) {
    if (lhs.get_dim() != rhs.get_dim()) return false;
    const char* p1 = lhs.get_data();
    const char* p2 = rhs.get_data();
    size_t c = lhs.get_dim() / 8;
    size_t l = lhs.get_dim() % 8;
    for (size_t i = 0; i < c; ++i) {
        if (((std::uint64_t)*(p1 + i)) != ((std::uint64_t)*(p2 + i))) {
            return false;
        }
    }
    if (l) {
        return std::memcmp(p1 + 8 * c, p2 + 8 * c, l) == 0;
    } 
}

template <typename T>
double dist(const my_vector<T>& lhs, const my_vector<T>& rhs) {
    assert_eq(lhs.get_dim(), rhs.get_dim());
    euclidean_distance<T> euc_dist(lhs.get_dim());
    return euc_dist.dist(lhs.get_data(), rhs.get_data());
}

} // namespace ann

#endif
