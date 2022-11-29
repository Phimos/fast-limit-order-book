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

**Note**: this project is still under development, so the API is not stable.