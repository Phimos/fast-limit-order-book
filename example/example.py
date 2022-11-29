import pandas as pd

from flob import LimitOrderBook

PRE_OPENING_SESSION_START = (9, 15)
PRE_OPENING_SESSION_END = (9, 25)

MORNING_SESSION_START = (9, 30)
MORNING_SESSION_END = (11, 30)

AFTERNOON_SESSION_START = (13, 0)
AFTERNOON_SESSION_END = (14, 57)

CLOSING_SESSION_START = (14, 57)
CLOSING_SESSION_END = (15, 0)


lob = LimitOrderBook()
lob.load("sample.csv")

# Pre-opening session
lob.set_status("CallAuction")
lob.until(*PRE_OPENING_SESSION_END)
lob.match_call_auction()
lob.show()

# Morning session
lob.set_status("ContinuousTrading")
lob.until(*MORNING_SESSION_END)
lob.show()

# Afternoon session
lob.until(*AFTERNOON_SESSION_END)
lob.show()

# Closing session
lob.set_status("CallAuction")
lob.until(*CLOSING_SESSION_END)
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
