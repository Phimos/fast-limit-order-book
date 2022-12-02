import pandas as pd

import flob
from flob import LimitOrderBook

schedule = [
    (
        flob.TradingStatus.CallAuction,
        pd.Timedelta("09:15:00").value,
        pd.Timedelta("09:25:00").value,
    ),
    (
        flob.TradingStatus.ContinuousTrading,
        pd.Timedelta("09:30:00").value,
        pd.Timedelta("11:30:00").value,
    ),
    (
        flob.TradingStatus.ContinuousTrading,
        pd.Timedelta("13:00:00").value,
        pd.Timedelta("14:57:00").value,
    ),
    (
        flob.TradingStatus.CallAuction,
        pd.Timedelta("14:57:00").value,
        pd.Timedelta("15:00:00").value,
    ),
]


lob = LimitOrderBook()
lob.load("data/sample.csv")
lob.set_schedule(schedule)
lob.set_snapshot_gap(pd.Timedelta("00:00:03").value)
lob.run()

ticks = lob.get_ticks()
ticks = [
    {
        "timestamp": item.timestamp,
        "open": item.open,
        "high": item.high,
        "low": item.low,
        "close": item.close,
        "volume": item.volume,
        "amount": item.amount,
        **{f"bid_price_{i+1}": item.bid_prices[i] for i in range(5)},
        **{f"ask_price_{i+1}": item.ask_prices[i] for i in range(5)},
        **{f"bid_volume_{i+1}": item.bid_volumes[i] for i in range(5)},
        **{f"ask_volume_{i+1}": item.ask_volumes[i] for i in range(5)},
    }
    for item in ticks
]

ticks = pd.DataFrame(ticks)
ticks.timestamp = pd.to_datetime(ticks.timestamp)
print(ticks)
