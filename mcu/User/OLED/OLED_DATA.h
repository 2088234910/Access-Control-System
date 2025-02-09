#ifndef __OLED_DATA_H
#define __OLED_DATA_H

#include <stdint.h>

/*中文字符字节宽度*/
#define OLED_CHN_CHAR_WIDTH			2		//UTF-8编码格式给3，GB2312编码格式给2


/*ASCII字模数据声明*/
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F7x12[][14];
extern const uint8_t OLED_F6x8[][6];

#endif

