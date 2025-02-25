#include <string.h>  
#include <stdlib.h>
#include "Menu.h"
#include "OLED.h"
#include "Key.h"

MENU* nowMenu = NULL;

Coordinate CurrentCursor={0,0,0,0};   //��ǰ���
Coordinate TargetCursor={0,0,0,0};    //Ŀ����

CoordinateStyle Style = {   //�����ʽ
    1, //�ƶ���ʽ��0�����ƶ���1pid�ص��ƶ�,2�������ƶ�
	4, //��״��0ʵ�ľ��Σ�1Բ�Ǿ��Σ�2���ľ��ο�,3Բ�ǿ��ľ��ο�4����ѡ��
	0  //�ٶȣ�0,1,2�ֱ�������п�
};

/***************************  static��������  ***************************/

static MENU* Creat_Menu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)());
static MENU* Creat_BrotherMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)());
static MENU* Creat_ChildMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)());
static MENU* Circle_Menu(void);
static void MENU_child(void);
static void MENU_parent(void);
static void No_Fun(void);
static uint16_t numabs(int16_t num);
static int16_t max(int16_t a, int16_t b, int16_t c, int16_t d);
static void CurrentCursorInit(void);
static void ReverseCoordinate(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
static void DrawFrame(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,uint8_t Style);
static void ShowMenuList(void);
static void ChangeTargetCursor(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
static void MoveCursorLinear(void);
static void MoveCursorPID(void);
static void MoveCursorUnLinear(void);
static void MoveCursor(void);

/***************************  User����  ***************************/

void Menu_Init(void)
{
    nowMenu = Creat_Menu("- ����ʶ��",108,16,0,OLED_8X16,*No_Fun);
    nowMenu = Creat_BrotherMenu("- ����ע��",108,16,0,OLED_8X16,*No_Fun);
    nowMenu = Creat_BrotherMenu("- ����",72,16,0,OLED_8X16,*No_Fun);
    nowMenu = Creat_BrotherMenu("- ¼��",72,16,0,OLED_8X16,*No_Fun);
    nowMenu = Creat_BrotherMenu("- ����",72,16,0,OLED_8X16,*MENU_child);
        nowMenu = Creat_ChildMenu("- A",72,16,0,OLED_8X16,*No_Fun);
        nowMenu = Creat_BrotherMenu("- B",72,16,0,OLED_8X16,*No_Fun);
        nowMenu = Creat_BrotherMenu("- C",72,16,0,OLED_8X16,*No_Fun);
        nowMenu = Creat_BrotherMenu("- <<<",72,16,0,OLED_8X16,*MENU_parent);
        nowMenu = Circle_Menu();
    nowMenu = Circle_Menu();
}

void Menu_Choose(void)
{
    OLED_Clear();
    //DrawFrame(2,2,82,62,2);      //���ߴ硢�߿�ߴ硢�ı�λ�ö��д�����
    CurrentCursorInit();
	ShowMenuList();
	while(1){
        if(Key_State != Key_NULL){
            if(Key_State == Key_UP){
                nowMenu = nowMenu->last;
                ChangeTargetCursor(0,CurrentCursor.Y-nowMenu->Height,nowMenu->Width,nowMenu->Height);
            }
            else if(Key_State == Key_DOWN){
                nowMenu = nowMenu->next;
                ChangeTargetCursor(0,CurrentCursor.Y+CurrentCursor.Height,nowMenu->Width,nowMenu->Height);
            }
            else if(Key_State == Key_MID){
                (*nowMenu->Function)();
            }
            if (CurrentCursor.Y == TargetCursor.Y || Key_State == Key_MID)  ShowMenuList();
            MoveCursor(); //����ƶ�
            Key_State = Key_NULL;
        }  
	}
}

/***************************  ���ɲ���  ***************************/

static MENU* Creat_Menu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)())
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

static MENU* Creat_BrotherMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)())
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

static MENU* Creat_ChildMenu(char* Name,int16_t Width,int16_t Height,int16_t Frame,uint8_t FontSize,void(* Function)())
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

static MENU* Circle_Menu(void)
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

static void MENU_child(void)
{
    nowMenu = nowMenu->child;
    ChangeTargetCursor(0,0,nowMenu->Width,nowMenu->Height);
}

static void MENU_parent(void)
{
    nowMenu = nowMenu->parent;
    ChangeTargetCursor(0,0,nowMenu->Width,nowMenu->Height);
}
static void No_Fun(void)
{
    
}

/***************************  ��ʾ����  ***************************/

static void CurrentCursorInit()
{
    CurrentCursor.Width = nowMenu->Width;
    CurrentCursor.Height = nowMenu->Height;
}

//��ĳһ����ȡ��ɫ
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
	OLED_ReverseArea(X, Y, Width,Height);
	return;
	
}
//�����˵����
static void DrawFrame(uint8_t X,uint8_t Y,uint8_t Width,uint8_t Height,uint8_t Style)
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

