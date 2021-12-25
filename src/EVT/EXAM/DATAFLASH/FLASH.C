/********************************** (C) COPYRIGHT *******************************
* File Name          : FLASH.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/05
* Description        : CH557 Flash�ֽڶ�д��������
*******************************************************************************/
#include "Flash.H"

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : FlashErasePage
* Description    : ����ҳ������ÿ64�ֽ�Ϊһҳ����ҳ���������ݱ�Ϊ0x00
* Input          : Addr:������ַ����ҳ
* Return         : status�����ز���״̬
*                   0x00-�ɹ�  
*                   0x01-��ַ��Ч  
*                   0x02-δ֪�����ʱ
*******************************************************************************/
UINT8 FlashErasePage(UINT16 Addr)
{
	bit e_all;
	UINT8 status;                                    /* ���ز���״̬ */
	UINT8 FlashType;                                 /* Flash ���ͱ�־ */
	
	e_all = EA;
	EA = 0;                                          /* �ر�ȫ���ж�,��ֹFlash��������� */
	Addr &= 0xFFC0;                                  /* 64�ֽڶ��� */
	if((Addr>=DATA_FLASH_ADDR) && (Addr<BOOT_LOAD_ADDR))/* DataFlash���� */
	{
		FlashType = bDATA_WE;
	}
	else                                             /* CodeFlash���� */
	{
		FlashType = bCODE_WE;
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= FlashType;
	ROM_ADDR = Addr;                                 /* д��Ŀ���ַ */
	ROM_BUF_MOD = bROM_BUF_BYTE;                     /* ѡ������ģʽ���ֽڱ��ģʽ */
	ROM_DAT_BUF = 0;                                 /* ��д���ݻ������Ĵ���Ϊ0 */
	if (ROM_STATUS & bROM_ADDR_OK)                 /* ������ַ��Ч */
	{
		ROM_CTRL = ROM_CMD_ERASE;                    /* �������� */
		if(ROM_STATUS & bROM_CMD_ERR)
		{
			status = 0x02;                           /* δ֪�����ʱ */
		}
		else
		{
			status = 0x00;                           /* �����ɹ� */
		}
	}
	else
	{
		status = 0x01;                               /* ��ַ��Ч */
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~FlashType;                        /* ����д���� */
	EA = e_all;                                      /* �ָ�ȫ���ж�״̬ */
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashProgByte
* Description    : Flash �ֽڱ��
* Input          : Addr��д���ַ
*                  Data���ֽڱ�̵�����
* Return         : status�����״̬���� 
*                   0x00-�ɹ�  
*                   0x01-��ַ��Ч  
*                   0x02-δ֪�����ʱ
*******************************************************************************/
UINT8 FlashProgByte(UINT16 Addr,UINT8 Data)
{
	bit e_all;
	UINT8 status;                                    /* ���ز���״̬ */
	UINT8 FlashType;                                 /* Flash ���ͱ�־ */
	
	e_all = EA;
	EA = 0;                                          /* �ر�ȫ���ж�,��ֹFlash��������� */
	if((Addr>=DATA_FLASH_ADDR) && (Addr<BOOT_LOAD_ADDR))/* DataFlash���� */
	{
		FlashType = bDATA_WE;
	}
	else                                             /* CodeFlash���� */
	{
		FlashType = bCODE_WE;
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= FlashType;
	ROM_ADDR = Addr;                                 /* д��Ŀ���ַ */
	ROM_BUF_MOD = bROM_BUF_BYTE;                     /* ѡ������ģʽ���ֽڱ��ģʽ */
	ROM_DAT_BUF = Data;                              /* ���ݻ������Ĵ��� */
	if (ROM_STATUS & bROM_ADDR_OK)                 /* ������ַ��Ч */
	{
		ROM_CTRL = ROM_CMD_PROG ;                    /* ������� */
		if(ROM_STATUS & bROM_CMD_ERR)
		{
			status = 0x02;                           /* δ֪�����ʱ */
		}
		else
		{
			status = 0x00;                           /* �����ɹ� */
		}
	}
	else
	{
		status = 0x01;                               /* ��ַ��Ч */
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~FlashType;                        /* ����д���� */
	EA = e_all;                                      /* �ָ�ȫ���ж�״̬ */
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashProgPage
* Description    : ҳ���,����̵�ǰAddr����ҳ��
* Input          : Addr��д���ַ
*                  Buf��Buf��ַ�ĵ�6λҪ��Addr��ַ��6λ��ȣ�Ҳ���ǣ�Buf��ַ%64����ҳ��ƫ�Ƶ�ַҪ��ͬ
*                  len: д�볤�ȣ����64
* Return         : status�����״̬���� 
*                   0x00-�ɹ�  
*                   0x01-��ַ��Ч  
*                   0x02-δ֪�����ʱ
*                   0xFC-��ʼ��ַ���ϱ���д���ֽ���������ǰҳ
*                   0xFB-buf��ַ��6λ����ʼ��ַ��6λ�����
*******************************************************************************/
UINT8 FlashProgPage(UINT16 Addr, PUINT8X Buf,UINT8 len)
{
	bit e_all;
	UINT8 status;                                    /* ���ز���״̬ */
	UINT8 FlashType;                                 /* Flash ���ͱ�־ */
	UINT8 page_offset;                               /* Addr�ڵ�ǰҳ��ƫ�Ƶ�ַ */
	
	e_all = EA;
	EA = 0;                                          /* �ر�ȫ���ж�,��ֹFlash��������� */
	if((Addr>=DATA_FLASH_ADDR) && (Addr<BOOT_LOAD_ADDR))/* DataFlash���� */
	{
		FlashType = bDATA_WE;
	}
	else                                             /* CodeFlash���� */
	{
		FlashType = bCODE_WE;
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= FlashType;
	page_offset = Addr & MASK_ROM_ADDR;
	if (len > (ROM_PAGE_SIZE - page_offset))
	{
		return 0xFC;                              /* ��ʼ��ַ���ϱ���д���ֽ������ܳ�����ǰҳ, ÿ64�ֽ�Ϊһҳ, ���β������ó�����ǰҳ */
	}
	if (((UINT8)Buf & MASK_ROM_ADDR) != page_offset)
	{
		return 0xFB;                              /* xdata��������ַ��6λ��������ʼ��ַ��6λ��� */
	}
	ROM_ADDR = Addr;
	ROM_BUF_MOD = page_offset + len - 1;             /* ҳ��̽�����ַ��6λ�����ĵ�ַ */
	DPL = (UINT8)Buf;
	DPH = (UINT8)( (UINT16)Buf >> 8 );
	if (ROM_STATUS & bROM_ADDR_OK)                 /* ������ַ��Ч */
	{
		ROM_CTRL = ROM_CMD_PROG ;                    /* ������� */
		if(ROM_STATUS & bROM_CMD_ERR)
		{
				status = 0x02;                           /* δ֪�����ʱ */
		}
		else
		{
				status = 0x00;                           /* �����ɹ� */
		}
	}
	else
	{
		status = 0x01;                               /* ��ַ��Ч */
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~FlashType;                        /* ����д���� */
	EA = e_all;                                      /* �ָ�ȫ���ж�״̬ */
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashReadBuf
* Description    : ��Flash������data��code��
* Input          : Addr��������ַ
*                  buf����������
*                  len����������
* Return         : i:����ʵ�ʶ�������
*******************************************************************************/
UINT8 FlashReadBuf(UINT16 Addr,PUINT8 buf,UINT16 len)
{
	UINT16 i;
    
	for(i=0; i!=len; i++)
	{
		buf[i] = *(PUINT8C)Addr;
		if(Addr==0xFFFF)
		{
			i++;
			break;
		}
		Addr++;
	}
	return i;
}

/*******************************************************************************
* Function Name  : FlashProgOTPbyte
* Description    : ���ֽ�дOTP����ֻ��0���1,�Ҳ��ɲ���
* Input          : Addr��0x20~0x3F
*                  Data:
* Return         : status:����״̬ 
*                   0x00-�ɹ�  
*                   0x02-δ֪�����ʱ
*******************************************************************************/
UINT8 FlashProgOTPbyte(UINT8 Addr, UINT8 Data)
{
	bit e_all;
	UINT8 status;                                    /* ���ز���״̬ */
	
	e_all = EA;
	EA = 0;                                          /* �ر�ȫ���ж�,��ֹFlash��������� */
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
	ROM_ADDR = Addr;
	ROM_BUF_MOD = bROM_BUF_BYTE;
	ROM_DAT_BUF = Data;
	ROM_CTRL = ROM_CMD_PG_OTP;
	if(ROM_STATUS & bROM_CMD_ERR)
	{
		status = 0x02;                               /* δ֪�����ʱ */
	}
	else
	{
		status = 0x00;                               /* �����ɹ� */
	}
	SAFE_MOD = 0x55;                                 /* ���밲ȫģʽ */
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;                         /* ����д���� */
	EA = e_all;                                      /* �ָ�ȫ���ж�״̬ */
	return status;
}

/*******************************************************************************
* Function Name  : FlashReadOTPword
* Description    : 4�ֽ�Ϊ��λ��ȡReadOnly������OTP��
* Input          : Addr��0x00~0x3F
* Return         : temp����ȡ�����ֽ�����
*******************************************************************************/
UINT32 FlashReadOTPword(UINT8 Addr)
{
	UINT32 temp;
	
	ROM_ADDR = Addr;
	ROM_CTRL = ROM_CMD_RD_OTP;
	temp = ROM_DATA_HI;
	temp <<= 16;
	temp |= ROM_DATA_LO;
	return temp;
}