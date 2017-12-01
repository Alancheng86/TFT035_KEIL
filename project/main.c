/******************** (C) COPYRIGHT 2010 GIANTPLUS ********************
* File Name          : main.c
* Author             : Jason
* Version            : V1.0.0
* Date               : 2011.01.20
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "platform_config.h"
#include <stdio.h>
#include "FAT.h"
#include "sdcard.h"
#include "TSLIIC.h"
#include "lcd.h"


GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
static vu32 TimingDelay = 0;
extern BYTE FAT32_Enable; 
extern WORD SectorsPerClust;
extern u16 image_count ;
extern u16 image_all ;
SD_Error Status = SD_OK;
SD_CardInfo SDCardInfo;
u32  sd_Capacity;
SD_Error SD_InitAndConfig(void);
void RCC_Configuration(void);
void InterruptConfig(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);
void Decrement_TimingDelay(void);
void SysTick_Config(void);
void USART_Configuration(void);
void USART1GPIO_Configuration(void); 
void check_power(void);
//----------------以上内容不需要修改，详情参阅readme.txt---------------

//----------------------------------------
void DelayKEY (u32 k); //按住，画面暂停
void KEYGPIO_Init(void);///*[把PA78配置成输入模式] */
#define KEYA7      GPIOA->IDR&GPIO_Pin_7
#define KEYA8      GPIOA->IDR&GPIO_Pin_8
#define KEYC6      GPIOC->IDR&GPIO_Pin_6
u8 cfont[5]={13,14,15,16,'\0'};	 //vcom
u8 cfont1[4]={17,19,20,'\0'};	 //已烧入
u8 cfont2[4]={18,19,20,'\0'};	 //未烧入
//============================== *** =============================
//#include "PICC1.h"   //该芯片内部Flash容量为512KB，可保存小幅图片数据


//for  SD use only
extern u8 FontR,FontG,FontB;
u8   SDConfigFlag = 0;
u8   SDShowFlag=1; 
u8   SDShowTimes = 1;
void SDPicShow(u8 pic);

//----------------------------------------------------------------------------==

/*******************************************************************************/


void I2C_DATAL_COMPARE(void);
void I2C_MEMSURE1(int kk, int vcomdcdata );
void I2C_MEMSURE()	;
void I2C_DATAL_COMPAREX(void);
void I2C_DATAL_COMPARE(void);


u16 DATA_SUM1[30],DATA_SUMY[30],count1=0,count2=0;
u16 SURE[30];
u16 vcomdc_value=0;
//u8 VCOMDC = 0X19;	  /////
//u8 VCOMDC = 0X19;	  /////	2013-12-13	  1D
u8 VCOMDC=0x15;				   //////GPM1461A0_D  use
u8 VCOM1=0x15;	
u16 aat=0,aat2=1;  


#define YDP 240	   //对应0XB0使用0x30
#define XDP 480  

u16 VCOMDC1=0x0000;
u8 STEP=24;  //采样数组上限
//u8 cfonts[]={1,2,3,4,5,6,2,7,8,'\0'};
//u8 cfont[]={9,10,11,'\0'}; //二合一
void MTP(void);
u8 OTP_VALUE1 = 0;
u8 OTP_VALUE2 = 0;
u8 OTP_TIMES = 0;
u8 OTP_TIMES_BEFORE=0;
void MTP_ID(void);
u8 ID_OK = 1;
u8 ID_CHECK(u16 adx,u16 ady);
u8 ID_CHECK_A(u16 adx,u16 ady);
void OTP_ERROR(void);
void OTP_NO(void);
void check_power(void);
		  
void SHOW_value(u16 addx,u16 addy,u32 Data);
void SHOW_IC_VALUE(u16 adx,u16 ady,u32 cmd);
void SHOW_IC_VALUE_A(u16 adx,u16 n,u8 data) ;
u8 MIPI_READ_DATA[10] = {0,0,0,0,0,0,0,0,0,0};


u8 temp=1;
u8 READ_ID = 0;
void showid_vcom_A(void);

u8 VCOM_GET(void);

