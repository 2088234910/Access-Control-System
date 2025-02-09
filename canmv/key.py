# key - By: ljc - 2025/2/6

import time
from machine import Pin, FPIOA

fpioa = FPIOA()
fpioa.set_function(21,FPIOA.GPIO21)
KEY=Pin(21,Pin.IN,Pin.PULL_UP)

def is_key_pressed():
    if KEY.value()==0:
        time.sleep_ms(10)
        if KEY.value()==0:
            print("key pressed")
            while not KEY.value():
                pass
            return True
    return False

