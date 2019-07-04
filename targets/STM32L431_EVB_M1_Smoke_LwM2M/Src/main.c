
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sys_init.h"

/* Includes LiteOS------------------------------------------------------------------*/
#include "los_base.h"
#include "los_sys.h"
#include "los_typedef.h"
#include "los_task.ph"

#if defined WITH_AT_FRAMEWORK
#include "at_frame/at_api.h"
#if defined USE_NB_NEUL95
#include "nb_iot/los_nb_api.h"
#include "at_device/bc95.h"
#include "nb_iot_demo.h"
#include "nb_iot_cmd_ioctl.h"
#endif
#endif


UINT32 g_TskHandle;

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	printf("Welcome to IoT-CluB, This is EVB-M1 Board.\r\n");
}


VOID main_task(VOID)
{	
	
#if defined USE_NB_NEUL95
#define AT_DTLS 0
#if AT_DTLS
    sec_param_s sec;
    sec.pskid = "869505046440341";
    sec.psk = "15ac615293a0761f9c3c8db7e825dec5";
#endif
    printf("\r\n=====================================================");
    printf("\r\nSTEP1: Init NB Module( NB Init )");
    printf("\r\n=====================================================\r\n");
#if AT_DTLS
    los_nb_init((const int8_t*)"49.4.85.232",(const int8_t*)"5684",&sec);
#else
    los_nb_init((const int8_t*)"49.4.85.232",(const int8_t*)"5683",NULL);   //IP需要改成与自己平台对应的
#endif
    printf("\r\n=====================================================");
    printf("\r\nSTEP2: Register Command( NB Notify )");
    printf("\r\n=====================================================\r\n");
    los_nb_notify("+NNMI:",strlen("+NNMI:"),nb_cmd_data_ioctl,OC_cmd_match);
    LOS_TaskDelay(3000);
    printf("\r\n=====================================================");
    printf("\r\nSTEP3: Report Data to Server( NB Report )");
    printf("\r\n=====================================================\r\n");
		
    nb_iot_entry();//   案例程序入口
		
#endif
}

UINT32 creat_main_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "main_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)main_task;
    task_init_param.uwStackSize = 0x1000;

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
			
    uwRet = creat_main_task();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }

    LOS_Start();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