void KEY_adjust(void);
void showid_vcom(void);
/*******************************************************************************/
int main(void)
{   // uint testf;		//only for test 2014.1.27
//	u8 mm=1;
//	u32 rand_val;
	u32 sdcap=1;
	u8 READ_ID = 0;
	u16 mm_KEYB1 = 1;

//	u8 temp[9];

  #ifdef DEBUG
  debug();
  #endif
  /* System Clocks Configuration */
	RCC_Configuration();	 	
  /* NVIC Configuration */
	NVIC_Configuration();   //中断管理初始化   
	SysTick_Config();		    //延时用  	
	  
  //-------------------------------------------------
  /* Initialize the LCD */
	STM32_SSD1963_Init();

	KEYGPIO_Init();
	Sensor_GPIO_Config();
	

	
///SD  卡初始化部分。
	if(SDConfigFlag == 0)
	{
		SDShowFlag=1;
	
		Status =SD_InitAndConfig();  /*设置SD接口,初始化SD卡*/
		sdcap=sd_Capacity>>20; //(sdcap=sd_Capacity/1024/1024)
		All_Color(255,255,255)   ;

			if(sdcap == 0)
			{
				FontR = 255;
				FontB = 20;
				FontG = 20;
//				LCD_DisplayStringLine_A(0,Line0,"SD ERROR!");	

				FontR = FontG = FontB = 255;		//屏蔽部分字符时候使用此段；
//				while(1)
//				{
//					DelayKEY(100);
//				}
			}
			else
			{
				FontR = FontG = FontB = 255;		//屏蔽部分字符时候使用此段；
				SDConfigFlag = 1;
					
			}
	}


	FontR = FontG = FontB = 255;			   //屏蔽字符
	  	if (FAT_Init())	//初始化FAT文件系统
		  {	
		  		All_Color(255,255,255)   ;
				FontR = 255;
				FontB = 20;
				FontG = 20;
//				LCD_DisplayStringLine_A(0,Line0,"SD ERROR!");
//				while(1)
				{
//					DelayKEY(100);
				}
    
		  }
          FontR = FontG = FontB = 255;		//屏蔽部分字符时候使用此段；
		LCD_DisplayStringLine(0,Line2,"Waiting....");
		
			SearchInit();

		//////////////////show the project ----------------------------------------------------
		




	     
	  	FontR = FontG = FontB = 0;
		LCD_DisplayStringLine_A(30,160,"TFT9K2353_A");	  /////2014/4/4   /////2014/4/4  M1461A0_C
//		LCD_DisplayStringLine_A(30,360,"FGA7_20160426");
		DelayKEY(100);
//		VCOM_GET();
		
		
	FontR = FontG = FontB = 255;			   //屏蔽字符
	 

		SDShowTimes = 0;

		while (SDShowFlag) //开始BMP文件显示
		{	  
		 Disp_BMP() ;
		  Delay(1);
		  SDShowFlag = 0;	
		}				   
	
//	ID_CHECK_A(10,100);  /////check ID ,if ID NG,,while(1);  if ID ok,DO nothing..	
	
	FontR = FontG = FontB = 0;			   //显示字符


	 
		
//	sdcap = KEYA7;


////	while(sdcap!=0)
//	
//	{sdcap = KEYA7;}

//		 
//		   if((sdcap==0)||(OTP_TIMES_BEFORE<=0))
//		   {	
//		   		Flicker_sub_pixel() ;	
//			   ////////識別光感板是否放置到預定位置。
//					sdcap = KEYA8;
//					while(sdcap != 0)	
//					{	
//						LCD_DisplayStringLine(20,100,"Adjust sensor");	
//						Delay(1);
//						sdcap = KEYA8;	
//					}

//				Flicker_sub_pixel() ;
//				LCD_WriteArea(3,20,10,790,255,0,0);		   ////start auto otp flow
//		   		I2C_DATAL_COMPAREX();
//				LCD_WriteArea(3,20,10,790,0,255,0);			  ////to show the best flicker。。。
//				DelayKEY(1);
//				Flicker_sub_pixel();	
//				DelayKEY(1);
//				if(DATA_SUM1[count2] <= 350)			 ///for check OTP is OK or NG....		 96
//				{
//						if(DATA_SUM1[count2] >= 5)
//						{
//							DelayKEY(1);
//							sdcap = 1;
//							LCD_CS(1);
//							Delay(10);	
//							sdcap = KEYA7;
////						    check_power();	   	 	Delay(150);	
////							sdcap = 0;
////							if(sdcap == 0)
//						    {
//								
//								aat = 1;
//								MTP_ID();
//								FontR = FontG = FontB = 0;
//					
////								Soft_reset();
//					//			OTP_FLAG = 0;
//								aat = 0;
//							}
//							LCD_CS(0);
//							Delay(10);
//							Soft_reset();
//						    Flicker_sub_pixel();								
//							FontR = 50; FontG = 255; FontB = 50;

//							VCOM_GET();
//							sdcap= MIPI_READ_DATA[4];
//							if(sdcap != VCOMDC1)
//							{
//								OTP_ERROR(); while(1);
//							}
//							else
//							{
//								LCD_DisplayStringLine(20,100,"Flicker OTP ok");		
//							}
//							FontR = 255; FontG = 0; FontB = 0;
//							LCD_DisplayStringLine(20,130,"Check flicker again!"); 	DelayKEY(100);
//							
//							FontR = 0; FontG = 0; FontB = 0;
//							
//						//	while(1);

//						}
//						else	   //////////if VCOM value not found perfect,goto error
//						{
//							OTP_ERROR(); while(1);
//						}
//				}
//				else	   //////////if VCOM value not found perfect,goto error
//				{
//					OTP_ERROR(); while(1);
//				}
//				
//				
//		   }
//	else
//	{
//		Soft_reset();    ///////复位后使用客户初始化code，以便拦检OTP漏烧产品。
//	}
//	KEY_adjust();
//	Flicker_PIXEL();
//	showid_vcom()	;								Delay(150);			mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }


LCDTest:
	
		   	All_Color(255,0,0)   ; 							DelayKEY(100);
//
/////////*-------enter sleep mode------- */ 
//////  	enter_sleep_mode( ) ;
//////	DelayKEY(250);
//////	exit_sleep_mode( );
//////*-------exit sleep mode------- */
////
  	All_Color(0,255,0)   ;					  		DelayKEY(100);
	All_Color(0,0,255)   ;					  		DelayKEY(100);
	All_Color(255,255,255)   ;				  	DelayKEY(100);
	LCD_WriteArea(1,XDP-2,1,YDP-2,0,0,0);		DelayKEY(100);
	All_Color(0,0,0)   ;				  		DelayKEY(100);
    All_Color(60,60,60)   ;				  		DelayKEY(100);
	All_Color(127,127,127)   ;				  		DelayKEY(100);
    RGB_color();                                    DelayKEY(100);
    Flicker_PIXEL();                                DelayKEY(100);
	SDPicShow(1);				 			   		DelayKEY(80);	


		   
//	RGB_color();									
//	LCD_DisplayStringLine(40,820,"RED");	 LCD_DisplayStringLine(140,820,"GREEN");		LCD_DisplayStringLine(265,820,"BLUE");		  LCD_DisplayStringLine(380,820,"WHITE");
//	DelayKEY(80);															mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
//	SDPicShow(2);				 			   		Delay(150);			  mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }//SD  1   pic 
//	All_Color(255,0,0)   ; 							Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
//	All_Color(0,255,0)   ;					  		Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
//	All_Color(0,0,255)   ;					  		Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
//	All_Color(255,255,255)   ;					   
//	LCD_WriteArea(147,147,169,169,160,255,255);				   ////檢測暗點	   R160,G210,B0
//	LCD_WriteArea(231,231,169,169,255,210,255);
//	LCD_WriteArea(311,311,169,169,255,255,0);			  	Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
//	All_Color(0,0,0)   ;							
//	LCD_WriteArea(147,147,169,169,53,0,0);				   ////檢測亮點		 R49,G29,B69
//	LCD_WriteArea(231,231,169,169,0,35,0);
//	LCD_WriteArea(311,311,169,169,0,0,89);					  		Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
// 	SDPicShow(1);				 			   		Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   } //SD  1   pic
//	Flicker_PIXEL();								Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
// 	showid_vcom()	;								Delay(150);			 mm_KEYB1=KEYC6;	 while(mm_KEYB1 != 0)	{	mm_KEYB1=KEYC6;   }
  goto LCDTest;

}

























