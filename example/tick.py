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

transactions = lob.get_transactions()
print(len(transactions))
transactions = [
    {
        "timestamp": item.timestamp,
        "bid_uid": item.bid_uid,
        "ask_uid": item.ask_uid,
        "price": item.price,
        "volume": item.quantity,
    }
    for item in transactions
]

transactions = pd.DataFrame(transactions)
transactions.timestamp = pd.to_datetime(transactions.timestamp)
print(transactions)