//��ӡ�˵��б�
static void ShowMenuList(void)
{
	MENU * NowP = nowMenu;
    int16_t NowY = TargetCursor.Y;
    OLED_Clear();
    while(NowY < 64){
        //OLED_ClearArea(0,NowY,NowP->Width,NowP->Height);
        //OLED_ShowStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,NowP->FontSize);//�޺�����ʾ
        OLED_ShowMixStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,OLED_16X16,NowP->FontSize);//�����ʾ
        NowP = NowP->next;
        NowY += 16; 
    }
    NowP = nowMenu->last;
    NowY = TargetCursor.Y;
    while(NowY > 0){
        NowY -= 16; 
        //OLED_ClearArea(0,NowY,NowP->Width,NowP->Height);
        //OLED_ShowStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,NowP->FontSize);//�޺�����ʾ
        OLED_ShowMixStringArea(0,NowY,NowP->Width,NowP->Height,8,NowY,NowP->Name,OLED_16X16,NowP->FontSize);//�����ʾ
        NowP = NowP->last;
    }
    ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height); //��ɫ
    OLED_Update();
}

/***************************  �ƶ�����  ***************************/

//�ı�Ŀ����λ�����С
static void ChangeTargetCursor(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
	if (Y<0)  Y = 0;    // 
    if (Y>48) Y = 48;   // 64-16
    TargetCursor.X=X;
	TargetCursor.Y=Y;
	TargetCursor.Width=Width;
	TargetCursor.Height=Height; 
}

//ȡ���ֵ����
static int16_t max(int16_t a, int16_t b, int16_t c, int16_t d)
{
    int16_t max_val = a; // ����a������

    if (b > max_val) {
        max_val = b; // ���b���ڵ�ǰ���ֵ����������ֵΪb
    }
    if (c > max_val) {
        max_val = c; // ���c���ڵ�ǰ���ֵ����������ֵΪc
    }
    if (d > max_val) {
        max_val = d; // ���d���ڵ�ǰ���ֵ����������ֵΪd
    }
    
    return max_val; // �������ֵ
}
//ȡ����ֵ����
static uint16_t numabs(int16_t num)
{
	if(num>0)
		return num;
	if(num<0)
		return -num;
	return 0;
}

static void MoveCursorLinear(void)
{
	loop:
	//�����ǰ������Ŀ���꣬��ִ�й���ƶ�������ֱ��return��
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

	DeltaX = TargetCursor.X - CurrentCursor.X;//Ŀ��ֵ�뵱ǰֵ�Ĳ�ֵ
	DeltaY = TargetCursor.Y - CurrentCursor.Y;
	DeltaWidth = TargetCursor.Width - CurrentCursor.Width;
	DeltaHeight = TargetCursor.Height - CurrentCursor.Height;
    
	// ���㲽����ȡ���
    steps=max(numabs(DeltaX),numabs(DeltaY),numabs(DeltaWidth),numabs(DeltaHeight))/speed;
	if (steps == 0) return; 
    // ÿ�����ƶ���
    float stepX = DeltaX / steps;
    float stepY = DeltaY / steps;
	float width = DeltaWidth / steps;
    float height = DeltaHeight / steps;
	Coordinate Target_old = TargetCursor; //�洢��ǰĿ������
	for(int i=0;i<steps;i++){
		//�ڵ�ǰ����Ѿ���ʾ������£���ת�Ѿ���ʾ�Ĺ�꣬ʹ�ù����ʧ
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		
		currentx+=stepX;
		currenty+=stepY;
		currentwidth+=width;
		currentheight+=height;
		
		CurrentCursor.X=currentx;
		CurrentCursor.Y=currenty;
		CurrentCursor.Width=currentwidth;
		CurrentCursor.Height=currentheight;
		//�ڵ�ǰû�й����ʾ��ʱ�򣬷�ת���������ĵ�ǰ������꣬ʹ�ù����ʾ
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		OLED_Update();
		//����������ƶ��Ĺ��̵��У�Ŀ�����귢���ı䣬��ô�ͻص�������ʼ�㣬��ʹ���д�϶���
		if(Target_old.X != TargetCursor.X||Target_old.Y != TargetCursor.Y||Target_old.Width != TargetCursor.Width||Target_old.Height != TargetCursor.Height)
		{
			goto loop;
		}
	}
	//�ڵ�ǰ����Ѿ���ʾ������£���ת�Ѿ���ʾ�Ĺ�꣬ʹ�ù����ʧ
	ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
	//�ڵ�ǰû�й����ʾ��ʱ�򣬷�תĿ�������꣬ʹ�ù����ʾ���˲����ܹ���ֹ����������ת������΢Сƫ��
	CurrentCursor = TargetCursor; 
    ReverseCoordinate(TargetCursor.X, TargetCursor.Y, TargetCursor.Width, TargetCursor.Height);
	OLED_Update();
     
}

