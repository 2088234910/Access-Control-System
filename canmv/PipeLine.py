from libs.PipeLine import PipeLine, ScopedTiming
from media.media import *
import gc
import sys,os
from media.sensor import *

if __name__ == "__main__":
    # 显示模式，默认"hdmi",可以选择"hdmi"和"lcd"
    display_mode="hdmi"
    if display_mode=="hdmi":
        display_size=[1920,1080]
    else:
        display_size=[800,480]

    sensor = Sensor()
    sensor.reset()
    sensor.set_framesize(width=800,height=480, chn=CAM_CHN_ID_1)
    sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_1)
#    #MediaManager.init()
#    sensor.run()

    pl = PipeLine(rgb888p_size=[1920,1080], display_size=display_size, display_mode=display_mode)
    pl.create(sensor)  # 创建PipeLine实例


    while True:
        img_np = pl.get_frame()
        img = pl.sensor.snapshot(chn=CAM_CHN_ID_1)
        img.save("/data/1.jpg")
        gc.collect()                    # 垃圾回收

