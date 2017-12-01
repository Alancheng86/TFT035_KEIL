#ifndef	__TSLIIC_h__
#define	__TSLIIC_h__



#include "stm32f10x_lib.h"
//#include "platform_config.h"
//#include "stm32f10x_gpio.h"

//============================
 #define PIN_SCL  GPIO_Pin_5
#define PIN_SDA  GPIO_Pin_3
#define GPIO_Sensor                               GPIOA
#define RCC_APB2Periph_GPIO_TSLIIC RCC_APB2Periph_GPIOA
//------------------------------------------------------------
#define TSCL_L GPIO_ResetBits(GPIO_Sensor, PIN_SCL)  //SCL
#define TSCL_H GPIO_SetBits(GPIO_Sensor, PIN_SCL)
#define TSDA_L GPIO_ResetBits(GPIO_Sensor, PIN_SDA)  //SDA
#define TSDA_H GPIO_SetBits(GPIO_Sensor, PIN_SDA)

#define S1_addr 0X92
#define S2_addr 0X72
#define S3_addr 0X52 

#define ID_REG		0x1e		//////ID reg

#define	ID_TSL2581	0x00		////TSL2581
#define	ID_TSL2583	0x30		////TSL2583

unsigned short GET_FLICKER(void);

unsigned char sensor_check (unsigned char Device_N);
void TSL2561Init(unsigned char Device_N);
unsigned int GetBright(unsigned char Device_N);
unsigned int GetBright2(unsigned char Device_N);
void Sensor_GPIO_Config(void);


//#define PIN_SCL1  GPIO_Pin_5
//#define PIN_SDA1  GPIO_Pin_6
//#define GPIO_EEPROM                                  GPIOA
//#define RCC_APB2Periph_GPIO_EEPROM    RCC_APB2Periph_GPIOA
//
//#define ESCL_L GPIO_ResetBits(GPIO_EEPROM, PIN_SCL1)  //SCL
//#define ESCL_H GPIO_SetBits(GPIO_EEPROM, PIN_SCL1)
//#define ESDA_L GPIO_ResetBits(GPIO_EEPROM, PIN_SDA1)  //SDA
//#define ESDA_H GPIO_SetBits(GPIO_EEPROM, PIN_SDA1)


//--------------------------------------------------------------
#endif