static void MoveCursorPID(void)
{
	loop:
	//�����ǰ������Ŀ���꣬��ִ�й���ƶ�������ֱ��return��
    if(CurrentCursor.X==TargetCursor.X && CurrentCursor.Y==TargetCursor.Y && CurrentCursor.Width==TargetCursor.Width && CurrentCursor.Height==TargetCursor.Height){
		return;
	}
	
	// Ĭ��PID���Ʋ���,�ٶ��е�
    float Kp_x = 0.4;  // ����ϵ�� X
    float Kd_x = 0.2;  // ����ϵ�� X
    float Ki_x = 0.1;  // ����ϵ�� X
          
    float Kp_y = 0.5;  // ����ϵ�� Y
    float Kd_y = 0.0;  // ����ϵ�� Y
    float Ki_y = 0.1;  // ����ϵ�� Y
          
    float Kp_w = 0.4;  // ����ϵ�� Width
    float Kd_w = 0.2;  // ����ϵ�� Width
    float Ki_w = 0.1;  // ����ϵ�� Width
          
    float Kp_h = 0.4;  // ����ϵ�� Height
    float Kd_h = 0.2;  // ����ϵ�� Height
    float Ki_h = 0.1;  // ����ϵ�� Height
	
	if(Style.Speed==0) {//����
		Kp_x = 0.4;  // ����ϵ�� X
		Kd_x = 0.3;  // ����ϵ�� X
		Ki_x = 0.1;  // ����ϵ�� X
		
		Kp_y = 0.3;  // ����ϵ�� Y
		Kd_y = 0.2;  // ����ϵ�� Y
		Ki_y = 0.05;  // ����ϵ�� Y
		
		Kp_w = 0.4;  // ����ϵ�� Width
		Kd_w = 0.2;  // ����ϵ�� Width
		Ki_w = 0.1;  // ����ϵ�� Width
		
		Kp_h = 0.4;  // ����ϵ�� Height
		Kd_h = 0.2;  // ����ϵ�� Height
		Ki_h = 0.1;  // ����ϵ�� Height
	}
	if(Style.Speed==1) {//�е��ٶ�
		Kp_x = 0.4;  // ����ϵ�� X
		Kd_x = 0.2;  // ����ϵ�� X
		Ki_x = 0.1;  // ����ϵ�� X
		
		Kp_y = 0.5;  // ����ϵ�� Y
		Kd_y = 0.0;  // ����ϵ�� Y
		Ki_y = 0.1;  // ����ϵ�� Y
		
		Kp_w = 0.5;  // ����ϵ�� Width
		Kd_w = 0.2;  // ����ϵ�� Width
		Ki_w = 0.1;  // ����ϵ�� Width
		
		Kp_h = 0.4;  // ����ϵ�� Height
		Kd_h = 0.2;  // ����ϵ�� Height
		Ki_h = 0.1;  // ����ϵ�� Height
	}
	if(Style.Speed==2) {//����
		Kp_x = 0.7;  // ����ϵ�� X
		Kd_x = 0.2;  // ����ϵ�� X
		Ki_x = 0.3;  // ����ϵ�� X
		
		Kp_y = 0.7;  // ����ϵ�� Y
		Kd_y = 0.0;  // ����ϵ�� Y
		Ki_y = 0.3;  // ����ϵ�� Y
		
		Kp_w = 0.8;  // ����ϵ�� Width
		Kd_w = 0.2;  // ����ϵ�� Width
		Ki_w = 0.2;  // ����ϵ�� Width
		
		Kp_h = 0.8;  // ����ϵ�� Height
		Kd_h = 0.2;  // ����ϵ�� Height
		Ki_h = 0.1;  // ����ϵ�� Height
	}
	
	Coordinate Target_old = TargetCursor;
	
    int steps = max(numabs(TargetCursor.X - CurrentCursor.X),numabs(TargetCursor.Y - CurrentCursor.Y),numabs(TargetCursor.Width - CurrentCursor.Width),numabs(TargetCursor.Height - CurrentCursor.Height))*0.9;  // ���岽��
    float last_error_x = 0, integral_x = 0;
    float last_error_y = 0, integral_y = 0;
    float last_error_w = 0, integral_w = 0;
    float last_error_h = 0, integral_h = 0;

    for (int i = 0; i < steps; i++) {
		//�ڵ�ǰ����Ѿ���ʾ������£���ת�Ѿ���ʾ�Ĺ�꣬ʹ�ù����ʧ
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
        float error_x = TargetCursor.X - CurrentCursor.X;
        float error_y = TargetCursor.Y - CurrentCursor.Y;
        float error_w = TargetCursor.Width - CurrentCursor.Width;
        float error_h = TargetCursor.Height - CurrentCursor.Height;

        // ����PID��
        float change_x = Kp_x * error_x + Kd_x * (error_x - last_error_x) + Ki_x * integral_x;
        float change_y = Kp_y * error_y + Kd_y * (error_y - last_error_y) + Ki_y * integral_y;
        float change_w = Kp_w * error_w + Kd_w * (error_w - last_error_w) + Ki_w * integral_w;
        float change_h = Kp_h * error_h + Kd_h * (error_h - last_error_h) + Ki_h * integral_h;

        // ���������ʷ�ͻ�����
        last_error_x = error_x;
        last_error_y = error_y;
        last_error_w = error_w;
        last_error_h = error_h;

        integral_x += error_x;
        integral_y += error_y;
        integral_w += error_w;
        integral_h += error_h;

        // ���µ�ǰλ�úͳߴ�
        CurrentCursor.X += change_x;
        CurrentCursor.Y += change_y;
        CurrentCursor.Width += change_w;
        CurrentCursor.Height += change_h;

        // ������λ��
        ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
        OLED_Update();
		if(Target_old.X != TargetCursor.X||Target_old.Y != TargetCursor.Y||Target_old.Width != TargetCursor.Width||Target_old.Height != TargetCursor.Height)
		{
			goto loop;
		}
    }
    //�ڵ�ǰ����Ѿ���ʾ������£���ת�Ѿ���ʾ�Ĺ�꣬ʹ�ù����ʧ
	ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
	//�ڵ�ǰû�й����ʾ��ʱ�򣬷�תĿ�������꣬ʹ�ù����ʾ���˲����ܹ���ֹ����������ת������΢Сƫ��
	CurrentCursor = TargetCursor; 
    ReverseCoordinate(TargetCursor.X, TargetCursor.Y, TargetCursor.Width, TargetCursor.Height);
	OLED_Update();
}