void check_power(void)
{
	  	ENTER_LP_mode(); //enter  LP mode

	 	MIPI_SPI_Write(0x03,0x39,0x00,0x00);
		MIPI_SPI_Write(0x05,0x39,0xFF,0x80,0x09,0x01);
		
		MIPI_SPI_Write(0x03,0x39,0x00,0x80);
		MIPI_SPI_Write(0x04,0x39,0xFF,0x80,0x09);

		MIPI_SPI_Write(0x03,0x39,0x00,0x00);
		MIPI_SPI_Write(0x03,0x39,0xD0,0xD5);

		READ_IC(0XD3,0x00);

   
 
		if(MIPI_READ_DATA[0] == 0x03)
		{
			ID_OK = 0;
			FontR = 0; FontG = 255; FontB = 0;
		//		LCD_DisplayStringLine_A( 10 ,800,"otp power OK");
		}
		else
		{
			ID_OK = 1;
			FontR = 255; FontG = 0; FontB = 0;
			LCD_DisplayStringLine(100,700,"otp  power");
			SHOW_IC_VALUE_A(300,700,MIPI_READ_DATA[0]);
			LCD_DisplayStringLine_A( 50 ,800,"otp power NG");
			while(1);
		//		LCD_DisplayStringLine_A(adx+150,ady,"NG");
		}
		
		////	Delay(5);

}

void KEY_adjust(void)
{
	u16 mm_KEYA7 = 1 ;
	u16 mm_KEYA8 = 1 ;
	u16 mm_KEYC6 = 1;
	u8 OTP_FLAG = 1;


   while(OTP_FLAG)
   {
		mm_KEYA7 = 	KEYA7;
		mm_KEYA8 =  KEYA8;
		mm_KEYC6 = 	KEYC6;
		
		if(mm_KEYA7 == 0)
		{
		
			 while(!mm_KEYA7)
			{	mm_KEYA7 =  KEYA7;}

			VCOMDC++;
			ENTER_LP_mode();
			MIPI_SPI_Write(0x03,0x39,0x00,0x00);
        	MIPI_SPI_Write(0x03,0x39,0xD9,VCOMDC);
			SHOW_IC_VALUE_A(270,800, VCOMDC);
			VIDEO_ON();
		}
		if(mm_KEYA8 == 0)
		{
		  
			 while(!mm_KEYA8)
			  {	mm_KEYA8 =  KEYA8;}
			VCOMDC--;
			ENTER_LP_mode();
			MIPI_SPI_Write(0x03,0x39,0x00,0x00);
        	MIPI_SPI_Write(0x03,0x39,0xD9,VCOMDC);
			SHOW_IC_VALUE_A(270,800, VCOMDC);
			VIDEO_ON();
		}
		if(mm_KEYC6 == 0)
		{
	
			 while(!mm_KEYC6)
			 {		mm_KEYC6 =  KEYC6;}
			aat = 1;
			ENTER_LP_mode();
			MTP_ID();
			FontR = FontG = FontB = 0;

			Soft_reset();
			OTP_FLAG = 0;
			aat = 0;
		}
		Delay(25);	
		
		
	}
}



void OTP_ERROR(void)
{
		
	FontR = 50; FontG = 255; FontB = 50;
	LCD_DisplayStringLine(20,100,"Flicker OTP NG");		
	FontR = 255; FontG = 0; FontB = 0;
	LCD_DisplayStringLine(20,130,"Reset do flicker OTP again!"); //	DelayKEY(100);
	while(1);
}

void OTP_NO(void)
{
	while(1)
	{
		
		LCD_DisplayStringLine_A(10,700,"OTP");
		FontR = 255; FontG = FontB = 0;
		LCD_DisplayStringLine_A(160,700,"NO");
		Delay(100);
		FontR = FontG = FontB = 0;	
	}
}

void SHOW_value(u16 addx,u16 addy,u32 Data)
{
	u32 sdcap;
	u8 TCH[7];

	sdcap = Data;
	TCH[4]=0;
	TCH[3]=(sdcap%10+0x30);sdcap/=10;
	TCH[2]=(sdcap%10+0x30);sdcap/=10;
	TCH[1]=(sdcap%10+0x30);sdcap/=10;
	TCH[0]=(sdcap%10+0x30);

	LCD_DisplayStringLine(addx,addy,TCH);

}

void READ_SSD2825(u8 cmd)
{
	u16 R_data=0;
	u32 sdcap=0;
	u8 TCH[6];

	R_data = SSD2825_READ(cmd);


	sdcap=R_data;
	   TCH[5]=0;
	   TCH[4]=(sdcap%10+0x30);sdcap/=10;
	   TCH[3]=(sdcap%10+0x30);sdcap/=10;
	   TCH[2]=(sdcap%10+0x30);sdcap/=10;
	   TCH[1]=(sdcap%10+0x30);sdcap/=10;
	   TCH[0]=(sdcap%10+0x30);
	    
	   LCD_DisplayStringLine(95,40,TCH);
	   DelayKEY(30);
}


void SDPicShow(u8 pic)
{
	u8 k;
	k = 0;
		SDShowFlag = 1;
		SDShowTimes = 1;

		image_count = pic;

		while(SDShowFlag) 
	{	  
	  Disp_BMP() ;
	  DelayKEY(1);

   		if((image_count >= pic)&&(k <= 1))
		{
			image_count = pic;
			k++;
		}

			  //此段为了使用SD卡的指定图片调用；；
		if(k >= 1)
		{
			SDShowFlag=0;	
		} 

	}

}


void SHOW_IC_VALUE(u16 adx,u16 ady,u32 cmd)
{
	
//	u16 R_data=0;
	u32 sdcap=0;
	u8 TCH[7];

	sdcap=cmd;
	if(sdcap == 0x55)
	{	LCD_DisplayStringLine(adx,ady,"0x55");	}	 			//ID1 = 0X55
	else if(sdcap == 0x44)
	{	LCD_DisplayStringLine(adx,ady,"0x44");	}			    //ID2 = 0X44
	else if(sdcap == 0xF0)
	{	LCD_DisplayStringLine(adx,ady,"0xF0");	}			    //ID3 = 0XF0
	else
	{
	   TCH[6]=0;
	   TCH[5]=0;
	   TCH[4]=(sdcap%10+0x30);sdcap/=10;
	   TCH[3]=(sdcap%10+0x30);sdcap/=10;
	   TCH[2]=(sdcap%10+0x30);sdcap/=10;
	   TCH[1]=(sdcap%10+0x30);sdcap/=10;
	   TCH[0]=(sdcap%10+0x30);
	    
	   LCD_DisplayStringLine(adx,ady,TCH);
	}
}

