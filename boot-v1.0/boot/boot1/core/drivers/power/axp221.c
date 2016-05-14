/*
 * (C) Copyright 2012
 *     wangflord@allwinnerstech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */
#include "include.h"
#include "axp221.h"
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
int axp221_set_charge_control(void)
{
	u8 reg_value;
	//disable ts adc, enable all other adc
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_ADC_EN, &reg_value))
    {
        return -1;
    }
    reg_value |= 0xfe;
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_ADC_EN, reg_value))
    {
        return -1;
    }
    //enable charge
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
    reg_value |= 0x80;
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_CHARGE1, reg_value))
    {
        return -1;
    }

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
int axp221_clear_data_buffer(void)
{
	int i;

	for(i=BOOT_POWER22_DATA_BUFFER0;i<=BOOT_POWER22_DATA_BUFFER11;i++)
	{
		if(axp_i2c_write(AXP22_ADDR, i, 0x00))
	    {
	        return -1;
	    }
	}

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
int axp221_probe_dcin_exist(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_STATUS, &reg_value))
    {
        return -1;
    }

    return (reg_value & 0x50);
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
int axp221_probe_battery_ratio(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_COULOMB_CAL, &reg_value))
    {
        return -1;
    }

	return reg_value & 0x7f;
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
int axp221_probe_battery_exist(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_MODE_CHGSTATUS, &reg_value))
    {
        return -1;
    }

	if(reg_value & 0x10)
	{
		return (reg_value & 0x20);
	}
	else
	{
		return -1;
	}
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
int axp221_probe_power_status(void)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_STATUS, &reg_value))
    {
        return -1;
    }
	if(reg_value & 0x40)		//dc in exist
	{
		return 3;
	}
	if(reg_value & 0x10)		//vbus exist
	{
		return 2;
	}
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
int axp221_probe_battery_vol(void)
{
	u8  reg_value_h, reg_value_l;
	int bat_vol, tmp_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_BAT_AVERVOL_H8, &reg_value_h))
    {
        return -1;
    }
    if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_BAT_AVERVOL_L4, &reg_value_l))
    {
        return -1;
    }
    tmp_value = (reg_value_h << 4) | reg_value_l;
    bat_vol = tmp_value * 11;
    bat_vol /= 10;

	return bat_vol;
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
int axp221_probe_buttery_resistance_record(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DATA_BUFFER1, &reg_value))
    {
        return -1;
    }

	return reg_value;
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
int axp221_probe_key(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS3, &reg_value))
    {
        return -1;
    }
    reg_value &= 0x03;
	if(reg_value)
	{
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS3, reg_value))
	    {
	        return -1;
	    }
	}

	return reg_value;
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
int axp221_probe_last_poweron_status(void)
{
	u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DATA_BUFFER11, &reg_value))
    {
        return -1;
    }

	return reg_value;
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
int axp221_set_next_poweron_status(int data)
{
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DATA_BUFFER11, (u8)data))
    {
        return -1;
    }

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
int axp221_write_data_buffer(int buffer, __u8 value)
{
	if(axp_i2c_write(AXP22_ADDR, (u8)buffer, value))
    {
        return -1;
    }

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
int axp221_probe_poweron_cause(void)
{
    __u8   reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_STATUS, &reg_value))
    {
        return -1;
    }

    return reg_value & 0x01;
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
int axp221_set_dc1sw(int on_off)
{
    u8   reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		eGon2_printf("%d\n", __LINE__);
        return -1;
    }
    if(on_off)
    {
		reg_value |= (1 << 7);
	}
	else
	{
		reg_value &= ~(1 << 7);
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		eGon2_printf("sunxi pmu error : unable to set dcdc1\n");

		return -1;
	}

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
int axp221_set_dldo3(int on_off)
{
    u8   reg_value;

	if(axp_i2c_write(NULL, BOOT_POWER22_OUTPUT_DLDO3_VO, 0x0b))
	{
		eGon2_printf("sunxi pmu error : unable to set dldo3\n");

		return -1;
	}

	if(axp_i2c_read(NULL, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
    {
		eGon2_printf("sunxi pmu error : unable to read output ctl 2\n");
        return -1;
    }
    if(on_off)
    {
		reg_value |= (1 << 5);
	}
	else
	{
		reg_value &= ~(1 << 5);
	}

	if(axp_i2c_write(NULL, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	{
		wlibc_uprintf("sunxi pmu error : unable to set output ctl 2\n");

		return -1;
	}

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
int axp221_set_dcdc1(int set_vol)
{
    u8   reg_value;

	if((set_vol < 1600) || (set_vol > 3400))
	{
		eGon2_printf("%d\n", __LINE__);
		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, &reg_value))
    {
		eGon2_printf("%d\n", __LINE__);
        return -1;
    }
	reg_value = ((set_vol - 1600)/100);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC1OUT_VOL, reg_value))
	{
		eGon2_printf("sunxi pmu error : unable to set dcdc1\n");

		return -1;
	}

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
int axp221_set_dcdc2(int set_vol)
{
    uint tmp, i;
    int  vol;
    u8   reg_value;

	if((set_vol < 600) || (set_vol > 1540))
	{
		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, &reg_value))
    {
        return -1;
    }
    reg_value &= ~0x3f;
    reg_value |= (set_vol - 600)/20;
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC2OUT_VOL, reg_value))
    {
        return -1;
    }

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
int axp221_set_dcdc3(int set_vol)
{
    u8   reg_value;
    u8   reg_dump;

	if((set_vol < 600) || (set_vol > 1860))
	{
		eGon2_printf("%d\n", __LINE__);

		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, &reg_value))
    {
    	eGon2_printf("%d\n", __LINE__);

        return -1;
    }
	reg_value = ((set_vol - 600)/20);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC3OUT_VOL, reg_value))
	{
		eGon2_printf("sunxi pmu error : unable to set dcdc3\n");

		return -1;
	}

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
int axp221_set_dcdc4(int set_vol)
{
    u8   reg_value;

	if((set_vol < 600) || (set_vol > 1540))
	{
		eGon2_printf("%d\n", __LINE__);

		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, &reg_value))
    {
    	eGon2_printf("%d\n", __LINE__);

        return -1;
    }
	reg_value = ((set_vol - 600)/20);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC4OUT_VOL, reg_value))
	{
		eGon2_printf("sunxi pmu error : unable to set dcdc1\n");

		return -1;
	}

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
int axp221_set_dcdc5(int set_vol)
{
    u8   reg_value;

	if((set_vol < 1000) || (set_vol > 2550))
	{
		eGon2_printf("%d\n", __LINE__);

		return -1;
	}
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, &reg_value))
    {
    	eGon2_printf("%d\n", __LINE__);

        return -1;
    }
	reg_value = ((set_vol - 1000)/50);
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_DC5OUT_VOL, reg_value))
	{
		eGon2_printf("sunxi pmu error : unable to set dcdc1\n");

		return -1;
	}

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
int axp221_set_ldo2(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<6);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO2OUT_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO2OUT_VOL, reg_value))
	    {
	        return -1;
	    }
	}

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
int axp221_set_ldo3(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, &reg_value))
		{
			return -1;
		}
		reg_value &= ~(1<<7);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, reg_value))
		{
			return -1;
		}
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO3OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO3OUT_VOL, reg_value))
		{
			return -1;
		}
	}

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
int axp221_set_ldo4(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL3, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<7);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL3, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO4OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO4OUT_VOL, reg_value))
		{
			return -1;
		}
	}
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
int axp221_set_ldo5(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<3);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO5OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO5OUT_VOL, reg_value))
		{
			return -1;
		}
	}

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
int axp221_set_ldo6(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<4);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO6OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO6OUT_VOL, reg_value))
		{
			return -1;
		}
	}

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
int axp221_set_ldo7(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<5);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO7OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO7OUT_VOL, reg_value))
		{
			return -1;
		}
	}

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
int axp221_set_ldo8(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<6);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO8OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO8OUT_VOL, reg_value))
		{
			return -1;
		}
	}
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
int axp221_set_ldo9(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~1;
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO9OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO9OUT_VOL, reg_value))
		{
			return -1;
		}
	}
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
int axp221_set_ldo10(int set_vol)
{
	u8 reg_value;

	if(!set_vol)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~(1<<1);
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, reg_value))
	    {
	        return -1;
	    }
	}
	else
	{
		if((set_vol < 700) || (set_vol > 3300))
		{
			return -1;
		}
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_LDO10OUT_VOL, &reg_value))
		{
			return -1;
		}
		reg_value &= 0x1f;
		reg_value |= ((reg_value - 700)/100);
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_LDO11OUT_VOL, reg_value))
		{
			return -1;
		}
	}
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
int axp221_set_gpio1ldo(int onoff, int set_vol)
{
	u8 reg_value;

	if(onoff)
	{
		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO1_VOL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= 0xf0;
	    if(set_vol < 700)
	    {
	    	set_vol = 700;
	    }
	    else if(set_vol > 3300)
	    {
	    	set_vol = 3300;
	    }
	    reg_value |= ((set_vol - 700)/100) & 0x0f;
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO1_VOL, reg_value))
	    {
	        return -1;
	    }

		if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_GPIO1_CTL, &reg_value))
	    {
	        return -1;
	    }
	    reg_value &= ~7;
	    reg_value |=  3;
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_GPIO1_CTL, reg_value))
	    {
	        return -1;
	    }
	}

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
int axp221_set_power_off(void)
{
    u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_OFF_CTL, &reg_value))
    {
        return -1;
    }
    reg_value |= 1 << 7;
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OFF_CTL, reg_value))
    {
        return -1;
    }

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
int axp221_set_poweroff_vol(int set_vol)
{
	u8 reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_VOFF_SET, &reg_value))
    {
        return -1;
    }
	reg_value &= 0xf8;
	if(set_vol >= 2600 && set_vol <= 3300)
	{
		reg_value |= (set_vol - 2600)/100;
	}
	else if(set_vol <= 2600)
	{
		reg_value |= 0x00;
	}
	else
	{
		reg_value |= 0x07;
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_VOFF_SET, reg_value))
    {
        return -1;
    }

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
int axp221_set_charge_current(int current)
{
	u8   reg_value;
	int  step;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
	reg_value &= ~0x0f;
	if(current > 2550)
	{
		current = 2550;
	}
	else if(current < 300)
	{
		current = 300;
	}
	step       = (current/150) - 2;
	reg_value |= (step & 0x0f);

	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_CHARGE1, reg_value))
    {
        return -1;
    }

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
int axp221_probe_charge_current(void)
{
	__u8  reg_value;
	int	  current;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
	reg_value &= 0x0f;
	current = (reg_value + 2) * 150;

	return current;
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
int axp221_probe_charge_status(void)
{
	__u8  reg_value;

	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS2, &reg_value))
    {
        return -1;
    }
	return (reg_value >> 2) & 0x01;
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
int axp221_set_vbus_cur_limit(int current)
{
	__u8 reg_value;

	//set bus current limit off
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_IPS_SET, &reg_value))
    {
        return -1;
    }
    reg_value &= 0xfC;
	if(!current)
	{
	    reg_value |= 0x03;
	}
	else if(current <= 500)		//limit to 500
	{
		reg_value |= 0x01;
	}
	else						//limit to 900
	{
		reg_value |= 0;
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_IPS_SET, reg_value))
    {
        return -1;
    }

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
int axp221_set_vbus_vol_limit(int vol)
{
	__u8 reg_value;

	//set bus vol limit off
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_IPS_SET, &reg_value))
    {
        return -1;
    }
    reg_value &= ~(7 << 3);
	if(!vol)
	{
	    reg_value &= ~(1 << 6);
	}
	else
	{
		if(vol < 4000)
		{
			vol = 4000;
		}
		else if(vol > 4700)
		{
			vol = 4700;
		}
		reg_value |= ((vol-4000)/100) << 3;
	}
	if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_IPS_SET, reg_value))
    {
        return -1;
    }

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
int axp221_set_chgcur(int cur)
{
	__u8 reg_value;

	if(cur < 300)
	{
		cur = 300;
	}
	else if(cur > 2550)
	{
		cur = 2550;
	}
	//set charge current
	if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_CHARGE1, &reg_value))
    {
        return -1;
    }
    reg_value &= 0xf0;
    reg_value |= (((cur - 300)/150) & 0x0f);
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_CHARGE1, reg_value))
    {
        return -1;
    }

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
int axp221_probe_rest_battery_capacity(void)
{
	u8  reg_value;
	int bat_rest;

	if(axp221_probe_charge_status() == 1)
	{
		return 100;
	}
    if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_COULOMB_CAL, &reg_value))
    {
        return -1;
    }
	bat_rest = reg_value & 0x7F;
    return bat_rest;
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
int axp221_int_query(__u8 *addr)
{
	int   i;
	__u8  int_reg = BOOT_POWER22_INTSTS1;

	for(i=0;i<5;i++)
	{
	    if(axp_i2c_read(AXP22_ADDR, BOOT_POWER22_INTSTS1 + i, addr + i))
	    {
	        return -1;
	    }
	}

	for(i=0;i<5;i++)
	{
	    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_INTSTS1 + i, 0xff))
	    {
	        return -1;
	    }
	}

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
int axp221_int_enable_read(__u8 *addr)
{
	int   i;
	__u8  int_reg = BOOT_POWER22_INTEN1;

	for(i=0;i<5;i++)
	{
		if(axp_i2c_read(AXP22_ADDR, int_reg, addr + i))
	    {
	        return -1;
	    }
	    int_reg ++;
	}

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
int axp221_int_enable_write(__u8 *addr)
{
	int   i;
	__u8  int_reg = BOOT_POWER22_INTEN1;

	for(i=0;i<5;i++)
	{
		if(axp_i2c_write(AXP22_ADDR, int_reg, addr[i]))
	    {
	        return -1;
	    }
	    int_reg ++;
	}

	return 0;
}

int axp221_reset_power_source(void)
{
		if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL1, 0xbf))
    {
        return -1;
    }
    if(axp_i2c_write(AXP22_ADDR, BOOT_POWER22_OUTPUT_CTL2, 0x00))
    {
        return -1;
    }
    return 0;
}
