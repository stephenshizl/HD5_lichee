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

#include  "include.h"
#include  "common_res.h"

/*
*******************************************************************************
*                     De_SetBlkColor
*
* Description:
*    ���ñ�����ɫ
*
* Parameters:
*    Color  :  input. ��ɫ
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_SetBlkColor(__disp_color_t *Color)
{
    __s32 ret;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)Color;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_SET_BKCOLOR, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_SET_BKCOLOR failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_GetProperHDMIMode
*
* Description:
*    ��ȡhdmi��֧�ֵ���߷ֱ���ģʽ
*
* Parameters:
*    void
*
* Return value:
*    mode  :  ��߷ֱ���ģʽ
*   -1 :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_GetProperHDMIMode(void)
{
    __u32 arg[3];
    __u32 i;
    __s32 ret;
    __u32 tv_mode[13]={
    DISP_TV_MOD_1080P_60HZ,      
    DISP_TV_MOD_1080P_50HZ,    
    DISP_TV_MOD_1080P_24HZ,
    DISP_TV_MOD_1080I_60HZ,           
    DISP_TV_MOD_1080I_60HZ,        
    DISP_TV_MOD_1080I_50HZ,        
    DISP_TV_MOD_720P_60HZ ,        
    DISP_TV_MOD_720P_50HZ ,        
    DISP_TV_MOD_576P      ,        
    DISP_TV_MOD_480P      ,        
    DISP_TV_MOD_576I      ,        
    DISP_TV_MOD_480I      ,
    };

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }
    
    for(i=0; i<13; i++)
    {
        arg[0] = tv_mode[i];
        arg[1] = 0;
        arg[2] = 0;
        ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_SUPPORT_MODE, 0, arg);

        if(ret == 1)
        {
            break;
        }
    }

    if(i == 13)
    {
        ret = -1;
    }else
    {
        ret = tv_mode[i];
    }

    return ret;
}



/*
*******************************************************************************
*                     De_OpenDevice
*
* Description:
*    ��TV
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_OpenDevice(__s32 display_source)
{
    __s32 ret = 0;
    __s32 aux;
    __s32 high_flag, low_flag;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }
    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
#if 0    
    high_flag = (display_source >> 16) & 0xffff;
    low_flag  = (display_source >>  0) & 0xffff;

    if(DISP_OUTPUT_TYPE_LCD == high_flag)   //TV���
    {
        ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LCD_ON, 0, (void*)arg);
    }
    else if(DISP_OUTPUT_TYPE_TV == high_flag)
    {
        if(DISP_TV_CVBS == low_flag)
        {
            aux = DISP_TV_MOD_720P_50HZ;
        }
        else if(DISP_TV_YPBPR == low_flag)
        {
            aux = DISP_TV_MOD_PAL;
        }
        else
        {
            return -1;
        }
        arg[0] = aux;
        wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_TV_SET_MODE, 0, arg);
        ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_TV_ON, 0, NULL);
    }
    else if(DISP_OUTPUT_TYPE_HDMI == high_flag)
    {
    	arg[0] = low_flag&0xff;
        wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_SET_MODE, 0, arg);
        ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_ON, 0, NULL);;
    }
    else
    {
        return -1;
    }

    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_HDMI_ON failed\n");
    }
#else
{
    int  value = 1;
    int  ret;
    __u32 output_type = 0;
    __u32 output_mode = 0;
    __u32 auto_hpd = 0;
    __u32 err_count = 0;
    __u32 arg[4];
    int i;

    if(wBoot_script_parser_fetch("boot_disp", "output_type", &value, 1) < 0)
    {
        __wrn("fetch script data boot_disp.output_type fail\n");
        err_count ++;
        value = 0;
    }else
    {
    	__inf("boot_disp.output_type=%d\n", value);
    }

    if(value == 0)
    {
        output_type = DISP_OUTPUT_TYPE_NONE;
    }
    else if(value == 1)
    {
        output_type = DISP_OUTPUT_TYPE_LCD;
    }
    else if(value == 2)
    {
        output_type = DISP_OUTPUT_TYPE_TV;
    }
    else if(value == 3)
    {
        output_type = DISP_OUTPUT_TYPE_HDMI;
    }
    else if(value == 4)
    {
        output_type = DISP_OUTPUT_TYPE_VGA;
    }
    else
    {
        __wrn("invalid output_type %d\n", value);
        return -1;
    }
//screen0_output_mode    
    if(wBoot_script_parser_fetch("boot_disp", "output_mode", &value, 1) < 0)
    {
        __wrn("fetch script data boot_disp.output_mode fail\n");
        err_count ++;
        value = 0;
    }else
    {
    	__inf("boot_disp.output_mode=%d\n", value);
    }
    
    if(output_type == DISP_OUTPUT_TYPE_TV || output_type == DISP_OUTPUT_TYPE_HDMI)
    {
        output_mode = (__disp_tv_mode_t)value;
    }
    else if(output_type == DISP_OUTPUT_TYPE_VGA)
    {
        output_mode = (__disp_vga_mode_t)value;
    }

//auto hot plug detect    
    if(wBoot_script_parser_fetch("boot_disp", "auto_hpd", &value, 1) < 0)
    {
        __wrn("fetch script data boot_disp.auto_hpd fail\n");
        err_count ++;
        value = 0;
    }else
    {    
    	__inf("boot_disp.auto_hpd=%d\n", value);
    }


    if(err_count == 3)//no boot_disp config
    {
        if(wBoot_script_parser_fetch("lcd0_para", "lcd_used", &value, 1) < 0)
        {
            __wrn("fetch script data lcd0_para.lcd_used fail\n");
            value = 0;
        }else
        {    
        	__inf("lcd0_para.lcd_used=%d\n", value);
        }

        if(value == 1) //lcd available
        {
            output_type = DISP_OUTPUT_TYPE_LCD;
        }else
        {
            arg[0] = 0;
            arg[1] = 0;
            arg[2] = 0;
            ret = 0;
            for(i=0; (i<3)&&(ret==0); i++)
            {
                ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_GET_HPD_STATUS, 0, (void*)arg);
            }
            if(ret == 1)
            {
                output_type = DISP_OUTPUT_TYPE_HDMI;
                //output_mode = De_GetProperHDMIMode();
                //output_mode = (output_mode == -1)? DISP_TV_MOD_720P_50HZ:output_mode;
                output_mode = DISP_TV_MOD_720P_50HZ;
            }
        }
        
    }else//has boot_disp config
    {
        if(output_type == DISP_OUTPUT_TYPE_LCD)
        {

        }else if(auto_hpd == 1)
        {
            if(wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_GET_HPD_STATUS, 0, (void*)arg) == 1)
            {
                output_type = DISP_OUTPUT_TYPE_HDMI;
                //output_mode = De_GetProperHDMIMode();
                //output_mode = (output_mode == -1)? DISP_TV_MOD_720P_50HZ:output_mode;
                output_mode = DISP_TV_MOD_720P_50HZ;
            }
        }
    }
    
    if(output_type == DISP_OUTPUT_TYPE_LCD)
    {
        __inf("lcd open\n");
        arg[0] = 0;
        arg[1] = 0;
        arg[2] = 0;
        ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LCD_ON, 0, (void*)arg);
        __inf("lcd open,ret=%d\n",ret);
    }
    else if(output_type == DISP_OUTPUT_TYPE_HDMI)
    {
        __inf("hdmi open\n");
        arg[0] = output_mode;
        arg[1] = 0;
        arg[2] = 0;
        wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_SET_MODE, 0, arg);
        ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_HDMI_ON, 0, NULL);
    }
}
#endif
    return ret;
}

/*
*******************************************************************************
*                     De_CloseDevice
*
* Description:
*    �ر�TV
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_CloseDevice(__s32 display_source)
{
    __s32 ret = 0;
    __u32 arg[3], cmd;
    __s32 high_flag;

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }
    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    high_flag = (display_source >> 16) & 0xffff;

    if(DISP_OUTPUT_TYPE_LCD == high_flag)   //TV���
    {
        cmd = DISP_CMD_LCD_OFF;
    }
    else if(DISP_OUTPUT_TYPE_TV == high_flag)
    {
        cmd = DISP_CMD_TV_OFF;
    }
    else if(DISP_OUTPUT_TYPE_HDMI == high_flag)
    {
        cmd = DISP_CMD_HDMI_OFF;
    }
    else
    {
        return -1;
    }

    ret = wBoot_driver_ioctl(board_res.disp_hd, cmd, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: display driver turn off display device failed\n");
    }

    return ret;
}
/*
*******************************************************************************
*                     De_IsLCDOpen
*
* Description:
*    LCD�Ƿ�� ��
*
* Parameters:
*    void
*
* Return value:
*    <0	:  �ɹ�
*    0 	:  LCD���ڳ�ʼ��
*    >0	:  ������ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_IsLCDOpen(void)
{
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    return wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LCD_CHECK_OPEN_FINISH, 0, (void*)arg);
}

/*
*******************************************************************************
*                     De_IsLCDClose
*
* Description:
*    LCD�Ƿ�ر� ��
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_IsLCDClose(void)
{
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;
    return wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LCD_CHECK_CLOSE_FINISH, 0, (void*)arg);
}
/*
*******************************************************************************
*                     De_RequestLayer
*
* Description:
*    ����ͼ��
*
* Parameters:
*    layer_para  :  input.  ͼ�����
*    Layer_hd    :  output. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__hdle De_RequestLayer(__disp_layer_work_mode_t mode)
{
    __hdle Layer_hd = NULL;
    __u32  arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return (__hdle)0;
    }

    arg[0] = mode;
    arg[1] = 0;
    arg[2] = 0;
    Layer_hd = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_REQUEST, 0, (void*)arg);
    if(Layer_hd == NULL)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_REQUEST failed\n");
    }

    return Layer_hd;
}


/*
*******************************************************************************
*                     De_ReleaseLayer
*
* Description:
*    �ͷ�ͼ��
*
* Parameters:
*    Layer_hd    :  input. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_ReleaseLayer(__s32 layer_hdl)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_RELEASE, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_RELEASE failed\n");
    }

    return ret;
}


/*
*******************************************************************************
*                     De_OpenLayer
*
* Description:
*    ��ͼ��
*
* Parameters:
*    Layer_hd    :  input. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_OpenLayer(__s32 layer_hdl)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_OPEN, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_OPEN failed\n");
    }

    return ret;
}


/*
*******************************************************************************
*                     De_CloseLayer
*
* Description:
*    �ر�ͼ��
*
* Parameters:
*    Layer_hd    :  input. ͼ����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_CloseLayer(__s32 layer_hdl)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = 0;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_CLOSE, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_CLOSE failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_GetFrameBuffer
*
* Description:
*    ��� FrameBuffer
*
* Parameters:
*    Layer_hd    :  input.  ͼ����
*    FrameBuffer :  output. ͼ�㻺����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_GetFrameBuffer(__s32 layer_hdl, __disp_fb_t *FrameBuffer)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)FrameBuffer;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_GET_FB, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_GET_FB failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_SetFrameBuffer
*
* Description:
*    ����FrameBuffer
*
* Parameters:
*    Layer_hd    :  input.  ͼ����
*    FrameBuffer :  input.  ͼ�㻺����
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_SetFrameBuffer(__s32 layer_hdl, __disp_fb_t *FrameBuffer)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)FrameBuffer;
    arg[2] = 0;

    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_SET_FB, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_SET_FB failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_GetSceenWidth
*
* Description:
*    �����Ļ�Ŀ��
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__u32 De_GetSceenWidth(void)
{
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return 0;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;

    return wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_SCN_GET_WIDTH, 0, (void*)arg);
}



/*
*******************************************************************************
*                     De_GetSceenWidth
*
* Description:
*    �����Ļ�ĸ߶�
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__u32 De_GetSceenHeight(void)
{
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return 0;
    }

    arg[0] = 0;
    arg[1] = 0;
    arg[2] = 0;

    return wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_SCN_GET_HEIGHT, 0, (void*)arg);
}

/*
*******************************************************************************
*                     De_GetLayerPara
*
* Description:
*    ���ͼ�����
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_GetLayerPara(__s32 layer_hdl, __disp_layer_info_t * layer_para)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)layer_para;
    arg[2] = 0;
    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_GET_PARA, 0, (void*)arg);

    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_GET_PARA failed\n");
    }

    return ret;
}

/*
*******************************************************************************
*                     De_SetLayerPara
*
* Description:
*    ����ͼ�����
*
* Parameters:
*    void
*
* Return value:
*    0  :  �ɹ�
*   !0  :  ʧ��
*
* note:
*    void
*
*******************************************************************************
*/
__s32 De_SetLayerPara(__s32 layer_hdl, __disp_layer_info_t * layer_para)
{
    __s32 ret = 0;
    __u32 arg[3];

    if(board_res.disp_hd == NULL)
    {
        DMSG_PANIC("ERR: display driver not open yet\n");

        return -1;
    }

    arg[0] = (__u32)layer_hdl;
    arg[1] = (__u32)layer_para;
    arg[2] = 0;

    ret = wBoot_driver_ioctl(board_res.disp_hd, DISP_CMD_LAYER_SET_PARA, 0, (void*)arg);
    if(ret)
    {
        DMSG_PANIC("ERR: wBoot_driver_ioctl DISP_CMD_LAYER_SET_PARA failed\n");
    }

    return ret;
}


