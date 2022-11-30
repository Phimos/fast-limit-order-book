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

```python
import flob
from flob import LimitOrderBook, Quote

lob = LimitOrderBook()
lob.write(Quote(12, 1186, 100, 1664529300000000000, flob.Bid, flob.LimitOrder))
lob.show()
```

Output

```bash
┌────────────────────┬────────────────────┬────────────────────┐
│     Timestamp      │       Price        │      Quantity      │
╞════════════════════╪════════════════════╪════════════════════╡
│        11:29:47.110│             1191.00│                 200│
│        11:29:47.110│             1191.00│                1100│
│        11:29:50.480│             1190.00│                 300│
│        11:29:53.350│             1191.00│                1000│
│        11:29:53.350│             1191.00│                1000│
│        11:29:53.350│             1191.00│                2600│
│        11:29:53.350│             1191.00│                 200│
│        11:29:53.350│             1191.00│                1000│
│        11:29:53.350│             1191.00│                1000│
│        11:29:53.350│             1191.00│                 200│
└────────────────────┴────────────────────┴────────────────────┘
```

```bash
┌────────────────────┬────────────────────┐
│       Price        │      Quantity      │
╞════════════════════╪════════════════════╡
│               11.95│              316600│
│               11.94│              191200│
│               11.93│              158600│
│               11.92│               93400│
│               11.91│               11000│
├────────────────────┼────────────────────┤
├────────────────────┼────────────────────┤
│               11.90│               95334│
│               11.89│              316400│
│               11.88│              289300│
│               11.87│              281535│
│               11.86│              171600│
└────────────────────┴────────────────────┘
```

**Note**: this project is still under development, so the API is not stable.