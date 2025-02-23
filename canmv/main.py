import time, os, sys, _thread
from media.sensor import *
from media.display import *
from media.media import *
from machine import Pin
from machine import FPIOA
import key
import uart

#----- uart -----#
class SerialCommands(enum.Enum):
    COMMAND_ONE = 1
    COMMAND_TWO = 2
    COMMAND_THREE = 3

def handle_command_one():
    print("Handling command one")

def handle_command_two():
    print("Handling command two")

def handle_command_three():
    print("Handling command three")

# 创建一个字典，将枚举值映射到对应的处理函数
command_handlers = {
    SerialCommands.COMMAND_ONE: handle_command_one,
    SerialCommands.COMMAND_TWO: handle_command_two,
    SerialCommands.COMMAND_THREE: handle_command_three,
}

#----- sensor & display -----#

sensor = Sensor()
sensor.reset()
sensor.set_framesize(width=800,height=480)
sensor.set_pixformat(Sensor.RGB565)
Display.init(Display.ST7701)
MediaManager.init()
sensor.run()

#----- test -----#

def shoot():
    clock = time.clock()
    while True:
        clock.tick()
        img = sensor.snapshot()
        Display.show_image(img, layer=Display.LAYER_OSD0)
        print(clock.fps()) #打印FPS

#----- main -----#

while True:
    if key.is_key_pressed():
        shoot()
    if ser.in_waiting > 0:
        received_data = int(uart.readline().strip().decode('utf-8'))

        try:
            command = SerialCommands(received_data)
            command_handlers[command]()
        except ValueError:
            print(f"Unknown command received: {received_data}")

# 串口关闭等清理操作



