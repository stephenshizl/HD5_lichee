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
#include "include.h"
#include "common_res.h"

#define   BAT_VOL_ACTIVE     (10)
//#define   FORCE_BOOT_STANDBY
#undef    FORCE_BOOT_STANDBY

volatile __u32  boot_standby_action = 0;
__u32  pic_layer_para;

static int show_battery_full(int *status);
static int shut_battery_full(void);
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
#if 0
__s32 check_file_to_fel(char *name)
{
    H_FILE  hfile = NULL;

    hfile = wBoot_fopen(name, "r");
    if(hfile)
    {
        wBoot_fclose(hfile);
        wBoot_rmfile(name);
        hfile = NULL;
    	hfile = wBoot_fopen(name, "r");
    	if(!hfile)
    	{
    		__inf("dest file is not exist\n");
    	}
    	else
    	{
    		wBoot_fclose(hfile);
    	}

    	return 0;
    }

    return -1;
}
#endif
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
*    ˵��    �������������ɣ�ֱ�ӹػ�
*
*
************************************************************************************************************
*/
#if 0
__s32 check_natch_time(char *file_name, __u32 work_mode)
{
    H_FILE  hfile = NULL;
    __s32   ret = -1;

    hfile = wBoot_fopen(file_name, "r");
    if(hfile)
    {
        if(work_mode == WORK_MODE_PRODUCT)
        {
            ret = 0;
        }
        wBoot_fclose(hfile);
        wBoot_rmfile(file_name);
    }

    return ret;
}
#endif
/*
************************************************************************************************************
*
*                                             function
*
*    �������ƣ�
*
*    �����б�
*
*    ����ֵ  �� 0:��������
*			   -1:�ػ�
*
*    ˵��    �������������ɣ�ֱ�ӹػ�
*
*
************************************************************************************************************
*/
#if 0
__s32 check_power_status(void)
{
	if(power_set_limit())
	{
		usb_detect_enter();
	}

	return 0;
}
#else
__s32 check_power_status(void)
{
	__s32 status;
	__s32 power_start;

	//this is used to clear key pending
	wBoot_power_get_key();
	status = wBoot_power_get_level();
	if(status == BATTERY_RATIO_TOO_LOW_WITHOUT_DCIN)
	{
		__inf("battery low power with no dc or ac, should set power off\n");
		ShowPictureEx("c:\\os_show\\low_pwr.bmp", 0);
		wBoot_timer_delay(3000);

		return -1;
	}
	power_start = 0;
	//power_start�ĺ���
	//0: ��������ţֱ�ӿ���������ͨ���жϣ�����������������ֱ�ӿ���������power������ǰ����ϵͳ״̬�������ص������ͣ���������
	//1: ����״̬�£�������ţֱ�ӿ�����ͬʱҪ���ص����㹻��
	//2: ��������ţֱ�ӿ���������ͨ���жϣ�����������������ֱ�ӿ���������power������ǰ����ϵͳ״̬����Ҫ���ص���
	//3: ����״̬�£�������ţֱ�ӿ�������Ҫ���ص���
	wBoot_script_parser_fetch("target", "power_start", &power_start, 1);
	__inf("power status = %d\n", status);
	__inf("power start cause = %d\n", power_start);
	if(status == BATTERY_RATIO_TOO_LOW_WITH_DCIN)//�͵磬ͬʱ���ⲿ��Դ״̬��
	{
		if(!(power_start & 0x02))	//��Ҫ�жϵ�ǰ��ص�����Ҫ��power_start�ĵ�1bit��ֵΪ0
		{							//��������£�ֱ�ӹػ�
			__inf("battery low power with dc or ac, should charge longer\n");
			ShowPictureEx("c:\\os_show\\bempty.bmp", 0);
			wBoot_timer_delay(3000);

			return -1;
		}
		else
		{
			if(power_start == 3)	//����Ҫ�жϵ�ǰ��ص��������Ϊ3�������ϵͳ�����Ϊ0������к����ж�
			{
				return 0;
			}
		}
	}
	else							//��ص����㹻����£�����û�е��
	{
		if(power_start & 0x01)		//�����0bit��ֵΪ1�������ϵͳ
		{
			return 0;
		}
	}

	status = -1;
	status = wBoot_power_check_startup();
	__inf("startup status = %d\n", status);
#ifdef FORCE_BOOT_STANDBY
	status = 1;
#endif
	if(status <= 0)
	{
		return 0;
	}
	//if(1)
	{
		int   dcin, bat_exist, counter;
		__s32 bat_cal, this_bat_cal;
		__u32 bat_show_hd = NULL;
		int   i, j, key_status;
		int   bat_full_status = 0;
		int   bat_init = 0;

		//��ǰ����ȷ���ǻ�ţ�����������Ƿ񿪻�����ȷ������Ҫȷ�ϵ���Ƿ����
		//����ز����ڼ���������ش�����ػ�
		counter = 4;
		do
		{
			dcin = 0;
			bat_exist = 0;
			wBoot_power_get_dcin_battery_exist(&dcin, &bat_exist);
			if(bat_exist == -1)
			{
				if(!bat_init)
				{
					bat_show_hd = ShowBatteryCharge_init(0);
					bat_init = 1;
				}
				wBoot_timer_delay(500);
			}
			else
			{
				break;
			}
		}
		while(counter --);
#ifdef FORCE_BOOT_STANDBY
		bat_exist = 1;
#endif
		if(bat_exist <= 0)
		{
			__inf("no battery exist\n");
			if(bat_init)
			{
				ShowBatteryCharge_exit(bat_show_hd);
			}
			return 0;
		}
		this_bat_cal = wBoot_power_get_cal();
		__inf("base bat_cal = %d\n", this_bat_cal);
		if(this_bat_cal > 95)
		{
			this_bat_cal = 100;
		}
		//�����жϼ��
		//if(status == 2)	//only vbus exist
		{
			usb_detect_enter();
			wBoot_timer_delay(BOOT_USB_DETECT_DELAY_TIME + 200);
			usb_detect_exit();
		}
		power_limit_detect_enter();

		if(this_bat_cal == 100)
		{
			if(bat_init)
			{
				ShowBatteryCharge_exit(bat_show_hd);
			}
			bat_show_hd = NULL;
			show_battery_full(&bat_full_status);
			for(i =0;i<12;i++)
			{
				if(boot_standby_action & 0x02)	//�̰�
				{
					boot_standby_action &= ~2;
					i = 0;
				}
				else if(boot_standby_action & 0x01) //����
				{
					power_limit_detect_exit();

					return 0;
				}
				wBoot_timer_delay(250);
			}
		}
		else
		{
			int one_delay;

			if(!bat_init)
			{
				bat_show_hd = ShowBatteryCharge_init(0);
			}
			one_delay = 1000/(10 - (this_bat_cal/10));
			for(j=0;j<3;j++)
			{
				for(i=this_bat_cal;i<110;i+=10)
				{
					ShowBatteryCharge_rate(bat_show_hd, i);
					if(boot_standby_action & 0x08)		//����USB�ӿڵ�Դ
					{
						boot_standby_action &= ~8;
						j = 0;
					}
					else if(boot_standby_action & 0x02)	//�̰�
					{
						boot_standby_action &= ~2;
						j = 0;
					}
					else if(boot_standby_action & 0x01) //����
					{
						ShowBatteryCharge_exit(bat_show_hd);

						power_limit_detect_exit();
						return 0;
					}
					else if(boot_standby_action & 0x10) //usb out
					{
						status = 10;
						boot_standby_action &= ~0x10;

						goto __start_case_status__;
					}
					wBoot_timer_delay(one_delay);
				}
			}
			ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
			for(j=0;j<4;j++)
			{
				if(boot_standby_action & 0x08)		//����USB�ӿڵ�Դ
				{
					boot_standby_action &= ~8;
					j = 0;
				}
				else if(boot_standby_action & 0x10) //usb out
				{
					status = 10;
					boot_standby_action &= ~0x10;

					goto __start_case_status__;
				}
				wBoot_timer_delay(250);
			}
		}
		__inf("extenal power low go high startup\n");
/******************************************************************
*
*	standby ����ֵ˵��
*
*	   -1: ����standbyʧ��
*		1: ��ͨ��������
*		2: ��Դ�����̰�����
*		3: ��Դ������������
*		4: �ⲿ��Դ�Ƴ�����
*		5: ��س�����
*		6: �ڻ���״̬���ⲿ��Դ���Ƴ�
*		7: �ڻ���״̬�³�����
*
******************************************************************/
		do
		{
			__inf("enter standby\n");
			De_CloseLayer(board_res.layer_hd);
			power_limit_detect_exit();
			status = wBoot_standby();
			__inf("exit standby by %d\n", status);

			bat_cal = wBoot_power_get_cal();
			__inf("current bat_cal = %d\n", bat_cal);
			if(bat_cal > this_bat_cal)
			{
				this_bat_cal = bat_cal;
			}
__start_case_status__:
			__inf("status = %d\n", status);
			switch(status)
			{
				case 2:		//�̰�power�������»���
					//�����жϼ��
					boot_standby_action = 0;
					power_limit_detect_enter();
					De_OpenLayer(board_res.layer_hd);
				case 1:
					if(this_bat_cal == 100)
					{
						if(bat_show_hd)
						{
							ShowBatteryCharge_exit(bat_show_hd);
							bat_show_hd = NULL;
						}
						show_battery_full(&bat_full_status);
						for(i=0;i<12;i++)
						{
							if(boot_standby_action & 0x08)		//����USB�ӿڵ�Դ
							{
								boot_standby_action &= ~8;
								i = 0;
							}
							else if(boot_standby_action & 0x02)	//�̰�
							{
								boot_standby_action &= ~2;
								i = 0;
							}
							else if(boot_standby_action & 0x01) //����
							{
								ShowBatteryCharge_exit(bat_show_hd);
								power_limit_detect_exit();

								return 0;
							}
							else if(boot_standby_action & 0x10) //usb out
							{
								status = 10;
								boot_standby_action &= ~0x10;

								goto __start_case_status__;
							}

							wBoot_timer_delay(250);
						}
					}
					else
					{
						int one_delay;

						one_delay = 1000/(10 - (this_bat_cal/10));
						for(j=0;j<3;j++)
						{
							for(i=this_bat_cal;i<110;i+=10)
							{
								ShowBatteryCharge_rate(bat_show_hd, i);
								if(boot_standby_action & 0x08)		//����USB�ӿڵ�Դ
								{
									boot_standby_action &= ~8;
									j = 0;
								}
								if(boot_standby_action & 0x02)	//�̰�
								{
									boot_standby_action &= ~2;
									j = 0;
								}
								else if(boot_standby_action & 0x01) //����
								{
									ShowBatteryCharge_exit(bat_show_hd);
									power_limit_detect_exit();

									return 0;
								}
								else if(boot_standby_action & 0x10) //usb out
								{
									status = 10;
									boot_standby_action &= ~0x10;

									goto __start_case_status__;
								}
								wBoot_timer_delay(one_delay);
							}
						}
						ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
						for(j=0;j<4;j++)
						{
							if(boot_standby_action & 0x08)		//����USB�ӿڵ�Դ
							{
								boot_standby_action &= ~8;
								j = 0;
							}
							else if(boot_standby_action & 0x10) //usb out
							{
								status = 10;
								boot_standby_action &= ~0x10;

								goto __start_case_status__;
							}
							wBoot_timer_delay(250);
						}
					}

					break;

				case 3:		//������Դ����֮�󣬹رյ��ͼ�꣬����ϵͳ
					ShowBatteryCharge_exit(bat_show_hd);

					return 0;

				case 4:		//���Ƴ��ⲿ��Դʱ��������ʾ��ǰ���ͼ���3���ػ�
				case 5:		//����س����ɵ�ʱ����Ҫ�ػ�
					//�����жϼ��
					boot_standby_action = 0;
					power_limit_detect_enter();

					De_OpenLayer(board_res.layer_hd);
					ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
				case 6:
				case 7:
					if((status != 4) && (status != 5))
					{
						De_OpenLayer(board_res.layer_hd);
						ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
					}
				case 10:
					ShowBatteryCharge_rate(bat_show_hd, this_bat_cal);
					wBoot_timer_delay(500);
					if(bat_show_hd)
					{
						int ret;

						do
						{
							if(!(boot_standby_action & 0x04))
							{
								ret = ShowBatteryCharge_degrade(bat_show_hd, 10);
							}
							else
							{
								status = 1;
								ShowBatteryCharge_reset(bat_show_hd);

								goto __start_case_status__;
							}
						}
						while(!ret);

						ShowBatteryCharge_exit(bat_show_hd);
					}
					else
					{
						shut_battery_full();
					}
					power_limit_detect_exit();

					return -1;

				break;
				case 8:		//standby�����м�⵽vbus����
				{
					usb_detect_enter();
					wBoot_timer_delay(BOOT_USB_DETECT_DELAY_TIME + 200);
					usb_detect_exit();
				}
				break;
				case 9:		//standby�����м�⵽vbus�Ƴ���ͬʱ������ͨdc
				{
//					power_set_usbpc();
				}
				break;

				default:
					break;
			}
		}
		while(1);
	}
}
#endif

static int show_battery_full(int *status)
{
	if(!*status)
	{
		*status = 1;
		pic_layer_para = ShowPictureEx("c:\\os_show\\bat10.bmp", 0);
	}
	else
	{
		De_OpenLayer(board_res.layer_hd);
	}

	return 0;
}

static int shut_battery_full(void)
{
	int  alpha_step, i;
	int  aplha, delay_time;
	display_layer_info_t *layer_para;

	if(!pic_layer_para)
    {
        return -1;
    }
    layer_para = (display_layer_info_t *)pic_layer_para;
	alpha_step = 5;
	delay_time = 50;
	aplha = layer_para->alpha_val;

	for(i=0xff;i>0;i -= alpha_step)
	{
		layer_para->alpha_en = 1;
		aplha -= alpha_step;
		if(aplha > 0)
		{
			De_SetLayerPara(board_res.layer_hd, layer_para);
			wBoot_timer_delay(delay_time);
			layer_para->alpha_val = aplha;
		}
		else
		{
			break;
		}
	}

	return 0;
}

