#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "limit_order_book.hpp"

namespace py = pybind11;

PYBIND11_MODULE(flob, m)
{
    m.doc() = "fast-limit-order-book";

    py::class_<LimitOrderBook>(m, "LimitOrderBook")
        .def(py::init<size_t>(), py::arg("decimal_places") = 2)
        .def("clear", &LimitOrderBook::clear);
}