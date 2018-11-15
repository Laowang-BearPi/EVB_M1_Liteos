/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
/* Includes LiteOS------------------------------------------------------------------*/
#include "los_base.h"
#include "los_sys.h"
#include "los_typedef.h"
#include "los_task.ph"
#include "sys_init.h"

#if defined WITH_AT_FRAMEWORK
#include "at_frame/at_api.h"
#if defined USE_NB_NEUL95
#include "nb_iot/los_nb_api.h"
#include "at_device/bc95.h"
#endif
#endif



UINT32 g_TskHandle;
msg_sys_type bc95_net_data;
msg_for_BH1750      BH1750_send;
char messageid[4];
char mid[4];
char value[6];
char Switch[3];
char ack_messageid[2]="05";
char ack_errcode[2]="00";
typedef struct
{
  char messageid[1];
  char errcode[1];
	char mid[4];
}ack;
ack switch_ack={04};



VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	dwt_delay_init(SystemCoreClock);
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	printf("Welcome to IoT-Club, This is EVB-M1 Board.\r\n");

	
}


void user_hw_init(void)
{
	char *str = "     EVB_M1";
	OLED_Init();
	OLED_Clear();
	OLED_ShowCHinese(0+9,0,0);
	OLED_ShowCHinese(18+9,0,1);
	OLED_ShowCHinese(36+9,0,2);
	OLED_ShowCHinese(54+9,0,3);
	OLED_ShowCHinese(72+9,0,4);
	OLED_ShowCHinese(90+9,0,5);
	OLED_ShowString(0,2,(uint8_t*)str,16);
}

int32_t nb_cmd_data_ioctl(void* arg, int8_t  * buf, int32_t len)
{
		int readlen = 0;
		char tmpbuf[1064] = {0};
    if (NULL == buf || len <= 0)
    {
        AT_LOG("param invailed!");
        return -1;
    }
				sscanf((char *)buf,"\r\n+NNMI:%d,%s\r\n",&readlen,tmpbuf);
				memset(bc95_net_data.net_nmgr, 0, 30);
    if (readlen > 0)
    {  
				HexStrToStr(tmpbuf,  bc95_net_data.net_nmgr,readlen*2);
    }
				AT_LOG("cmd is:%s\n",bc95_net_data.net_nmgr);
		if(strcmp(bc95_net_data.net_nmgr,"ON")==0) //开灯
			{	
					HAL_GPIO_WritePin(Light_GPIO_Port,Light_Pin,GPIO_PIN_RESET);    // 输出低电平
			}
		if(strcmp(bc95_net_data.net_nmgr,"OFF")==0) //关灯
			{	
					HAL_GPIO_WritePin(Light_GPIO_Port,Light_Pin,GPIO_PIN_SET);  // 输出高电平
			}
/*******************************END**********************************************/
		return 0;
}

int32_t OC_cmd_match(const char *buf, char* featurestr,int len)
{

    if(strstr(buf,featurestr) != NULL)
        return 0;
    else
        return -1;
}


VOID data_collection_task(VOID)
{
	UINT32 uwRet = LOS_OK;
	
	short int Lux;   
	user_hw_init();
	Init_BH1750();									//初始化传感器
	
	while (1)
  {

		printf("This is data_collection_task !\r\n");
		Lux=(int)Convert_BH1750();		//采集传感器数据
	  printf("\r\n******************************BH1750 Value is  %d\r\n",Lux);
		
		sprintf(BH1750_send.Lux, "%5d", Lux);	  //将传感器数据存入发送数据的结构体中

		uwRet=LOS_TaskDelay(1000);
		if(uwRet !=LOS_OK)
		return;
	
  }
}
UINT32 creat_data_collection_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "data_collection_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_collection_task;
    task_init_param.uwStackSize = 0x800;

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

VOID data_report_task(VOID)
{
	UINT32 uwRet = LOS_OK;
	
	#define AT_DTLS 0
#if AT_DTLS
    sec_param_s sec;
    sec.pskid = "868744031131026";
    sec.psk = "d1e1be0c05ac5b8c78ce196412f0cdb0";
#endif
    printf("\r\n=====================================================");
    printf("\r\nSTEP1: Init NB Module( NB Init )");
    printf("\r\n=====================================================\r\n");
#if AT_DTLS
    los_nb_init((const int8_t*)"139.159.140.34",(const int8_t*)"5684",&sec);
#else
    los_nb_init((const int8_t*)"139.159.140.34",(const int8_t*)"5683",NULL);
#endif
    printf("\r\n=====================================================");
    printf("\r\nSTEP2: Register Command( NB Notify )");
    printf("\r\n=====================================================\r\n");
    los_nb_notify("+NNMI:",strlen("+NNMI:"),nb_cmd_data_ioctl,OC_cmd_match);
    LOS_TaskDelay(3000);
    printf("\r\n=====================================================");
    printf("\r\nSTEP3: Report Data to Server( NB Report )");
    printf("\r\n=====================================================\r\n");
		while(1)
	{

		if(los_nb_report((const char*)(&BH1750_send),sizeof(BH1750_send))>=0)		//发送数据到平台	
				printf("ocean_send_data OK!\n");                                                  //发生成功
		else                                                                                  //发送失败
			{
				printf("ocean_send_data Fail!\n"); 
			}

	  uwRet=LOS_TaskDelay(1000);
		if(uwRet !=LOS_OK)
		return;
	}
}
UINT32 creat_data_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "data_report_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_report_task;
    task_init_param.uwStackSize = 0x400;
    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
	
    HardWare_Init();
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
		
    uwRet = creat_data_collection_task();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
		
		uwRet = creat_data_report_task();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }

    LOS_Start();
}


