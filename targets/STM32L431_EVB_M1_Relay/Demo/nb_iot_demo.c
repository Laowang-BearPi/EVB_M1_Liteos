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

msg_for_Relay Relay_send;

VOID nb_reply_task(VOID)
{
		while (1)
		{				                                               
				LOS_SemPend(reply_sem, LOS_WAIT_FOREVER);
				printf("This is reply_report_task!\n"); 
			
				HexStrToStr((const unsigned char*)s_resp_buf,(unsigned char*)Relay_send.Relay,strlen(s_resp_buf));
				los_nb_report((const char*)(&Relay_send), sizeof(Relay_send));		//发送数据到平台			
				memset(s_resp_buf, 0, sizeof(s_resp_buf));
		}
}

UINT32 creat_nb_reply_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
	  UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "nb_reply_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)nb_reply_task;
    task_init_param.uwStackSize = 0x1000;
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

		uwRet = LOS_SemCreate(0,&reply_sem);
		if (uwRet != LOS_OK)
    {
        return ;
    }				
		uwRet = creat_nb_reply_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
}