__u32 De_TvModetoWidth(__disp_tv_mode_t mode)
{
    __u32 width = 0;

    switch(mode)
    {
        case DISP_TV_MOD_480I:
        case DISP_TV_MOD_576I:
        case DISP_TV_MOD_480P:
        case DISP_TV_MOD_576P:
        case DISP_TV_MOD_PAL:
        case DISP_TV_MOD_NTSC:
        case DISP_TV_MOD_PAL_SVIDEO:
        case DISP_TV_MOD_NTSC_SVIDEO:
        case DISP_TV_MOD_PAL_M:
        case DISP_TV_MOD_PAL_M_SVIDEO:
        case DISP_TV_MOD_PAL_NC:
        case DISP_TV_MOD_PAL_NC_SVIDEO:
            width = 720;
            break;
        case DISP_TV_MOD_720P_50HZ:
        case DISP_TV_MOD_720P_60HZ:
        case DISP_TV_MOD_720P_50HZ_3D_FP:
        case DISP_TV_MOD_720P_60HZ_3D_FP:
            width = 1280;
            break;
        case DISP_TV_MOD_1080I_50HZ:
        case DISP_TV_MOD_1080I_60HZ:
        case DISP_TV_MOD_1080P_24HZ:
        case DISP_TV_MOD_1080P_50HZ:
        case DISP_TV_MOD_1080P_60HZ:
        case DISP_TV_MOD_1080P_24HZ_3D_FP:
            width = 1920;
            break;
        default:
            width = 0;
            break;
    }

    return width;
}


