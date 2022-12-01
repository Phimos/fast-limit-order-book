import pandas as pd

from flob import LimitOrderBook

PRE_OPENING_SESSION_START = pd.Timedelta("09:15:00")
PRE_OPENING_SESSION_END = pd.Timedelta("09:30:00")

MORNING_SESSION_START = pd.Timedelta("09:30:00")
MORNING_SESSION_END = pd.Timedelta("11:30:00")

AFTERNOON_SESSION_START = pd.Timedelta("13:00:00")
AFTERNOON_SESSION_END = pd.Timedelta("14:57:00")

CLOSING_SESSION_START = pd.Timedelta("14:57:00")
CLOSING_SESSION_END = pd.Timedelta("15:00:00")


rule = flob.TradingRule(
    [
        (
            flob.TradingStatus.CallAuction,
            PRE_OPENING_SESSION_START.value,
            PRE_OPENING_SESSION_END.value,
        ),
        (
            flob.TradingStatus.ContinuousTrading,
            MORNING_SESSION_START.value,
            MORNING_SESSION_END.value,
        ),
        (
            flob.TradingStatus.ContinuousTrading, 
            AFTERNOON_SESSION_START.value,
            AFTERNOON_SESSION_END.value,
        ),
        (
            flob.TradingStatus.CallAuction,
            CLOSING_SESSION_START.value,
            CLOSING_SESSION_END.value,
        ),
    ]
)


lob = LimitOrderBook()
lob.load("data/sample.csv")

# Pre-opening session
lob.set_status("CallAuction")
lob.until(PRE_OPENING_SESSION_END.value)
lob.match_call_auction()
lob.show()

# Morning session
lob.set_status("ContinuousTrading")
lob.until(MORNING_SESSION_END.value)
lob.show_transactions()

# Afternoon session
lob.until(AFTERNOON_SESSION_END.value)
lob.show()

# Closing session
lob.set_status("CallAuction")
lob.until(CLOSING_SESSION_END.value)
lob.show()
lob.match_call_auction()
lob.show()

transactions = lob.get_transactions()
transactions = [
    {
        "bid_uid": item.bid_uid,
        "ask_uid": item.ask_uid,
        "price": item.price,
        "volume": item.quantity,
        "timestamp": item.timestamp,
    }
    for item in transactions
]

transactions = pd.DataFrame(transactions)
transactions.timestamp = pd.to_datetime(transactions.timestamp)
print(transactions)
