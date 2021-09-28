#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //rand();
#include "delay.h"
#include "lcd.h"

#define I2C_SPEED 50000 //50Khz speed for I2C
#define I2C1_SLAVE_ADDRESS7 0x48 //Adresa for tmp102
#define I2C1_SLAVE2_ADDRESS7 0x00 //Addresa pentru TC
//Definitii
GPIO_InitTypeDef GPIO_InitStructure;
int temp;



void initGPIO()
{
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //pp output - push-pull .
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
}



void I2C_Setup(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /*enable I2C*/
    I2C_Cmd(I2C1,ENABLE);

    /* I2C1 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* I2C1 SDA and SCL configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /*SCL is pin06 and SDA is pin 07 for I2C1*/

    /* I2C1 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED ;
    I2C_Init(I2C1, &I2C_InitStructure);

}

void init_TMP102(void)
{

    /* initiate start sequence */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* check start bit flag */
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
    /*send write command to chip*/
    I2C_Send7bitAddress(I2C1, I2C1_SLAVE_ADDRESS7<<1, I2C_Direction_Transmitter);
    /*check master is now in Tx mode*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /*mode register address*/
    I2C_SendData(I2C1, 0x02);
    /*wait for byte send to complete*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /*clear bits*/
    I2C_SendData(I2C1, 0x00);
    /*wait for byte send to complete*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /*generate stop*/
    I2C_GenerateSTOP(I2C1, ENABLE);
    /*stop bit flag*/
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));

}


void init_TC74(void)
{

	//To DO: initializeaza senzorul TC74, conform exemplului de mai sus. 	
	
		/* initiate start sequence */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* check start bit flag */
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
    /*send write command to chip*/
    I2C_Send7bitAddress(I2C1, I2C1_SLAVE_ADDRESS7<<1, I2C_Direction_Transmitter);
    /*check master is now in Tx mode*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /*mode register address*/
    I2C_SendData(I2C1, 0x02);
    /*wait for byte send to complete*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /*clear bits*/
    I2C_SendData(I2C1, 0x00);
    /*wait for byte send to complete*/
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    /*generate stop*/
    I2C_GenerateSTOP(I2C1, ENABLE);
    /*stop bit flag*/
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
}


float Receive(u8 Address, u8 Register)
{
		float tempCelsius;
    u8 XMSB,XLSB; /* variables to store temporary values in */

    /*left align address*/
    Address = Address<<1;
    /*re-enable ACK bit incase it was disabled last call*/
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    /* Test on BUSY Flag */
    while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
    /* Enable the I2C peripheral */
/*======================================================*/
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test on start flag */
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB));
    /* Send device address for write */
    I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Transmitter);
    /* Test on master Flag */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /* Send the device's internal address to write to */
    I2C_SendData(I2C1,Register);
    /* Test on TXE FLag (data sent) */
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
/*=====================================================*/
      /* Send START condition a second time (Re-Start) */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test start flag */
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB));
    /* Send address for read */
    I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Receiver);
    /* Test Receive mode Flag */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    /* load in all 6 registers */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    XMSB = I2C_ReceiveData(I2C1);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    XLSB = I2C_ReceiveData(I2C1);
    
    
    /*enable NACK bit */
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)); // stop bit flag
    
    /*sort into 3 global variables*/
    temp = (((XMSB<<8) | XLSB)>>4);

		tempCelsius=temp*0.0625;
		
		return tempCelsius;
}


int Receive_TC74(u8 Address, u8 Register)
{

    /* variables to store temporary values in */
		//To do :
		//Pe baza exemplului de mai sus, cititi temperatura de la senzorul TC74. 
		//Nota: Valoarea returnata de acest senzor este intreaga. 
	
		int tempCelsius;
    u8 XMSB,XLSB; /* variables to store temporary values in */

    /*left align address*/
    Address = Address<<1;
    /*re-enable ACK bit incase it was disabled last call*/
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    /* Test on BUSY Flag */
    while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
    /* Enable the I2C peripheral */
/*======================================================*/
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test on start flag */
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB));
    /* Send device address for write */
    I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Transmitter);
    /* Test on master Flag */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    /* Send the device's internal address to write to */
    I2C_SendData(I2C1,Register);
    /* Test on TXE FLag (data sent) */
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
/*=====================================================*/
      /* Send START condition a second time (Re-Start) */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test start flag */
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB));
    /* Send address for read */
    I2C_Send7bitAddress(I2C1, Address, I2C_Direction_Receiver);
    /* Test Receive mode Flag */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    /* load in all 6 registers */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    XMSB = I2C_ReceiveData(I2C1);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    XLSB = I2C_ReceiveData(I2C1);
    
    
    /*enable NACK bit */
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)); // stop bit flag
    
    /*sort into 3 global variables*/
    temp = (((XMSB<<8) | XLSB)>>4);

		tempCelsius=temp*0.0625;
		
		
		return tempCelsius;
}




int main()
{		
		uint8_t i;
		char s[70];
		float tCel1=0.0;
		int tCel3=0;

		initGPIO();
		I2C_Setup();
		DelayInit();
		init_TMP102();
		lcd16x2_init(LCD16X2_DISPLAY_ON_CURSOR_OFF_BLINK_OFF);


		

		while(1)
		{
			tCel1=Receive(I2C1_SLAVE_ADDRESS7, 0x00);
					
			
			sprintf(s,"T: %.2f grade \n\n\n",tCel1);
			lcd16x2_clrscr();
			lcd16x2_gotoxy(0, 0);
			lcd16x2_puts(s);

			DelayMs(1000);
			
		}
}
