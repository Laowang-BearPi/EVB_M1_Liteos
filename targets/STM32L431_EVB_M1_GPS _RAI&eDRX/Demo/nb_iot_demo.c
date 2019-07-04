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

#include "nb_iot_demo.h"
#include "sys_init.h"
#if defined USE_NB_NEUL95
#include "nb_iot/los_nb_api.h"
#include "at_device/bc95.h"
#endif

gps_msg             gpsmsg;
msg_for_GPS         GPS_send;
float Longitude;
float Latitude;
static unsigned char gps_uart[1000];

VOID data_collection_task(VOID)
{
	UINT32 uwRet = LOS_OK;
	
	MX_USART3_UART_Init();
  LOS_HwiCreate(USART3_IRQn, 7,0,USART3_IRQHandler,NULL);	//初始化传感器
	OLED_Clear();
	OLED_ShowString(30,1,(uint8_t *)"Report",16);
	while (1)
  {

		printf("This is data_collection_task !\r\n");
		
	  HAL_UART_Receive_IT(&huart3,gps_uart,1000);
		NMEA_BDS_GPRMC_Analysis(&gpsmsg,(uint8_t*)gps_uart);	//分析字符串
		Longitude=(float)((float)gpsmsg.longitude_bd/100000);	
		printf("Longitude:%.5f %lc     \r\n",Longitude,gpsmsg.ewhemi_bd);
		Latitude=(float)((float)gpsmsg.latitude_bd/100000);
		printf("Latitude:%.5f %1c   \r\n",Latitude,gpsmsg.nshemi_bd);	

		uwRet=LOS_TaskDelay(1000);
		if(uwRet !=LOS_OK)
		return;
	
  }
}
UINT32 creat_data_collection_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
		UINT32 TskHandle;
    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "data_collection_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_collection_task;
    task_init_param.uwStackSize = 0x800;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

VOID data_report_task(VOID)
{
	UINT32 uwRet = LOS_OK;	
	nb_set_eDRX("0111","0010");                                           //设置PTW和eDRX周期
	while(1)
	{
		
		  if(Latitude!=0&&Longitude!=0)
		{
			memset(GPS_send.Latitude, 0, 8);
			memset(GPS_send.Longitude, 0, 9);
			sprintf(GPS_send.Latitude, "%.5f", Latitude);
			sprintf(GPS_send.Longitude, "%.5f", Longitude);		
			if(los_nb_report((const char*)(&GPS_send),sizeof(GPS_send))>=0)		//发送数据到平台	
				printf("ocean_send_data OK!\n");                                            //发生成功
		else                                                                            //发送失败
			{
				printf("ocean_send_data Fail!\n"); 
			}
			gpsmsg.longitude_bd=0;
			gpsmsg.latitude_bd=0;
		HAL_GPIO_WritePin(GPS_EN_GPIO_Port,GPS_EN_Pin,GPIO_PIN_SET);    // 输出高电平
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
	  UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "data_report_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)data_report_task;
    task_init_param.uwStackSize = 0x400;
    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}



void nb_iot_entry(void)
{
    UINT32 uwRet = LOS_OK;

    uwRet = creat_data_collection_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
		
		uwRet = creat_data_report_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
}