void SHOW_IC_VALUE_A(u16 adx,u16 n,u8 data)           //被调函数要在调用函数之前！
{
    u8 table[17]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'};
    u8 value[5]={0};
    u16 temp;  
    
//    temp=data>>12;
//    temp=temp&0x0f;
//    value[0]=table[temp];
//    temp=data>>8;
//    temp=temp&0x0f;
//    value[1]=table[temp];			 ///屏蔽掉的代碼為16位字符顯示
    value[0]=0x30;	//////數字0
	value[1]=0x78;	//////小寫x		 ///定值為8位字符顯示的0x
	temp=data>>4;
    temp=temp&0x0f;
    value[2]=table[temp];
    temp=data;
    temp=temp&0x0f;
    value[3]=table[temp];
    value[4]=table[16];

    LCD_DisplayStringLine(adx,n,value);
}

u8 ID_CHECK(u16 adx,u16 ady)
{
//	READ_IC(0XB6,0x00);
FontR = 0; FontG = 0; FontB = 0;
	READ_IC(0XDA,0x00);
	OTP_VALUE1 =   MIPI_READ_DATA[0];
	READ_IC(0XDB,0x00);
	OTP_VALUE2 =   MIPI_READ_DATA[0];
	READ_IC(0XDC,0x00);
	OTP_TIMES =   MIPI_READ_DATA[0];
//	READ_IC(0X04,0x00);

	LCD_DisplayStringLine(100,100, "ID1");
	SHOW_IC_VALUE_A(200,100, OTP_VALUE1);
	 LCD_DisplayStringLine(100,200, "ID2");
	SHOW_IC_VALUE_A(200,200, OTP_VALUE2);
	 LCD_DisplayStringLine(100,300, "ID3");
	SHOW_IC_VALUE_A(200,300, OTP_TIMES);
//	LCD_DisplayStringLine_A(adx,ady,"ID=");

	if((OTP_VALUE1 == 0xD5)&&(OTP_VALUE2 == 0x97)&&(OTP_TIMES == 0xCA))
	{
		ID_OK = 0;
		FontR = 0; FontG = 255; FontB = 0;
//		LCD_DisplayStringLine_A(adx+150,ady,"OK");
		LCD_DisplayStringLine_A(50,400,"ID CODE PASS");
	}
	else
	{
		ID_OK = 1;
		FontR = 255; FontG = 0; FontB = 0;
//		LCD_DisplayStringLine_A(adx+150,ady,"NG");
		LCD_DisplayStringLine_A( 50 ,400,"ID CODE NG");
	}
	FontR = 0; FontG = 0; FontB = 0;
//	DelayKEY(15);

	return ID_OK;
}

u8 ID_CHECK_A(u16 adx,u16 ady)
{
    READ_IC(0X5F,0x00);
	OTP_TIMES =   MIPI_READ_DATA[0];
	
	READ_IC(0XDA,0x00);
	OTP_VALUE1 =   MIPI_READ_DATA[0];
	READ_IC(0XDB,0x00);
	OTP_VALUE2 =   MIPI_READ_DATA[0];
	READ_IC(0XDC,0x00);
	OTP_TIMES =   MIPI_READ_DATA[0];

//	LCD_DisplayStringLine_A(adx,ady,"ID=");

	if((OTP_VALUE1 == 0xD5)&&(OTP_VALUE2 == 0x97)&&(OTP_TIMES == 0xCA))
	{
		ID_OK = 0;
		FontR = 0; FontG = 255; FontB = 0;
//		LCD_DisplayStringLine_A(adx+150,ady,"OK");
	}
	else
	{
		ID_OK = 1;
		FontR = 255; FontG = 0; FontB = 0;
		LCD_DisplayStringLine_A( 50 ,400,"ID CODE NG");
//		while(1);
//		LCD_DisplayStringLine_A(adx+150,ady,"NG");
	}


	return ID_OK;
}




void showid_vcom(void)
{
	u8  vcom;

	Flicker_PIXEL();		   DelayKEY(1);
	
	
	READ_IC_A(0XD9,0x00);
	FontR = 0; FontG = 0; FontB = 0;
	vcom= MIPI_READ_DATA[0];
	LCD_DisplayStringLine(100,600,"VCOM");
	SHOW_IC_VALUE_A(300,600,MIPI_READ_DATA[0]);
	
	while(ID_CHECK(20,700) == 1)
	{
		READ_ID++;
		if(READ_ID >3)
		{		while(1);	}
		
		temp = 1;
		temp = KEYA7;
		//		temp = 0 ;
		
		if(temp == 0)
		{
			aat2 = 0;
		//			ENTER_LP_mode();
		//			MTP_ID();
			aat2 = 1;
		}
		
		FontR = FontG = FontB = 0;
		Soft_reset();
		Delay(10);	
	
	}
	
	Delay(10);						
	//	READ_IC_A(0XD9,0x00);
	
	//	Delay(10);
	//   LCD_DisplayStringLine(100,600,"VCOM");
	////   SHOW_IC_VALUE_A(300,600,MIPI_READ_DATA[0]);
	//	SHOW_IC_VALUE_A(300,600,VCOMDC);
	////
	////  if(VCOMDC== MIPI_READ_DATA[0] )	
	////	{
	//		LCD_DisplayStringLine_A(50,700,"VCOMDC PASS");
	//	}
	//	else
	//	{
	//	
	//		LCD_DisplayStringLine_A(50,700,"VCOMDC NG");
	//			while(1);
	//	}
	//		
	FontR = FontG = FontB = 0;
	LCD_DisplayChinese(20,700,cfont,2);

	if(vcom==0x39)
	{
		FontR =255; FontG =0; FontB = 0;
		LCD_DisplayChinese(270,700,cfont2,2);
	}
	else
	{
		FontR =0; FontG =255; FontB = 0;
		LCD_DisplayChinese(270,700,cfont1,2);
	}
	
	FontR = FontG = FontB = 0;
	

}


