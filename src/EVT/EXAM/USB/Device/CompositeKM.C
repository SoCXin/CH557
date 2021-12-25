/********************************** (C) COPYRIGHT *******************************
* File Name          : CompositeKM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557ģ��USB���󸴺��豸,֧��������,֧�ֻ���
                       ��ʾ�������򵥲�����������ֵ���ο� HID USAGE TABLEЭ���ĵ�
                       ����0�����ַ���
                       ��L����������
                       ��R��: ����Ҽ�
                       ��A��: ���̡�A����
                       ��Q��: ���̡�Caps����
                    �����ַ�������˯��״̬��,�豸Զ�̻���������ע���豸һ�����Թ���,��Ϊ�������߿���USB��Ҳ����磩
*******************************************************************************/
#include "DEBUG.H"

//#define Fullspeed
#ifdef  Fullspeed
#define THIS_ENDP0_SIZE         64
#else
#define THIS_ENDP0_SIZE         8                                              //����USB���жϴ��䡢���ƴ�����������Ϊ8
#endif

#define ENDP1_IN_SIZE           8                                              //���̶˵����ݰ���С
#define ENDP2_IN_SIZE           4                                              //���˵����ݰ���С
UINT8X  Ep0Buffer[MIN(64,THIS_ENDP0_SIZE+2)] _at_ 0x0000;                    //�˵�0 OUT&IN��������������ż��ַ
UINT8X  Ep1Buffer[MIN(64,ENDP1_IN_SIZE)]     _at_ MIN(64,THIS_ENDP0_SIZE+2);                 //�˵�1 IN������,������ż��ַ
UINT8X  Ep2Buffer[MIN(64,ENDP2_IN_SIZE)]     _at_ (MIN(64,THIS_ENDP0_SIZE+2)+MIN(64,ENDP1_IN_SIZE+2));     //�˵�2 IN������,������ż��ַ

UINT8   SetupReq,Ready,UsbConfig;

#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)

#pragma NOAREGS
/*�豸������*/
UINT8C DevDesc[] = { 0x12,0x01,0x10,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                     0x86,0x1a,0xe1,0xe6,0x00,0x01,0x01,0x02,
                     0x00,0x01
                   };
/*�ַ���������*/
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };                                    // ����������
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 }; // ������Ϣ
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '4', 0, '5', 0 };         // ��Ʒ��Ϣ
/*HID�౨��������*/
UINT8C KeyRepDesc[] =
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0
};
UINT8C MouseRepDesc[] =
{
    0x05,0x01,0x09,0x02,0xA1,0x01,0x09,0x01,
    0xA1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
    0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x03,
    0x81,0x02,0x75,0x05,0x95,0x01,0x81,0x01,
    0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
    0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
    0x81,0x06,0xC0,0xC0
};
/*����������*/
UINT8C CfgDesc[] =
{
    0x09,0x02,0x3b,0x00,0x02,0x01,0x00,0xA0,0x32,                    //����������

    0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,                    //�ӿ�������,����
    0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc)&0xFF,sizeof(KeyRepDesc)>>8,//HID��������
    0x07,0x05,0x81,0x03,ENDP1_IN_SIZE,0x00,0x0a,                       //�˵�������

    0x09,0x04,0x01,0x00,0x01,0x03,0x01,0x02,0x00,                    //�ӿ�������,���
    0x09,0x21,0x10,0x01,0x00,0x01,0x22,sizeof(MouseRepDesc)&0xFF,sizeof(MouseRepDesc)>>8,//HID��������
    0x07,0x05,0x82,0x03,ENDP2_IN_SIZE,0x00,0x0a                        //�˵�������
};
/*��������*/
UINT8 HIDKey[ENDP1_IN_SIZE];
/*�������*/
UINT8 HIDMouse[ENDP2_IN_SIZE];
UINT8 Endp1Busy = 0;                                                 //������ɿ��Ʊ�־λ
UINT8 Endp2Busy = 0;
UINT8 WakeUpEnFlag = 0;                                              //Զ�̻���ʹ�ܱ�־

