#ifndef __READ_DATA_H__
#define __READ_DATA_H__

#include <fstream>
#include <string>
#include <cassert>

#include "utils.h"

namespace ann
{

template <typename T>
class data_reader
{
private:
    int _dim;
    size_t _num_vec;
    size_t _next_idx;
    std::ifstream _ifs;
    char* _buf;

public:
    data_reader(int d, size_t n, std::string f):
    _dim(d), _num_vec(n), _next_idx(0), _ifs(f, std::ios::binary) {
        assert(sizeof(int) == 4);
        _buf = new char[sizeof(T) * _dim];
    }
    ~data_reader() {
        delete [] _buf;
    }

public:
    char* read_one_vec() {
        int d  = 0;
        _ifs.read((char*)&d, 4);
        debug_print("d", d, "_dim", _dim);
        assert(d == _dim);
        _ifs.read(_buf, sizeof(T) * _dim);
        ++_next_idx;
        return _buf;
    }

    size_t next_idx() const {
        return _next_idx;
    }

    void read(char* buf, size_t len) {
        _ifs.read(buf, len);
    }
};

template <typename T>
class dreader
{
private:
    std::ifstream _ifs;
    char* _buf;
    int _dim;
    bool _has_len_head; //TODO change to length of per-vector head

public:
    dreader(int d, std::string fname, bool has_head):
        _dim(d), _ifs(fname, std::ios::binary), _buf(nullptr), _has_len_head(has_head) {
        _buf = new char[sizeof(T) * _dim];
    }
    ~dreader() {
        if (_buf) {
            delete [] _buf;
        }
    }

public:
    const char* read_one_vec() {
        if (_has_len_head) {
            int d  = 0;
            _ifs.read((char*)&d, 4);
            if (_ifs.eof()) return nullptr;
            assert(d == _dim);
        }
        _ifs.read(_buf, sizeof(T) * _dim);
        if (_ifs.eof()) return nullptr;
        return _buf;
    }
    int read_one_vec(char* dest, size_t sz) {
        assert(sz == sizeof(T) * _dim);
        if (_has_len_head) {
            int d = 0;
            _ifs.read((char*)&d, 4);
            if (_ifs.eof()) return 0;
            assert(d == _dim);
        }
        _ifs.read(dest, sz);
        if (_ifs.eof()) return 0;
        return sz;
    }
    void read(char* buf, size_t len) {
        _ifs.read(buf, len);
    }
};

} // namespace ann

#endif // __READ_DATA_H__
