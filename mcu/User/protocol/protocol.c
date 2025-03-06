#include "protocol.h"
#include "usart.h"

PacketType Packet;

void SendPacket(PacketType *packet) 
{
    uint8_t buffer[2];
    uint16_t bufferIndex = 0;
 
    buffer[bufferIndex++] = SOF;
    buffer[bufferIndex++] = packet->cmd0;
    buffer[bufferIndex++] = EOF;
 
    my_uart1_send(buffer, 3);   //���ڷ����ƺ������⣬����ʱ���1�ֽڴ���
}

void ParseCmd(void)
{
    if (Usart2type.UsartRecFlag == 1) {
        if (Usart2type.UsartDMARecBuffer[0]==SOF) {
            if (Usart2type.UsartDMARecBuffer[1]==FACEREG) {
                if (Usart2type.UsartDMARecBuffer[2]==0x01) {
                    //����ע��ɹ�
                } else {
                    //����ע��ʧ��
                }
            }
          
        }
      Usart2type.UsartRecFlag = 0;
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
