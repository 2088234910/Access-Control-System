import time
from machine import UART, FPIOA

fpioa = FPIOA()
fpioa.set_function(11,FPIOA.UART2_TXD)
fpioa.set_function(12,FPIOA.UART2_RXD)
uart = UART(UART.UART2,115200)

if __name__=="__main__":
#    uart.write('Hello 01Studio!')#发送一条数据

    while True:
        command = uart.read(3)
        if command is not None and len(command) == 3:
            print(f"command: {command}")
            byte1, byte2, byte3 = command
            hex1, hex2, hex3= hex(byte1), hex(byte2), hex(byte3)
            print(f"Received bytes (hex): {hex1}, {hex2}, {hex3}")

        time.sleep(0.1) #100ms
