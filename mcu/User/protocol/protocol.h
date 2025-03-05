#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <stdint.h>

#define FACEREG     0x30
#define FACERCO     0x31
#define FACEALARM   0x32
#define RING        0x33

typedef struct
{
    uint8_t sof;    /*(0x51)  start of frame*/
//    uint16_t crc;   /* crc */
//    uint8_t len;    /* one packet length */
    uint8_t cmd0;   /* command0 */
//    uint8_t cmd1;   /* command1 */
//    uint8_t *data;  /* fops  sizeof(data) = (len - 8) */
    uint8_t eof;    /*(0xFF) End of frame */
} PacketType;

//void ParseCmd(void);
void COM_FaceRegistration(void);
void COM_FaceRecognition(void);
void COM_FaceAlarm(void);
void COM_Ring(void);

#endif

