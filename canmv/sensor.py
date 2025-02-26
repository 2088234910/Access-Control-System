import time, os, sys
from media.sensor import *
from media.display import *
from media.media import *

sensor = Sensor()
sensor.reset()
sensor.set_framesize(width=800,height=480)
sensor.set_pixformat(Sensor.RGB565)
Display.init(Display.ST7701)
MediaManager.init()
sensor.run()

def shoot():
    clock = time.clock()
    while True:
        clock.tick()
        img = sensor.snapshot()
        Display.show_image(img, layer=Display.LAYER_OSD0)
        print(clock.fps()) #打印FPS

if __name__=="__main__":
    shoot()
