#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Esp8266.h"

/**
 * @brief �ж����ַ������Ƿ�������ַ���
 * @param main_string ���ַ�������������NULL��β����Ч�ַ�����
 * @param substring Ҫ���ҵ����ַ�������������NULL��β����Ч�ַ�����
 * @return int 
 *   - 1: �ҵ����ַ���,�����ַ����ǿ�
 *   - 0: δ�ҵ����ַ��������ַ���Ϊ�գ����������ΪNULL
 * @note 
 *   - �����ڲ���������Ϸ��ԣ������ָ�뵼�µ�δ������Ϊ��
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
 * @brief ��Esp8266����ָ��ȴ���Ӧ
 *
 * @param str Ҫ���͵�ָ��
 * @param res ��������Ӧ
 * @return int ִ�н����ʼ�շ���0
 */
int Esp_Cmd(const char *str, const char *res)
{
    my_uart_receive_clean(&Usart2type);

    uint8_t RetryTimes = 0;
    while (RetryTimes <= 7)
    {
        my_uart_send_variable(&Usart2type, (uint8_t *)str);
        HAL_Delay(300); // �ȴ���Ӧ
        if(contains_substring(Usart2type.UsartRecBuffer, res) == 1) {
            return 0;
        }
        my_uart_receive_clean(&Usart2type);
        RetryTimes++;
    }
	return 1;
}

/**
 * @brief ��ʼ��Esp8266ģ��
 *
 * @return char ��ʼ�������ʼ�շ���0
 */
char Esp_Init(void)
{
    //ע��ָ���β����\r\n !!!
    uint8_t ret = 0;

	/* 1.��λָ�� */
	//Esp_Rst();
    ret = Esp_Cmd("AT+RST\r\n", "OK");
    if (ret) return 1;

	/* 2.����Ϊstationģʽ */
	ret = Esp_Cmd("AT+CWMODE=1\r\n", "OK");
    if (ret) return 1;

	/* 3.����DHCP */
	ret = Esp_Cmd("AT+CWDHCP=1,1\r\n", "OK");
    if (ret) return 1;

	/* 4.�����ȵ� */
	ret = Esp_Cmd("AT+CWJAP=\"Treasure\",\"ccnu-105\"\r\n", "OK");
    if (ret) return 1;

	/* 5.����MQTT�û���Ϣ */
	ret = Esp_Cmd("AT+MQTTUSERCFG=0,1,\"door\",\"YBUi5NO4ng\",\"version=2018-10-31&res=products%2FYBUi5NO4ng%2Fdevices%2Fdoor&et=2082701400&method=md5&sign=YV6VchyftwWOnncKcpyglQ%3D%3D\",0,0,\"\"\r\n", "OK");
    if (ret) return 1;

	/* 6.����MQTT���� */
	ret = Esp_Cmd("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n", "OK");
    if (ret) return 1;

	/* 7.�������� */
	ret = Esp_Cmd("AT+MQTTSUB=0,\"$sys/YBUi5NO4ng/door/thing/property/post/reply\",1\r\n", "OK");
    if (ret) return 1;
	ret = Esp_Cmd("AT+MQTTSUB=0,\"$sys/YBUi5NO4ng/door/thing/property/post/set_reply\",1\r\n", "OK");
    if (ret) return 1;
	ret = Esp_Cmd("AT+MQTTSUB=0,\"$sys/YBUi5NO4ng/door/thing/property/set\",1\r\n", "OK");
    if (ret) return 1;

	/* 8.������Ϣ */
    //�趨��״̬��ʼֵ
	ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"door_flag\\\":{\\\"value\\\":false\\}}}\",0,0\r\n", "OK");
    if (ret) return 1;
    //İ���˲���
//	ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/event/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"ret\\\":{\\\"value\\\":{\\\"MatchingRate\\\":0.01\\,\\\"Text\\\":\\\"Stranger!!!\\\"}}}}\",0,0\r\n", "OK");
//    if (ret) return 1;
    
    my_uart_receive_clean(&Usart2type);
    
	return 0;
}


/**
 * @brief �����յ����������Ƿ����ָ�����ַ�����
 *
 * @param res1 ָ�����ַ�����
 * @param res2 ָ�����ַ�����
 * @return int ������յ��������а���ָ�����ַ������򷵻�1�����򷵻�0��
 */
static int Esp_Get(const char *res1, const char *res2)
{
	// ʾ���������ݸ�ʽ��"+MQTTSUBRECV:0,\"$sys/0S43la9qNI/LED/thing/property/set\",58,{\"id\":\"58\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":false}}"

	if (contains_substring(Usart2type.UsartRecBuffer, res1) == 1) {
		if (contains_substring(Usart2type.UsartRecBuffer, res2) == 1) {
			return 1;
		}
	}
	return 0;
}

static int Esp_ID()
{
    const char *id_start = strstr((const char *)Usart2type.UsartRecBuffer, "\"id\":\"");
    if (id_start) {
        id_start += strlen("\"id\":\"");    //ֵ����ʼλ��
        const char *id_end = strchr(id_start, '\"');    //����ֵ�Ľ���λ�ã���һ��˫���ţ�
        if (id_end) {
            size_t id_length = id_end - id_start;
            char id_str[32]; // ����id����󳤶�Ϊ32
            strncpy(id_str, id_start, id_length);
            id_str[id_length] = '\0';
            char *endptr;
            int id = (int)strtol(id_str, &endptr, 10);
            if (*endptr == '\0') {  //�ַ���ת����ֵ�ɹ�
                return id;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void Esp_Set_Reply()
{
    if (Usart2type.UsartRecFlag == 1) {
        if (Esp_Get("/property/set", "door_flag")) {
            char cmd[256] = {0};
            int id = Esp_ID();
            if (Esp_Get("door_flag", "true")) {
                Esp_Door_Open();
            } else if (Esp_Get("door_flag", "false")) {
                Esp_Door_Close();
            }
            snprintf(cmd, 256, "AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/property/set_reply\",\"{\\\"id\\\":\\\"%d\\\"\\,\\\"code\\\":200\\,\\\"msg\\\":\\\"success\\\"}\",0,0\r\n", id);
            Esp_Cmd(cmd, "OK");
        }
        my_uart_receive_clean(&Usart2type);
    }
}

uint8_t Esp_Door_Open()
{
    uint8_t ret = 0;
    ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"door_flag\\\":{\\\"value\\\":true\\}}}\",0,0\r\n", "OK");
    return ret;
}

uint8_t Esp_Door_Close()
{
    uint8_t ret = 0;
    ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"door_flag\\\":{\\\"value\\\":false\\}}}\",0,0\r\n", "OK");
    return ret;
}

uint8_t Esp_Face_Alarm()
{
    uint8_t ret = 0;
    ret = Esp_Cmd("AT+MQTTPUB=0,\"$sys/YBUi5NO4ng/door/thing/event/post\",\"{\\\"id\\\":\\\"12\\\"\\,\\\"params\\\":{\\\"ret\\\":{\\\"value\\\":{\\\"MatchingRate\\\":0.01\\,\\\"Text\\\":\\\"Stranger!!!\\\"}}}}\",0,0\r\n", "OK");
//    MatchingRate��δʵ��
    return ret;
}
