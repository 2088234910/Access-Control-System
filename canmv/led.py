from machine import Pin #导入Pin模块
from machine import FPIOA
import time

#将GPIO52配置为普通GPIO
fpioa = FPIOA()
fpioa.set_function(52,FPIOA.GPIO52)

LED=Pin(52,Pin.OUT)
#LED.value(1)
