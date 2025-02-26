import time
from machine import UART, FPIOA

fpioa = FPIOA()
fpioa.set_function(11,FPIOA.UART2_TXD)
fpioa.set_function(12,FPIOA.UART2_RXD)
uart = UART(UART.UART2,115200)

if __name__=="__main__":
    uart.write('Hello 01Studio!')#发送一条数据

    while True:
        text = uart.read(128) #接收128个字符
        if text != None:
            decoded_text = text.decode('utf-8')
            print(decoded_text)
        time.sleep(0.1) #100ms