__u32 De_TvModetoHeight(__disp_tv_mode_t mode)
{
    __u32 height = 0;

    switch(mode)
    {
        case DISP_TV_MOD_480I:
        case DISP_TV_MOD_480P:
        case DISP_TV_MOD_NTSC:
        case DISP_TV_MOD_NTSC_SVIDEO:
        case DISP_TV_MOD_PAL_M:
        case DISP_TV_MOD_PAL_M_SVIDEO:
            height = 480;
            break;
        case DISP_TV_MOD_576I:
        case DISP_TV_MOD_576P:
        case DISP_TV_MOD_PAL:
        case DISP_TV_MOD_PAL_SVIDEO:
        case DISP_TV_MOD_PAL_NC:
        case DISP_TV_MOD_PAL_NC_SVIDEO:
            height = 576;
            break;
        case DISP_TV_MOD_720P_50HZ:
        case DISP_TV_MOD_720P_60HZ:
            height = 720;
            break;
        case DISP_TV_MOD_1080I_50HZ:
        case DISP_TV_MOD_1080I_60HZ:
        case DISP_TV_MOD_1080P_24HZ:
        case DISP_TV_MOD_1080P_50HZ:
        case DISP_TV_MOD_1080P_60HZ:
            height = 1080;
            break;
        case DISP_TV_MOD_1080P_24HZ_3D_FP:
            height = 1080*2;
            break;
        case DISP_TV_MOD_720P_50HZ_3D_FP:
        case DISP_TV_MOD_720P_60HZ_3D_FP:
            height = 720*2;
            break;
        default:
            height = 0;
            break;
    }

    return height;
}

