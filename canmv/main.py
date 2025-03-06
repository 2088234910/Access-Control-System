import time, os, sys
from machine import Pin
from machine import FPIOA
import key
import uart
#import sensor
from media.sensor import *
import face_registration
import face_recognition
import ulab.numpy as np
import image
import gc
#检查各文件中头文件是否有多余

#----- face -----#

freg = face_registration.face_reg_init()
frec, pl = face_recognition.face_rec_init()
print("face init success")

face_save_dir = "/data/face_reg/"
indexes = [int(f.split('_')[1].split('.')[0]) for f in os.listdir(face_save_dir) if f.endswith('.bin')]
face_index_max = max(indexes) if indexes else 0
print(f"face_index_max: {face_index_max}")


def FaceRegistration():
    global face_index_max
    print("FaceRegistration running")

    img_np = pl.get_frame()
    img_res = freg.dimension_convert(img_np)
#    print(img_res.shape)

    file_name = f"face_{face_index_max+1}.jpg"
    if freg.run(img_res, file_name) == 1:
        img = pl.sensor.snapshot(chn=CAM_CHN_ID_1)
        img.save(face_save_dir+file_name)
        face_index_max += 1
        uart.uart.write(bytes.fromhex('513001'))
    else:
        uart.uart.write(bytes.fromhex('513002'))
    gc.collect()

def FaceRecognition():
    print("FaceRecognition running")

    clock = time.clock()
    while True:
        os.exitpoint()
        clock.tick()
        img=pl.get_frame()
        det_boxes,recg_res=frec.run(img)
        frec.draw_result(pl,det_boxes,recg_res) # 绘制推理结果
        pl.show_image()
        if (det_boxes and recg_res[0] != 'unknown'):
            print(det_boxes,recg_res)
            uart.uart.write(bytes.fromhex('513101'))
            time.sleep(2)
            pl.osd_img.clear()
            pl.show_image()
            gc.collect()
            break
        else:
            uart.uart.write(bytes.fromhex('513102'))
        gc.collect()
#        print(clock.fps())

def Ring():
    print("Ring running")

#----- protocol -----#

SerialCommands = {
    "FaceRegistration": 0x30,
    "FaceRecognition": 0x31,
    "Ring": 0x33,
}

command_handlers = {
    SerialCommands["FaceRegistration"]: FaceRegistration,
    SerialCommands["FaceRecognition"]: FaceRecognition,
    SerialCommands["Ring"]: Ring,
}

#----- main -----#

def main():
    print("main running")
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