/*******************************************************************************
* Function Name  : CH557USBDevWakeup
* Description    : CH557�豸ģʽ��������������K�ź�
* Input          : None
* Return         : None
*******************************************************************************/
void CH554USBDevWakeup(void)
{
#ifdef Fullspeed
    UDEV_CTRL |= bUD_LOW_SPEED;
    mDelaymS(2);
    UDEV_CTRL &= ~bUD_LOW_SPEED;
#else
    UDEV_CTRL &= ~bUD_LOW_SPEED;
    mDelaymS(2);
    UDEV_CTRL |= bUD_LOW_SPEED;
#endif
}

/*******************************************************************************
* Function Name  : Enp1IntIn
* Description    : USB�豸ģʽ�˵�1���ж��ϴ�
* Input          : *buf: �ϴ�����
*                  len:�ϴ����ݳ���
* Return         : None
*******************************************************************************/
void Enp1IntIn( UINT8 *buf,UINT8 len )
{
    memcpy( Ep1Buffer, buf, len );                                            //�����ϴ�����
    UEP1_T_LEN = len;                                                         //�ϴ����ݳ���
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                 //������ʱ�ϴ����ݲ�Ӧ��ACK
}
/*******************************************************************************
* Function Name  : Enp2IntIn
* Description    : USB�豸ģʽ�˵�2���ж��ϴ�
* Input          : *buf: �ϴ�����
*                  len:�ϴ����ݳ���
* Return         : None
*******************************************************************************/
void Enp2IntIn( UINT8 *buf,UINT8 len )
{
    memcpy( Ep2Buffer, buf, len);                                              //�����ϴ�����
    UEP2_T_LEN = len;                                                          //�ϴ����ݳ���
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //������ʱ�ϴ����ݲ�Ӧ��ACK
}
/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB�жϷ������
* Input          : None
* Return         : None
*******************************************************************************/
void	USB_DeviceInterrupt(void) interrupt INT_NO_USB using 1	
{
	UINT8	len;
	static	UINT8	SetupReqCode;
  static  UINT16 SetupLen;
	static	PUINT8	pDescr;
	if(UIF_TRANSFER){  // USB�������
  USB_DevIntNext:
		switch(USB_INT_ST & ( bUIS_SETUP_ACT | MASK_UIS_TOKEN | MASK_UIS_ENDP )){  // �����������ƺͶ˵��
			case UIS_TOKEN_IN | 2:  // endpoint 2# �����˵��ϴ�
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 2:
					UEP2_T_LEN = 0;                                                     //Ԥʹ�÷��ͳ���һ��Ҫ���
					UEP2_CTRL ^= bUEP_T_TOG;                                            //�ֶ���ת
					Endp2Busy = 0;
					UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //Ĭ��Ӧ��NAK
				break;
			case UIS_TOKEN_IN | 1:  // endpoint 1# �ж϶˵��ϴ�
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 1:
					UEP1_T_LEN = 0;                                                     //Ԥʹ�÷��ͳ���һ��Ҫ���
					UEP1_CTRL ^= bUEP_T_TOG;                                            //�ֶ���ת
					Endp1Busy = 0;
					UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //Ĭ��Ӧ��NAK
				break;
			case UIS_TOKEN_IN | 0:  // endpoint 0# IN
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 0:
				switch(SetupReqCode){
					case USB_GET_DESCRIPTOR:
						len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // ���δ��䳤��
						memcpy( Ep0Buffer, pDescr, len );  /* �����ϴ����� */
						SetupLen -= len;
						pDescr += len;
						UEP0_T_LEN = len;
						UEP0_CTRL ^= bUEP_T_TOG;  // ��ת
						break;
					case USB_SET_ADDRESS:
						USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
					default:
						UEP0_T_LEN = 0;  // ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
				}
				break;
			case UIS_TOKEN_OUT | 0:  // endpoint 0# OUT
			case bUIS_SETUP_ACT | UIS_TOKEN_OUT | 0:
				len = USB_RX_LEN;
				if(SetupReqCode == 0x09)
				{
					if(Ep0Buffer[0])
					{
						printf("Light on Num Lock LED!\n");
					}
					else if(Ep0Buffer[0] == 0)
					{
						printf("Light off Num Lock LED!\n");
					}
				}
				UEP0_CTRL ^= bUEP_R_TOG;                                      //ͬ����־λ��ת
				break;
			default:
			if ((USB_INT_ST & (bUIS_SETUP_ACT | MASK_UIS_TOKEN)) == (bUIS_SETUP_ACT | UIS_TOKEN_FREE)){  // endpoint 0# SETUP
				UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; 
				SetupLen = ((UINT16)UsbSetupBuf->wLengthH<<8) + UsbSetupBuf->wLengthL;
				len = 0;
				SetupReqCode = UsbSetupBuf->bRequest;
				if( UsbSetupBuf->wLengthH || SetupLen > 0x7F) SetupLen = 0x7F;  // �����ܳ���
				len = 0;  // Ĭ��Ϊ�ɹ������ϴ�0����
				if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD){  
					switch(SetupReqCode){
						case 0x01://GetReport
							break;
						case 0x02://GetIdle
							break;
						case 0x03://GetProtocol
							break;
						case 0x09://SetReport
							break;
						case 0x0A://SetIdle
							break;
						case 0x0B://SetProtocol
							break;
						default:
							len = 0xFFFF;                                  /*���֧��*/
							break;
					}
				}
				else {  // ��׼����
					switch(SetupReqCode){  // ������
						case USB_GET_DESCRIPTOR:
							switch(UsbSetupBuf->wValueH ){
								case 1:  // �豸������
									pDescr = (PUINT8)( &DevDesc[0] );
									len = sizeof( DevDesc );
									break;
								case 2:  // ����������
									pDescr = (PUINT8)( &CfgDesc[0] );
									len = sizeof( CfgDesc );
									break;
								case 3:  // �ַ���������
									switch(UsbSetupBuf->wValueL){
										case 1:
											pDescr = (PUINT8)( &MyManuInfo[0] );
											len = sizeof( MyManuInfo );
											break;
										case 2:
											pDescr = (PUINT8)( &MyProdInfo[0] );
											len = sizeof( MyProdInfo );
											break;
										case 0:
											pDescr = (PUINT8)( &MyLangDescr[0] );
											len = sizeof( MyLangDescr );
											break;
										default:
											len = 0xFF;  // ��֧�ֵ��ַ���������
											break;
									}
									break;
								case USB_DESCR_TYP_REPORT:
									if(UsbSetupBuf->wIndexL == 0)                   //�ӿ�0����������
									{
										pDescr = KeyRepDesc;                        //����׼���ϴ�
										len = sizeof(KeyRepDesc);
									}
									else if(UsbSetupBuf->wIndexL == 1)              //�ӿ�1����������
									{
										pDescr = MouseRepDesc;                      //����׼���ϴ�
										len = sizeof(MouseRepDesc);
									}
									else
									{
										len = 0xFFFF;                                 //������ֻ��2���ӿڣ���仰����������ִ��
									}
									break;
								default:
									len = 0xFF;  // ��֧�ֵ�����������
									break;
							}
							if(SetupLen > len) SetupLen = len;  // �����ܳ���
							len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // ���δ��䳤��
							memcpy(Ep0Buffer, pDescr, len);  /* �����ϴ����� */
							SetupLen -= len;
							pDescr += len;
							break;                              																
						case USB_SET_ADDRESS:
							SetupLen = UsbSetupBuf->wValueL;  // �ݴ�USB�豸��ַ
							break;
						case USB_GET_CONFIGURATION:
							Ep0Buffer[0] = UsbConfig;
							if ( SetupLen >= 1 ) len = 1;
							break;
						case USB_SET_CONFIGURATION:
							UsbConfig = UsbSetupBuf->wValueL;
							if(UsbConfig)
							{
								printf("SET CONFIG.\n");
								Ready = 1;                                                   //set config����һ�����usbö����ɵı�־
							}
							break;
						case USB_CLEAR_FEATURE:
							if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP){  // �˵�
								switch(UsbSetupBuf->wIndexL){
									case 0x82:
										UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										break;
									case 0x02:
										UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
										break;
									case 0x81:
										UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										break;
									case 0x01:
										UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
										break;
									default:
										len = 0xFF;  // ��֧�ֵĶ˵�
										break;
								}
							}
							else if((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE)// �豸
							{
								WakeUpEnFlag = 0;
								printf("Wake\n");
								break;
							}
							else
							{
								len = 0xFFFF;                                                // ���Ƕ˵㲻֧��
							}
							break;
						case USB_SET_FEATURE:                                               /* Set Feature */
							if((UsbSetupBuf->bRequestType & 0x1F) == 0x00)                    /* �����豸 */
							{
								if(((( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL) == 0x01)
								{
									if(CfgDesc[ 7 ] & 0x20)
									{
										WakeUpEnFlag = 1;                                   /* ���û���ʹ�ܱ�־ */
										printf("Enable Remote Wakeup.\n");
									}
									else
									{
										len = 0xFFFF;                                        /* ����ʧ�� */
									}
								}
								else
								{
									len = 0xFFFF;                                            /* ����ʧ�� */
								}
							}
							else if((UsbSetupBuf->bRequestType & 0x1F) == 0x02)        /* ���ö˵� */
							{
								if((((UINT16)UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL) == 0x00)
								{
									switch(((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL){
										case 0x82:
											UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* ���ö˵�2 IN STALL */
											break;
										case 0x02:
											UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* ���ö˵�2 OUT Stall */
											break;
										case 0x81:
											UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* ���ö˵�1 IN STALL */
											break;
										default:
											len = 0xFFFF;                               //����ʧ��
											break;
									}
								}
								else
								{
									len = 0xFFFF;                                   //����ʧ��
								}
							}
							else
							{
								len = 0xFFFF;                                      //����ʧ��
							}
							break;
						case USB_GET_INTERFACE:
							Ep0Buffer[0] = 0x00;
							if(SetupLen >= 1) len = 1;
							break;
						case USB_GET_STATUS:
							Ep0Buffer[0] = 0x00;
							Ep0Buffer[1] = 0x00;
							if ( SetupLen >= 2 ) len = 2;
							else len = SetupLen;
							break;
						default:
							len = 0xFF;  // ����ʧ��
							printf("ErrEp0ReqCode=%02X\n",(UINT16)SetupReqCode);
							break;
					}
				}
				if(len == 0xFF){  // ����ʧ��
					SetupReqCode = 0xFF;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;  // STALL
				}
				else if(len <= THIS_ENDP0_SIZE){  // �ϴ����ݻ���״̬�׶η���0���Ȱ�
					UEP0_T_LEN = len;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // Ĭ�����ݰ���DATA1
				}
				else{  // �´����ݻ�����
					UEP0_T_LEN = 0;  // ��Ȼ��δ��״̬�׶Σ�������ǰԤ���ϴ�0�������ݰ��Է�������ǰ����״̬�׶�
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // Ĭ�����ݰ���DATA1
				}
				break;
			}
			else{
				printf("ErrEndpInt\n");
				break;
			}
		}

		UIF_TRANSFER = 0;  // ���жϱ�־
	}
	else if(UIF_BUS_RST){  // USB���߸�λ
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = UEP_T_RES_NAK;
        UEP2_CTRL = UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Ready = 0;
        UIF_BUS_RST = 0;                                                 //���жϱ�־
	}
	else if(UIF_SUSPEND){  // USB���߹���/�������
		UIF_SUSPEND = 0;
		if (USB_MIS_ST & bUMS_SUSPEND) {  // ����

		}
		else{  // ����
			
		}
	}
	else{  // ������ж�,�����ܷ��������
		printf("UnknownInt\n");
		USB_INT_FG = 0xFF;  // ���жϱ�־
	}
	if(UIF_TRANSFER) goto USB_DevIntNext;
	
}

/*******************************************************************************
* Function Name  : InitUSB_Device
* Description    : USB�豸��ʼ��
* Input          : None
* Return         : None
*******************************************************************************/
void	InitUSB_Device(void)  // ��ʼ��USB�豸
{
	IE_USB = 0;
	USB_CTRL = 0x00;  // ���趨ģʽ
	UEP1_T_LEN = 0;                                                            //Ԥʹ�÷��ͳ���һ��Ҫ���
	UEP2_T_LEN = 0;                                                            //Ԥʹ�÷��ͳ���һ��Ҫ���
	UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //�˵�0��64�ֽ��շ�������
	UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN;                    // �˵�1�ϴ�IN
	UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;                    // �˵�2�ϴ�IN
	UEP0_DMA = Ep0Buffer;
	UEP1_DMA = Ep1Buffer;
	UEP2_DMA = Ep2Buffer;
	USB_DEV_AD = 0x00;
	UDEV_CTRL &= ~ bUD_PD_EN;  // ��ֹDP/DM��������
	
#ifndef Fullspeed
	UDEV_CTRL |= bUD_LOW_SPEED;                                                //ѡ�����1.5Mģʽ
	USB_CTRL |= bUC_LOW_SPEED;
#else
	UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //ѡ��ȫ��12Mģʽ��Ĭ�Ϸ�ʽ
	USB_CTRL &= ~bUC_LOW_SPEED;
#endif

	USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY;  // ����USB�豸��DMA�����ж��ڼ��жϱ�־δ���ǰ�Զ�����NAK
	UDEV_CTRL |= bUD_PORT_EN;  // ����USB�˿�
	USB_INT_FG = 0xFF;  // ���жϱ�־
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
	IE_USB = 1;
}

/*******************************************************************************
* Function Name  : HIDValueHandle
* Description    : HID��ֵ����
* Input          : None
* Return         : None
*******************************************************************************/
void HIDValueHandle()
{
	UINT8 i;
	i = _getkey( );
	printf( "%c", (UINT8)i );
	if( WakeUpEnFlag )                                                   //����������
	{
		CH554USBDevWakeup();                                               //��������
	}
	else
	{
		switch(i)
		{
			//��������ϴ�ʾ��
			case 'L':                                                        //���
				HIDMouse[0] = 0x01;
				while( Endp2Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp2Busy = 1;                                                 //����Ϊæ״̬
				Enp2IntIn(HIDMouse,sizeof(HIDMouse));
				HIDMouse[0] = 0;                                               //̧��
				while( Endp2Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp2Busy = 1;                                                 //����Ϊæ״̬
				Enp2IntIn(HIDMouse,sizeof(HIDMouse));
				break;
			case 'R':                                                        //�Ҽ�
				HIDMouse[0] = 0x02;
				while( Endp2Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp2Busy = 1;                                               //����Ϊæ״̬
				Enp2IntIn(HIDMouse,sizeof(HIDMouse));
				HIDMouse[0] = 0;                                             //̧��
				while( Endp2Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp2Busy = 1;                                               //����Ϊæ״̬
				Enp2IntIn(HIDMouse,sizeof(HIDMouse));
				break;
				
			//���������ϴ�ʾ��
			case 'A':                                                         //A��
				HIDKey[2] = 0x04;                                             //������ʼ
				while( Endp1Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp1Busy = 1;                                               //����Ϊæ״̬
				Enp1IntIn(HIDKey,sizeof(HIDKey));
				HIDKey[2] = 0;                                                //��������
				while( Endp1Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp1Busy = 1;                                               //����Ϊæ״̬
				Enp1IntIn(HIDKey,sizeof(HIDKey));
				break;
			case 'Q':                                                         //CAP��
				HIDKey[2] = 0x39;
				while( Endp1Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp1Busy = 1;                                               //����Ϊæ״̬
				Enp1IntIn(HIDKey,sizeof(HIDKey));
				HIDKey[2] = 0;                                                //��������
				while( Endp1Busy )
				{
						;    //���æ����һ������û�д���ȥ������ȴ���
				}
				Endp1Busy = 1;                                               //����Ϊæ״̬
				Enp1IntIn(HIDKey,sizeof(HIDKey));
				break;
			default:                                                          //����
				break;
		}
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	CfgFsys( );                                                           //CH557ʱ��ѡ������
	mDelaymS(20);                                                         //�޸���Ƶ�ȴ��ڲ������ȶ�,�ؼ�
	mInitSTDIO( );                                                        //����0��ʼ��
	printf("KM Device start ...\n");
	InitUSB_Device();                                                     //USB�豸ģʽ��ʼ��
	EA = 1;                                                               //������Ƭ���ж�
	memset(HIDKey,0,sizeof(HIDKey));                                     //��ջ�����
	memset(HIDMouse,0,sizeof(HIDMouse));
	
	while(1)
	{
		if(Ready)
		{
			HIDValueHandle();                                                 //����0,�����ͣ��getkey�����ȴ�����һ���ַ�
		}
	}
}