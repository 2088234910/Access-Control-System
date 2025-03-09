#include <string.h>  
#include <stdlib.h>
#include "Menu.h"
#include "OLED.h"

MENU* nowMenu = NULL;

Coordinate CurrentCursor={0,0,0,0};   //当前光标
Coordinate TargetCursor={0,0,0,0};    //目标光标

CoordinateStyle Style = {   //光标样式
    1, //移动方式，0线性移动，1pid回弹移动,2非线性移动
	4, //形状，0实心矩形，1圆角矩形，2空心矩形框,3圆角空心矩形框，4矩形选框
	0  //速度，0,1,2分别代表慢中快
};

/***************************  static函数申明  ***************************/

static uint16_t numabs(int16_t num);
static int16_t max(int16_t a, int16_t b, int16_t c, int16_t d);
static void ReverseCoordinate(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
static void MoveCursorLinear(void);
static void MoveCursorPID(void);
static void MoveCursorUnLinear(void);

/***************************  linked-list部分  ***************************/

MENU* Creat_Menu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)())
{
    MENU* node = malloc(sizeof(MENU));
    strcpy(node->Name, Name);  
    node->Width = Width;
    node->Height = Height;
    node->Frame = Frame;
    node->FontSize = FontSize;
    node->Function = Function;
    
    node->last   = NULL;
    node->next   = NULL;
    node->parent = node;
    node->child  = node;
    
    return node;
}

MENU* Creat_BrotherMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)())
{
    MENU* node = malloc(sizeof(MENU));
    strcpy(node->Name, Name);  
    node->Width = Width;
    node->Height = Height;
    node->Frame = Frame;
    node->FontSize = FontSize;
    node->Function = Function;
    
    node->last   = nowMenu;
    node->next   = NULL;
    node->parent = nowMenu->parent;
    node->child  = node;
    
    nowMenu->next = node;
    
    return node;
}

MENU* Creat_ChildMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)())
{
    MENU* node = malloc(sizeof(MENU));
    strcpy(node->Name, Name);  
    node->Width = Width;
    node->Height = Height;
    node->Frame = Frame;
    node->FontSize = FontSize;
    node->Function = Function;
    
    node->last   = NULL;
    node->next   = NULL;
    node->parent = nowMenu;
    node->child  = node;
    
    nowMenu->child = node;
    
    return node;
}

MENU* Circle_Menu(void)
{
    MENU* finalMenu = nowMenu;
    while(nowMenu->last != NULL)
    {
        nowMenu = nowMenu->last;
    }
    nowMenu->last = finalMenu;
    finalMenu->next = nowMenu;
    
    return nowMenu->parent;
}

void MENU_child(void)
{
    nowMenu = nowMenu->child;
    ChangeTargetCursor(0,0,nowMenu->Width,nowMenu->Height);
}

void MENU_parent(void)
{
    nowMenu = nowMenu->parent;
    ChangeTargetCursor(0,0,nowMenu->Width,nowMenu->Height);
}
void No_Fun(void)
{
    
}

/***************************  Driver display部分  ***************************/

void CursorInit()
{
    CurrentCursor.Width = nowMenu->Width;
    CurrentCursor.Height = nowMenu->Height;
    TargetCursor.Width = nowMenu->Width;
    TargetCursor.Height = nowMenu->Height;
}

