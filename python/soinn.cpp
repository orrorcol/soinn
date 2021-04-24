#include <assert.h>
#include <string.h>

#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "soinn.h"
#include "data_interface.h"

namespace py = pybind11;

py::object learn(
        py::object data, 
        int dead_age = 100,
        int lambda = 100,
        float noise = 0.5,
        int num_layer = 1)
         {
    ann::soinn<float> alg(num_layer, dead_age, lambda, noise);
    py::array_t<float, py::array::c_style | py::array::forcecast> items(data);

    auto buffer = items.request();
    size_t rows = buffer.shape[0];
    size_t cols = buffer.shape[1];

    assert(rows > 0);
    assert(cols > 0);

    float* ptr = (float*)buffer.ptr;
    std::vector<ann::my_vector<float>> cpp_data;
    for (size_t i = 0; i < rows; ++i) {
        ann::my_vector<float> tmp_vec(cols); 
        memcpy(tmp_vec.get_data(), ptr, sizeof(float) * cols);
        cpp_data.push_back(tmp_vec);
    }

    alg.learn(cpp_data);
    auto res = alg.get_cendroids();
    res.resize(10 * cols, 3.6);
    rows = res.size() / cols;
    assert(rows * cols == res.size());

    py::capsule free_when_done(res.data(), [](void *f) { return; });

    return py::array_t<float>(
        {rows, cols},
        {cols * sizeof(float), sizeof(float)},
        res.data(),
        free_when_done);
}


PYBIND11_MODULE(soinn, m) {
    m.doc() = "pybind of soinn"; // optional module docstring

    m.def("learn", &learn, "soinn learn", 
        py::arg("data") = py::none(),
        py::arg("dead_age")=100,
        py::arg("lambda")=100,
        py::arg("noise")=0.5,
        py::arg("num_layer")=1);
        //"dead_age"_a=100,
        //"lambda"_a=100,
        //"noise"_a=0.5,
        //"num_layer"_a=1);
}
