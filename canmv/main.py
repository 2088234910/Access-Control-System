import time, os, sys, _thread   #_thread是否需要？
from machine import Pin
from machine import FPIOA
import key
import uart
#import sensor
import face_registration
import face_recognition
#检查各文件中头文件是否有多余

#----- protocol -----#

SerialCommands = {
    "COMMAND_ONE": 1,
    "COMMAND_TWO": 2,
    "COMMAND_THREE": 3,
}

# 定义处理函数
def handle_command_one():
    print("Handling command one")

def handle_command_two():
    print("Handling command two")

def handle_command_three():
    print("Handling command three")

# 创建字典，将枚举值映射到对应的处理函数
command_handlers = {
    SerialCommands["COMMAND_ONE"]: handle_command_one,
    SerialCommands["COMMAND_TWO"]: handle_command_two,
    SerialCommands["COMMAND_THREE"]: handle_command_three,
}

#----- face -----#

freg = face_registration.face_reg_init()
frec, pl = face_recognition.face_rec_init()

#----- main -----#

def main():
    print("running")
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



