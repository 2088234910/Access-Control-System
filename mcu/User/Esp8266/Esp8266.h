#ifndef __ESP8266_H
#define __ESP8266_H

#include <stdint.h>

int contains_substring(const uint8_t *main_string, const char *substring);
char Esp_Init(void);

#endif
