import time
from machine import UART
from machine import FPIOA

fpioa = FPIOA()
fpioa.set_function(11,FPIOA.UART2_TXD)
fpioa.set_function(12,FPIOA.UART2_RXD)
uart = UART(UART.UART2,115200)

if __name__=="__main__":
    uart.write('Hello 01Studio!')#发送一条数据

    while True:
        text=uart.read(128) #接收128个字符
        if text != None:
            print(text) #通过REPL打印串口接收的数据
        time.sleep(0.1) #100ms