//对某一区域取反色
static void ReverseCoordinate(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	if(Style.Form==0){
		OLED_ReverseArea(X, Y, Width, Height);
		return;
	}
	if(Style.Form==1){
		OLED_ReverseArea(X, Y, Width, Height);
		OLED_ReverseArea(X, Y, 1, 1);
		OLED_ReverseArea(X+Width-1, Y, 1, 1);
		OLED_ReverseArea(X, Y+Height-1, 1, 1);
		OLED_ReverseArea(X+Width-1, Y+Height-1, 1, 1);
		return;
	}
	if(Style.Form==2){
		OLED_ReverseArea(X, Y, Width, Height);
		OLED_ReverseArea(X+1, Y+1, Width-2, Height-2);
		return;
	}
	if(Style.Form==3){
		OLED_ReverseArea(X, Y, Width, Height);
		OLED_ReverseArea(X+1, Y+1, Width-2, Height-2);
		OLED_ReverseArea(X, Y, 1, 1);
		OLED_ReverseArea(X+Width-1, Y, 1, 1);
		OLED_ReverseArea(X, Y+Height-1, 1, 1);
		OLED_ReverseArea(X+Width-1, Y+Height-1, 1, 1);
		return;
	}
	if(Style.Form==4){
		OLED_ReverseArea(X, Y, Width, Height);
		OLED_ReverseArea(X+1, Y+1, Width-2, Height-2);
		OLED_ReverseArea(X+5, Y, Width-9, 1);
		OLED_ReverseArea(X+5, Y+Height-1, Width-9, 1);
		OLED_ReverseArea(X, Y+5, 1, Height-9);
		OLED_ReverseArea(X+Width-1, Y+5, 1, Height-9);
		return;
	}
//	OLED_ReverseArea(X, Y, Width, Height);
	return;
	
}
//画出菜单外框
void DrawFrame(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,uint8_t Style)
{
	if(Style==0){return;}
	if(Style==1){
		OLED_DrawRectangle(X, Y,Width,Height,OLED_UNFILLED);
		return;
	}
	if(Style==2){
		OLED_DrawRectangle(X, Y,Width,Height,OLED_UNFILLED);
		OLED_ReverseArea(X,Y,1,1);
		OLED_ReverseArea(X+Width-1,Y,1,1);
		OLED_ReverseArea(X,Y+Height-1,1,1);
		OLED_ReverseArea(X+Width-1,Y+Height-1,1,1);
		return;
	}
	OLED_DrawRectangle(X, Y,Width,Height,OLED_UNFILLED);
}

//打印菜单列表
void ShowMenuList(void)
{
	MENU * NowP = nowMenu;
    int16_t NowY = TargetCursor.Y;
    OLED_Clear();
    while(NowY < 64){
        //OLED_ClearArea(0,NowY,NowP->Width,NowP->Height);
        //OLED_ShowStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,NowP->FontSize);//无汉字显示
        OLED_ShowMixStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,OLED_16X16,NowP->FontSize);//混合显示
        NowP = NowP->next;
        NowY += 16;
    }
    NowP = nowMenu->last;
    NowY = TargetCursor.Y;
    while(NowY > 0){
        NowY -= 16; 
        //OLED_ClearArea(0,NowY,NowP->Width,NowP->Height);
        //OLED_ShowStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,NowP->FontSize);//无汉字显示
        OLED_ShowMixStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,OLED_16X16,NowP->FontSize);//混合显示
        NowP = NowP->last;
    }
    ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height); //反色
    OLED_Update();
}

/***************************  Driver cursor部分  ***************************/

//取最大值函数
static int16_t max(int16_t a, int16_t b, int16_t c, int16_t d)
{
    int16_t max_val = a;

    if (b > max_val) {
        max_val = b;
    }
    if (c > max_val) {
        max_val = c;
    }
    if (d > max_val) {
        max_val = d;
    }
    
    return max_val;
}
//取绝对值函数
static uint16_t numabs(int16_t num)
{
	if(num>0)
		return num;
	if(num<0)
		return -num;
	return 0;
}

//改变目标光标位置与大小
void ChangeTargetCursor(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	if (Y<0)  Y = 0;    // 
    if (Y>48) Y = 48;   // 64-16
    TargetCursor.X=X;
	TargetCursor.Y=Y;
	TargetCursor.Width=Width;
	TargetCursor.Height=Height; 
}

void MoveCursor(void)
{
	if(Style.Move==0){
		MoveCursorLinear();
		return;
	}
	if(Style.Move==1){
		MoveCursorPID();
		return;
	}
	if(Style.Move==2){
		MoveCursorUnLinear();
		return;
	}
}

