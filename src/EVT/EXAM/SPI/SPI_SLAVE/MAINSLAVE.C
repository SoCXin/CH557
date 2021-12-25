/********************************** (C) COPYRIGHT *******************************
* File Name          : MAINSLAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 SPI�豸������ʾ������SPI�������������շ�
*******************************************************************************/
#include "CH557X.H"
#include "Debug.H"
#include "SPI.H"

/*Ӳ���ӿڶ���*/
/******************************************************************************
ʹ��CH557 Ӳ��SPI�ӿ�
         CH557
         P1.4����SCS
         P1.5����MOSI
         P1.6����MISO
         P1.7����SCK
*******************************************************************************/

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	UINT8 ret,i=0;
	
	CfgFsys();
	mDelaymS(5);                                                               //�޸�ϵͳ��Ƶ�������Լ���ʱ�ȴ���Ƶ�ȶ�
	mInitSTDIO();                                                             //����0��ʼ��
	printf("EXAM SPI Slave\n");
	SPISlvModeSet();                                                          //SPI�ӻ�ģʽ����
	
	while(1)
	{
#ifndef SPI_INTERRUPT
		ret = CH557SPISlvRead();                                               //��������CS=0
		CH557SPISlvWrite(ret^0xFF);                                            //SPI�ȴ�����������ȡ��,SPI ����ÿ�ζ�֮ǰ�Ƚ�CS=0�������CS=1
		printf("Read#%02x\n",(UINT16)ret);

#endif
	}
}