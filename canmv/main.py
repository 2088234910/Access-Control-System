import time, os, sys
from machine import Pin
from machine import FPIOA
import led
import key
import uart
import audio
#import sensor
from media.sensor import *
import face_registration
import face_recognition
import ulab.numpy as np
import image
import gc
from libs.PipeLine import ScopedTiming

#----- face -----#

freg = face_registration.face_reg_init()
frec, pl = face_recognition.face_rec_init()
print("face init success")

face_save_dir = "/data/face_reg/"
indexes = [int(f.split('_')[1].split('.')[0]) for f in os.listdir(face_save_dir) if f.endswith('.bin')]
face_index_max = max(indexes) if indexes else 0
print(f"face_index_max: {face_index_max}")

# 人脸注册
def FaceRegistration():
    global face_index_max
    print("FaceRegistration running")

    img_np = pl.get_frame()
    img_res = freg.dimension_convert(img_np)
    print(img_res.shape)

    file_name = f"face_{face_index_max+1}.jpg"
    with ScopedTiming("FaceRegistration time used",1):
        regi_res = freg.run(img_res, file_name)
    if regi_res == 1:
        img = pl.sensor.snapshot(chn=CAM_CHN_ID_1)
        img.save(face_save_dir+file_name)
        face_index_max += 1
        frec.database_init()
        uart.uart.write(bytes.fromhex('513001'))    # 注册成功
    else:
        uart.uart.write(bytes.fromhex('513002'))    # 注册失败
    gc.collect()

# 人脸识别
def FaceRecognition():
    print("FaceRecognition running")

    img=pl.get_frame()
    with ScopedTiming("FaceRecognition time used",1):
        det_boxes,recg_res=frec.run(img)
    if det_boxes:
        print(det_boxes,recg_res)
        if recg_res[0] == 'empty':
            uart.uart.write(bytes.fromhex('513104'))    # 数据库中无人脸
        elif recg_res[0] == 'stranger':
            uart.uart.write(bytes.fromhex('513103'))    # 陌生人
#            millis_timestamp = int(time.time() * 1000)
#            print("当前毫秒级时间戳:", millis_timestamp)    # 打印时间戳，用于计算告警耗时
        else:
            uart.uart.write(bytes.fromhex('513101'))    # 识别成功
        frec.draw_result(pl,det_boxes,recg_res)     # 绘制推理结果
        pl.show_image()
        time.sleep(2)
        pl.osd_img.clear()
        pl.show_image()
    else :
        uart.uart.write(bytes.fromhex('513102'))    # 未检测到人脸
    gc.collect()

#----- audio -----#

# 打铃
def Ring():
    uart.uart.write(bytes.fromhex('513201'))
    print("Ring running")
    audio.play_audio('/data/audio/ring.wav')

# 录音
def Record():
    uart.uart.write(bytes.fromhex('513301'))
    print("Record running")
    audio.loop_audio(5) #采集音频并输出

#----- protocol -----#

SerialCommands = {
    "FaceRegistration": 0x30,
    "FaceRecognition": 0x31,
    "Ring": 0x32,
    "Record": 0x33,
}

command_handlers = {
    SerialCommands["FaceRegistration"]: FaceRegistration,
    SerialCommands["FaceRecognition"]: FaceRecognition,
    SerialCommands["Ring"]: Ring,
    SerialCommands["Record"]: Record,
}

#----- main -----#

def main():
    led.LED.value(1)
    print("main running")
    # 增加亮灯，表示程序已正常运行
    while True:
        command = uart.uart.read(3)
        if command is not None and len(command) == 3:
            byte1, byte2, byte3 = command
            print(f"Received bytes (hex): 0x{byte1:02X}, 0x{byte2:02X}, 0x{byte3:02X}")
            if byte1 == 0x51:
                if byte2 in command_handlers:
                    command_handlers[byte2]()
                else:
                    print(f"Unknown command received: 0x{byte2:02X}")
        time.sleep(0.1) #100ms

if __name__=="__main__":
    main()



