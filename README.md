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

### Basic Usage

you can use this project as a python package, to display the details of the order book:

```python
import flob
from flob import LimitOrderBook, Quote

lob = LimitOrderBook(decimal_places=2)
lob.write(Quote(12, 1186, 300, 1664529300000000000, flob.Bid, flob.LimitOrder))
lob.write(Quote(13, 1187, 100, 1664529500000000000, flob.Bid, flob.LimitOrder))
lob.write(Quote(14, 1188, 200, 1664530200000000000, flob.Bid, flob.LimitOrder))
lob.write(Quote(15, 1187, 400, 1664531500000000000, flob.Ask, flob.LimitOrder))
lob.show(n=1)
```

*expected output:*
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

*expected output:*
```bash
┌────────────────────┬────────────────────┬────────────────────┐
│     Timestamp      │       Price        │      Quantity      │
╞════════════════════╪════════════════════╪════════════════════╡
│        09:51:40.000│               11.88│                 200│
│        09:51:40.000│               11.87│                 100│
└────────────────────┴────────────────────┴────────────────────┘
```


### Batch 

you can load quotes from a csv file and batch process them:

*the columns of the csv file must be the same as the sample file*

```python
import pandas as pd

from flob import LimitOrderBook

lob = LimitOrderBook(decimal_places=2)
lob.load("data/sample.csv")

lob.set_status("CallAuction")
lob.until(pd.Timedelta("09:25:00").value)
lob.match_call_auction()

lob.set_status("ContinuousTrading")
lob.until(pd.Timedelta("11:30:00").value)

lob.show()
```

*expected output:*
```bash
┌────────────────────┬────────────────────┐
│       Price        │      Quantity      │
╞════════════════════╪════════════════════╡
│               12.00│              760600│
│               11.99│              307500│
│               11.98│              422100│
│               11.97│              315800│
│               11.96│              323000│
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
│               11.85│              207800│
│               11.84│              131200│
│               11.83│              264500│
│               11.82│              507200│
│               11.81│              333800│
└────────────────────┴────────────────────┘
```

you can also see the list of transactions:


```python
lob.show_transactions()
```

*expected output:*
```bash
┌────────────────────┬────────────────────┬────────────────────┐
│     Timestamp      │       Price        │      Quantity      │
╞════════════════════╪════════════════════╪════════════════════╡
│        11:29:47.110│               11.91│                 200│
│        11:29:47.110│               11.91│                1100│
│        11:29:50.480│               11.90│                 300│
│        11:29:53.350│               11.91│                1000│
│        11:29:53.350│               11.91│                1000│
│        11:29:53.350│               11.91│                2600│
│        11:29:53.350│               11.91│                 200│
│        11:29:53.350│               11.91│                1000│
│        11:29:53.350│               11.91│                1000│
│        11:29:53.350│               11.91│                 200│
└────────────────────┴────────────────────┴────────────────────┘
```

### Scripts

there are example scripts in the `example` folder. it can be used to generate transactions and tick data in any frequency.

```bash
python example/transaction.py    \
    --data data/sample.csv       \
    --schedule AShare            \
    --output data/transactions.csv
```

```bash
python example/tick.py        \
    --data data/sample.csv    \
    --schedule AShare         \
    --snapshot_gap 3s         \
    --topk 5                  \
    --output data/ticks.csv
```

**Note**: this project is still under development, so the API is not stable.
