#ifndef __MENU_H
#define __MENU_H
#include <stdint.h>

//按键
typedef enum{
    Key_NULL = 0,
	Key_UP,				
	Key_DOWN,			
	Key_MID						
}KSTATE;

//光标结构体，存储着光标左上角坐标以及光标的长度与宽度
typedef struct Coordinate{
    int16_t X;
    int16_t Y;
    int16_t Width;
    int16_t Height;
}Coordinate;

//光标外观结构体
typedef struct CoordinateStyle{
	uint8_t Move;  //移动方式, 移动方式，0线性移动，1pid回弹移动,2非线性移动
	uint8_t Form;	//形状，0实心矩形，1圆角矩形，2空心矩形框,3圆角空心矩形框，4矩形选框
	uint8_t Speed;	//速度，0,1,2分别代表慢中快
}CoordinateStyle;

//菜单结构体
typedef struct Menu{
    char Name[15];
    int16_t Width; //宽
    int16_t Height;//高  高度与字体大小需搭配合适
    int16_t Frame; //0无外框，1方框，2圆角矩形框
	uint8_t FontSize; //指Ascii字体大小，中文暂只支持16*16
    void (*Function)();
    struct Menu* last;
    struct Menu* next;
    struct Menu* parent;
    struct Menu* child;
}MENU;

/***************************  菜单生成部分  ***************************/
void Menu_Init(void);
MENU* Creat_Menu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)());
MENU* Creat_BrotherMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)());
MENU* Creat_ChildMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)());
MENU* Circle_Menu(void);
void MENU_child(void);
void MENU_parent(void);
void No_Fun(void);
void Menu_Choose(void);

/***************************  菜单显示与移动部分  ***************************/
uint16_t numabs(int16_t num);
int16_t max(int16_t a, int16_t b, int16_t c, int16_t d);
void ReverseCoordinate(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void DrawFrame(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,uint8_t Style);
void ShowMenuList(void);
void ChangeTargetCursor(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void MoveCursorLinear(void);
void MoveCursorPID(void);
void MoveCursorUnLinear(void);
void MoveCursor(void);

#endif