static void MoveCursorUnLinear(void)
{
loop:
	//�����ǰ������Ŀ���꣬��ִ�й���ƶ�������ֱ��return��
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

	DeltaX = TargetCursor.X - CurrentCursor.X;//Ŀ��ֵ�뵱ǰֵ�Ĳ�ֵ
	DeltaY = TargetCursor.Y - CurrentCursor.Y;
	DeltaWidth = TargetCursor.Width - CurrentCursor.Width;
	DeltaHeight = TargetCursor.Height - CurrentCursor.Height;
    
	// ���㲽����ȡ���
    steps=20/speed;
	if (steps == 0) return; 
    // ÿ�����ƶ���
    float stepX = DeltaX / steps;
    float stepY = DeltaY / steps;
	float width = DeltaWidth / steps;
    float height = DeltaHeight / steps;
	Coordinate Target_old = TargetCursor; //�洢��ǰĿ������
	while(1){
		//�ڵ�ǰ����Ѿ���ʾ������£���ת�Ѿ���ʾ�Ĺ�꣬ʹ�ù����ʧ
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		
		DeltaX = TargetCursor.X - CurrentCursor.X;//Ŀ��ֵ�뵱ǰֵ�Ĳ�ֵ
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
		//�ڵ�ǰû�й����ʾ��ʱ�򣬷�ת���������ĵ�ǰ������꣬ʹ�ù����ʾ
		ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
		OLED_Update();
		if(numabs(CurrentCursor.X-TargetCursor.X)<=1&&numabs(CurrentCursor.Y-TargetCursor.Y)<=1&&numabs(CurrentCursor.Width-TargetCursor.Width)<=1&&numabs(CurrentCursor.Height-TargetCursor.Height)<=1){
			break;
		}
		//����������ƶ��Ĺ��̵��У�Ŀ�����귢���ı䣬��ô�ͻص�������ʼ�㣬��ʹ���д�϶���
		if(Target_old.X != TargetCursor.X||Target_old.Y != TargetCursor.Y||Target_old.Width != TargetCursor.Width||Target_old.Height != TargetCursor.Height)
		{
			goto loop;
		}
	}
	//�ڵ�ǰ����Ѿ���ʾ������£���ת�Ѿ���ʾ�Ĺ�꣬ʹ�ù����ʧ
	ReverseCoordinate(CurrentCursor.X, CurrentCursor.Y, CurrentCursor.Width, CurrentCursor.Height);
	//�ڵ�ǰû�й����ʾ��ʱ�򣬷�תĿ�������꣬ʹ�ù����ʾ���˲����ܹ���ֹ����������ת������΢Сƫ��
	CurrentCursor = TargetCursor; 
    ReverseCoordinate(TargetCursor.X, TargetCursor.Y, TargetCursor.Width, TargetCursor.Height);
	OLED_Update();
     
}
static void MoveCursor(void)
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

