#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "Esp8266.h"

/**
 * @brief �ж����ַ������Ƿ�������ַ���
 *
 * @param main_string ���ַ���
 * @param substring ���ַ���
 * @return int ����������ַ�������1�����򷵻�0
 */
 //δ��main_string��substring���ǿռ��
int contains_substring(const uint8_t *main_string, const char *substring)
{
	// ���strstr()���طǿ�ָ�룬���ʾ�ҵ������ַ���
	if (strstr((const char *)main_string, substring) != NULL) {
		return 1; // �ҵ����ַ���
	} else {
		return 0; // δ�ҵ����ַ���
	}
}

/**
 * @brief ��λEsp8266ģ��
 *
 */
void Esp_Rst(void)
{			                    
    my_uart2_send_variable((uint8_t *)"AT+RST\r\n");                // ���͸�λָ��
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
    memset(Usart2type.UsartRecBuffer, 0, USART2_REC_SIZE); // ��ս��ջ�����
    Usart2type.UsartRecLen = 0;
    my_uart2_send_variable((uint8_t *)str);
	while (contains_substring(Usart2type.UsartRecBuffer, res) == 0) // �ȴ���������Ӧ
	{
		HAL_Delay(75); // �ȴ���Ӧ  
        my_uart2_send_variable((uint8_t *)str);
        //ʱ�����޸�,���볬ʱ����
	}

	return 0;
}

/**
 * @brief ��ʼ��Esp8266ģ��
 *
 * @return char ��ʼ�������ʼ�շ���0
 */
char Esp_Init(void)
{
	/* 1.��λָ�� */
	Esp_Rst();

	/* 2.����Ϊstationģʽ */
	Esp_Cmd("AT+CWMODE=1\r\n", "OK");

	/* 3.����DHCP */
	Esp_Cmd("AT+CWDHCP=1,1\r\n", "OK");

	/* 4.�����ȵ� */
	Esp_Cmd("AT+CWJAP=\"Treasure\",\"ccnu-105\"\r\n", "OK");

	/* 5.����MQTT�û���Ϣ */
	Esp_Cmd("AT+MQTTUSERCFG=0,1,\"door\",\"YBUi5NO4ng\",\"version=2018-10-31&res=products%2FYBUi5NO4ng%2Fdevices%2Fdoor&et=2082701400&method=md5&sign=YV6VchyftwWOnncKcpyglQ%3D%3D\",0,0,\"\"\r\n", "OK");

	/* 6.����MQTT���� */
	Esp_Cmd("AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n", "OK");

	/* 7.�������� */
	// �������ڽ��շ������Կͻ��˷�����Ϣ�Ļظ�
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/post/reply\",1", "OK");

	// Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/post/set_reply\",1", "OK");

	// �������ڽ��շ������·���������������
	//Esp_Cmd("AT+MQTTSUB=0,\"$sys/0S43la9qNI/LED/thing/property/set\",1", "OK");

	/* 8.������Ϣ */
	//Esp_Cmd("AT+MQTTPUB=0,\"$sys/0S43la9qNI/LED/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\\,\\\"params\\\":{\\\"LightSwitch\\\":{\\\"value\\\":true}}}\",0,0", "success");

	// Esp_Cmd("AT+MQTTPUB=0,\"$sys/0S43la9qNI/LED/thing/property/set_reply\",\"{\\\"id\\\":\\\"56\\\",\\\"code\\\":200,\\\"msg\\\":\\\"success\\\"}\",0,0", "ok");

	// Esp_Cmd("AT+MQTTPUB=0,\"$sys/0S43la9qNI/LED/thing/property/set_reply\",\"{\\\"id\\\":\\\"52\\\",\\\"code\\\":200,\\\"msg\\\":\\\"success\\\",\\\"data\\\":{\\\"LightSwitch\\\":true}}\",0,0", "ok");
	return 0;
}


/**
 * @brief �����յ����������Ƿ�����ض����豸�����ԡ�
 *
 * @param res1 �豸����ؼ��֣�����ʶ���豸��
 * @param res2 ����ֵ������ʶ���豸������״̬��
 * @return int ������յ��������а���ָ�����豸�����ԣ��򷵻�1�����򷵻�0��
 */
int Esp_Get(const char *res1, const char *res2)
{
	// ʾ���������ݸ�ʽ��"+MQTTSUBRECV:0,\"$sys/0S43la9qNI/LED/thing/property/set\",58,{\"id\":\"58\",\"version\":\"1.0\",\"params\":{\"LightSwitch\":false}}"

	/* �����ջ��������Ƿ����ָ�����豸����ؼ��� */
	if (contains_substring(Usart2type.UsartRecBuffer, res1) == 1) {
		/* ��������豸�����ټ���Ƿ����ָ��������ֵ */
		if (contains_substring(Usart2type.UsartRecBuffer, res2) == 1) {
			return 1; // �ҵ�ƥ����豸������
		}
	}
	return 0; // δ�ҵ�ƥ����豸������
}


void Esp_Get_Data(void)
{
	//���ܺ���
	
}

