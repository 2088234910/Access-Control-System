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
 
    my_uart_send(&Usart1type, buffer, 3);   //串口发送似乎有问题，发送时最后1字节错误
}

uint8_t ParseCmd(void)
{
    if (Usart1type.UsartRecFlag == 1) {
        if (Usart1type.UsartRecBuffer[0] == PSOF) {
            uint8_t res = Usart1type.UsartRecBuffer[2];
            Usart1type.UsartRecFlag = 0;
            my_uart_receive_clean(&Usart1type);
            if (res == POK) {
                return POK;
            } else if (res == PFAIL) {
                return PFAIL;
            } else {
                return res;
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

