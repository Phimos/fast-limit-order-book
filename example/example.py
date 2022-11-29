import flob

book = flob.LimitOrderBook()
book.load("sample.csv")
book.set_status("CallAuction")
book.until(9, 30)
book.show()
book.match_call_auction()
book.show()

book.set_status("ContinuousTrading")
book.until(9, 31)
book.show()
