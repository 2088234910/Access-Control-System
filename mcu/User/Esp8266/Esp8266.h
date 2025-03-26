#ifndef __ESP8266_H
#define __ESP8266_H

#include <stdint.h>

int contains_substring(const uint8_t *main_string, const char *substring);
char Esp_Init(void);
void Esp_Set_Reply(void);
uint8_t Esp_Door_Open(void);
uint8_t Esp_Door_Close(void);
uint8_t Esp_Face_Alarm(void);

#endif