static void MoveCursorLinear(void)
{
	loop:
	//如果当前光标等于目标光标，则不执行光标移动操作，直接return。
    if(CurrentCursor.X==TargetCursor.X && CurrentCursor.Y==TargetCursor.Y && CurrentCursor.Width==TargetCursor.Width && CurrentCursor.Height==TargetCursor.Height){
		return;
	}
	
	uint8_t steps=0,speed=2;
	if(Style.Speed==0){
		speed=2;
	}
	if(Style.Speed==1){
		speed=4;
	}
	if(Style.Speed==2){
		speed=6;
	}
	float DeltaX=0,DeltaY=0,DeltaWidth=0,DeltaHeight=0,currentx=0,currenty=0,currentwidth=0,currentheight=0;
	currentx = CurrentCursor.X;
	currenty = CurrentCursor.Y;
	currentwidth = CurrentCursor.Width;
	currentheight = CurrentCursor.Height;

	DeltaX = TargetCursor.X - CurrentCursor.X;//目标值与当前值的差值
	DeltaY = TargetCursor.Y - CurrentCursor.Y;
	DeltaWidth = TargetCursor.Width - CurrentCursor.Width;
	DeltaHeight = TargetCursor.Height - CurrentCursor.Height;
    
	// 计算步数，取最大
    steps=max(numabs(DeltaX),numabs(DeltaY),numabs(DeltaWidth),numabs(DeltaHeight))/speed;
	if (steps == 0) return; 
    // 每步的移动量
    float stepX = DeltaX / steps;
    float stepY = DeltaY / steps;
	float width = DeltaWidth / steps;
    float height = DeltaHeight / steps;
	Coordinate Target_old = TargetCursor; //存储当前目标坐标
	for(int i=0;i<steps;i++){
		//在当前光标已经显示的情况下，反转已经显示的光标，使得光标消失
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		
		currentx+=stepX;
		currenty+=stepY;
		currentwidth+=width;
		currentheight+=height;
		
		CurrentCursor.X=currentx;
		CurrentCursor.Y=currenty;
		CurrentCursor.Width=currentwidth;
		CurrentCursor.Height=currentheight;
		//在当前没有光标显示的时候，反转经过计算后的当前光标坐标，使得光标显示
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		OLED_Update();
		//如果在坐标移动的过程当中，目标坐标发生改变，那么就回到函数起始点，以使得有打断动画
		if(Target_old.X != TargetCursor.X||Target_old.Y != TargetCursor.Y||Target_old.Width != TargetCursor.Width||Target_old.Height != TargetCursor.Height)
		{
			goto loop;
		}
	}
	//在当前光标已经显示的情况下，反转已经显示的光标，使得光标消失
	ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
	//在当前没有光标显示的时候，反转目标光标坐标，使得光标显示，此操作能够防止浮点数计算转整数的微小偏差
	CurrentCursor = TargetCursor; 
    ReverseCoordinate(TargetCursor.X, TargetCursor.Y, TargetCursor.Width, TargetCursor.Height);
	OLED_Update();
}