void showid_vcom_A(void)
{
	


   while(ID_CHECK(20,700) == 1)
	{
		READ_ID++;
		if(READ_ID >3)
		{		while(1);	}

//		temp = 1;
//		temp = KEYA7;
//		temp = 0 ;
//
//		if(temp == 0)
//		{
//			aat2 = 0;
//			ENTER_LP_mode();
//			MTP_ID();
//			aat2 = 1;
//		}

		FontR = FontG = FontB = 0;
		Soft_reset();


	}

	Delay(10);						
//	READ_IC_A(0XD9,0x00);

	Delay(10);
   LCD_DisplayStringLine(100,600,"VCOM");
//   SHOW_IC_VALUE_A(300,600,MIPI_READ_DATA[0]);
	SHOW_IC_VALUE_A(300,600,VCOMDC);
//
//  if(VCOMDC== MIPI_READ_DATA[0] )	
//	{
//		LCD_DisplayStringLine_A(50,700,"VCOMDC PASS");
//	}
//	else
//	{
//	
//		LCD_DisplayStringLine_A(50,700,"VCOMDC NG");
//			while(1);
//	}
//		




}







//=================================以下程序尽量不要修改==================================
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  /* Enable USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
//------------zp2000--------------------------------
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//该函数调用了两次

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

}

/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InterruptConfig(void)
{ 
  /* Deinitializes the NVIC */
  NVIC_DeInit();

  NVIC_Configuration();

  /* Configure the Priority Group to 2 bits */
  //  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		

  /* Configure the SysTick handler priority */
  //为了使用SDIO中断，下面的中断优先级被我改低了
  NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 1, 1);
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length (time base 10 ms).
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nCount)				 //////delay  10ms
{
  TimingDelay = nCount*10;
  /* Enable the SysTick Counter */
  //SysTick_CounterCmd(SysTick_Counter_Enable);	
  SysTick_ITConfig(ENABLE);//ENABLE DISABLE		
  while(TimingDelay != 0)
  {;;}
  SysTick_ITConfig(DISABLE);//ENABLE DISABLE 
}



void MDelay(u32 nCount)						 ////delay 1ms		for auto OTP use
{
  TimingDelay = nCount;
  /* Enable the SysTick Counter */
  //SysTick_CounterCmd(SysTick_Counter_Enable);	
  SysTick_ITConfig(ENABLE);//ENABLE DISABLE		
  while(TimingDelay != 0)
  {;;}
  SysTick_ITConfig(DISABLE);//ENABLE DISABLE 
}
/*******************************************************************************
* Function Name  : Decrement_TimingDelay
* Description    : Decrements the TimingDelay variable.
* Input          : None
* Output         : TimingDelay
* Return         : None
*******************************************************************************/
void Decrement_TimingDelay(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

/*******************************************************************************
* Function Name  : SysTick_Config
* Description    : Configure a SysTick Base time to 10 ms.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Config(void)
{
  /* Configure HCLK clock as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
 
  /* SysTick interrupt each 100 Hz with HCLK equal to 72MHz */
  SysTick_SetReload(72000);

  /* Enable the SysTick Interrupt */
  SysTick_ITConfig(DISABLE);//ENABLE DISABLE

  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
}
//----设定按键接口--------------

void KEYGPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructure; 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); 
  
  /*[把KEYGPIO KEY7/8配置成输入模式] */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;    // 
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;    // 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
//    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;    // 
//  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

void DelayKEY (u32 k)
{ 
    volatile u16 m=1;
	volatile u32 j;

      for (j=0; j<k; j++)
         {  
		 	m=KEYC6;	
			Delay(2);
            while(m==0)
              {
			  	m=KEYC6; 
			  	Delay(2);

			  }            
         }	

}

void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef  USART_ClockInitStructure;

/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
/* Configure the USART1 synchronous paramters */
USART_ClockInit(USART1, &USART_ClockInitStructure);

USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No ;
USART_InitStructure.USART_HardwareFlowControl = 
USART_HardwareFlowControl_None;

USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
/* Configure USART1 basic and asynchronous paramters */
USART_Init(USART1, &USART_InitStructure);
    
  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
  
}

