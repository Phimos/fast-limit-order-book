# Flob: Fast-Limit-Order-Book

use this project to learn smart pointers

## Installation

install required package:

```bash
pip install pybind11
```

clone this project & build:

```
git clone https://github.com/Phimos/fast-limit-order-book
cd fast-limit-order-book
python setup.py install
```

## Usage

you can use this project as a python package, to display the details of the order book:

```python
import flob
from flob import LimitOrderBook, Quote

lob = LimitOrderBook()
lob.write(Quote(12, 1186, 300, 1664529300000000000, flob.Bid, flob.LimitOrder))
lob.write(Quote(13, 1187, 100, 1664529500000000000, flob.Bid, flob.LimitOrder))
lob.write(Quote(15, 1187, 200, 1664531500000000000, flob.Ask, flob.LimitOrder))
lob.show(n=1)
```

*Expected output:*
```bash
┌────────────────────┬────────────────────┐
│       Price        │      Quantity      │
╞════════════════════╪════════════════════╡
│               11.87│                 100│
├────────────────────┼────────────────────┤
├────────────────────┼────────────────────┤
│               11.86│                 300│
└────────────────────┴────────────────────┘
```

you can also see the list of transactions:

```python
lob.show_transactions()
```

*Expected output:*
```bash
┌────────────────────┬────────────────────┬────────────────────┐
│     Timestamp      │       Price        │      Quantity      │
╞════════════════════╪════════════════════╪════════════════════╡
│        09:51:40.000│               11.87│                 100│
└────────────────────┴────────────────────┴────────────────────┘
```

there are example scripts in the `example` folder. it can be used to generate transactions and tick data in any frequency.

```bash
python example/transaction.py
```

```bash
python example/tick.py
```

**Note**: this project is still under development, so the API is not stable.