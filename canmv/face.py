# face - By: ljc - 2025/2/6

import time, os, sys, _thread
from media.sensor import *
from media.display import *
from media.media import *
from machine import Pin
from machine import FPIOA
import key

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