/*******************************************************************************
* Function Name  : USART1GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/* Private functions ---------------------------------------------------------*/
SD_Error SD_InitAndConfig(void)
{
  Status = SD_Init();

  if (Status == SD_OK)
  {
    /*----------------- Read CSD/CID MSD registers ------------------*/
    Status = SD_GetCardInfo(&SDCardInfo);
  }
  
  if (Status == SD_OK)
  {
    /*----------------- Select Card --------------------------------*/
    Status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
  }
  
  if (Status == SD_OK)
  {
    /*----------------- Set BusWidth ------------------------------*/
    Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  }
  
  /* Set Device Transfer Mode to INTERRUPT to DMA */
  if (Status == SD_OK)
  {  
    Status = SD_SetDeviceMode(SD_DMA_MODE);//SD_DMA_MODE,SD_INTERRUPT_MODE
  }
  return Status;
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/

void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

void SENSOR_Check(u8 Device_N)
{
	u16 temp1 =0;
//	temp1 = I2C_READ_BYTE(Device_N,0x9e);  /////ADC channel 0 data0low
	temp1 = sensor_check(Device_N);
	if(temp1 == 0)	  
	{	
//		Delay(1);	
		;
	}
//	else if	(temp1 == 0)	 	
//	{
//		;	
////		Delay(1);	
//	}
	else
	{
		while(1)
		{
			if(Device_N == S3_addr)	  {		LCD_DisplayStringLine(100,80,"S3");		}
			if(Device_N == S2_addr)	  {		LCD_DisplayStringLine(140,80,"S2");		}
			if(Device_N == S1_addr)	  {		LCD_DisplayStringLine(180,80,"S1");		}

			LCD_DisplayStringLine(20,50,"sensor  error");		//红色高亮显示sensor error，警示光感失灵，暂停在此处。
			while(1);
		}
	}	
}


void I2C_MEMSURE1(int kk  , int vcomdcdata)
{ int i=0,tep1=0,tep2=0,flag=0; //2014.1.27  HFZ
	int sss=0,yyy=0,aaa=0,temp;	  //temp 为加的阶数
	for (yyy=0;yyy<kk;yyy++)	 //Ω?
	{SURE[sss]=vcomdcdata+aaa;

//	HX8369_cmd_4(0xb6,SURE[sss],SURE[sss],0X00);		  ////set vcom
	MIPI_SPI_Write(0x05,0x39,0xB9,0xff,0x83,0x69); 
//				MIPI_SPI_Write(0x04,0x39,0xB6,SURE[sss],0x28);
		MIPI_SPI_Write(0x04,0x39,0xB6,SURE[sss],SURE[sss]);
		
//		MIPI_SPI_Write(0x03,0x39,0x00,0x00);
//        	MIPI_SPI_Write(0x03,0x39,0xD9,SURE[sss]);
//	 SHOW_IC_VALUE_A(300,600,SURE[sss]);
		
	SENSOR_Check(0x52);			/////S3		
	SENSOR_Check(0x72);			/////S2

	
//	SENSOR_Check(0x92);			/////S1

	Delay(5);
	DATA_SUM1[count1] =	GET_FLICKER();	 ///get flicker value
///////////////*------新增一部分光感ID检验功能，一旦光感失效，即停止在此处。-----*/
  //------2014.1.27--HFZ-START-------
		if (flag==0)                             //判断是否找到了最低点
			{
				tep2=tep1=DATA_SUM1[count1];
				flag=1;
			}
		else
			{ 
				tep1=DATA_SUM1[count1];
					if(tep1<tep2)
					{
							tep2=tep1;
							 flag=1;  //此处flag再次赋值是为了在一旦再次出现更小值的情况，清除flag计数 令后续可以找到真正的最小值
					}
					else   // if (tep1>tep2)       //如果找到了最低点，则继续寻找两次(flag=1~3)
					  	flag++;
					
					if ((flag==3) & (tep2<300))    //判断是否是真的最低点，再确认是否小于预定值，如果是则停止寻找以节约时间。
					{
						for(i=(count1+1);i<STEP;i++)
							DATA_SUM1[i]=500;                     //填充剩下的数组，否则后续处理会出错
					
							yyy=kk;     //改变判断条件，使本函数提前结束
					}
			}
		  //
	 //------2014.1.27----HFZ--END------	
				
//	temp=1;			
	if(DATA_SUM1[count1] <180)
	{temp=1;  }
	else if((DATA_SUM1[count1]>=180)&&(DATA_SUM1[count1] < 200*2))
	{temp=2; }
	else if((DATA_SUM1[count1]>=400)&&(DATA_SUM1[count1] < 400*2))
	{ temp=4;}
	else
	{ temp=8;}
//	SHOW_value(3,125,DATA_SUM1[coufnt1]);
	count1=count1+1;
//	SHOW_value(3,90,sss);
//	SHOW_value(3,50,vcomdcdata+aaa); //DelayKEY(1);

	aaa=aaa+temp;
	sss=sss+1;
	}
}
void I2C_MEMSURE(void)
{ 
		u16 mm_KEYA7 = 1;
	int temp;	  //temp 为加的阶数
    u16 a=0x2A,b;
		SENSOR_Check(0x52);			/////S3		
	SENSOR_Check(0x72);			/////S2
   while(1)
   {
        	Delay(4);	
   	  	mm_KEYA7 =  KEYA7;
   if(mm_KEYA7 == 0)
		{
	
			 while(!mm_KEYA7)
			 {		mm_KEYA7 =  KEYA7;}


   
//	HX8369_cmd_4(0xb6,SURE[sss],SURE[sss],0X00);		  ////set vcom
	       MIPI_SPI_Write(0x03,0x39,0x00,0x00);
        	MIPI_SPI_Write(0x03,0x39,0xD9,a);
		  Delay(5);
//	 	 READ_IC_A(0XD9,0x00);

 //   LCD_DisplayStringLine(100,600,"VCOM");
 SHOW_IC_VALUE_A(300,600,a);
 	
//	SENSOR_Check(0x92);			/////S1

	Delay(5);
        b=	GET_FLICKER();	 ///get flicker value
//	  SHOW_IC_VALUE_A(300,600,b);
	if(b <= 180)
	{temp=1;  }
	else if((b>180)&&(b< 180*2))
	{temp=2; }
	else
	{ temp=3;}
	a=a+1;
			
	}
	

   }
}
void I2C_DATAL_COMPAREX(void)
{

//	TSL2583_init();			////must initial TSL2583,,,for AUTO OTP use..
//	while(1)
//	{
	GET_FLICKER();
	I2C_MEMSURE1(STEP , VCOMDC);   //最大找寻 20次，调整的阶数值保存于SURE数组中
	I2C_DATAL_COMPARE();      //  
    count1=0;

	
/*在最小值附近停下来。。。后续开启OTP功能即可。。*/
//		VCOMDC1 = SURE[count2];				 //挑选出最好的VCOM的值，下CODE，看效果。
//		SHOW_value(3,50,count2);
//		SHOW_value(3,50,VCOMDC1);



//		VCOMDC1 = SURE[count2 ] + 7;
//		VCOMDC1 = SURE[count2];					///////////////2013-12-05
		VCOMDC1 = SURE[count2];					///////////////2013-12-15
//	  VCOMDC1 = SURE[count2]+0;  //2014.1.27 count2为最优vcom所在的序号，根据序号在SURE数组找到对应阶数
//		HX8369_cmd_4(0xb6,VCOMDC1,VCOMDC1,0X00);	   ////write the perfect    vcom value

//			ENTER_LP_mode();
			MIPI_SPI_Write(0x05,0x39,0xB9,0xff,0x83,0x69); 
//				MIPI_SPI_Write(0x04,0x39,0xB6,VCOMDC1,0x28);
				MIPI_SPI_Write(0x04,0x39,0xB6,VCOMDC1,VCOMDC1);
//				
//			MIPI_SPI_Write(0x03,0x39,0x00,0x00);
//        	MIPI_SPI_Write(0x03,0x39,0xD9,VCOMDC1);
//			VIDEO_ON();
//	}
//		VCOMDC1 = DATA_SUM1[count2];		////挑选出最小的FLICKER值，显示出来。
//		SHOW_value(3,125,VCOMDC1);

}
void I2C_DATAL_COMPARE(void)
{
		int yy=1,zz=0;
	while(yy<count1 && zz<count1)
	{
		if(	DATA_SUM1[zz]>DATA_SUM1[yy])
		{
			count2=yy;zz=zz+1;	
		}
		else
		{
			count2=zz;yy=yy+1;
		}
	}	
}

void MTP(void)
{
	///display off
			SSD1963GPIOOUTCOM(0xBC);		                
	SSD1963GPIOOUTDATA_16(0x0001);					
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x28);	  //
	Delay(5);			 //delay 40ms

			SSD1963GPIOOUTCOM(0xBC);		                
	SSD1963GPIOOUTDATA_16(0x0002);					
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	MDelay(20);			   ////delay 1ms

	//////////set otp_index[8:0]
			SSD1963GPIOOUTCOM(0xBC);		                
	SSD1963GPIOOUTDATA_16(0x0006);					
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0d);		   /////烧录otp的0x0d的VCOM_F1
	SSD1963GPIOOUTDATA_18(0x00);
	MDelay(10);	
	
	///////////////set otp_mask[7:0]
			SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0006);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0d);		   /////烧录otp的0x0d的VCOM_F1
	SSD1963GPIOOUTDATA_18(0x00);
	MDelay(10);		

	/////////////set otp address
			SSD1963GPIOOUTCOM(0xBC);		              
	SSD1963GPIOOUTDATA_16(0x0006);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0d);		   /////烧录otp的0x0d的VCOM_F1
	SSD1963GPIOOUTDATA_18(0x00);
	MDelay(10);

	/////////////set otp_prog = 1;
			SSD1963GPIOOUTCOM(0xBC);		             
	SSD1963GPIOOUTDATA_16(0x0006);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0d);		   /////烧录otp的0x0d的VCOM_F1
	SSD1963GPIOOUTDATA_18(0x01);
	MDelay(45);		 		///DELAY 11ms

	/////////////set otp_index[8:0]
			SSD1963GPIOOUTCOM(0xBC);		              
	SSD1963GPIOOUTDATA_16(0x0006);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0e);				/////烧录otp的0x0E的VCOM_B1
	SSD1963GPIOOUTDATA_18(0x00);
	MDelay(10);

	/////////////set otp_mask[7:0]
			SSD1963GPIOOUTCOM(0xBC);		                
	SSD1963GPIOOUTDATA_16(0x0006);						
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0e);				/////烧录otp的0x0E的VCOM_B1
	SSD1963GPIOOUTDATA_18(0x00);
	MDelay(10);

	/////////////set otp address
			SSD1963GPIOOUTCOM(0xBC);		                 
	SSD1963GPIOOUTDATA_16(0x0006);					
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0e);				/////烧录otp的0x0E的VCOM_B1
	SSD1963GPIOOUTDATA_18(0x00);
	MDelay(10);

	/////////////set otp_prog = 1;
			SSD1963GPIOOUTCOM(0xBC);		                 
	SSD1963GPIOOUTDATA_16(0x0006);						  
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0x5a);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x0e);				/////烧录otp的0x0E的VCOM_B1
	SSD1963GPIOOUTDATA_18(0x01);
	MDelay(45);		 		///DELAY 11ms

   	//////MTP   OVER ,,,OTP  Disable
			SSD1963GPIOOUTCOM(0xBC);		                 
	SSD1963GPIOOUTDATA_16(0x0002);						  
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xb7);	  //
	SSD1963GPIOOUTDATA_18(0xff);
	MDelay(10);

	///display on
			SSD1963GPIOOUTCOM(0xBC);		                
	SSD1963GPIOOUTDATA_16(0x0001);					
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x29);	  //
	Delay(5);			 //delay 40ms
}


