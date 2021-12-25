/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 SPI������ģʽ�ӿں���
ע��Ƭѡ��Чʱ���ӻ����Զ�����SPI0_S_PRE��Ԥ��ֵ��������λ��������������ÿ�����Ƭѡ
��Чǰ��SPI0_S_PRE�Ĵ���д��Ԥ��ֵ�������������˶����׸������ֽڣ�����ʱע����������
ȡ��SPI0_S_PRE�����ֵ����һ��S0_IF_BYTE�жϡ�
���Ƭѡ����Ч����Ч���ӻ����Ƚ��з��͵Ļ�����ð���������ֽڷŵ�SPI0_S_PRE�Ĵ����У�
����Ѿ�����Ƭѡ��Ч�Ļ�������ʹ��SPI0_DATA�Ϳ���
*******************************************************************************/
#include "SPI.H"
#pragma  NOAREGS

/*******************************************************************************
* Function Name  : SPIMasterModeSet
* Description    : SPI����ģʽ��ʼ��
* Input          : mode��SPIģʽѡ��
*                    0-ģʽ0
*                    3-ģʽ3
* Return         : None
*******************************************************************************/
void SPIMasterModeSet(UINT8 mode)
{
	SCS = 1;
	P1_MOD_OC &= ~(bSCS|bMOSI|bSCK);
	P1_DIR_PU |= (bSCS|bMOSI|bSCK);                                            //SCS,MOSI,SCK���������
	P1_MOD_OC |= bMISO;                                                        //MISO ��������
	P1_DIR_PU |= bMISO;
	SPI0_SETUP = 0;                                                            //Masterģʽ,��λ��ǰ
	
	if(mode == 0)
	{
		SPI0_CTRL = (bS0_MOSI_OE|bS0_SCK_OE);                                  //ģʽ0
	}
	else if(mode == 3)
	{
		SPI0_CTRL = (bS0_MOSI_OE|bS0_SCK_OE|bS0_MST_CLK);                      //ģʽ3
	}
}

/*******************************************************************************
* Function Name  : CH557SPIMasterWrite
* Description    : CH557Ӳ��SPIд����,����ģʽ
* Input          : dat����������
* Return         : None
*******************************************************************************/
void CH557SPIMasterWrite(UINT8 dat)
{
	SPI0_DATA = dat;
	
	while(S0_FREE == 0)
	{
			;    //�ȴ��������
	}
}

/*******************************************************************************
* Function Name  : CH557SPIMasterRead
* Description    : CH557Ӳ��SPI0�����ݣ�����ģʽ
* Input          : None
* Return         : ret����������
*******************************************************************************/
UINT8 CH557SPIMasterRead(void)
{
	SPI0_DATA = 0xff;   //����һ��SPI����

	while(S0_FREE == 0)
	{
		;
	}
	return SPI0_DATA;
}

/*******************************************************************************
* Function Name  : SPISlvModeSet
* Description    : SPI�ӻ�ģʽ��ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void SPISlvModeSet(void)
{
	P1_MOD_OC &= ~(bSCS|bMOSI|bSCK);                                          //SCS,MOSI,SCK ��������
	P1_DIR_PU &= ~(bSCS|bMOSI|bSCK);
	P1_MOD_OC &= ~bMISO;                                                      //MISO�������
	P1_DIR_PU |= bMISO;
	SPI0_S_PRE = 0x66;                                                        //Ԥ��ֵ,����ֵ
	SPI0_SETUP = bS0_MODE_SLV;                                                //Slvģʽ,��λ��ǰ
	SPI0_CTRL = bS0_MISO_OE;                                                  //MISO ���ʹ��

#ifdef SPI_INTERRUPT
	SPI0_SETUP |= bS0_IE_FIRST | bS0_IE_BYTE;
	IE_SPI0 = 1;
	EA = 1;
	
#endif
}

/*******************************************************************************
* Function Name  : CH557SPISlvWrite
* Description    : CH557Ӳ��SPIд���ݣ��ӻ�ģʽ
* Input          : dat����������
* Return         : None
*******************************************************************************/
void CH557SPISlvWrite(UINT8 dat)
{
	SPI0_DATA = dat;
	
	while(S0_FREE == 0)
	{
			;
	}
}

/*******************************************************************************
* Function Name  : CH557SPISlvRead
* Description    : CH557Ӳ��SPI0�����ݣ��ӻ�ģʽ
* Input          : None
* Return         : ret����������
*******************************************************************************/
UINT8 CH557SPISlvRead(void)
{
	while(S0_FREE == 0)
	{
			;
	}
	return SPI0_DATA;
}

#ifdef SPI_INTERRUPT
/*******************************************************************************
* Function Name  : SPIInterrupt
* Description    : SPI0�жϷ�����
* Input          : None
* Return         : None
*******************************************************************************/
void SPIInterrupt(void) interrupt INT_NO_SPI0 using 1                      //SPI�жϷ������,ʹ�üĴ�����1
{
	UINT8 dat;
	dat = CH557SPISlvRead();
	CH557SPISlvWrite(dat^0xFF);

#if DE_PRINTF
	printf("Read#%02x\n",(UINT16)dat);
	
#endif
}

#endif