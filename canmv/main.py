import time, os, sys
from machine import Pin
from machine import FPIOA
import key
import uart
#import sensor
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
    print(img_res.shape)

    file_name = f"face_{face_index_max+1}.jpg"
    if (freg.run(img_res, file_name)):
# 人脸图片保存暂未实现
#        img = image.Image(960, 540, image.ARGB8888, alloc=image.ALLOC_REF, data=img_np)
#        img.to_rgb565(True)
#        img.save(face_save_dir+file_name)
        face_index_max += 1
    gc.collect()
    print("FaceRegistration done!")

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
            # 发送命令返回值
            time.sleep(2)
            pl.osd_img.clear()
            pl.show_image()
            gc.collect()
            break
        gc.collect()
#        print(clock.fps())

def Ring():
    print("Ring running")

#----- protocol -----#

SerialCommands = {
    "FaceRegistration": 1,
    "FaceRecognition": 2,
    "Ring": 3,
}

command_handlers = {
    SerialCommands["FaceRegistration"]: FaceRegistration,
    SerialCommands["FaceRecognition"]: FaceRecognition,
    SerialCommands["Ring"]: Ring,
}

#----- main -----#

def main():
    print("main running")
    FaceRecognition()
    while True:
        text = uart.uart.read(128) #接收128个字符
        if text != None:
            received_data = int(uart.readline().strip().decode('utf-8'))
            print(received_data)
            try:
                command = SerialCommands(received_data)
                command_handlers[command]()
            except ValueError:
                print(f"Unknown command received: {received_data}")
# 串口关闭等清理操作待添加

if __name__=="__main__":
    main()