__u32 De_VgaModetoWidth(__disp_vga_mode_t mode)
{
    __u32 width = 0;

    switch(mode)
    {
    	case DISP_VGA_H1680_V1050:
    		width = 1680;
            break;
    	case DISP_VGA_H1440_V900:
    		width = 1440;
            break;
    	case DISP_VGA_H1360_V768:
    		width = 1360;
            break;
    	case DISP_VGA_H1280_V1024:
    		width = 1280;
            break;
    	case DISP_VGA_H1024_V768:
    		width = 1024;
            break;
    	case DISP_VGA_H800_V600:
    		width = 800;
            break;
    	case DISP_VGA_H640_V480:
    		width = 640;
            break;
    	case DISP_VGA_H1440_V900_RB:
    		width = 1440;
            break;
    	case DISP_VGA_H1680_V1050_RB:
    		width = 1680;
            break;
    	case DISP_VGA_H1920_V1080_RB:
    	case DISP_VGA_H1920_V1080:
    		width = 1920;
            break;
        case DISP_VGA_H1280_V720:
            width = 1280;
            break;
    	default:
    		width = 0;
            break;
    }

    return width;
}


__u32 De_VgaModetoHeight(__disp_vga_mode_t mode)
{
    __u32 height = 0;

    switch(mode)
    {
    case DISP_VGA_H1680_V1050:
        height = 1050;
        break;
    case DISP_VGA_H1440_V900:
        height = 900;
        break;
    case DISP_VGA_H1360_V768:
        height = 768;
        break;
    case DISP_VGA_H1280_V1024:
        height = 1024;
        break;
    case DISP_VGA_H1024_V768:
        height = 768;
        break;
    case DISP_VGA_H800_V600:
        height = 600;
        break;
    case DISP_VGA_H640_V480:
        height = 480;
        break;
    case DISP_VGA_H1440_V900_RB:
        height = 1440;
        break;
    case DISP_VGA_H1680_V1050_RB:
        height = 1050;
        break;
    case DISP_VGA_H1920_V1080_RB:
    case DISP_VGA_H1920_V1080:
        height = 1080;
        break;
    case DISP_VGA_H1280_V720:
        height = 720;
        break;
    default:
        height = 0;
        break;
    }

    return height;
}


