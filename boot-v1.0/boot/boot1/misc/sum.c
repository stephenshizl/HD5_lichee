/*
**********************************************************************************************************************
*											        eGon
*						           the Embedded GO-ON Bootloader System
*									       eGON arm boot sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    :
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include <egon2.h>

typedef struct tag_CRC32_DATA
{
	unsigned int CRC;				//int�Ĵ�С��32λ
	unsigned int CRC_32_Tbl[256];	//�����������
}CRC32_DATA_t;

//#pragma arm section

#define  ADD_SUM_MAGIC	0x62332672

__s32 verify_sum( void *mem_base, __u32 size )
{
	__u32 *buf;
	__u32 count;
	__u32 sum;

	count = size >> 2;                         // �� �֣�4bytes��Ϊ��λ����
	sum = 0;
	buf = (__u32 *)mem_base;
	while(count--)
	{
		sum += *buf++;                         // �����ۼӣ����У���
	};

	switch(size & 0x03)
	{
		case 0:
			return sum;
		case 1:
			sum += (*buf & 0x000000ff);
			break;
		case 2:
			sum += (*buf & 0x0000ffff);
			break;
		case 3:
			sum += (*buf & 0x00ffffff);
			break;
	}

	return sum;
}
/*
************************************************************************************************************
*
*                                             _calc_crc32
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
__u32 calc1_crc32(void * buffer, __u32 length)
{
	__u32 i, j;
	CRC32_DATA_t crc32;		//
	__u32 CRC32 = 0xffffffff; //���ó�ʼֵ

	crc32.CRC = 0;

	for( i = 0; i < 256; ++i)//��++i�����Ч��
	{
		crc32.CRC = i;
		for( j = 0; j < 8 ; ++j)
		{
			//���ѭ��ʵ���Ͼ�����"���㷨"����ȡCRC��У����
			if(crc32.CRC & 1)
				crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
			else //0xEDB88320����CRC-32������ʽ��ֵ
				crc32.CRC >>= 1;
		}
		crc32.CRC_32_Tbl[i] = crc32.CRC;
	}

	CRC32 = 0xffffffff; //���ó�ʼֵ
    for( i = 0; i < length; ++i)
    {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32>>8);
    }

    //return CRC32;
	return CRC32^0xffffffff;
}




