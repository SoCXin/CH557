/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/06
* Description        : CH557 SPI
*******************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__

#include "CH557.H"
#include "Debug.H"


//#define  SPI_INTERRUPT                                                      //�ж�ʹ�ܣ���Slaveģʽ��
#define  SPI_CK_SET(n) (SPI0_CK_SE = n)                                     //SPIʱ�����ú��� 


//Master��غ���
extern void  SPIMasterModeSet(UINT8 mode);                                    //SPI����ģʽ��ʼ��
extern void  CH557SPIMasterWrite(UINT8 dat);                                  //CH557Ӳ��SPIд���ݣ�����ģʽ
extern UINT8 CH557SPIMasterRead(void);                                            //CH557Ӳ��SPI0�����ݣ�����ģʽ

//Slave��غ���
extern void  SPISlvModeSet(void);                                                //SPI�ӻ�ģʽ��ʼ��
extern void  CH557SPISlvWrite(UINT8 dat);                                     //CH557Ӳ��SPIд���ݣ��ӻ�ģʽ
extern UINT8 CH557SPISlvRead(void);                                               //CH557Ӳ��SPI0�����ݣ��ӻ�ģʽ

#endif