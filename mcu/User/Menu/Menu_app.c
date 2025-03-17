#include <string.h>  
#include <stdlib.h>
#include "Menu.h"
#include "OLED.h"
#include "Key.h"
#include "protocol.h"
#include "Esp8266.h"

void FaceRegistration()
{
    OLED_Clear();
    OLED_ShowMixStringArea(0,32,255,16,8,32,"人脸注册中",OLED_16X16,OLED_8X16);
    OLED_Update();
    COM_FaceRegistration();
    Key_State = Key_NULL;
    uint8_t res = NRES;
    while(1)
    {
        if (Key_State == Key_MID) {
            break;
        }
        res = ParseCmd();
        if (res == POK) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"人脸注册成功",OLED_16X16,OLED_8X16);
            OLED_Update();
            HAL_Delay(1500);
            break;
        } else if (res == PFAIL) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"未检测到人脸",OLED_16X16,OLED_8X16);
            OLED_Update();
            HAL_Delay(1500);
            break;
        } else if (res == PERROR) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"发生错误",OLED_16X16,OLED_8X16);
            OLED_Update();
            HAL_Delay(1500);
            break;
        }   
    }
}

void FaceRecognition()
{
    OLED_Clear();
    OLED_ShowMixStringArea(0,32,255,16,8,32,"人脸识别中",OLED_16X16,OLED_8X16);
    OLED_Update();
    COM_FaceRecognition();
    Key_State = Key_NULL;
    uint8_t res = NRES;
    while(1)
    {
        if (Key_State == Key_MID) {
            break;
        }
        res = ParseCmd();
        if (res == POK) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"人脸识别成功",OLED_16X16,OLED_8X16);
            OLED_Update();
            Esp_Door_Open();
            HAL_Delay(1500);
            break;
        } else if (res == PFAIL) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"未检测到人脸",OLED_16X16,OLED_8X16);
            OLED_Update();
            HAL_Delay(1500);
            break;
        } else if (res == PERROR) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"发生错误",OLED_16X16,OLED_8X16);
            OLED_Update();
            HAL_Delay(1500);
            break;
        } else if (res == 0x03) {
            OLED_Clear();
            OLED_ShowMixStringArea(0,32,255,16,8,32,"检测到陌生人！",OLED_16X16,OLED_8X16);
            OLED_Update();
            HAL_Delay(1500);
            break;
        }
    }
}

void Ring()
{
    OLED_Clear();
    OLED_ShowMixStringArea(0,32,255,16,8,32,"打铃中",OLED_16X16,OLED_8X16);
    OLED_Update();
    COM_Ring();
    HAL_Delay(1000);
    uint8_t res = ParseCmd();;
    if (res != POK) {
        OLED_Clear();
        OLED_ShowMixStringArea(0,32,255,16,8,32,"打铃失败",OLED_16X16,OLED_8X16);
        OLED_Update();
        HAL_Delay(1500);
    }
}

void Record()
{
    OLED_Clear();
    OLED_ShowMixStringArea(0,32,255,16,8,32,"录音中",OLED_16X16,OLED_8X16);
    OLED_Update();
    COM_Record();
    HAL_Delay(5000);
    uint8_t res = ParseCmd();;
    if (res != POK) {
        OLED_Clear();
        OLED_ShowMixStringArea(0,32,255,16,8,32,"录音失败",OLED_16X16,OLED_8X16);
        OLED_Update();
        HAL_Delay(1500);
    }
}

void Door_close()
{
    Esp_Door_Close();
}

void Menu_Init(void)
{
    nowMenu = Creat_Menu("- 人脸识别",108,16,0,OLED_8X16,*FaceRecognition);
    nowMenu = Creat_BrotherMenu("- 人脸注册",108,16,0,OLED_8X16,*FaceRegistration);
    nowMenu = Creat_BrotherMenu("- 打铃",72,16,0,OLED_8X16,*Ring);
    nowMenu = Creat_BrotherMenu("- 录音",72,16,0,OLED_8X16,*Record);
    nowMenu = Creat_BrotherMenu("- 模拟关门",108,16,0,OLED_8X16,*Door_close);
    nowMenu = Creat_BrotherMenu("- 设置",72,16,0,OLED_8X16,*MENU_child);
        nowMenu = Creat_ChildMenu("- A",72,16,0,OLED_8X16,*No_Fun);
        nowMenu = Creat_BrotherMenu("- B",72,16,0,OLED_8X16,*No_Fun);
        nowMenu = Creat_BrotherMenu("- C",72,16,0,OLED_8X16,*No_Fun);
        nowMenu = Creat_BrotherMenu("- <<<",72,16,0,OLED_8X16,*MENU_parent);
        nowMenu = Circle_Menu();
    nowMenu = Circle_Menu();
    
    OLED_Init();
    CursorInit();
	ShowMenuList();
}

void Menu_Choose(void)
{
    if(Key_State != Key_NULL) {
        if (Key_State == Key_UP) {
            nowMenu = nowMenu->last;
            ChangeTargetCursor(0,CurrentCursor.Y-nowMenu->Height,nowMenu->Width,nowMenu->Height);
        }
        else if (Key_State == Key_DOWN) {
            nowMenu = nowMenu->next;
            ChangeTargetCursor(0,CurrentCursor.Y+CurrentCursor.Height,nowMenu->Width,nowMenu->Height);
        }
        else if (Key_State == Key_MID) {
            (*nowMenu->Function)();
            Key_State = Key_MID;
        }
        if (CurrentCursor.Y == TargetCursor.Y || Key_State == Key_MID)  ShowMenuList();
        MoveCursor(); //光标移动
        Key_State = Key_NULL;
    }
}
