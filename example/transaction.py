import argparse

import pandas as pd

from flob import LimitOrderBook


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--data", type=str, default="data/sample.csv")
    parser.add_argument("--schedule", type=str, default="AShare")
    parser.add_argument("--output", type=str, default="data/transactions.csv")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    lob = LimitOrderBook(schedule=args.schedule)
    lob.load(args.data)
    lob.run()

    transactions = lob.get_transactions()
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
    transactions = transactions.round({"price": 2})
    transactions.to_csv(args.output, index=False)
