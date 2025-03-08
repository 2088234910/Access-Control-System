#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "Esp8266.h"

/**
 * @brief 判断主字符串中是否包含子字符串
 * @param main_string 主字符串（必须是以NULL结尾的有效字符串）
 * @param substring 要查找的子字符串（必须是以NULL结尾的有效字符串）
 * @return int 
 *   - 1: 找到子字符串,且子字符串非空
 *   - 0: 未找到子字符串、子字符串为空，或输入参数为NULL
 * @note 
 *   - 若 `substring` 为空字符串（`""`），函数将返回 0（因为空字符串无实际匹配意义）。
 *   - 函数内部会检查参数合法性，避免空指针导致的未定义行为。
 */
int contains_substring(const uint8_t *main_string, const char *substring) {
    if (main_string == NULL || substring == NULL) {
        return 0;
    }
    if (strlen(substring) == 0) {
        return 0;
    }
    
    const char *main_str = (const char *)main_string;
    const char *sub_str = (const char *)substring;
    
    return (strstr(main_str, sub_str) != NULL) ? 1 : 0;
}

/**
 * @brief 向Esp8266发送指令并等待响应
 *
 * @param str 要发送的指令
 * @param res 期望的响应
 * @return int 执行结果，始终返回0
 */
int Esp_Cmd(const char *str, const char *res)
{
    my_uart_receive_clean(&Usart2type);

    uint8_t RetryTimes = 0;
    while (RetryTimes <= 7)
    {
        my_uart_send_variable(&Usart2type, (uint8_t *)str);
        HAL_Delay(300); // 等待响应
        if(contains_substring(Usart2type.UsartRecBuffer, res) == 1) {
            return 0;
        }
        my_uart_receive_clean(&Usart2type);
        RetryTimes++;
    }
	return 1;
}

/**
 * @brief 初始化Esp8266模块
 *
 * @return char 初始化结果，始终返回0
 */
char Esp_Init(void)
{
    uint8_t ret = 0;

	/* 1.复位指令 */
	//Esp_Rst();
    ret = Esp_Cmd("AT+RST\r\n", "OK");
    if (ret) return 1;

	/* 2.设置为station模式 */
	ret = Esp_Cmd("AT+CWMODE=1\r\n", "OK");
    if (ret) return 1;

	/* 3.启动DHCP */
	ret = Esp_Cmd("AT+CWDHCP=1,1\r\n", "OK");
    if (ret) return 1;

	/* 4.连接热点 */
	ret = Esp_Cmd("AT+CWJAP=\"Treasure\",\"ccnu-105\"\r\n", "OK");
    if (ret) return 1;

	/* 5.配置MQTT用户信息 */
	ret = Esp_Cmd("AT+MQTTUSERCFG=0,1,\"door\",\"YBUi5NO4ng\",\"version=2018-10-31&res=products%2FYBUi5NO4ng%2Fdevices%2Fdoor&et=2082701400&method=md5&sign=YV6VchyftwWOnncKcpyglQ%3D%3D\",0,0,\"\"\r\n", "OK");
    if (ret) return 1;

	/* 6.建立MQTT连接 */
	ret = Esp_Cmd("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n", "OK");
    if (ret) return 1;

	/* 7.订阅主题 */
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/post/reply\",1", "OK");
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/post/set_reply\",1", "OK");
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/set\",1", "OK");

	/* 8.发布消息 */

    //设定门状态初始值
	ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"open_flag\\\":{\\\"value\\\":0\\}}}\",0,0\r\n", "OK");
    if (ret) return 1;
    //陌生人测试
//	ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/event/post\",\"{\\\"id\\\":\\\"12\\\"\\,\\\"params\\\":{\\\"ret\\\":{\\\"value\\\":{\\\"MatchingRate\\\":0.25\\,\\\"Text\\\":\\\"moshengren\\\"}}}}\",0,0\r\n", "OK");
//    if (ret) return 1;

	return 0;
}


/**
 * @brief 检查接收到的数据中是否包含特定的设备和属性。
 *
 * @param res1 设备名或关键字，用于识别设备。
 * @param res2 属性值，用于识别设备的属性状态。
 * @return int 如果接收到的数据中包含指定的设备和属性，则返回1，否则返回0。
 */
int Esp_Get(const char *res1, const char *res2)
{
	// 示例接收数据格式："+MQTTSUBRECV:0,\"$sys/0S43la9qNI/LED/thing/property/set\",58,{\"id\":\"58\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":false}}"

	/* 检查接收缓冲区中是否包含指定的设备名或关键字 */
	if (contains_substring(Usart2type.UsartRecBuffer, res1) == 1) {
		/* 如果包含设备名，再检查是否包含指定的属性值 */
		if (contains_substring(Usart2type.UsartRecBuffer, res2) == 1) {
			return 1; // 找到匹配的设备和属性
		}
	}
	return 0; // 未找到匹配的设备和属性
}


void Esp_Get_Data(void)
{
	//功能函数
	
}

