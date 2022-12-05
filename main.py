from ibapi.client import EClient
from ibapi.wrapper import EWrapper
from ibapi.contract import *
from ibapi.order import *
from playsound import playsound
import serial


class IBApi(EWrapper, EClient):
    #static var that connects to arduino
    ser = serial.Serial('/dev/tty.usbmodem101', 9600)
    def __init__(self):
        EClient.__init__(self, self)
        #create the contract that we will be trading
        contract = Contract()
        contract.symbol = "NQ"
        contract.secType = "FUT"
        contract.exchange = "CME"
        contract.currency = "USD"
        contract.localSymbol = "NQZ2"
        self.contract = contract

    def nextValidId(self, orderId:int)->None:
        super().nextValidId(orderId)
        self.nextValidOrderId = orderId

    def nextOrderId(self)->int:
        """
        All transactions to TWS needs a order id. This is a helper function to assist with these order id's 
        ---
        Returns an integer being the the next valid order id
        """
        id = self.nextValidOrderId
        self.nextValidOrderId += 1
        return id

    def buy(self)->None:
        """
        Create an executes a buy market order for the contract we are trading.
        """
        order = Order()
        order.action = "BUY"
        order.orderType = "MKT"
        order.totalQuantity = 1
        order.eTradeOnly = False
        order.firmQuoteOnly = False
        self.placeOrder(self.nextOrderId(), self.contract, order)

    def sell(self)->None:
        """
        Create an executes a sell market order for the contract we are trading.
        """
        order = Order()
        order.action = "SELL"
        order.orderType = "MKT"
        order.totalQuantity = 1
        order.eTradeOnly = False
        order.firmQuoteOnly = False
        self.placeOrder(self.nextOrderId(), self.contract, order)

    def msgLoopTmo(self)->None:
        """ 
        This function is a overloaded function from EClient that gets called every repitivly in a loop while connected to the TWS
        """        
        #decode msg
        msg = IBApi.ser.readline().rstrip().decode('utf-8')
        print(msg)
        # Handle message
        if msg == "BUY":
            self.buy()
            playsound('./sounds/buy.wav')
        elif msg == "SELL":
            self.sell()
            playsound('./sounds/sell.wav')
        elif msg == "authenticated":
            playsound('./sounds/tadah.wav')
        elif msg == "invalid":
            playsound('./sounds/buzzer.wav')


if __name__ == '__main__':
    #create connection to the port TWS is running on
    ib = IBApi()
    ib.connect("127.0.0.1",7496,1)
    #run our program
    ib.run()