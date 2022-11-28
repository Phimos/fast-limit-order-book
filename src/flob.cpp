#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "limit_order_book.hpp"
#include "struct.hpp"

namespace py = pybind11;

PYBIND11_MODULE(flob, m)
{
    m.doc() = "fast-limit-order-book";

    py::class_<LimitOrderBook>(m, "LimitOrderBook")
        .def(py::init<size_t>(), py::arg("decimal_places") = 2)
        .def("clear", &LimitOrderBook::clear)
        .def("show", &LimitOrderBook::show);

    py::enum_<TradingStatus>(m, "TradingStatus")
        .value("CallAuction", TradingStatus::CallAuction)
        .value("ContinuousTrading", TradingStatus::ContinuousTrading)
        .value("ClosingAuction", TradingStatus::ClosingAuction);

    py::enum_<Side>(m, "Side")
        .value("Bid", Side::Bid)
        .value("Ask", Side::Ask)
        .export_values();
}