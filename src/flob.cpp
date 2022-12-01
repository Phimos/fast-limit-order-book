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
        .def("write", &LimitOrderBook::write, py::arg("quote"))
        .def("set_status", py::overload_cast<TradingStatus>(&LimitOrderBook::set_status), py::arg("status"))
        .def("set_status", py::overload_cast<const std::string &>(&LimitOrderBook::set_status), py::arg("status"))
        .def("set_schedule", &LimitOrderBook::set_schedule, py::arg("schedule"))
        .def("load", &LimitOrderBook::load, py::arg("filename"), py::arg("header") = true)
        .def("until", &LimitOrderBook::until, py::arg("timestamp"))
        .def("run", &LimitOrderBook::run)
        .def("match_call_auction", &LimitOrderBook::match_call_auction, py::arg("timestamp") = 0)
        .def("get_topk_bid_price", &LimitOrderBook::get_topk_bid_price, py::arg("k"))
        .def("get_topk_ask_price", &LimitOrderBook::get_topk_ask_price, py::arg("k"))
        .def("get_topk_bid_volume", &LimitOrderBook::get_topk_bid_volume, py::arg("k"))
        .def("get_topk_ask_volume", &LimitOrderBook::get_topk_ask_volume, py::arg("k"))
        .def("get_kth_bid_price", &LimitOrderBook::get_kth_bid_price, py::arg("k"))
        .def("get_kth_ask_price", &LimitOrderBook::get_kth_ask_price, py::arg("k"))
        .def("get_kth_bid_volume", &LimitOrderBook::get_kth_bid_volume, py::arg("k"))
        .def("get_kth_ask_volume", &LimitOrderBook::get_kth_ask_volume, py::arg("k"))
        .def("get_transactions", &LimitOrderBook::get_transactions)
        .def("show", &LimitOrderBook::show, py::arg("n") = 10)
        .def("show_transactions", &LimitOrderBook::show_transactions, py::arg("n") = 10);

    py::class_<Quote>(m, "Quote")
        .def(py::init<uint64_t, uint64_t, uint64_t, uint64_t, Side, QuoteType>(),
             py::arg("uid"), py::arg("price"), py::arg("quantity"),
             py::arg("timestamp"), py::arg("side"), py::arg("type"))
        .def_readonly("uid", &Quote::uid)
        .def_readonly("price", &Quote::price)
        .def_readonly("quantity", &Quote::quantity)
        .def_readonly("timestamp", &Quote::timestamp)
        .def_readonly("side", &Quote::side)
        .def_readonly("type", &Quote::type);

    py::class_<Transaction>(m, "Transaction")
        .def_readonly("bid_uid", &Transaction::bid_uid)
        .def_readonly("ask_uid", &Transaction::ask_uid)
        .def_readonly("price", &Transaction::real_price)
        .def_readonly("quantity", &Transaction::quantity)
        .def_readonly("timestamp", &Transaction::timestamp);

    py::enum_<QuoteType>(m, "QuoteType")
        .value("LimitOrder", QuoteType::LimitOrder)
        .value("MarketOrder", QuoteType::MarketOrder)
        .value("BestPriceOrder", QuoteType::BestPriceOrder)
        .value("CancelOrder", QuoteType::CancelOrder)
        .value("FillOrder", QuoteType::FillOrder)
        .export_values();

    py::enum_<TradingStatus>(m, "TradingStatus")
        .value("CallAuction", TradingStatus::CallAuction)
        .value("ContinuousTrading", TradingStatus::ContinuousTrading)
        .value("Closed", TradingStatus::Closed);

    py::class_<TradingRule>(m, "TradingRule")
        .def(py::init<std::vector<std::tuple<TradingStatus, uint64_t, uint64_t>>>());

    py::enum_<Side>(m, "Side")
        .value("Bid", Side::Bid)
        .value("Ask", Side::Ask)
        .export_values();
}