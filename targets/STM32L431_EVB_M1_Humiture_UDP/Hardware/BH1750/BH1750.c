/********************************************************************************
    * �ļ����� ��BH1750.c
    * ��     �ߣ����������ֲ�
    * ��     ����V1.0
    * ��д���� ��2018-4-1
    * ��     �ܣ�EVB_M/H ����ǿ����չ������
*********************************************************************************
    * ˵    �� ���������������������ֲ�EVB_M1������ʹ��
    *
    * ��     ����https://shop128001708.taobao.com/
    * ��     ̳��bbs.iot-club.cn
*********************************************************************************/

#include "bh1750.h"
#include "stm32l4xx.h"
//#include "dwt.h"

float result_lx=0;
uint8_t BUF[2]={0};
uint16_t result=0;

static void delayus(uint16_t time)
{
	uint8_t i;

  while(time)
  {
	  for (i = 0; i < 10; i++)
    {

    }
    time--;
  }
}

/**
  * ��������: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void I2C_InitGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��GPIOʱ�� */
	I2C_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = I2C_SCL_PIN|I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH ;
  HAL_GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStruct);

	I2C_SCL(HIGH); 
	I2C_SDA(HIGH); 
}

/**
  * ��������: CPU����I2C���������ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	SDA_OUT();
	I2C_SDA(HIGH); 
	I2C_SCL(HIGH); 
	delayus(4);
	I2C_SDA(LOW); 
	delayus(4);
	I2C_SCL(LOW); 
}

/**
  * ��������: CPU����I2C����ֹͣ�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	SDA_OUT();
	I2C_SCL(LOW); 
	I2C_SDA(LOW); 
	delayus(4);
	I2C_SCL(HIGH); 	
	I2C_SDA(HIGH); 
	delayus(4);
}

/**
  * ��������: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
  * �������: ��
  * �� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
  * ˵    ������
  */
uint8_t I2C_WaitAck(void)
{
  uint8_t ucErrTime=0;
	SDA_IN(); 
	I2C_SDA(HIGH); 	/* CPU�ͷ�SDA���� */
	delayus(4);
	I2C_SCL(HIGH); 	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	delayus(4);
	while (I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return 1;
		}
	}
	
	I2C_SCL(LOW); 
	return 0;
}

/**
  * ��������: CPU����һ��ACK�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_Ack(void)
{

	I2C_SCL(LOW); 
	SDA_OUT(); 
	I2C_SDA(LOW); 	/* CPU����SDA = 0 */
	delayus(2);
	I2C_SCL(HIGH); 	/* CPU����1��ʱ�� */
	delayus(2);
	I2C_SCL(LOW); 
}

/**
  * ��������: CPU����1��NACK�ź�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_NAck(void)
{
	
	I2C_SCL(LOW); 
	SDA_OUT();
	I2C_SDA(HIGH); 	/* CPU����SDA = 1 */
	delayus(2);
	I2C_SCL(HIGH); 	/* CPU����1��ʱ�� */
	delayus(2);
	I2C_SCL(LOW); 
}

/**
  * ��������: CPU��I2C�����豸����8bit����
  * �������: Byte �� �ȴ����͵��ֽ�
  * �� �� ֵ: ��
  * ˵    ������
  */
void I2C_SendByte(uint8_t Byte)
{
  uint8_t t;  
      
    SDA_OUT();                                                      //sda???  
    I2C_SCL(LOW);                                                   //??????????  
    for (t = 0; t < 8; t++)  
    {  
        I2C_SDA((Byte & 0x80) >> 7);  
        Byte <<= 1;  
        delayus(2);  
        I2C_SCL(HIGH);  
        delayus(2);  
        I2C_SCL(LOW);  
        delayus(2);  
    } 
}


/**
  * ��������: CPU��I2C�����豸��ȡ8bit����
  * �������: ��
  * �� �� ֵ: ����������
  * ˵    ������
  */
uint8_t I2C_ReadByte(uint8_t ack)
{
	uint8_t i;
	uint8_t value;
  SDA_IN();
	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
for (i = 0; i < 8; i++)  
    {  
        I2C_SCL(LOW);  
        delayus(2);  
        I2C_SCL(HIGH);  
        value <<= 1;  
        if (I2C_SDA_READ() ) value++;  
        delayus(1);  
    }  
		if (!ack)
        I2C_NAck();//����nACK
    else
        I2C_Ack(); //����ACK  
	return value;
}


/**
  * ��������: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
  * �������: _Address���豸��I2C���ߵ�ַ
  * �� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
  * ˵    �����ڷ���I2C�豸ǰ�����ȵ��� I2C_CheckDevice() ���I2C�豸�Ƿ��������ú���������GPIO
  */
uint8_t I2C_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	I2C_InitGPIO();		/* ����GPIO */	
	I2C_Start();		/* ���������ź� */
	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	I2C_SendByte(_Address | I2C_WR);
	ucAck = I2C_WaitAck();	/* ����豸��ACKӦ�� */
	I2C_Stop();			/* ����ֹͣ�ź� */
	return ucAck;
}

void Cmd_Write_BH1750(uint8_t cmd)
{
    I2C_Start();                  //��ʼ�ź�
    I2C_SendByte(BH1750_Addr+0);   //�����豸��ַ+д�ź�
	while(I2C_WaitAck());
	delayus(100);
    I2C_SendByte(cmd);    //�ڲ��Ĵ�����ַ
	while(I2C_WaitAck());
	delayus(100);
    I2C_Stop();                   //����ֹͣ�ź�
	LOS_TaskDelay(5);
}
void Init_BH1750(void)
{
	I2C_InitGPIO();	
	Cmd_Write_BH1750(0x01);
}
void Start_BH1750(void)
{
	Cmd_Write_BH1750(BH1750_ON);	 //power on
	Cmd_Write_BH1750(BH1750_CON);
}
void Read_BH1750(void)
{   uint8_t i;	
    I2C_Start();                          //��ʼ�ź�
    I2C_SendByte(BH1750_Addr+1);         //�����豸��ַ+���ź�
	while(I2C_WaitAck());
	for (i=0; i<3; i++)                      //��?D??����?6??��??����y?Y��?��?���?DBUF
    {     
        if (i ==3)
        {
        BUF[i]=I2C_ReadByte(0); 
                   
        }
        else
        {	BUF[i]=I2C_ReadByte(1);  //����ACK	
         
       }
   }
	
	 //����NACK

    I2C_Stop();                          //ֹͣ�ź�
    LOS_TaskDelay(5);
}
float Convert_BH1750(void)
{
	Start_BH1750();
	LOS_TaskDelay(180);
	Read_BH1750();
	result=BUF[0];
	result=(result<<8)+BUF[1];  //�ϳ����ݣ�����������	
	result_lx=(float)(result/1.2);
  return result_lx;
}


