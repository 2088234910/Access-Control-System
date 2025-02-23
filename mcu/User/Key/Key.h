#ifndef __KEY_H
#define __KEY_H

//按键
typedef enum{
    Key_NULL = 0,
	Key_UP,				
	Key_DOWN,			
	Key_MID						
}KSTATE;

extern KSTATE Key_State;

#endif
