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

#ifndef __NB_IOT_DEMO_H_
#define __NB_IOT_DEMO_H_

#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"

/*temperature and humidity message*/
typedef struct
{
	char temp[4];
	char hum[4];	
}msg_for_DHT11;
extern msg_for_DHT11 DHT11_send;

/***********smoke message********/
typedef struct
{
	char index;
	unsigned char CSQ;
	char Value[4];
	char MaxValue[4];		
}msg_for_SMOKE;
extern msg_for_SMOKE SMOKE_send;

/**********GPS message***********/
typedef struct
{
	char Latitude[8];
	char Longitude[9];	
}msg_for_GPS;
extern msg_for_GPS GPS_send;

/*illumination intensity message*/
typedef struct
{
	char Lux[5];
}msg_for_BH1750;
extern msg_for_BH1750 BH1750_send;

/*CH2O message*/
typedef struct
{
	char ch2o[7];
}msg_for_CH2O;
extern msg_for_CH2O CH2O_send;


void nb_iot_entry(void);

#endif 

