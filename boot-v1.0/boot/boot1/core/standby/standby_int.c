/*
**********************************************************************************************************************
*											        eGon
*						                     the Embedded System
*									       script parser sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : standby.c
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
#include "include.h"
#include "arm_a8.h"
/*
************************************************************************************************************
*
*                                             eGon2_int_enter_standby
*
*    �������ƣ�
*
*    �����б�
*
*
*
*    ����ֵ  ��
*
*    ˵��    ��	����standby
*				1) ���DMA PENDING������DMA enable
*				2) �ر�DMA AHB
*
************************************************************************************************************
*/
int standby_int_disable(void)
{
	asm("mrs r0, cpsr");
	asm("orr r0, r0, #(0x40|0x80)");
	asm("msr cpsr_c, r0");

    return 0;
}
/*
************************************************************************************************************
*
*                                             eGon2_int_exit_standby
*
*    �������ƣ�
*
*    �����б�
*
*
*
*    ����ֵ  ��
*
*    ˵��    ��	�˳�standby
*				1) ��DMA AHB
*				2) ���DMA PENDING���ָ�DMA enable
*
*
************************************************************************************************************
*/
int standby_int_enable(void)
{
	asm("mrs r0, cpsr");
	asm("bic r0, r0, #(0x40|0x80)");
	asm("msr cpsr_c, r0");

    return 0;
}
/*
*********************************************************************************************************
*										   EnableInt
*
* Description:  ʹ���ж�
*
* Arguments	 : irq_no     �жϺ�
*
* Returns	 :
*
*********************************************************************************************************
*/
static int standby_enableint(__u32 irq_no)
{
	__u32 reg_val;
	__u32 offset;

	if (irq_no >= GIC_IRQ_NUM)
	{
		return -1;
	}

	offset   = irq_no >> 5; // ��32
	reg_val  = GICD_ISENABLER(offset);
	reg_val |= 1 << (irq_no & 0x1f);
	GICD_ISENABLER(offset) = reg_val;

    return 0;
}
/*
*********************************************************************************************************
*										   DisableInt
*
* Description:  ��ֹ�ж�
*
* Arguments	 : irq_no     �жϺ�
*
* Returns	 :
*
*********************************************************************************************************
*/
__s32 standby_disableint(__u32 irq_no)
{
	__u32 reg_val;
	__u32 offset;

	if (irq_no >= GIC_IRQ_NUM)
	{
		return -1;
	}

	offset   = irq_no >> 5; // ��32
	reg_val  = GICD_ISENABLER(offset);
	reg_val &= ~(1 << (irq_no & 0x1f));
	GICD_ISENABLER(offset) = reg_val;

    return 0;
}
/*
************************************************************************************************************
*
*                                             function
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
void standby_gic_store(void)
{
//	int max_no, i;
//
//	max_no = GIC_IRQ_NUM/32;
//	if(GIC_IRQ_NUM%32)
//	{
//		max_no ++;
//	}
//
//	memset(int_enable_number, 0, sizeof(int) * max_no);
//	for(i=0;i<max_no;i++)
//	{
//		int_enable_number[i] = GICD_ISENABLER(i);
//		GICD_ISENABLER(i) = 0;
//		GICD_ICPENDR(i) = 0xffffffff;
//	}
	standby_enableint(AW_IRQ_NMI);
}
/*
************************************************************************************************************
*
*                                             function
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
void standby_gic_restore(void)
{
//	int max_no, i;
//
//	max_no = GIC_IRQ_NUM/32;
//	if(GIC_IRQ_NUM%32)
//	{
//		max_no ++;
//	}
//
//	for(i=0;i<max_no;i++)
//	{
//		GICD_ISENABLER(i) = int_enable_number[i];
//	}
//	memset(int_enable_number, 0, sizeof(int) * max_no);
	standby_disableint(AW_IRQ_NMI);
}
/*
************************************************************************************************************
*
*                                             function
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
void standby_gic_clear_pengding(void)
{
	__u32 reg_val;
	__u32 offset;
	__u32 idnum;

	idnum = GICC_IAR;

	GICC_EOIR = idnum;
	GICC_DIR  = idnum;

	offset = idnum >> 5; // ��32
	reg_val = GICD_ICPENDR(offset);
	reg_val |= (1 << (idnum & 0x1f));
	GICD_ICPENDR(offset) = reg_val;

	return ;
}