static void MoveCursorPID(void)
{
	loop:
	//如果当前光标等于目标光标，则不执行光标移动操作，直接return。
    if(CurrentCursor.X==TargetCursor.X && CurrentCursor.Y==TargetCursor.Y && CurrentCursor.Width==TargetCursor.Width && CurrentCursor.Height==TargetCursor.Height){
		return;
	}

	// 默认PID控制参数,速度中等
    float Kp_x = 0.4;  // 比例系数 X
    float Kd_x = 0.2;  // 导数系数 X
    float Ki_x = 0.1;  // 积分系数 X

    float Kp_y = 0.5;  // 比例系数 Y
    float Kd_y = 0.0;  // 导数系数 Y
    float Ki_y = 0.1;  // 积分系数 Y

    float Kp_w = 0.4;  // 比例系数 Width
    float Kd_w = 0.2;  // 导数系数 Width
    float Ki_w = 0.1;  // 积分系数 Width

    float Kp_h = 0.4;  // 比例系数 Height
    float Kd_h = 0.2;  // 导数系数 Height
    float Ki_h = 0.1;  // 积分系数 Height

	if(Style.Speed==0) {//慢速
		Kp_x = 0.4;  // 比例系数 X
		Kd_x = 0.3;  // 导数系数 X
		Ki_x = 0.1;  // 积分系数 X

		Kp_y = 0.3;  // 比例系数 Y
		Kd_y = 0.2;  // 导数系数 Y
		Ki_y = 0.05;  // 积分系数 Y

		Kp_w = 0.4;  // 比例系数 Width
		Kd_w = 0.2;  // 导数系数 Width
		Ki_w = 0.1;  // 积分系数 Width

		Kp_h = 0.4;  // 比例系数 Height
		Kd_h = 0.2;  // 导数系数 Height
		Ki_h = 0.1;  // 积分系数 Height
	}
	if(Style.Speed==1) {//中等速度
		Kp_x = 0.4;  // 比例系数 X
		Kd_x = 0.2;  // 导数系数 X
		Ki_x = 0.1;  // 积分系数 X

		Kp_y = 0.5;  // 比例系数 Y
		Kd_y = 0.0;  // 导数系数 Y
		Ki_y = 0.1;  // 积分系数 Y

		Kp_w = 0.5;  // 比例系数 Width
		Kd_w = 0.2;  // 导数系数 Width
		Ki_w = 0.1;  // 积分系数 Width

		Kp_h = 0.4;  // 比例系数 Height
		Kd_h = 0.2;  // 导数系数 Height
		Ki_h = 0.1;  // 积分系数 Height
	}
	if(Style.Speed==2) {//快速
		Kp_x = 0.7;  // 比例系数 X
		Kd_x = 0.2;  // 导数系数 X
		Ki_x = 0.3;  // 积分系数 X

		Kp_y = 0.7;  // 比例系数 Y
		Kd_y = 0.0;  // 导数系数 Y
		Ki_y = 0.3;  // 积分系数 Y

		Kp_w = 0.8;  // 比例系数 Width
		Kd_w = 0.2;  // 导数系数 Width
		Ki_w = 0.2;  // 积分系数 Width

		Kp_h = 0.8;  // 比例系数 Height
		Kd_h = 0.2;  // 导数系数 Height
		Ki_h = 0.1;  // 积分系数 Height
	}

	Coordinate Target_old = TargetCursor;

    int steps = max(numabs(TargetCursor.X - CurrentCursor.X),numabs(TargetCursor.Y - CurrentCursor.Y),numabs(TargetCursor.Width - CurrentCursor.Width),numabs(TargetCursor.Height - CurrentCursor.Height))*0.9;  // 定义步数
    float last_error_x = 0, integral_x = 0;
    float last_error_y = 0, integral_y = 0;
    float last_error_w = 0, integral_w = 0;
    float last_error_h = 0, integral_h = 0;

    for (int i = 0; i < steps; i++) {
		//在当前光标已经显示的情况下，反转已经显示的光标，使得光标消失
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
        float error_x = TargetCursor.X - CurrentCursor.X;
        float error_y = TargetCursor.Y - CurrentCursor.Y;
        float error_w = TargetCursor.Width - CurrentCursor.Width;
        float error_h = TargetCursor.Height - CurrentCursor.Height;

        // 计算PID项
        float change_x = Kp_x * error_x + Kd_x * (error_x - last_error_x) + Ki_x * integral_x;
        float change_y = Kp_y * error_y + Kd_y * (error_y - last_error_y) + Ki_y * integral_y;
        float change_w = Kp_w * error_w + Kd_w * (error_w - last_error_w) + Ki_w * integral_w;
        float change_h = Kp_h * error_h + Kd_h * (error_h - last_error_h) + Ki_h * integral_h;

        // 更新误差历史和积分项
        last_error_x = error_x;
        last_error_y = error_y;
        last_error_w = error_w;
        last_error_h = error_h;

        integral_x += error_x;
        integral_y += error_y;
        integral_w += error_w;
        integral_h += error_h;

        // 更新当前位置和尺寸
        CurrentCursor.X += change_x;
        CurrentCursor.Y += change_y;
        CurrentCursor.Width += change_w;
        CurrentCursor.Height += change_h;

        // 绘制新位置
        ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
        OLED_Update();
		if(Target_old.X != TargetCursor.X||Target_old.Y != TargetCursor.Y||Target_old.Width != TargetCursor.Width||Target_old.Height != TargetCursor.Height)
		{
			goto loop;
		}
    }
    //在当前光标已经显示的情况下，反转已经显示的光标，使得光标消失
	ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
	//在当前没有光标显示的时候，反转目标光标坐标，使得光标显示，此操作能够防止浮点数计算转整数的微小偏差
	CurrentCursor = TargetCursor; 
    ReverseCoordinate(TargetCursor.X, TargetCursor.Y, TargetCursor.Width, TargetCursor.Height);
	OLED_Update();
}