void cycle_OTP(u8 OTPValue)
{


	MIPI_SPI_Write(0x06,0x39,0xBB,0x00,0x00,OTPValue,0x00);
	MIPI_SPI_Write(0x06,0x39,0xBB,0x00,0x00,OTPValue,0x01);
	Delay(5);
	MIPI_SPI_Write(0x06,0x39,0xBB,0x00,0x00,OTPValue,0x00);

}

void initial_OTP(void)
{

//	MIPI_SPI_Write(0x06,0x39, 0xB9,0xFF,0x83,0x69);//EXTC Command Set enable register
//
//	OPEN_OTP_Voltage;//Open 7.5V
//	Delay(50);//Wait 500ms for PVSS stable	8369 no delay time
//
////	MIPI_SPI_Write(0x06,0x39,0xB6,0x2E,0x2E);	//Set VCOM
//	MIPI_SPI_Write(0x06,0x39,0xE9,0xAA,0x55);	//Set OTP

//-----------------------GIP timing------------------------//
//	MIPI_SPI_Write(0x06,0x39,0xBB,0x00,0x00,0xB3,0x00);
//	MIPI_SPI_Write(0x30,0x39,0xBB,0x00,0x00,0xB3,0x01);
//	Delay(5);
//	MIPI_SPI_Write(0x30,0x39,0xBB,0x00,0x00,0xB3,0x00);



cycle_OTP(0xB0);
cycle_OTP(0xB1);
cycle_OTP(0xB2);
cycle_OTP(0xB3);

//cycle_OTP(0x1C);
//cycle_OTP(0x1D);

cycle_OTP(0x31);
cycle_OTP(0x32);
cycle_OTP(0x33);
cycle_OTP(0x34);
cycle_OTP(0x35);
cycle_OTP(0x36);
cycle_OTP(0x37);
cycle_OTP(0x38);
cycle_OTP(0x39);
cycle_OTP(0x3A);
cycle_OTP(0x3B);
cycle_OTP(0x3C);
cycle_OTP(0x3D);
cycle_OTP(0x3E);
cycle_OTP(0x3F);

cycle_OTP(0x40);
cycle_OTP(0x41);
cycle_OTP(0x42);
cycle_OTP(0x43);
cycle_OTP(0x44);
cycle_OTP(0x45);
cycle_OTP(0x46);
//cycle_OTP(0x47);
cycle_OTP(0x48);
cycle_OTP(0x49);
cycle_OTP(0x4A);

cycle_OTP(0x78);
cycle_OTP(0x79);
cycle_OTP(0x7A);
cycle_OTP(0x7B);
cycle_OTP(0x7C);
cycle_OTP(0x7D);
cycle_OTP(0x7E);
cycle_OTP(0x7F);

cycle_OTP(0x80);
cycle_OTP(0x81);
cycle_OTP(0x82);
cycle_OTP(0x83);
cycle_OTP(0x84);
cycle_OTP(0x85);
cycle_OTP(0x86);
cycle_OTP(0x87);
cycle_OTP(0x88);
cycle_OTP(0x89);
cycle_OTP(0x8A);
cycle_OTP(0x8B);
cycle_OTP(0x8C);
cycle_OTP(0x8D);
cycle_OTP(0x8E);
cycle_OTP(0x8F);

cycle_OTP(0x90);
cycle_OTP(0x91);
cycle_OTP(0x92);
cycle_OTP(0x93);
cycle_OTP(0x94);
cycle_OTP(0x95);
cycle_OTP(0x96);
cycle_OTP(0x97);
cycle_OTP(0x98);
cycle_OTP(0x99);
cycle_OTP(0x9A);
cycle_OTP(0x9B);
cycle_OTP(0x9C);
cycle_OTP(0x9D);
cycle_OTP(0x9E);
cycle_OTP(0x9F);

cycle_OTP(0xA0);
cycle_OTP(0xA1);
cycle_OTP(0xA2);
cycle_OTP(0xA3);
cycle_OTP(0xA4);
cycle_OTP(0xA5);
cycle_OTP(0xA6);
cycle_OTP(0xA7);
cycle_OTP(0xA8);
cycle_OTP(0xA9);
cycle_OTP(0xAA);
cycle_OTP(0xAB);
cycle_OTP(0xAC);
cycle_OTP(0xAD);
cycle_OTP(0xAE);
cycle_OTP(0xAF);




//	MIPI_SPI_Write(0x06,0x39,0xE9,0x00,0x00);	//Set OTP
//
//	CLOSE_OTP_Voltage;//Close 7.5V 
//	Delay(10);//Wait 10ms	8369 no delay time
	
//	SSD1963Command_8(0xBa); 
//	SSD1963Data_8(0x0E);   //RESET=0
//	Delay(12);
//	SSD1963Command_8(0xBa); 
//	SSD1963Data_8(0x0F);   //RESET=1 
//	Delay(12);
	
//	SSD1963INITSSD2825();
//	OTP_initial();


	}

