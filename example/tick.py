import argparse

import pandas as pd
from flob import LimitOrderBook


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--data", type=str, default="data/sample.csv")
    parser.add_argument("--schedule", type=str, default="AShare")
    parser.add_argument("--snapshot_gap", type=str, default="3s")
    parser.add_argument("--topk", type=int, default=5)
    parser.add_argument("--output", type=str, default="data/ticks.csv")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()

    lob = LimitOrderBook(
        schedule=args.schedule,
        snapshot_gap=pd.Timedelta(args.snapshot_gap).value,
        topk=args.topk,
    )
    lob.load(args.data)
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
            **{f"bid_price_{i+1}": item.bid_prices[i] for i in range(args.topk)},
            **{f"ask_price_{i+1}": item.ask_prices[i] for i in range(args.topk)},
            **{f"bid_volume_{i+1}": item.bid_volumes[i] for i in range(args.topk)},
            **{f"ask_volume_{i+1}": item.ask_volumes[i] for i in range(args.topk)},
        }
        for item in ticks
    ]
    ticks = pd.DataFrame(ticks)
    ticks.timestamp = pd.to_datetime(ticks.timestamp)
    ticks = ticks.round({"open": 2, "high": 2, "low": 2, "close": 2, "amount": 2})
    ticks = ticks.round({f"bid_price_{i+1}": 2 for i in range(args.topk)})
    ticks = ticks.round({f"ask_price_{i+1}": 2 for i in range(args.topk)})
    ticks.to_csv(args.output, index=False)