static void MoveCursorUnLinear(void)
{
loop:
	//如果当前光标等于目标光标，则不执行光标移动操作，直接return。
    if(CurrentCursor.X==TargetCursor.X && CurrentCursor.Y==TargetCursor.Y && CurrentCursor.Width==TargetCursor.Width && CurrentCursor.Height==TargetCursor.Height){
		return;
	}
	
	uint8_t steps=0,speed=2;
	if(Style.Speed==0){
		speed=2;
	}
	if(Style.Speed==1){
		speed=4;
	}
	if(Style.Speed==2){
		speed=6;
	}
	float DeltaX=0,DeltaY=0,DeltaWidth=0,DeltaHeight=0,currentx=0,currenty=0,currentwidth=0,currentheight=0;
	currentx = CurrentCursor.X;
	currenty = CurrentCursor.Y;
	currentwidth = CurrentCursor.Width;
	currentheight = CurrentCursor.Height;

	DeltaX = TargetCursor.X - CurrentCursor.X;//目标值与当前值的差值
	DeltaY = TargetCursor.Y - CurrentCursor.Y;
	DeltaWidth = TargetCursor.Width - CurrentCursor.Width;
	DeltaHeight = TargetCursor.Height - CurrentCursor.Height;

	// 计算步数，取最大
    steps=20/speed;
	if (steps == 0) return; 
    // 每步的移动量
    float stepX = DeltaX / steps;
    float stepY = DeltaY / steps;
	float width = DeltaWidth / steps;
    float height = DeltaHeight / steps;
	Coordinate Target_old = TargetCursor; //存储当前目标坐标
	while(1){
		//在当前光标已经显示的情况下，反转已经显示的光标，使得光标消失
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);

		DeltaX = TargetCursor.X - CurrentCursor.X;//目标值与当前值的差值
		DeltaY = TargetCursor.Y - CurrentCursor.Y;
		DeltaWidth = TargetCursor.Width - CurrentCursor.Width;
		DeltaHeight = TargetCursor.Height - CurrentCursor.Height;

		stepX = DeltaX / steps;
		stepY = DeltaY / steps;
		width = DeltaWidth / steps;
		height = DeltaHeight / steps;

		currentx+=stepX;
		currenty+=stepY;
		currentwidth+=width;
		currentheight+=height;

		CurrentCursor.X=currentx;
		CurrentCursor.Y=currenty;
		CurrentCursor.Width=currentwidth;
		CurrentCursor.Height=currentheight;
		//在当前没有光标显示的时候，反转经过计算后的当前光标坐标，使得光标显示
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		OLED_Update();
		if(numabs(CurrentCursor.X-TargetCursor.X)<=1&&numabs(CurrentCursor.Y-TargetCursor.Y)<=1&&numabs(CurrentCursor.Width-TargetCursor.Width)<=1&&numabs(CurrentCursor.Height-TargetCursor.Height)<=1){
			break;
		}
		//如果在坐标移动的过程当中，目标坐标发生改变，那么就回到函数起始点，以使得有打断动画
		if(Target_old.X != TargetCursor.X||Target_old.Y != TargetCursor.Y||Target_old.Width != TargetCursor.Width||Target_old.Height != TargetCursor.Height)
		{
			goto loop;
		}
	}
	//在当前光标已经显示的情况下，反转已经显示的光标，使得光标消失
	ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
	//在当前没有光标显示的时候，反转目标光标坐标，使得光标显示，此操作能够防止浮点数计算转整数的微小偏差
	CurrentCursor = TargetCursor; 
    ReverseCoordinate(TargetCursor.X, TargetCursor.Y, TargetCursor.Width, TargetCursor.Height);
	OLED_Update();
}
