#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "Esp8266.h"

/**
 * @brief 判断主字符串中是否包含子字符串
 *
 * @param main_string 主字符串
 * @param substring 子字符串
 * @return int 如果包含子字符串返回1，否则返回0
 */
 //未对main_string和substring做非空检查
int contains_substring(const uint8_t *main_string, const char *substring)
{
	// 如果strstr()返回非空指针，则表示找到了子字符串
	if (strstr((const char *)main_string, substring) != NULL) {
		return 1; // 找到子字符串
	} else {
		return 0; // 未找到子字符串
	}
}

/**
 * @brief 复位Esp8266模块
 *
 */
void Esp_Rst(void)
{			                    
    my_uart2_send_variable((uint8_t *)"AT+RST\r\n");                // 发送复位指令
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
    memset(Usart2type.UsartRecBuffer, 0, USART2_REC_SIZE); // 清空接收缓冲区
    Usart2type.UsartRecLen = 0;
    my_uart2_send_variable((uint8_t *)str);
	while (contains_substring(Usart2type.UsartRecBuffer, res) == 0) // 等待期望的响应
	{
		HAL_Delay(75); // 等待响应  
        my_uart2_send_variable((uint8_t *)str);
        //时间需修改,加入超时处理
	}

	return 0;
}

/**
 * @brief 初始化Esp8266模块
 *
 * @return char 初始化结果，始终返回0
 */
char Esp_Init(void)
{
	/* 1.复位指令 */
	Esp_Rst();

	/* 2.设置为station模式 */
	Esp_Cmd("AT+CWMODE=1\r\n", "OK");

	/* 3.启动DHCP */
	Esp_Cmd("AT+CWDHCP=1,1\r\n", "OK");

	/* 4.连接热点 */
	Esp_Cmd("AT+CWJAP=\"Treasure\",\"ccnu-105\"\r\n", "OK");

	/* 5.配置MQTT用户信息 */
	Esp_Cmd("AT+MQTTUSERCFG=0,1,\"door\",\"YBUi5NO4ng\",\"version=2018-10-31&res=products%2FYBUi5NO4ng%2Fdevices%2Fdoor&et=2082701400&method=md5&sign=YV6VchyftwWOnncKcpyglQ%3D%3D\",0,0,\"\"\r\n", "OK");

	/* 6.建立MQTT连接 */
	Esp_Cmd("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n", "OK");

	/* 7.订阅主题 */
	// 主题用于接收服务器对客户端发布消息的回复
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/post/reply\",1", "OK");

	// Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/post/set_reply\",1", "OK");

	// 主题用于接收服务器下发的属性设置命令
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/set\",1", "OK");

	/* 8.发布消息 */
	//Esp_Cmd("AT+MQTTPUB=0,\"$sys/0S43la9qNI/LED/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\\,\\\"params\\\":{\\\"LightSwitch\\\":{\\\"value\\\":true}}}\",0,0", "success");

	// Esp_Cmd("AT+MQTTPUB=0,\"$sys/0S43la9qNI/LED/thing/property/set_reply\",\"{\\\"id\\\":\\\"56\\\",\\\"code\\\":200,\\\"msg\\\":\\\"success\\\"}\",0,0", "ok");

	// Esp_Cmd("AT+MQTTPUB=0,\"$sys/0S43la9qNI/LED/thing/property/set_reply\",\"{\\\"id\\\":\\\"52\\\",\\\"code\\\":200,\\\"msg\\\":\\\"success\\\",\\\"data\\\":{\\\"LightSwitch\\\":true}}\",0,0", "ok");
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