void MTP_ID(void)
{
	MIPI_SPI_Write(0x05,0x39, 0xB9,0xFF,0x83,0x69);//EXTC Command Set enable register

//	MIPI_SPI_Write(0x05,0x39, 0xBF,0xE0,0x00,0x00);
	
//	OPEN_OTP_Voltage;//Open 7.5V
	Delay(50);//Wait 500ms for PVSS stable	8369 no delay time
	
//	MIPI_SPI_Write(0x05,0x39, 0xB6,VCOMDC,0x28);//This command is used to set VCOM Voltage include VCOM Low and VCOM High Voltage.
	MIPI_SPI_Write(0x04,0x39, 0xB6,VCOMDC1,VCOMDC1);
	
	MIPI_SPI_Write(0x05,0x39, 0xE9,0xAA,0x55);// This command is used to set OTP key to enter or leave OTP program mode.
	Delay(50);//Wait 500ms for PVSS stable 8369 no delay time

	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,0x1C,0x00);

	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,0x1C,0x01);
	Delay(5);

	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,0x1C,0x00);
	Delay(5);

	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,0x1D,0x00);
	Delay(5);

	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,0x1D,0x01);
	Delay(5);

	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,0x1D,0x00);
	Delay(5);

	initial_OTP();

	MIPI_SPI_Write(0x06,0x39, 0xE9,0x00,0x00);//set OTP off

//	CLOSE_OTP_Voltage;//Close 7.5V 
//	Delay(10);//Wait 10ms	8369 no delay time
//	
//	SSD1963Command_8(0xBa); 
//	SSD1963Data_8(0x0E);   //RESET=0
//	Delay(12);
//	SSD1963Command_8(0xBa); 
//	SSD1963Data_8(0x0F);   //RESET=1 
//	Delay(12);
//	
//	SSD1963INITSSD2825();
//	OTP_initial();//GPI009A0 (HX8369-A01) initial code
	
	
}



u8 VCOM_GET(void)
{
	READ_VCOM_OTP_TIMES_HX8369(0x1B);
		DelayKEY(1);
		switch(MIPI_READ_DATA[4])
		{
			case 0xFF:
				
				OTP_TIMES_BEFORE = 0;
				LCD_DisplayStringLine(1,10,"otp times = ");
				SHOW_IC_VALUE_A(195,10,OTP_TIMES_BEFORE);
				break;
			
			case 0x6F:
				
				OTP_TIMES_BEFORE = 1;
				LCD_DisplayStringLine(1,10,"otp times = ");
				SHOW_IC_VALUE_A(195,10,OTP_TIMES_BEFORE);
				READ_VCOM_OTP_TIMES_HX8369(0x1C);
				LCD_DisplayStringLine(1,50,"vcom value = ");
				SHOW_IC_VALUE_A(195,50,MIPI_READ_DATA[4]);
				break;
			
			case 0x27:
				
				OTP_TIMES_BEFORE = 2;
				LCD_DisplayStringLine(1,10,"otp times = ");
				SHOW_IC_VALUE_A(195,10,OTP_TIMES_BEFORE);
				READ_VCOM_OTP_TIMES_HX8369(0x1E);
				LCD_DisplayStringLine(1,50,"vcom value = ");
				SHOW_IC_VALUE_A(195,50,MIPI_READ_DATA[4]);
				break;
			
			case 0x03:
				
				OTP_TIMES_BEFORE = 3;
				LCD_DisplayStringLine(1,10,"otp times = ");
				SHOW_IC_VALUE_A(195,10,OTP_TIMES_BEFORE);
				READ_VCOM_OTP_TIMES_HX8369(0x20);
				LCD_DisplayStringLine(1,50,"vcom value = ");
				SHOW_IC_VALUE_A(195,50,MIPI_READ_DATA[4]);
				break;
			
			case 0x00:
				
				OTP_TIMES_BEFORE = 0;
				LCD_DisplayStringLine(20,10,"otp read error");
				break;
			
			default:
				
				OTP_TIMES_BEFORE = 0;
				LCD_DisplayStringLine(20,10,"otp read error");
				break;
		}
		
		return 0;
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
