#include "protocol.h"
#include "usart.h"
#include <string.h>

PacketType Packet;

void SendPacket(PacketType *packet)
{
    uint8_t buffer[3];
 
    buffer[0] = PSOF;
    buffer[1] = packet->cmd0;
    buffer[2] = PEOF;
 
    my_uart1_send(buffer, 3);   //���ڷ����ƺ������⣬����ʱ���1�ֽڴ���
}

uint8_t ParseCmd(void)
{
    if (Usart1type.UsartRecFlag == 1) {
        if (Usart1type.UsartRecBuffer[0] == PSOF) {
            uint8_t res = Usart1type.UsartRecBuffer[2];
            Usart1type.UsartRecFlag = 0;
            Usart1type.UsartRecLen = 0;
            memset(Usart1type.UsartRecBuffer, 0x00, USART_REC_SIZE);   //�ɷ�װ�ɺ���
            if (res == POK) {
                return POK;
            } else if (res == PFAIL) {
                return PFAIL;
            } else {
                return PERROR;
            }
        } else {
            return PERROR;
        }
    } else {
        return NRES;
    }
}

void COM_FaceRegistration(void)
{
	Packet.cmd0 = FACEREG;
    SendPacket(&Packet);
}

void COM_FaceRecognition(void)
{
	Packet.cmd0 = FACERCO;
    SendPacket(&Packet);
}

void COM_FaceAlarm(void)
{
	Packet.cmd0 = FACEALARM;
    SendPacket(&Packet);
}

void COM_Ring(void)
{
	Packet.cmd0 = RING;
    SendPacket(&Packet);
}

void COM_Record(void)
{
	Packet.cmd0 = RECORD;
    SendPacket(&Packet);
}

