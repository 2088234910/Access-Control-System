#include "protocol.h"
#include "usart.h"

PacketType Packet = {
    .sof = 0x51,
    .eof = 0xFF,
};

void SendPacket(PacketType *packet) 
{
    uint8_t buffer[2];
    uint16_t bufferIndex = 0;
 
    buffer[bufferIndex++] = packet->sof;
    buffer[bufferIndex++] = packet->cmd0;
    buffer[bufferIndex++] = packet->eof;
 
    my_uart1_send(buffer, bufferIndex);
}

void ParseCmd(void)
{
	
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