__s32 De_GetFramebufferSize(__u32 *width, __u32 *height)
{
    __u32                   linux_output_type;
    __u32                   linux_output_mode;
    __u32                   value;
    
    
    if(wBoot_script_parser_fetch("disp_init", "screen0_output_type", &value, 1) < 0)
    {
        __wrn("fetch script data disp_init.screen0_output_type fail\n");
        value = 0;
    }
    
    __wrn("vale=%d\n", value);
    
    if(value == 0)
    {
        linux_output_type = DISP_OUTPUT_TYPE_NONE;
    }
    else if(value == 1)
    {
        linux_output_type = DISP_OUTPUT_TYPE_LCD;
    }
    else if(value == 2)
    {
        linux_output_type = DISP_OUTPUT_TYPE_TV;
    }
    else if(value == 3)
    {
        linux_output_type = DISP_OUTPUT_TYPE_HDMI;
    }
    else if(value == 4)
    {
        linux_output_type = DISP_OUTPUT_TYPE_VGA;
    }
    else
    {
        __wrn("invalid screen0_output_type %d\n", value);
        return -1;
    }
//screen0_output_mode    
    if(wBoot_script_parser_fetch("disp_init", "screen0_output_mode", &value, 1) < 0)
    {
        __wrn("fetch script data disp_init.screen0_output_mode fail\n");
        value = 0;
    }
    if(linux_output_type == DISP_OUTPUT_TYPE_TV || linux_output_type == DISP_OUTPUT_TYPE_HDMI)
    {
        linux_output_mode = (__disp_tv_mode_t)value;
    }
    else if(linux_output_type == DISP_OUTPUT_TYPE_VGA)
    {
        linux_output_mode = (__disp_vga_mode_t)value;
    }

    __wrn("vale=%d\n", value);

    switch(linux_output_type)
    {
        case DISP_OUTPUT_TYPE_LCD:
            {
                if(wBoot_script_parser_fetch("lcd0_para", "lcd_x", &value, 1) < 0)
                {
                    __wrn("fetch script data lcd0_para.lcd_x fail\n");
                    value = 0;
                }
                __wrn("vale=%d\n", value);
                *width = value;

                if(wBoot_script_parser_fetch("lcd0_para", "lcd_y", &value, 1) < 0)
                {
                    __wrn("fetch script data lcd0_para.lcd_y fail\n");
                    value = 0;
                }
                __wrn("vale=%d\n", value);
                *height = value;
            }
            break;
        case DISP_OUTPUT_TYPE_TV:
        case DISP_OUTPUT_TYPE_HDMI:
            *width = De_TvModetoWidth(linux_output_mode);
            *height = De_TvModetoHeight(linux_output_mode);
            break;
        case DISP_OUTPUT_TYPE_VGA:
            *width = De_VgaModetoWidth(linux_output_mode);
            *height = De_VgaModetoHeight(linux_output_mode);
            break;
        default:
            *width = 0;
            *height = 0;
            
    }

    return 0;
}

