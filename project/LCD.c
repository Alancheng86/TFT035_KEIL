/******************** (C) COPYRIGHT 2010 KSGIANTLUS ********************
* File Name          : lcd.c
* Author             : JASON
* Version            : V1.0
* Date               : 2010.01.20
*************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "fonts.h"
#include "chinese.h"
#include "stdarg.h"


//#define GPM1461A0_SC7789
//#define GPI009A0_HX8369_A
#define TFT9K_480X240


#ifdef TFT9K_480X240
//////mipi video mode setting
u16 value_HighSpeed = 393;		  //HS_speed   value	   384
u16 value_Lane_select = 2;		  //mipi lane select


u8 Frame_rate = 60;				//////刷新频率   HZ    2016-9-7 启用

u16  VDP= 240;     
u16  VBP= 12 ;    //
u16  VFP= 12 ;    //

u16  HDP= 480;     
u16  HBP= 40;     //	
u16  HFP= 89;     //	

u16	 VPW=4;  //通常不需要调整
u16	 HPW=6;  //通常不需要调整

//u8 Frame_rate = 60;				//////刷新频率   HZ    2016-9-7 启用

//u16  VDP= 240;     
//u16  VBP= 4 ;    //
//u16  VFP= 4 ;    //

//u16  HDP= 480;     
//u16  HBP= 40;     //	
//u16  HFP= 34;     //	

//u16	 VPW=2;  //通常不需要调整
//u16	 HPW=10;  //通常不需要调整

#define YDP 240	   //对应0XB0使用0x30
#define XDP 480   
#endif


#ifdef GPM1461A0_SC7789
//////mipi video mode setting
u16 value_HighSpeed = 393;		  //HS_speed   value	   384
u16 value_Lane_select = 2;		  //mipi lane select


//u8   DotclkValue = 31; //MHz		   //56HZ
u8 Frame_rate = 60;				//////刷新频率   HZ    2016-9-7 启用

u16  VDP= 854;     
u16  VBP= 22 ;    //
u16  VFP= 22 ;    //

u16  HDP= 480;     
u16  HBP= 64;     //	
u16  HFP= 64;     //	

u16	 VPW=4;  //通常不需要调整
u16	 HPW=22;  //通常不需要调整

#define YDP 854	   //对应0XB0使用0x30
#define XDP 480   
#endif


#ifdef  GPI009A0_HX8369_A
//////mipi video mode setting
u16 value_HighSpeed = 400;		  //HS_speed   value	   384
u16 value_Lane_select = 2;		  //mipi lane select


//u8   DotclkValue = 27; //MHz		   //56HZ
u8 Frame_rate = 60;				//////刷新频率   HZ    2016-9-7 启用

u16  HDP= 480;      
u16  HBP= 13 ;    //13
u16  HFP= 5;    //8
u16  VDP= 800;     
u16  VBP= 14;     //   14
u16  VFP= 15;     //   15			


u16	 VPW=4;  //通常不需要调整
u16	 HPW=5;  //通常不需要调整  5

//u16  VDP= 800;     
//u16  VBP= 22 ;    //
//u16  VFP= 82 ;    //

//u16  HDP= 480;     
//u16  HBP= 32;     //	
//u16  HFP= 32;     //	

//u16	 VPW=4;  //通常不需要调整
//u16	 HPW=22;  //通常不需要调整

#define YDP 800	   //对应0XB0使用0x30
#define XDP 480  
#endif

u8   DotclkValue = 27; //MHz
u8 Freq_OSC		= 24;		////MHZ	   mipi芯片晶振頻率 24MHZ

static  vu16 TextColor = 0x0000, BackColor = 0xFFFF;
u16 DeviceCode;
extern void Delay(u32 nCount);
void SSD2825_CLAN043LL_SC7789_A(void);
u16 mm=0;
u8 FontR,FontG,FontB;
extern u8 SDShowTimes;
extern u8 MIPI_READ_DATA[10];
extern u8 VCOMDC;				   //////GPM1461A0_D  use
int MIPI_Array[150];
/*******************************************************************************
* Function Name  : STM32_SSD1963_Init
* Description    : Initializes the SSD1963.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STM32_SSD1963_Init(void) //Initializes the SSD1963.
{ 
	u16   VT,HT,VPS,HPS;
	float temp ;
	u32  LCDC_FPR;
	
	
	VT=VBP+VFP+VDP;
	HT=HBP+HFP+HDP;
	HPS=HBP;
	VPS=VBP;
	
	DotclkValue = Frame_rate*HT*VT/1000000;  
	
	temp=((DotclkValue<<20)/100-1);
	LCDC_FPR=(u32)temp;
	
	Delay(5);
	/* Configure the LCD Control pins -LCD 相关控制端口初始化----------------------*/
	LCD_CtrlLinesConfig();
	
	Delay(5); /* delay 50 ms */
  
/* Start Initial Sequence ----------------------------------------------------*/
{//初始化SSD1963    
	
	LCD_RST(0);//硬件复位
	         SSD1963_CS(1); 
	         LCD_RS(1); 
	         LCD_WR(1); 
			 LCD_RD(1);	LCD_CS(0);
	Delay(5);
	LCD_RST(1) ;
	LCD_RD(1);
	Delay(20);

	{
	  //software reset
	   SSD1963Command_8(0x01); 
	   Delay(20);
//	   SSD1963Command_8(0x01); 
//	   Delay(20);   
	                       
	   //set_pixel_data_interface-设置MCU到1963的数据格式
	   SSD1963Command_8(0xf0);
	   SSD1963Data_8(0x00);   //0x00: 8-bit; 0x03:16-bit (565 format)
	   
	    SSD1963Command_8(0xB8);  //Set the GPIOs configuration. If the GPIOs are not used for LCD, set the direction. Otherwise, they are toggled with  LCD signals by 0xC0 – 0xCF.
	    SSD1963Data_8(0x0F);     //
	    SSD1963Data_8(0x01);     // 
	    
	   //set_pll_mn;  m= 3 m= 2  设置1963内部PLL，基本不须修改
	   SSD1963Command_8(0xe2); //VCO = Reference input clock x (M + 1);PLL frequency = VCO / (N + 1)
	   //* Note : 250MHz < VCO < 800MHz     
	   SSD1963Data_8(49);   //Multiplier (M) of PLL. (POR = 00101101) ,M<256
	   SSD1963Data_8(3);   //Divider (N) of PLL. (POR = 0011),N<16
	   SSD1963Data_8(0x04); // Effectuate the multiplier and divider value
	   
	   //Start the PLL. Before the start, the system was operated with the crystal oscillator or clock input.
	   SSD1963Command_8(0xe0);
	   SSD1963Data_8(0x01);
	   Delay(10);
	   SSD1963Command_8(0xe0);  
	   SSD1963Data_8(0x03); 
		Delay(28);


	   //dotclk setting,Set the LSHIFT (pixel clock) frequency --For parallel LCD interface:
	   SSD1963Command_8(0xe6);  //:Configure the pixel clock 
	   // For parallel LCD interface DOTCLK Freq =PLL freq x ((LCDC_FPR + 1) / 1048576)
	   SSD1963Data_8(LCDC_FPR>>16);
	   SSD1963Data_8(LCDC_FPR>>8); 
	   SSD1963Data_8(LCDC_FPR);
	
	
	   //Set_lcd_mode,Panel setting
	   SSD1963Command_8(0xb0);  // Delay(10);
	   SSD1963Data_8(0x34);   // Set the LCD panel mode and resolution;TFT color depth enhancement enable;;
	   //LSHIFT polarity;Set the horizontal sync pulse polarity;LFRAME polarity (POR = 0)
	   //此处vs hs dotclk的极性设置于1963规格书说明相反，估计资料错误
	   SSD1963Data_8(0x00);   //B[7] : LCD panel mode (POR = 0)
	                          //     0 Hsync+Vsync +DE mode
	                          //     1 TTL mode
	                          // B[6:5] : TFT type (POR = 01)
	                          //  00, 01 TFT mode
	                         //  10 Serial RGB mode
	                         //  11 Serial RGB+dummy mode
	   
	   //Horizontal panel size = (HDP + 1) pixels
	   SSD1963Data_8((HDP-1)>>8); //High byte of the horizontal panel size       
	   SSD1963Data_8(HDP-1);  //Low byte of the horizontal panel size;
	   
	   SSD1963Data_8((VDP-1)>>8); //High byte of the vertical panel size
	   //Vertical panel size = (VDP + 1) lines
	   SSD1963Data_8(VDP-1); //Low byte of the vertical panel size (POR = 11011111)
	   
	   SSD1963Data_8(0x23); //RGB sequence for serial TFT interface
	
	   //hsync setting
	   SSD1963Command_8(0xb4); 
	   SSD1963Data_8((HT-1)>>8);  //HT[10:8];(display + non-display),
	   SSD1963Data_8(HT-1);   //HT[7:0];Horizontal total period = (HT + 1) pixels
	   SSD1963Data_8(HPS>>8);  //HPS[10:8] : High byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data. (POR = 000)
	   SSD1963Data_8(HPS);   //HPS[7:0]
	   SSD1963Data_8(HPW);   //HPW[6:0] : Set the horizontal sync pulse width (LLINE) in pixel clock. (POR = 000111)
	                          //Horizontal Sync Pulse Width = (HPW + 1) pixels
	   SSD1963Data_8(0x00);  //LPS[10:8] :Set the horizontal sync pulse (LLINE) start location in pixel clock. (POR = 000)
	   SSD1963Data_8(0x00);   //LPS[7:0] :Set the horizontal sync pulse width (LLINE) in start. (POR = 00000000)
	   SSD1963Data_8(0x00);  //Set the horizontal sync pulse subpixel start position (POR = 00)
	
	   
	
	   //vsync setting
	   SSD1963Command_8(0xb6); //Set the vertical blanking interval between last scan line and next LFRAME pulse
	   SSD1963Data_8(VT>>8);    //VT:High byte of the vertical total (display + non-display) period in lines (POR = 001)
	   SSD1963Data_8(VT-1);   //Low byte of the vertical total (display + non-display) period in lines (POR = 11101111)
	                          //Vertical Total = (VT + 1) lines
	   SSD1963Data_8(VPS>>8);    //VPS[10:8] :High byte the non-display period in lines between the start of the frame and the first display data in line.
	   SSD1963Data_8(VPS);     //VPS[7:0] :The non-display period in lines between the start of the frame and the first display data in line.
	                          //Vertical Sync Pulse Start Position = VPS lines
	   SSD1963Data_8(VPW);    //VPW[6:0] :Set the vertical sync pulse width (LFRAME) in lines. (POR = 000001)
	                          //Vertical Sync Pulse Width = (VPW + 1) lines
	   SSD1963Data_8(0x00);   // FPS[10:8] : High byte of the vertical sync pulse (LFRAME) start location in lines. (POR = 000)
	   SSD1963Data_8(0x00);   // FPS[7:0] :Low byte of the vertical sync pulse (LFRAME) start location in lines. (POR = 00000000)
	                           //Vertical Display Period Start Position = FPS lines 
	
	    
	
	   SSD1963Command_8(0x36);//Set the read order from host processor to frame buffer by A[7:5] and A[3] and from frame buffer to the display panel by A[2:0] and A[4].
	   SSD1963Data_8(0x00);   //数据操作顺序为左至右，从上到下
	
	  
	   SSD1963Command_8(0x29); ////display on
	     
	 
	   SSD1963Command_8(0x2a);  //Set the column address of frame buffer
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x1);
	   SSD1963Data_8(0x67);// 
	
	   //row start_end
	   SSD1963Command_8(0x2b);   //Set the page address of the frame
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x02);
	   SSD1963Data_8(0x7f);// 
	}
	} 
//	SSD1963Command_8(0xB8);  //Set the GPIOs configuration. If the GPIOs are not used for LCD, set the direction. Otherwise, they are toggled with  LCD signals by 0xC0 – 0xCF.
//	    SSD1963Data_8(0x0F);     //
//	    SSD1963Data_8(0x01);     //
    
	  SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0F); 
       Delay(20);
    
	   SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0e);
	   Delay(5);   
	   
	   SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0F); 
//	   Delay(5);

//////////////=============================================================================
    LCD2353INITTION();
//    GPIO_SetBits(GPIOA, GPIO_Pin_8);      ////////exit sleep
    
//////////////在SSD1963的I/O工作正常后可以对SSD2825进行配置。以便后续对IC进行初始化配置
//	value_HighSpeed = DotclkValue*24*12/value_Lane_select/10;      /////Alan  add  2016-9-7   倍率1.2
//	
//	//对HX8369 IC进行初始化
//	SSD1963INITSSD2825(); 

////	while(READ_IC(0X0a,0x00)!=0x08)
////	{
////	   Delay(20);
////	}
//	ENTER_LP_mode(); 

//	INITIALOTM8018B();					   /////GPM1461A0  USE
////	SSD2825_CLAN043LL_SC7789_A();
////	SSD2825INITIAL_HX8369B();
/////////////此处对SSD2825重新配置为VIDEO模式，一定要在SSD1963时序生成OK后进行配置2825，
//	VIDEO_ON();

}

void STM32_SSD1963_Init_QC(void) //Initializes the SSD1963.
{ 
	u16   VT,HT,VPS,HPS;
	float temp ;
	u32  LCDC_FPR;
	
	
	VT=VBP+VFP+VDP;
	HT=HBP+HFP+HDP;
	HPS=HBP;
	VPS=VBP;
	
	DotclkValue = Frame_rate*HT*VT/1000000;  
	
	temp=((DotclkValue<<20)/100-1);
	LCDC_FPR=(u32)temp;
	
	Delay(5);
	/* Configure the LCD Control pins -LCD 相关控制端口初始化----------------------*/
	LCD_CtrlLinesConfig();
	
	Delay(5); /* delay 50 ms */
  
/* Start Initial Sequence ----------------------------------------------------*/
{//初始化SSD1963    
	
	LCD_RST(0);//硬件复位
	         SSD1963_CS(1); 
	         LCD_RS(1); 
	         LCD_WR(1); 
			 LCD_RD(1);	LCD_CS(0);
	Delay(5);
	LCD_RST(1) ;
	LCD_RD(1);
	Delay(20);

	{
	  //software reset
	   SSD1963Command_8(0x01); 
	   Delay(20);
//	   SSD1963Command_8(0x01); 
//	   Delay(20);   
	                       
	   //set_pixel_data_interface-设置MCU到1963的数据格式
	   SSD1963Command_8(0xf0);
	   SSD1963Data_8(0x00);   //0x00: 8-bit; 0x03:16-bit (565 format)
	   
	     
	    
	   //set_pll_mn;  m= 3 m= 2  设置1963内部PLL，基本不须修改
	   SSD1963Command_8(0xe2); //VCO = Reference input clock x (M + 1);PLL frequency = VCO / (N + 1)
	   //* Note : 250MHz < VCO < 800MHz     
	   SSD1963Data_8(49);   //Multiplier (M) of PLL. (POR = 00101101) ,M<256
	   SSD1963Data_8(3);   //Divider (N) of PLL. (POR = 0011),N<16
	   SSD1963Data_8(0x04); // Effectuate the multiplier and divider value
	   
	   //Start the PLL. Before the start, the system was operated with the crystal oscillator or clock input.
	   SSD1963Command_8(0xe0);
	   SSD1963Data_8(0x01);
	   Delay(10);
	   SSD1963Command_8(0xe0);  
	   SSD1963Data_8(0x03); 
		Delay(28);


	   //dotclk setting,Set the LSHIFT (pixel clock) frequency --For parallel LCD interface:
	   SSD1963Command_8(0xe6);  //:Configure the pixel clock 
	   // For parallel LCD interface DOTCLK Freq =PLL freq x ((LCDC_FPR + 1) / 1048576)
	   SSD1963Data_8(LCDC_FPR>>16);
	   SSD1963Data_8(LCDC_FPR>>8); 
	   SSD1963Data_8(LCDC_FPR);
	
	
	   //Set_lcd_mode,Panel setting
	   SSD1963Command_8(0xb0);  // Delay(10);
	   SSD1963Data_8(0x30);   // Set the LCD panel mode and resolution;TFT color depth enhancement enable;;
	   //LSHIFT polarity;Set the horizontal sync pulse polarity;LFRAME polarity (POR = 0)
	   //此处vs hs dotclk的极性设置于1963规格书说明相反，估计资料错误
	   SSD1963Data_8(0x00);   //B[7] : LCD panel mode (POR = 0)
	                          //     0 Hsync+Vsync +DE mode
	                          //     1 TTL mode
	                          // B[6:5] : TFT type (POR = 01)
	                          //  00, 01 TFT mode
	                         //  10 Serial RGB mode
	                         //  11 Serial RGB+dummy mode
	   
	   //Horizontal panel size = (HDP + 1) pixels
	   SSD1963Data_8((HDP-1)>>8); //High byte of the horizontal panel size       
	   SSD1963Data_8(HDP-1);  //Low byte of the horizontal panel size;
	   
	   SSD1963Data_8((VDP-1)>>8); //High byte of the vertical panel size
	   //Vertical panel size = (VDP + 1) lines
	   SSD1963Data_8(VDP-1); //Low byte of the vertical panel size (POR = 11011111)
	   
	   SSD1963Data_8(0x23); //RGB sequence for serial TFT interface
	
	   //hsync setting
	   SSD1963Command_8(0xb4); 
	   SSD1963Data_8((HT-1)>>8);  //HT[10:8];(display + non-display),
	   SSD1963Data_8(HT-1);   //HT[7:0];Horizontal total period = (HT + 1) pixels
	   SSD1963Data_8(HPS>>8);  //HPS[10:8] : High byte of the non-display period between the start of the horizontal sync (LLINE) signal and the first display data. (POR = 000)
	   SSD1963Data_8(HPS);   //HPS[7:0]
	   SSD1963Data_8(HPW);   //HPW[6:0] : Set the horizontal sync pulse width (LLINE) in pixel clock. (POR = 000111)
	                          //Horizontal Sync Pulse Width = (HPW + 1) pixels
	   SSD1963Data_8(0x00);  //LPS[10:8] :Set the horizontal sync pulse (LLINE) start location in pixel clock. (POR = 000)
	   SSD1963Data_8(0x00);   //LPS[7:0] :Set the horizontal sync pulse width (LLINE) in start. (POR = 00000000)
	   SSD1963Data_8(0x00);  //Set the horizontal sync pulse subpixel start position (POR = 00)
	
	   
	
	   //vsync setting
	   SSD1963Command_8(0xb6); //Set the vertical blanking interval between last scan line and next LFRAME pulse
	   SSD1963Data_8(VT>>8);    //VT:High byte of the vertical total (display + non-display) period in lines (POR = 001)
	   SSD1963Data_8(VT-1);   //Low byte of the vertical total (display + non-display) period in lines (POR = 11101111)
	                          //Vertical Total = (VT + 1) lines
	   SSD1963Data_8(VPS>>8);    //VPS[10:8] :High byte the non-display period in lines between the start of the frame and the first display data in line.
	   SSD1963Data_8(VPS);     //VPS[7:0] :The non-display period in lines between the start of the frame and the first display data in line.
	                          //Vertical Sync Pulse Start Position = VPS lines
	   SSD1963Data_8(VPW);    //VPW[6:0] :Set the vertical sync pulse width (LFRAME) in lines. (POR = 000001)
	                          //Vertical Sync Pulse Width = (VPW + 1) lines
	   SSD1963Data_8(0x00);   // FPS[10:8] : High byte of the vertical sync pulse (LFRAME) start location in lines. (POR = 000)
	   SSD1963Data_8(0x00);   // FPS[7:0] :Low byte of the vertical sync pulse (LFRAME) start location in lines. (POR = 00000000)
	                           //Vertical Display Period Start Position = FPS lines 
	
	    
	
	   SSD1963Command_8(0x36);//Set the read order from host processor to frame buffer by A[7:5] and A[3] and from frame buffer to the display panel by A[2:0] and A[4].
	   SSD1963Data_8(0x00);   //数据操作顺序为左至右，从上到下
	
	  
	   SSD1963Command_8(0x29); ////display on
	     
	 
	   SSD1963Command_8(0x2a);  //Set the column address of frame buffer
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x1);
	   SSD1963Data_8(0x67);// 
	
	   //row start_end
	   SSD1963Command_8(0x2b);   //Set the page address of the frame
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x00);
	   SSD1963Data_8(0x02);
	   SSD1963Data_8(0x7f);// 
	}
	} 
	SSD1963Command_8(0xB8);  //Set the GPIOs configuration. If the GPIOs are not used for LCD, set the direction. Otherwise, they are toggled with  LCD signals by 0xC0 – 0xCF.
	    SSD1963Data_8(0x0F);     //
	    SSD1963Data_8(0x01);     //
	  
	   SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0e);
	   Delay(5);   
	   
	   SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0F); 
	   Delay(20);

////////////=============================================================================
////////////在SSD1963的I/O工作正常后可以对SSD2825进行配置。以便后续对IC进行初始化配置
	value_HighSpeed = DotclkValue*24*12/value_Lane_select/10;      /////Alan  add  2016-9-7   倍率1.2
	
	//对HX8369 IC进行初始化
	SSD1963INITSSD2825(); 

//	while(READ_IC(0X0a,0x00)!=0x08)
//	{
//	   Delay(20);
//	}
	ENTER_LP_mode(); 

	INITIALOTM8018B_QC();					   /////GPM1461A0  USE
//	SSD2825_CLAN043LL_SC7789_A();
//	SSD2825INITIAL_HX8369B();
///////////此处对SSD2825重新配置为VIDEO模式，一定要在SSD1963时序生成OK后进行配置2825，
	VIDEO_ON();

}

void SSD1963Command_8(u8 cmd)
{            LCD_RS(0);
             SSD1963_CS(0); //asm("NOP");asm("NOP");
             
             LCD_WR(0);//_WR=0; 
             DATAOUT(cmd);//asm("NOP"); //P1=cmd;
             LCD_WR(1);//_WR=1; 
             SSD1963_CS(1);//CSB=1;
             
}     

void SSD1963Data_8(u8 sdata)
{      LCD_RS(1);
             SSD1963_CS(0);//asm("NOP");asm("NOP");
             //DCX=0; 
             LCD_WR(0);//_WR=0; 
             DATAOUT(sdata);//asm("NOP"); //P1=cmd;
             LCD_WR(1);//_WR=1; 
             SSD1963_CS(1);//CSB=1;
}
void SSD1963Data_8tt(u8 sdata)
{      //LCD_RS(1);
          //   SSD1963_CS(0);//asm("NOP");asm("NOP");
             //DCX=0; 
           // LCD_WR(0);//_WR=0; 
			 GPIOA->BSRR = 0X20;
			 DATAOUT(sdata);//asm("NOP"); //P1=cmd;
             GPIOA->BRR = 0X20;//ResetBits
	
		//	 LCD_WR(1);//_WR=1; 
          //   SSD1963_CS(1);//CSB=1;
}
//--------------------------------------------------------------------------------

void SSD1963GPIOOUTCOM (u8 instru_H)
{ 
	u8 i=0;
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x05);    //SDI=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x01);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x05);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(instru_H & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		instru_H=instru_H<<1;
	} 
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);
  	
}


void SSD1963GPIOOUTDATA_16 (u16 instru)  
{ 
	u8 i=0;
	u8 instru_H,instru_L;

	instru_L=instru;
	instru_H=instru>>8;

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x03);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x07);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(instru_L & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		instru_L=instru_L<<1;
	} 

	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x03);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x07);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(instru_H & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		instru_H=instru_H<<1;
	}
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);
  	
}

void SSD1963toSSD2825DATA_8BIT(u8 data)
{
	u8 i=0;

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x03);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x07);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(data & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		data=data<<1;
	} 
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);		//CS  = 1	
}

void SSD1963GPIOOUTDATA_8 (u8 instru_L,u8 instru_H)  
{ 
	u8 i=0;

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x03);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x07);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(instru_L & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		instru_L=instru_L<<1;
	} 

	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x03);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x07);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(instru_H & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		instru_H=instru_H<<1;
	}
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);
  	
}

void SSD1963GPIOOUTDATA_18 (u8 instru_L)  		 //通过SSD1963的GPIO 以三线串行方式送数据
{ 
	u8 i=0;

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x03);    //sck=0
	
	SSD1963Command_8(0xBa);  
	SSD1963Data_8(0x07);     //sck=1
	
	for (i=0;i<8;i++)     // 
	{ 
		if( !(instru_L & 0x80))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}
		
		instru_L=instru_L<<1;
	} 
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);
  	
}

void SPISDASetInput(void)
{
	GPIO_InitTypeDef QGPIO_InitStructure;
	QGPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	QGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; //GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &QGPIO_InitStructure);
}
void SPISDASetOutput(void)
{
	GPIO_InitTypeDef QGPIO_InitStructure;
	QGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	QGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	QGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  		//OUTPUT
	GPIO_Init(GPIOA, &QGPIO_InitStructure);
}

u16 SSD2825_READ(u8 CMD)
{
	u8 i;
	u8 mm_L,mm_H;
	u8 Parameter=0xfa;
		
	mm = 0;

	SPISDASetInput();
	SPISDASetInput();

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
    SSD1963Data_8(0x05);    //SDI=0
    SSD1963Command_8(0xBa); 
    SSD1963Data_8(0x01);    //sck=0

    SSD1963Command_8(0xBa);  
    SSD1963Data_8(0x05);     //sck=1

	for (i=0;i<8;i++)     // 
         { 
          if( !(CMD & 0x80))
              { 
			  	SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x05);    //SDI=0
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
  
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
              }
          else
          	  { 
			  	SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x07);    //SDI=1
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x03);    //sck=0
  
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x07);     //sck=1
               }
                
          CMD=CMD<<1;
         }	

	SSD1963Command_8(0xBa); 
    SSD1963Data_8(0x05);    //SDI=0
    SSD1963Command_8(0xBa); 
    SSD1963Data_8(0x01);    //sck=0

    SSD1963Command_8(0xBa);  
    SSD1963Data_8(0x05);     //sck=1
		 
	for (i=0;i<8;i++)     // 
         { 
          if( !(Parameter & 0x80))
              { 
			  	SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x05);    //SDI=0
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
  
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
              }
          else
          	  { 
			  	SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x07);    //SDI=1
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x03);    //sck=0
  
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x07);     //sck=1
               }
                
          Parameter=Parameter<<1;
         }
		 
	for (i=0;i<8;i++)     // 
         { 
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
//				MDelay(1);
				mm_L<<=1;
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
				
				mm_L=mm_L+GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//				MDelay(1);
         }
	for (i=0;i<8;i++)     // 
         { 
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
//				MDelay(1);
				mm_H<<=1;
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
				
				mm_H=mm_H+GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//				MDelay(1);
         }
	mm = mm_H;
	mm = mm_L + (mm<<8);

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0f);   //CS=1

	SPISDASetOutput();
	SPISDASetOutput();

	return mm;		 	 		 	
}

u16 SSD2825_READ_CONTINUE(u8 CMD)
{
	u8 i;
	u8 mm_L,mm_H;
//	u8 Parameter = 0xfa;
		
	mm = 0;

	SPISDASetInput();
	SPISDASetInput();

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
	SSD1963Command_8(0xBa); 
    SSD1963Data_8(0x05);    //SDI=0
    SSD1963Command_8(0xBa); 
    SSD1963Data_8(0x01);    //sck=0

    SSD1963Command_8(0xBa);  
    SSD1963Data_8(0x05);     //sck=1

	for (i=0;i<8;i++)     // 
         { 
          if( !(CMD & 0x80))
              { 
			  	SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x05);    //SDI=0
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
  
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
              }
          else
          	  { 
			  	SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x07);    //SDI=1
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x03);    //sck=0
  
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x07);     //sck=1
               }
                
          CMD=CMD<<1;
         }	
		 
	for (i=0;i<8;i++)     // 
         { 
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
//				MDelay(1);
				mm_L<<=1;
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
				
				mm_L=mm_L+GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//				MDelay(1);
         }
	for (i=0;i<8;i++)     // 
         { 
                SSD1963Command_8(0xBa); 
                SSD1963Data_8(0x01);    //sck=0
//				MDelay(1);
				mm_H<<=1;
                SSD1963Command_8(0xBa);  
                SSD1963Data_8(0x05);     //sck=1
				
				mm_H=mm_H+GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//				MDelay(1);
         }
	mm = mm_H;
	mm = mm_L + (mm<<8);

	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0f);   //CS=1

	SPISDASetOutput();
	SPISDASetOutput();

	return mm;		 	 		 	
}


/*******************************************************************************
* Function Name  : ENTER_LP_mode()
* Description    : set MIPI lane enter LP mode...
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENTER_LP_mode(void)
{
	
//	SSD1963Command_8(0xb9); //PLL disable 
//	SSD1963GPIOOUTDATA_16(0x0000);
//	SSD1963GPIOOUTDATA_18(0xba); //PLL setting 
//	SSD1963GPIOOUTDATA_16(0x400a);
//	SSD1963GPIOOUTDATA_18(0xbb); //LP clock divider 
//	SSD1963GPIOOUTDATA_16(0x0005);
//	SSD1963GPIOOUTDATA_18(0xb9); //PLL enable 
//	SSD1963GPIOOUTDATA_16(0x4001); //4分频，SYS_CLK输出24/4=6MHZ
//	Delay(1);
	SSD1963GPIOOUTCOM(0xb7); //DCS mode, LP mode
	SSD1963GPIOOUTDATA_16(0x0752); //short packet 
	Delay(1);
}

/*******************************************************************************
* Function Name  : ENTER_READ_mode()
* Description    : set MIPI lane D0   read mode...
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENTER_READ_mode(void)
{
	SSD1963GPIOOUTCOM(0xb7);
	SSD1963GPIOOUTDATA_16(0x03c2); //////enter read mode      for generic read					 調試OK，2013-10-24，，必須設置為0X03C2..
//	SSD1963GPIOOUTDATA_16(0x0382);
}

void ENTER_hs_mode(void)
{
//		SSD1963GPIOOUTCOM(0xb9);		//PLL disable 
//		SSD1963GPIOOUTDATA_16(0x0000);
//		Delay(1);
//			
//			SSD1963GPIOOUTCOM(0xba);		//PLL setting 
//		SSD1963GPIOOUTDATA_16(0x8016);			
//
//			SSD1963GPIOOUTCOM(0xbb);		//LP clock divider 
//		SSD1963GPIOOUTDATA_16(0x0007);			
//
//			SSD1963GPIOOUTCOM(0xb9);		//PLL enable 
//		SSD1963GPIOOUTDATA_16(0x4001);
//		Delay(1);

	SSD1963GPIOOUTCOM(0xb7);		//Generic mode, HS video mode 
	SSD1963GPIOOUTDATA_16(0x070b);	
}



void MIPI_Protocol_Write(void)
{
    uint k=0,j=2;
    uint Word_Count=(MIPI_Array[0]-0x01);
    
         SSD1963GPIOOUTCOM(0xBC);    
         SSD1963GPIOOUTDATA_16(Word_Count);  
         SSD1963GPIOOUTCOM(0xBF);     

         for(k=0;k<Word_Count;k++)
         {
            SSD1963GPIOOUTDATA_18(MIPI_Array[j]);
            j++;
         }  
   //      LCD_CSB = 1;    
}

void MIPI_SPI_Write(uint i,...)
{
        uint MIPI_Counter;
         va_list num_list;
         va_start(num_list,i);
         MIPI_Array[0]=i;
        for(MIPI_Counter=1;MIPI_Counter<=i;MIPI_Counter++)
        {
            MIPI_Array[MIPI_Counter]=(va_arg(num_list,uint));
        }
        MIPI_Protocol_Write();
        va_end(num_list);       
}
/*******************************************************************************
* Function Name  : TC358768_DCS_write_1A_0P()
* Description    : send the   IC register  which will return value data..... 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TC358768_DCS_write_1A_0P(u8 cmd,u8 cmd1)
{

	//////#password(Enable user command) 
			SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0004);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB9);
	SSD1963GPIOOUTDATA_18(0xf1);	  //
	SSD1963GPIOOUTDATA_18(0x08);	  //
	SSD1963GPIOOUTDATA_18(0x00);
//	Delay(1);
//	//////#password(Disable user command) 
//			SSD1963GPIOOUTCOM(0xBC);		               
//	SSD1963GPIOOUTDATA_16(0x0004);						 
//			SSD1963GPIOOUTCOM(0xbf);
//	SSD1963GPIOOUTDATA_18(0xB9);
//	SSD1963GPIOOUTDATA_18(0x00);
//	SSD1963GPIOOUTDATA_18(0x00);
//	SSD1963GPIOOUTDATA_18(0x00);

	SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0001);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(cmd);
	SSD1963GPIOOUTDATA_18(cmd1);

}
/*******************************************************************************
* Function Name  : READ_IC
* Description    : read data from IC register...
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 READ_IC(u8 cmd,u8 cmd1)
{
	u16 Value=0;
	u8 TIMES=0;
		
xxxxx:
	ENTER_LP_mode(); //enter  LP mode
	MDelay(10);

//	SSD1963GPIOOUTCOM(0xc4);		//enable BTA
//	SSD1963GPIOOUTDATA_16(0x0001);

	SSD1963GPIOOUTCOM(0xc1);		//Max. return packet is 255 bytes
	SSD1963GPIOOUTDATA_16(0x0004);

	ENTER_READ_mode(); //enter read mode	
	MDelay(5);


	TC358768_DCS_write_1A_0P(cmd,cmd1); //sent the read register...	
	MDelay(5);

	while(((SSD2825_READ(0xc6))&0x0001)!=1)
	{
		if(TIMES>5)
		{
			goto xxxxx;
		}	
//		ENTER_hs_mode();
//		MDelay(1);
		MDelay(5);
		TIMES++;
		
	}
//	while(((SSD2825_READ(0xc6))&0x0001)==1) 
	//读SSD2825寄存器 /// 
//	////判断READ Ready?//
//	//0xC6 Bit[0] = 0   – Not ready
//	//////0xC6 Bit[0] = 1 – Ready
	{
		MDelay(1);
		Value = SSD2825_READ(0xff); //读SSD2825寄存器0XFF,the return value is IC register value
		MIPI_READ_DATA[0] = Value;
		MIPI_READ_DATA[1] = Value>>8;
		Value = SSD2825_READ_CONTINUE(0xfA); 
		MIPI_READ_DATA[2] = Value;
		MIPI_READ_DATA[3] = Value>>8;

//		Value = SSD2825_READ_CONTINUE(0xfA); 
//		MIPI_READ_DATA[4] = Value;
//		MIPI_READ_DATA[5] = Value>>8;
//		Value = SSD2825_READ_CONTINUE(0xfA);
//		MIPI_READ_DATA[6] = Value;
//		MIPI_READ_DATA[7] = Value>>8;
//		Value = SSD2825_READ_CONTINUE(0xfA);
//		MIPI_READ_DATA[8] = Value;
//		MIPI_READ_DATA[9] = Value>>8;
	}
	ENTER_hs_mode();
	return Value;
}

/*******************************************************************************
* Function Name  : READ_IC_BACKNUM
* Description    : read data from IC register...
* Input          : u8 back_num------>set  how many num data turn back..   back_num   > 2;
* Output         : None
* Return         : None
*******************************************************************************/
u8 READ_IC_BACKNUM(u8 cmd,u8 back_num)
{
	u16 Value=0;
	u8 TIMES=0;
	u8 i=0;
		
xxxxx:
	ENTER_LP_mode(); //enter  LP mode
	MDelay(10);

//	SSD1963GPIOOUTCOM(0xc4);		//enable BTA
//	SSD1963GPIOOUTDATA_16(0x0001);

	SSD1963GPIOOUTCOM(0xc1);		//Max. return packet is 255 bytes
	SSD1963GPIOOUTDATA_16(back_num);

	ENTER_READ_mode(); //enter read mode	
	MDelay(5);

	MIPI_SPI_Write(0x02,0x29,cmd);
	MDelay(5);
	
//	TC358768_DCS_write_1A_0P(cmd,cmd1); //sent the read register...	
//	MDelay(5);

	while(((SSD2825_READ(0xc6))&0x0001)!=1)
	{
		if(TIMES>5)
		{
			Value = 0;
			goto ENDXXXX;
		}	
//		ENTER_hs_mode();
//		MDelay(1);
		MDelay(5);
		TIMES++;
		
	}
//	while(((SSD2825_READ(0xc6))&0x0001)==1) 
	//读SSD2825寄存器 /// 
//	////判断READ Ready?//
//	//0xC6 Bit[0] = 0   – Not ready
//	//////0xC6 Bit[0] = 1 – Ready
	{
		MDelay(1);
		Value = SSD2825_READ(0xff); //读SSD2825寄存器0XFF,the return value is IC register value
		MIPI_READ_DATA[0] = Value;
		MIPI_READ_DATA[1] = Value>>8;
		
		for(i=2;i<=back_num-1;i=i+2)
		{
			Value = SSD2825_READ_CONTINUE(0xfA); 
			MIPI_READ_DATA[i] = Value;
			MIPI_READ_DATA[i+1] = Value>>8;
		}
//		Value = SSD2825_READ_CONTINUE(0xfA); 
//		MIPI_READ_DATA[4] = Value;
//		MIPI_READ_DATA[5] = Value>>8;
//		Value = SSD2825_READ_CONTINUE(0xfA);
//		MIPI_READ_DATA[6] = Value;
//		MIPI_READ_DATA[7] = Value>>8;
//		Value = SSD2825_READ_CONTINUE(0xfA);
//		MIPI_READ_DATA[8] = Value;
//		MIPI_READ_DATA[9] = Value>>8;
	}
	ENDXXXX:	
	
	ENTER_hs_mode();
	return Value;
}

u8 READ_IC_A(u8 cmd,u8 cmd1)
{
	u16 Value=0;
	u8 TIMES=0;


	MIPI_SPI_Write(0x03,0x39,0x00,0x00);
	MIPI_SPI_Write(0x03,0x39,0xDb,0x92);


xxxxx:
	ENTER_READ_mode(); 
	MDelay(10);

	
//  SSD1963GPIOOUTCOM(0xb7);        //Generic mode, HS video mode
//       SSD1963GPIOOUTDATA_16(0x03f);
        
//	SSD1963GPIOOUTCOM(0xc4);		//enable BTA
//	SSD1963GPIOOUTDATA_16(0x0001);

	SSD1963GPIOOUTCOM(0xc1);		//Max. return packet is 255 bytes
	SSD1963GPIOOUTDATA_16(0x0001);

	MIPI_SPI_Write(0x02,0x29,cmd);
	MDelay(2);
//	ENTER_READ_mode(); //enter read mode	
	

//	MIPI_SPI_Write(0x03,0x39,0x00,cmd1);
//	MIPI_SPI_Write(0x03,0x39,cmd,cmd1);

//	TC358768_DCS_write_1A_0P(cmd,cmd1); //sent the read register...	
	MDelay(2);
	SSD1963GPIOOUTCOM(0xD4);        //SPI read register
    SSD1963GPIOOUTDATA_16(0x00FA);     //READ 

	while(((SSD2825_READ(0xc6))&0x0001)!=1)
	{
		if(TIMES>5)
		{
			goto xxxxx;
		}	
//		ENTER_hs_mode();
//		MDelay(1);
		TIMES++;
		
	}
//	while(((SSD2825_READ(0xc6))&0x0001)==1) 
	//读SSD2825寄存器 /// 
//	////判断READ Ready?//
//	//0xC6 Bit[0] = 0   – Not ready
//	//////0xC6 Bit[0] = 1 – Ready

		MDelay(1);
		Value = SSD2825_READ(0xff); //读SSD2825寄存器0XFF,the return value is IC register value
		MIPI_READ_DATA[0] = Value;
		MIPI_READ_DATA[1] = Value>>8;


	ENTER_hs_mode();
	return Value;
}



u8 READ_VCOM_OTP_TIMES_HX8369(u8 reg_index)
{
	ENTER_LP_mode(); //enter  LP mode
	MDelay(20);
	MIPI_SPI_Write(0x05,0x39, 0xB9,0xFF,0x83,0x69);//EXTC Command Set enable register
	MDelay(10);
	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,reg_index,0X00);//SET OTP_INDEX
	MDelay(10);
	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,reg_index,0X20);//SET OTP_POR
	MDelay(10);
	MIPI_SPI_Write(0x06,0x39, 0xBB,0x00,0x00,reg_index,0X00);//CLEAR OTP_POR
	MDelay(10);
	READ_IC_BACKNUM(0xBB,5);
	
	return 0;
}



void SSD1963INITSSD2825(void)		//通过SSD1963的GPIO以三线串行方式8bit对SSD2825进行初始化
{    
	u16 data = 0x400a;
	u16 VHPW,VHBP,VHFP;

	VHPW=(VPW<<8)+HPW;
	VHBP=(VBP<<8)+HBP;
	VHFP=(VFP<<8)+HFP;


		SSD1963GPIOOUTCOM(0xc0);
		SSD1963GPIOOUTDATA_16(0x0100);
		Delay(20);
	
	while( SSD2825_READ(0XB0) != 0x2828)
	{
		SSD1963GPIOOUTCOM(0xc0);
			SSD1963GPIOOUTDATA_16(0x0100);		
			Delay(20);
	}

			SSD1963GPIOOUTCOM(0xB1);
		SSD1963GPIOOUTDATA_16(VHPW);		//Vertical sync and horizontal sync active period 

			SSD1963GPIOOUTCOM(0xB2);
		SSD1963GPIOOUTDATA_16(VHBP);		//Vertical and horizontal back porch period 

			SSD1963GPIOOUTCOM(0xB3);
		SSD1963GPIOOUTDATA_16(VHFP);		//Vertical and horizontal front porch period 

			SSD1963GPIOOUTCOM(0xB4);
		SSD1963GPIOOUTDATA_16(HDP);		//Horizontal active period 

			SSD1963GPIOOUTCOM(0xB5);
		SSD1963GPIOOUTDATA_16(VDP);		//Vertical active period 

			SSD1963GPIOOUTCOM(0xB6);		//Video mode and video pixel format
		SSD1963GPIOOUTDATA_16(0x0007);		//24bit 

			SSD1963GPIOOUTCOM(0xDE);		
		SSD1963GPIOOUTDATA_16(value_Lane_select - 1);		//MIPI lane select  

			SSD1963GPIOOUTCOM(0xd6);	
		SSD1963GPIOOUTDATA_16(0x0001);		//Color order and endianess 



//	{
//            SSD1963GPIOOUTCOM(0xC9);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x2302);      ///////default  = 0x1402
//  
//            SSD1963GPIOOUTCOM(0xCA);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x2301);      ///////default  = 0x2803
//            
//            SSD1963GPIOOUTCOM(0xCB);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x0510);      ///////default  = 0x0416
//            
//            SSD1963GPIOOUTCOM(0xCC);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x1005);      ///////default  = 0x0a0a
//            
//            SSD1963GPIOOUTCOM(0xD6);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x0006);      ///////default  = 
//            
//            SSD1963GPIOOUTCOM(0xC4);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x0001);
//            
//            SSD1963GPIOOUTCOM(0xEB);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x8000);
//            
//            SSD1963GPIOOUTCOM(0xDD);		//add  -2016-3-3
//            SSD1963GPIOOUTDATA_16(0x0000);      ///////default  = 0x0000

//        }  



			SSD1963GPIOOUTCOM(0xb9);		//PLL disable 
		SSD1963GPIOOUTDATA_16(0x0000);
		Delay(1);
			
				data = value_HighSpeed/Freq_OSC;

		if(value_HighSpeed < 125 )
		{
			data =  data;	
		}
		else if(value_HighSpeed < 250 )
		{
			data = 0x4000 + data;	
		}
		else  if(value_HighSpeed < 500 )
		{
			data = 0x8000 + data;	
		}
		else
		{
			data = 0xc000 + data;	
		}
				
		SSD1963GPIOOUTCOM(0xba);		//PLL setting 
		SSD1963GPIOOUTDATA_16(data);

			SSD1963GPIOOUTCOM(0xbb);		//LP clock divider 
		SSD1963GPIOOUTDATA_16(0x0006);			

			SSD1963GPIOOUTCOM(0xb9);		//PLL enable 
		SSD1963GPIOOUTDATA_16(0x4001);
		Delay(1);

			SSD1963GPIOOUTCOM(0xb8);		//VC register 
		SSD1963GPIOOUTDATA_16(0x0000);

			SSD1963GPIOOUTCOM(0xb7);		//Generic mode, HS video mode
		SSD1963GPIOOUTDATA_16(0x0752);		//    			generic		long packet	  0X39-->0X0752

		Delay(5);

}

void VIDEO_ON(void)
{
			SSD1963GPIOOUTCOM(0xb7);		//Generic mode, HS video mode 
		SSD1963GPIOOUTDATA_16(0x070B);
		
		Delay(5);
}


/////////////GPI009A0  USE///    before  OTP   //
void INITIALOTM8018B(void)
{
		MIPI_SPI_Write(0x05,0x39,0xB9, 0xFF,0x83,0x69);//-----Enable extension command
		Delay(5);

		MIPI_SPI_Write(0x0E,0x39,0xBA, 0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6C,0x02,0x11,0x18,0x40);//2 lane mode
//		Delay(10);
  
		MIPI_SPI_Write(0x17,0x39,0xB1, 0x01,0x00,0x14,0x06,0x00,0x11,0x11,0x32,0x3A,0x23,0x23,0x07,0x23,0x07,0x23,0x01,0xE6,0xE6,0xE6,0xE6,0xE6);//-----Set Power
//		Delay(5);
			
//		MIPI_SPI_Write(0x0F,0x39,0xB2, 0x00,0x20,0x03,0x03,0x70,0x00,0xFF,0x00,0x00,0x00,0x03,0x03,0x00,0x01);//DSI CMD MODE
//		Delay(5);

		MIPI_SPI_Write(0x11,0x39,0xB2, 0x00,0x23,0x0A,0x0A,0x70,0x00,0xFF,0x00,0x00,0x00,0x00,0X03,0x03,0x00,0x01);//DSI Video MODE
////		Delay(5);
//	    MIPI_SPI_Write(0x11,0x39,0xB2, 0x00,0x23,0x20,0x20,0x70,0x00,0xFF,0x00,0x00,0x00,0x00,0X03,0x03,0x00,0x01);//DSI Video MODE
////		Delay(5);

		MIPI_SPI_Write(0x07,0x39,0xB4, 0x00,0x1C,0x84,0x06,0x02);//set CYC

//		MIPI_SPI_Write(0x03,0x39,0xB6,0x28,0x28);//Set VCOM----------0x01
//		Delay(5);
	
		MIPI_SPI_Write(0x1C,0x39,0xD5, 0x00,0x00,0x03,0x35,0x01,0x01,0x10,0x70,0x17,0x27,0x20,0x31,0x64,0xA8,0x75,0xB9,0x31,0x20,0x9B,0x57,0x8A,0x46,0x07,0x0F,0x02,0x82);//Set GIP
		
//		//--------------Panel setting---------------------//
		MIPI_SPI_Write(0x24,0x39,0xE0, 0x00,0x08,0x08,0x3C,0x3C,0x3F,0x28,0x49,0x0F,0x0F,0x0C,0x10,0x12,0x12,0x13,0x18,0x1E,0x00,0x08,0x08,0x3C,0x3C,0x3F,0x28,0x49,0x0F,0x0F,0x0C,0x10,0x12,0x12,0x13,0x18,0x1E);
		Delay(5);//Set Gamma

//		MIPI_SPI_Write(0x15,0x39,0xCC, 0x02);//Set Panel   02
//		Delay(5);

//		MIPI_SPI_Write(0x03,0x39,0xB6, 0x23);//-----璝璶縉Vcom, 硂琿Initial Code璶Mark奔 
//		Delay(10);

		
//		MIPI_SPI_Write(0x03,0x39,0x3A, 0x77);//Set Pixel format
		Delay(10);
		
		MIPI_SPI_Write(0x03,0x39,0x5E, 0x7F);//CABC  read  test  Alan
		Delay(5);
		
		MIPI_SPI_Write(0x03,0x39,0x36, 0x00);//Set Pixel format
		Delay(5);


		MIPI_SPI_Write(0x02,0x05,0x11);//sleep out
		Delay(15);

        MIPI_SPI_Write(0x02,0x05,0x29);//Display on					   
		Delay(1);
}

/////////////GPI009A0  USE///    after  OTP   //
void INITIALOTM8018B_QC(void)
{

	{     ///////OTP后的初始化code
 		MIPI_SPI_Write(0x05,0x39, 0xB9,0xFF,0x83,0x69);//EXTC Command Set enable register
		MIPI_SPI_Write(0x0E,0x39,0xBA, 0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6C,0x02,0x11,0x18,0x40);//2 lane mode
	}
		
		Delay(1);
		MIPI_SPI_Write(0x02,0x05,0x11);//sleep out
		Delay(15);

        MIPI_SPI_Write(0x02,0x05,0x29);//Display on					   
		Delay(1);
}

/////////////GPM1461A0  USE/////
//void INITIALOTM8018B(void)
//{
////////////////////////////////////////////////////////////////////////
////Panel:CPT_045LG 480X854
////VPNL :3.3V
////VDDI :3.3V
////Pump :X2
////VGH  :15.38V
////VGL  :-7.85V  <- The flicker is minimum if VGL is -7.85V
////GVDD :4.5V    <- To change voltage from 5V to 4.8V Because of gray level inversion
////NGVDD:-4.5V   <- To change voltage from -5V to -4.8V Because of gray level inversion
////////////////////////////////////////////////////////////////////////
////--------------Command 2 enable---------------------//
//		MIPI_SPI_Write(0x03,0x39,0x00,0x00);
//		MIPI_SPI_Write(0x05,0x39,0xFF,0x80,0x09,0x01);
//		
//		MIPI_SPI_Write(0x03,0x39,0x00,0x80);
//		MIPI_SPI_Write(0x04,0x39,0xFF,0x80,0x09);
//		
////	MIPI_SPI_Write(0x03,0x39,0x00,0x03);
////	MIPI_SPI_Write(0x03,0x39,0xFF,0x01);
////	//--------------Panel setting---------------------//
////	MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	MIPI_SPI_Write(0x08,0x39,0xC5,0x16,0x82,0x02,0x03,0x33,0x34);//ripple pass
////	
////	//MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	//MIPI_SPI_Write(0x08,0x39,0xC5,0x96,0x82,0x02,0x03,0x33,0x34);//audible noise pass
////	
////	//MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	//MIPI_SPI_Write(0x05,0x39,0xC5,0x96,0x82,0x00);//audible noise NG
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xB2);
////	MIPI_SPI_Write(0x03,0x39,0xC5,0x08);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x00);
////	MIPI_SPI_Write(0x04,0x39,0xD8,0x6f,0x6f);
////	
//////		MIPI_SPI_Write(0x03,0x39,0x00,0x00);
//////		MIPI_SPI_Write(0x03,0x39,0xD9,0x33);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x82);
////	MIPI_SPI_Write(0x03,0x39,0xC5,0xA3);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x81);
////	MIPI_SPI_Write(0x03,0x39,0xC1,0x55);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xB4);
////	MIPI_SPI_Write(0x04,0x39,0xC0,0x50,0x18);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x81);
////	MIPI_SPI_Write(0x03,0x39,0xC4,0x83);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x92);//480x854
////	MIPI_SPI_Write(0x03,0x39,0xB3,0x45);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	MIPI_SPI_Write(0x04,0x39,0xB3,0x02,0x20);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	MIPI_SPI_Write(0x08,0x39,0xC0,0x00,0x44,0x00,0x00,0x00,0x03);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xA0);
////	MIPI_SPI_Write(0x03,0x39,0xC1,0xEA);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xC0);
////	MIPI_SPI_Write(0x03,0x39,0xC5,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x8B);
////	MIPI_SPI_Write(0x03,0x39,0xB0,0x40);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xB1);
////	MIPI_SPI_Write(0x03,0x39,0xB0,0x80);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x88);
////	MIPI_SPI_Write(0x03,0x39,0xC4,0x80);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xA6);
////	MIPI_SPI_Write(0x05,0x39,0xC1,0x01,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xB2);
////	MIPI_SPI_Write(0x06,0x39,0xF5,0x15,0x00,0x15,0x00);
////	//--------------Gamma setting------------------------//
////	MIPI_SPI_Write(0x03,0x39,0x00,0x00); 
////	MIPI_SPI_Write(0x12,0x39,0xE1,0x05,0x0e,0x14,0x0d,0x06,0x0f,0x0b,0x0a,0x04,0x07,0x0c,0x08,0x0f,0x10,0x0a,0x05);
////	                         
////	MIPI_SPI_Write(0x03,0x39,0x00,0x00); 
////	MIPI_SPI_Write(0x12,0x39,0xE2,0x05,0x0e,0x14,0x0d,0x06,0x0f,0x0b,0x0a,0x04,0x07,0x0c,0x08,0x0f,0x10,0x0a,0x05); 
////	//----------------ID setting-----------------//
////	//MIPI_SPI_Write(0x03,0x39,0x00,0x00);
////	//MIPI_SPI_Write(0x03,0x39,0xD0,0xD5);
////	//
////	//MIPI_SPI_Write(0x03,0x39,0x00,0x00);
////	//MIPI_SPI_Write(0x04,0x39,0xD1,0x80,0xCA);
////	//---------------GOA setting-----------------//
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x80);
////	MIPI_SPI_Write(0x08,0x39,0xCE,0x87,0x03,0x00,0x86,0x03,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	MIPI_SPI_Write(0x08,0x39,0xCE,0x33,0x55,0x00,0x33,0x56,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xA0);
////	MIPI_SPI_Write(0x10,0x39,0xCE,0x38,0x03,0x03,0x54,0x00,0x00,0x00,0x38,0x02,0x03,0x55,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xB0);
////	MIPI_SPI_Write(0x10,0x39,0xCE,0x38,0x01,0x03,0x56,0x00,0x00,0x00,0x38,0x00,0x03,0x57,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xC0);
////	MIPI_SPI_Write(0x10,0x39,0xCE,0x30,0x00,0x03,0x58,0x00,0x00,0x00,0x30,0x01,0x03,0x59,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xD0);
////	MIPI_SPI_Write(0x10,0x39,0xCE,0x30,0x02,0x03,0x5A,0x00,0x00,0x00,0x30,0x03,0x03,0x5B,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xC0);
////	MIPI_SPI_Write(0x0C,0x39,0xCF,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x10);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xC0);
////	MIPI_SPI_Write(0x11,0x39,0xCB,0x00,0x14,0x14,0x14,0x14,0x00,0x00,0x14,0x14,0x14,0x14,0x00,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xD0);
////	MIPI_SPI_Write(0x11,0x39,0xCB,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x00,0x00,0x14,0x14,0x14);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xE0);
////	MIPI_SPI_Write(0x0C,0x39,0xCB,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x80);
////	MIPI_SPI_Write(0x0C,0x39,0xCC,0x00,0x26,0x25,0x02,0x06,0x00,0x00,0x0A,0x0c,0x0e);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x90);
////	MIPI_SPI_Write(0x11,0x39,0xCC,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x25,0x01,0x05);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xA0);
////	MIPI_SPI_Write(0x11,0x39,0xCC,0x00,0x00,0x09,0x0b,0x0d,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xB0);
////	MIPI_SPI_Write(0x0C,0x39,0xCC,0x00,0x25,0x26,0x05,0x01,0x00,0x00,0x0D,0x0B,0x09);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xC0);
////	MIPI_SPI_Write(0x11,0x39,0xCC,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x26,0x06,0x02);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xD0);
////	MIPI_SPI_Write(0x11,0x39,0xCC,0x00,0x00,0x0E,0x0C,0x0A,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);
////	
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x89);//source pre-charge ON
////	MIPI_SPI_Write(0x03,0x39,0xc4,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x87);//source pre-charge ON
////	MIPI_SPI_Write(0x03,0x39,0xc4,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xa2);//source pre-charge width
////	MIPI_SPI_Write(0x05,0x39,0xc0,0x0b,0x1a,0x02);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xc6);//MIPI read delay
////	MIPI_SPI_Write(0x03,0x39,0xb0,0x03);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x80);//blanking & sleep in source pull-low
////	MIPI_SPI_Write(0x03,0x39,0xc4,0x30);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x8a);//blanking & sleep in source pull-low
////	MIPI_SPI_Write(0x03,0x39,0xc4,0x40);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xb6);//sleep out power on seq. 
////	MIPI_SPI_Write(0x03,0x39,0xf5,0x06);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0x92);//sleep out power on seq. 
////	MIPI_SPI_Write(0x09,0x39,0xf5,0x0e,0x14,0x0e,0x14,0x00,0x14,0x00);
////	
////	MIPI_SPI_Write(0x03,0x39,0x00,0xba);//sleep out power on seq. 
////	MIPI_SPI_Write(0x04,0x39,0xf5,0x00,0x00);
//	
//	//---------------------------------------------------//
//	//MIPI_SPI_Write(0x03,0x39,0x00,0x00);
//	//MIPI_SPI_Write(0x05,0x39,0xFF,0xFF,0xFF,0xFF);  
////	//---------------------------------------------------//
////		MIPI_SPI_Write(0x03,0x39,0x00,0x91);
////		MIPI_SPI_Write(0x03,0x39,0xB3,0x20);    
////		
////		Delay( 20);
//		MIPI_SPI_Write(0x02,0x05,0x11);
//		
//		Delay( 20);
//		MIPI_SPI_Write(0x02,0x05,0x29);  
//			Delay( 10);
//}









/////////////CLAN043LL  USE/////
/////#password(Enable user command) 
u8 SC7789_A_B9[] = {0xB9,0xF1,0x08,0x00};
/////#VDDD
u8 SC7789_A_BC[] = {0xBC,0x67};
/////#set DSI (2Lane)
u8 SC7789_A_BA[] = {0xBA,0x31,0x00,0x44,0x25,0x91,0x0A,0x00,0x00,0xC2,0x34,0x00,0x00,0x04,0x02,0x1D,0xB9,0xEE,0x40};
//#                 Reg,  01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   
//#                                                                   34(DTC)       
/////#set panel(Enable ESD finction)
u8 SC7789_A_C6[] = {0xC6,0x00,0x00,0xFF};
/////#set panel(Backward,TN)
u8 SC7789_A_CC_B[] = {0xCC,0x0C};
/////#set panel(Forward,TN)
u8 SC7789_A_CC_F[] = {0xCC,0x00};
/////#set DISP(480RGB*800)
u8 SC7789_A_B2[] = {0xB2,0x23};
/////#set (column-00),(1dot-01),(2dot-02),(4dot-03),(8dot-04)
u8 SC7789_A_B4[] = {0xB4,0x00};
/////#set RGB 
u8 SC7789_A_B3[] = {0xB3,0x01,0x00,0x06,0x06,0x10,0x0A,0x45,0x40};
////#                Reg,  01   02   03   04   05   06   07   08  
/////#set GIP
u8 SC7789_A_E9[] = {0xE9,0x00,0x00,0x08,0x03,0x2F,0x89,0x6A,0x12,0x31,0x23,0x48,0x0C,0x89,0x6A,0x47,0x02,0x04,0x00,0x00,0x00,0x00,0x20,0x88,0x88,0x40,0x28,0x69,0x48,0x88,0x88,0x80,0x88,0x88,0x51,0x38,0x79,0x58,0x88,0x88,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
////#                Reg,  01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34   35   36   37   38   39   40   41   42   43   44   45   46   47   48   49   50   51 
/////#set GIP
u8 SC7789_A_EA[] = {0xEA,0x88,0x88,0x37,0x59,0x18,0x18,0x88,0x88,0x85,0x88,0x88,0x26,0x49,0x08,0x08,0x88,0x88,0x84,0x30,0x00,0x00,0xFF,0x00,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
////#                Reg,  01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34   35   36 
/////#set power
u8 SC7789_A_B1[] = {0xB1,0x22,0x1B,0x1B,0xB7,0x22,0x02,0xA8};
////#                Reg,  01   02   03   04   05   06   07 
////#                           VRP VRN  VSP/N FS2/1 AP  Vc 
/////#set Source EQ
u8 SC7789_A_E3[] = {0xE3,0x02,0x02,0x02,0x02};
/////#Set Power_ext
//u8 SC7789_A_B8[] = {0xB8,0x07,0x22};	   /////V3.1
//u8 SC7789_A_B8[] = {0xB8,0x03,0x22};	   /////V4.1   2013/11/1
u8 SC7789_A_B8[] = {0xB8,0x07,0x22};	   /////V4.2   2013/11/12
/////#VREF
u8 SC7789_A_B5[] = {0xB5,0x09,0x09};
/////#set Bias
u8 SC7789_A_C0[] = {0xC0,0x73,0x50,0x00,0x08,0x70};
/////#set gamma
u8 SC7789_A_E0[] = {0xE0,0x00,0x00,0x00,0x04,0x04,0x0A,0x18,0x2B,0x05,0x0C,0x11,0x16,0x18,0x16,0x16,0x15,0x19,0x00,0x00,0x00,0x04,0x04,0x0A,0x18,0x2C,0x05,0x0C,0x12,0x16,0x18,0x16,0x17,0x16,0x19};
////#                      01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31   32   33   34
////#                    PVR0 PVR1 PVR2 PVR3 PVR4 PVR5           PPK0 PPK1 PPK2 PPK3 PPK4 PPK5 PPK6 PPK7 PPK8 NVR0 NVR1 NVR2 NVR3 NVR4 NVR5           NPK0 NPK1 NPK2 NPK3 NPK4 NPK5 NPK6 NPK7 NPK8 
////#                                                  PPR0 PPR1                                                                            NPR0 NPR1
////#                                               2D==>0.44
////#                                               38==>0.41
/////#DGC
u8 SC7789_A_C1[] = {0xC1,0x01,0x00,0x03,0x0B,0x17,0x20,0x26,0x30,0x36,0x3F,0x48,0x50,0x58,0x5F,0x68,0x71,0x79,0x82,0x89,0x91,0x99,
						//R01   02   03   04   05   06   07   08   09   10   11   12   13   14   15   16   17   18   19   20   21   
						 0xA1,0xA9,0xB0,0xB8,0xC0,0xC8,0xD0,0xD6,0xDC,0xE4,0xED,0xF7,0xFF,0x00,0x00,0x00,0xC0,0x0A,0x80,0x30,0xE8,
						//22   23   24   25   26   27   28   29   30   31   32   33   34   35   36   37   38   39   40   41   42   
						 0x00,0x00,0x03,0x0B,0x17,0x20,0x27,0x30,0x37,0x40,0x48,0x50,0x58,0x5F,0x67,0x70,0x78,0x80,0x88,0x90,0x98,
					   //G43   44   45   46   47   48   49   50   51   52   53   54   55   56   57   58   59   60   61   62   63   
						 0xA0,0xA8,0xAF,0xB7,0xBF,0xC7,0xCF,0xD5,0xDC,0xE4,0xED,0xF7,0xFF,0x00,0x00,0x00,0xC0,0x0A,0x80,0x30,0xE8,
						//64   65   66   67   68   69   70   71   72   73   74   75   76   77   78   79   80   81   82   83   84   
						 0x00,0x00,0x03,0x0D,0x19,0x23,0x29,0x31,0x38,0x41,0x48,0x4F,0x57,0x5D,0x65,0x6C,0x74,0x7B,0x84,0x8B,0x94,
					    //B85   86   87   88   89   90   91   92   93   94   95   96   97   98   99  100  101  102  103  104  105   
						 0x9B,0xA4,0xAA,0xB4,0xB9,0xC3,0xCC,0xD6,0xDC,0xE4,0xED,0xF7,0xFF,0x00,0x00,0x00,0xC0,0x0A,0x80,0x30,0xE8,0x00
						//106  107  108  109  110  111  112  113  114  115  116  117  118  119  120  121  122  123  124  125  126  127
						};

void SEND_MIPI_DATA(u8 sum,u8 data_buf[])
{
	u8 data_num = 0;
	u8 temp = 0;
	
	SSD1963GPIOOUTCOM(0xBC);		                
	SSD1963GPIOOUTDATA_16(sum);						  
			SSD1963GPIOOUTCOM(0xbf);
	for(data_num=0;data_num<sum;data_num++)
	{
		temp = data_buf[data_num];
		SSD1963GPIOOUTDATA_18(temp);
	}
}

void HX8369_cmd_4(u8 cmd,u8 num,u8 data,u8 data2)
{
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0004);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(cmd);
	SSD1963GPIOOUTDATA_18(num);
	SSD1963GPIOOUTDATA_18(data);	  //
	SSD1963GPIOOUTDATA_18(data2);
}

void SSD2825_CLAN043LL_SC7789_A(void)
{
	///////FOR CLAN043LL USE
/*//////#password(Enable user command) 
			SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0004);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB9);
	SSD1963GPIOOUTDATA_18(0xf1);	  //
	SSD1963GPIOOUTDATA_18(0x08);	  //
	SSD1963GPIOOUTDATA_18(0x00);
	*/
	
	SEND_MIPI_DATA(4,SC7789_A_B9);
	SEND_MIPI_DATA(2,SC7789_A_BC);
	SEND_MIPI_DATA(19,SC7789_A_BA);
	SEND_MIPI_DATA(4,SC7789_A_C6);
	SEND_MIPI_DATA(2,SC7789_A_CC_B);
//	SEND_MIPI_DATA(2,SC7789_A_CC_F);
	SEND_MIPI_DATA(2,SC7789_A_B2);
	SEND_MIPI_DATA(2,SC7789_A_B4);
	SEND_MIPI_DATA(9,SC7789_A_B3);
	SEND_MIPI_DATA(52,SC7789_A_E9);
	SEND_MIPI_DATA(37,SC7789_A_EA);
	SEND_MIPI_DATA(8,SC7789_A_B1);
	SEND_MIPI_DATA(8,SC7789_A_E3);
	SEND_MIPI_DATA(3,SC7789_A_B8);
	SEND_MIPI_DATA(3,SC7789_A_B5);
	SEND_MIPI_DATA(6,SC7789_A_C0);
	SEND_MIPI_DATA(35,SC7789_A_E0);
	SEND_MIPI_DATA(128,SC7789_A_C1);

	
	
//////#setting sleep out	
	SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0001);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x11);
	SSD1963GPIOOUTDATA_18(0x00);
	Delay(20);

///////#setting Display	
	SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0001);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x29);
	SSD1963GPIOOUTDATA_18(0x00);
	Delay(5);
}

void SSD2825INITIAL_HX8369B(void)	//通过SSD1963的GPIO以三线串行方式8bit把数据送给SSD2825，通过SSD2825对模组进行初始化
{    
	///////FOR GPM1316B0 USE
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0004);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB9);
	SSD1963GPIOOUTDATA_18(0xff);	  //
	SSD1963GPIOOUTDATA_18(0x83);	  //
	SSD1963GPIOOUTDATA_18(0x69);

	/////////MIPI command
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x000F);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xBA);	  //
	SSD1963GPIOOUTDATA_18(0x31);	  //
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x16);
	SSD1963GPIOOUTDATA_18(0xCA);
	SSD1963GPIOOUTDATA_18(0xB1);
	SSD1963GPIOOUTDATA_18(0x0A);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x28);
	SSD1963GPIOOUTDATA_18(0x02);
	SSD1963GPIOOUTDATA_18(0x21);
	SSD1963GPIOOUTDATA_18(0x21);
	SSD1963GPIOOUTDATA_18(0x9A);
	SSD1963GPIOOUTDATA_18(0x1A);
	SSD1963GPIOOUTDATA_18(0x8F);
	
	//////////GOA Timing Control
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x005D);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xD5);	  //
	SSD1963GPIOOUTDATA_18(0x00);	//1  //
	SSD1963GPIOOUTDATA_18(0x00);	//2
	SSD1963GPIOOUTDATA_18(0x0F);	//3
	SSD1963GPIOOUTDATA_18(0x03);	//4
	SSD1963GPIOOUTDATA_18(0x36);	//5
	SSD1963GPIOOUTDATA_18(0x00);	//6
	SSD1963GPIOOUTDATA_18(0x00);	//7
	SSD1963GPIOOUTDATA_18(0x10);	//8
	SSD1963GPIOOUTDATA_18(0x01);	//9
	SSD1963GPIOOUTDATA_18(0x00);	//10
	SSD1963GPIOOUTDATA_18(0x00);	//11
	SSD1963GPIOOUTDATA_18(0x00);	//12
	SSD1963GPIOOUTDATA_18(0x1A);	//13
	SSD1963GPIOOUTDATA_18(0x50);	//14
	SSD1963GPIOOUTDATA_18(0x45);	//15
	SSD1963GPIOOUTDATA_18(0x00);	//16
	SSD1963GPIOOUTDATA_18(0x00);	//17
	SSD1963GPIOOUTDATA_18(0x13);	//18
	SSD1963GPIOOUTDATA_18(0x44);	//19
	SSD1963GPIOOUTDATA_18(0x39);	//20
	SSD1963GPIOOUTDATA_18(0x47);	//21
	SSD1963GPIOOUTDATA_18(0x00);	//22
	SSD1963GPIOOUTDATA_18(0x00);	//23
	SSD1963GPIOOUTDATA_18(0x02);	//24
	SSD1963GPIOOUTDATA_18(0x04);	//25
	SSD1963GPIOOUTDATA_18(0x00);	//26
	SSD1963GPIOOUTDATA_18(0x00);	//27
	SSD1963GPIOOUTDATA_18(0x00);	//28
	SSD1963GPIOOUTDATA_18(0x00);	//29
	SSD1963GPIOOUTDATA_18(0x00);	//30
	SSD1963GPIOOUTDATA_18(0x00);	//31
	SSD1963GPIOOUTDATA_18(0x00);	//32
	SSD1963GPIOOUTDATA_18(0x03);	//33
	SSD1963GPIOOUTDATA_18(0x00);	//34
	SSD1963GPIOOUTDATA_18(0x00);	//35
	SSD1963GPIOOUTDATA_18(0x08);	//36
	SSD1963GPIOOUTDATA_18(0x88);	//37
	SSD1963GPIOOUTDATA_18(0x88);	//38
	SSD1963GPIOOUTDATA_18(0x37);	//39
	SSD1963GPIOOUTDATA_18(0x5F);	//40
	SSD1963GPIOOUTDATA_18(0x1E);	//41
	SSD1963GPIOOUTDATA_18(0x18);	//42
	SSD1963GPIOOUTDATA_18(0x88);	//43
	SSD1963GPIOOUTDATA_18(0x88);	//44
	SSD1963GPIOOUTDATA_18(0x85);	//45
	SSD1963GPIOOUTDATA_18(0x88);	//46
	SSD1963GPIOOUTDATA_18(0x88);	//47
	SSD1963GPIOOUTDATA_18(0x40);	//48
	SSD1963GPIOOUTDATA_18(0x2F);	//49
	SSD1963GPIOOUTDATA_18(0x6E);	//50
	SSD1963GPIOOUTDATA_18(0x48);	//51
	SSD1963GPIOOUTDATA_18(0x88);	//52
	SSD1963GPIOOUTDATA_18(0x88);	//53
	SSD1963GPIOOUTDATA_18(0x80);	//54
	SSD1963GPIOOUTDATA_18(0x88);	//55
	SSD1963GPIOOUTDATA_18(0x88);	//56
	SSD1963GPIOOUTDATA_18(0x26);	//57
	SSD1963GPIOOUTDATA_18(0x4F);	//58
	SSD1963GPIOOUTDATA_18(0x0E);	//59
	SSD1963GPIOOUTDATA_18(0x08);	//60
	SSD1963GPIOOUTDATA_18(0x88);	//61
	SSD1963GPIOOUTDATA_18(0x88);	//62
	SSD1963GPIOOUTDATA_18(0x84);	//63
	SSD1963GPIOOUTDATA_18(0x88);	//64
	SSD1963GPIOOUTDATA_18(0x88);	//65
	SSD1963GPIOOUTDATA_18(0x51);	//66
	SSD1963GPIOOUTDATA_18(0x3F);	//67
	SSD1963GPIOOUTDATA_18(0x7E);	//68
	SSD1963GPIOOUTDATA_18(0x58);	//69
	SSD1963GPIOOUTDATA_18(0x88);	//70
	SSD1963GPIOOUTDATA_18(0x88);	//71
	SSD1963GPIOOUTDATA_18(0x81);	//72
	SSD1963GPIOOUTDATA_18(0x00);	//73
	SSD1963GPIOOUTDATA_18(0x00);	//74
	SSD1963GPIOOUTDATA_18(0x00);	//75
	SSD1963GPIOOUTDATA_18(0x01);	//76
	SSD1963GPIOOUTDATA_18(0x00);	//77
	SSD1963GPIOOUTDATA_18(0x00);	//78
	SSD1963GPIOOUTDATA_18(0x00);	//79
	SSD1963GPIOOUTDATA_18(0x07);	//80
	SSD1963GPIOOUTDATA_18(0xF8);	//81
	SSD1963GPIOOUTDATA_18(0x0F);	//82
	SSD1963GPIOOUTDATA_18(0xFF);	//83
	SSD1963GPIOOUTDATA_18(0xFF);	//84
	SSD1963GPIOOUTDATA_18(0x07);	//85
	SSD1963GPIOOUTDATA_18(0xF8);	//86
	SSD1963GPIOOUTDATA_18(0x0F);	//87
	SSD1963GPIOOUTDATA_18(0xFF);	//88
	SSD1963GPIOOUTDATA_18(0xFF);	//89
	SSD1963GPIOOUTDATA_18(0x00);	//90
	SSD1963GPIOOUTDATA_18(0x00);	//91
	SSD1963GPIOOUTDATA_18(0x5A);	//92
	
	////////SET Pixel 
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0002);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x3A);	  //
	SSD1963GPIOOUTDATA_18(0x70);	  //

	////////Display Direction
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0002);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x36);	  //
	SSD1963GPIOOUTDATA_18(0xc0);	  //
	
	////////BGP voltage
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0003);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB5);	  //
	SSD1963GPIOOUTDATA_18(0x12);	  //
	SSD1963GPIOOUTDATA_18(0x12);
	
	/////////////Power Control
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x000b);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB1);	  //
	SSD1963GPIOOUTDATA_18(0x12);	  //
	SSD1963GPIOOUTDATA_18(0x83);
	SSD1963GPIOOUTDATA_18(0x77);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x12);
	SSD1963GPIOOUTDATA_18(0x12);
	SSD1963GPIOOUTDATA_18(0x1a);	//1E
	SSD1963GPIOOUTDATA_18(0x1a);	//1E
	SSD1963GPIOOUTDATA_18(0x0c);
	SSD1963GPIOOUTDATA_18(0x1a);
	
	////////RGB Setting
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0005);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB3);	  //
	SSD1963GPIOOUTDATA_18(0x83);	  //
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x3a);
	SSD1963GPIOOUTDATA_18(0x17);
	
	////////Display Inversion Setting
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0002);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xB4);	  //
	SSD1963GPIOOUTDATA_18(0x00);	  // 01 DOT INVERSION     00 COLUMN INVERSION

	////////SET VCOM
//			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
//	SSD1963GPIOOUTDATA_16(0x0004);						  //接下来写入的数据
//			SSD1963GPIOOUTCOM(0xbf);
//	SSD1963GPIOOUTDATA_18(0xB6);	  //
//	SSD1963GPIOOUTDATA_18(0xbA);	//BD					//////because have auto otp function....so do not down this code..
//	SSD1963GPIOOUTDATA_18(0xbA);	//BC
//	SSD1963GPIOOUTDATA_18(0x00);
	
	////////SET EQ
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0005);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xe3);	  //
	SSD1963GPIOOUTDATA_18(0x0f);
	SSD1963GPIOOUTDATA_18(0x0f);
	SSD1963GPIOOUTDATA_18(0x0f);
	SSD1963GPIOOUTDATA_18(0x0f);
	
	////////SET SOURCE option
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0007);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xc0);	  //
	SSD1963GPIOOUTDATA_18(0x73);
	SSD1963GPIOOUTDATA_18(0x50);
	SSD1963GPIOOUTDATA_18(0x00);
	SSD1963GPIOOUTDATA_18(0x34);
	SSD1963GPIOOUTDATA_18(0xc4);
	SSD1963GPIOOUTDATA_18(0x00);
	
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x007F);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xc1);	  //
	SSD1963GPIOOUTDATA_18(0x01);	//1
	SSD1963GPIOOUTDATA_18(0x00);	//2
	SSD1963GPIOOUTDATA_18(0x0a);	//3
	SSD1963GPIOOUTDATA_18(0x15);	//4
	SSD1963GPIOOUTDATA_18(0x1f);	//5
	SSD1963GPIOOUTDATA_18(0x2a);	//6
	SSD1963GPIOOUTDATA_18(0x35);	//7
	SSD1963GPIOOUTDATA_18(0x42);	//8
	SSD1963GPIOOUTDATA_18(0x4c);	//9
	SSD1963GPIOOUTDATA_18(0x57);	//10
	SSD1963GPIOOUTDATA_18(0x60);	//11
	SSD1963GPIOOUTDATA_18(0x6a);	//12
	SSD1963GPIOOUTDATA_18(0x73);	//13
	SSD1963GPIOOUTDATA_18(0x7b);	//14
	SSD1963GPIOOUTDATA_18(0x83);	//15
	SSD1963GPIOOUTDATA_18(0x8a);	//16
	SSD1963GPIOOUTDATA_18(0x91);	//17
	SSD1963GPIOOUTDATA_18(0x99);	//18
	SSD1963GPIOOUTDATA_18(0xa0);	//19
	SSD1963GPIOOUTDATA_18(0xa7);	//20
	SSD1963GPIOOUTDATA_18(0xad);	//21
	SSD1963GPIOOUTDATA_18(0xB4);	//22
	SSD1963GPIOOUTDATA_18(0xBA);	//23
	SSD1963GPIOOUTDATA_18(0xC0);	//24
	SSD1963GPIOOUTDATA_18(0xC6);	//25
	SSD1963GPIOOUTDATA_18(0xCC);	//26
	SSD1963GPIOOUTDATA_18(0xD2);	//27
	SSD1963GPIOOUTDATA_18(0xD8);	//28
	SSD1963GPIOOUTDATA_18(0xDF);	//29
	SSD1963GPIOOUTDATA_18(0xE6);	//30
	SSD1963GPIOOUTDATA_18(0xEC);	//31
	SSD1963GPIOOUTDATA_18(0xF3);	//32
	SSD1963GPIOOUTDATA_18(0xF8);	//33
	SSD1963GPIOOUTDATA_18(0xFF);	//34
	SSD1963GPIOOUTDATA_18(0x22);	//35
	SSD1963GPIOOUTDATA_18(0xE0);	//36
	SSD1963GPIOOUTDATA_18(0x30);	//37
	SSD1963GPIOOUTDATA_18(0x4B);	//38
	SSD1963GPIOOUTDATA_18(0x43);	//39
	SSD1963GPIOOUTDATA_18(0x01);	//40
	SSD1963GPIOOUTDATA_18(0xAF);	//41
	SSD1963GPIOOUTDATA_18(0x0A);	//42
	SSD1963GPIOOUTDATA_18(0xC0);	//43
	SSD1963GPIOOUTDATA_18(0x00);	//44
	SSD1963GPIOOUTDATA_18(0x09);	//45
	SSD1963GPIOOUTDATA_18(0x13);	//46
	SSD1963GPIOOUTDATA_18(0x1C);	//47
	SSD1963GPIOOUTDATA_18(0x26);	//48
	SSD1963GPIOOUTDATA_18(0x2F);	//49
	SSD1963GPIOOUTDATA_18(0x37);	//50
	SSD1963GPIOOUTDATA_18(0x41);	//51
	SSD1963GPIOOUTDATA_18(0x49);	//52
	SSD1963GPIOOUTDATA_18(0x52);	//53
	SSD1963GPIOOUTDATA_18(0x5B);	//54
	SSD1963GPIOOUTDATA_18(0x64);	//55
	SSD1963GPIOOUTDATA_18(0x6C);	//56
	SSD1963GPIOOUTDATA_18(0x74);	//57
	SSD1963GPIOOUTDATA_18(0x7C);	//58
	SSD1963GPIOOUTDATA_18(0x84);	//59
	SSD1963GPIOOUTDATA_18(0x8B);	//60
	SSD1963GPIOOUTDATA_18(0x92);	//61
	SSD1963GPIOOUTDATA_18(0x9B);	//62
	SSD1963GPIOOUTDATA_18(0xA2);	//63
	SSD1963GPIOOUTDATA_18(0xA9);	//64
	SSD1963GPIOOUTDATA_18(0xB0);	//65
	SSD1963GPIOOUTDATA_18(0xB7);	//66
	SSD1963GPIOOUTDATA_18(0xBD);	//67
	SSD1963GPIOOUTDATA_18(0xC3);	//68
	SSD1963GPIOOUTDATA_18(0xCA);	//69
	SSD1963GPIOOUTDATA_18(0xD1);	//70
	SSD1963GPIOOUTDATA_18(0xD8);	//71
	SSD1963GPIOOUTDATA_18(0xE0);	//72
	SSD1963GPIOOUTDATA_18(0xE8);	//73
	SSD1963GPIOOUTDATA_18(0xEF);	//74
	SSD1963GPIOOUTDATA_18(0xF8);	//75
	SSD1963GPIOOUTDATA_18(0xFF);	//76
	SSD1963GPIOOUTDATA_18(0x22);	//77
	SSD1963GPIOOUTDATA_18(0x58);	//78
	SSD1963GPIOOUTDATA_18(0xBD);	//79
	SSD1963GPIOOUTDATA_18(0xF4);	//80
	SSD1963GPIOOUTDATA_18(0x5A);	//81
	SSD1963GPIOOUTDATA_18(0xA5);	//82
	SSD1963GPIOOUTDATA_18(0xFA);	//83
	SSD1963GPIOOUTDATA_18(0x4C);	//84
	SSD1963GPIOOUTDATA_18(0xC0);	//85
	SSD1963GPIOOUTDATA_18(0x00);	//86
	SSD1963GPIOOUTDATA_18(0x07);	//87
	SSD1963GPIOOUTDATA_18(0x0F);	//88
	SSD1963GPIOOUTDATA_18(0x16);	//89
	SSD1963GPIOOUTDATA_18(0x1E);	//90
	SSD1963GPIOOUTDATA_18(0x25);	//91
	SSD1963GPIOOUTDATA_18(0x2D);	//92
	SSD1963GPIOOUTDATA_18(0x34);	//93
	SSD1963GPIOOUTDATA_18(0x3C);	//94
	SSD1963GPIOOUTDATA_18(0x45);	//95
	SSD1963GPIOOUTDATA_18(0x4E);	//96
	SSD1963GPIOOUTDATA_18(0x57);	//97
	SSD1963GPIOOUTDATA_18(0x5F);	//98
	SSD1963GPIOOUTDATA_18(0x68);	//99
	SSD1963GPIOOUTDATA_18(0x70);	//100
	SSD1963GPIOOUTDATA_18(0x78);	//101
	SSD1963GPIOOUTDATA_18(0x80);	//102
	SSD1963GPIOOUTDATA_18(0x87);	//103
	SSD1963GPIOOUTDATA_18(0x8E);	//104
	SSD1963GPIOOUTDATA_18(0x96);	//105
	SSD1963GPIOOUTDATA_18(0x9D);	//106
	SSD1963GPIOOUTDATA_18(0xA4);	//107
	SSD1963GPIOOUTDATA_18(0xAB);	//108
	SSD1963GPIOOUTDATA_18(0xB3);	//109
	SSD1963GPIOOUTDATA_18(0xB9);	//110
	SSD1963GPIOOUTDATA_18(0xC0);	//111
	SSD1963GPIOOUTDATA_18(0xC8);	//112
	SSD1963GPIOOUTDATA_18(0xCF);	//113
	SSD1963GPIOOUTDATA_18(0xD7);	//114
	SSD1963GPIOOUTDATA_18(0xE1);	//115
	SSD1963GPIOOUTDATA_18(0xE9);	//116
	SSD1963GPIOOUTDATA_18(0xF4);	//117
	SSD1963GPIOOUTDATA_18(0xFF);	//118
	SSD1963GPIOOUTDATA_18(0x22);	//119
	SSD1963GPIOOUTDATA_18(0x22);	//120
	SSD1963GPIOOUTDATA_18(0x17);	//121
	SSD1963GPIOOUTDATA_18(0xC9);	//122
	SSD1963GPIOOUTDATA_18(0x1C);	//123
	SSD1963GPIOOUTDATA_18(0xAD);	//124
	SSD1963GPIOOUTDATA_18(0xF7);	//125
	SSD1963GPIOOUTDATA_18(0xC9);	//126
	
	////////SET SOURCE 
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0004);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xc6);	  //
	SSD1963GPIOOUTDATA_18(0x41);
	SSD1963GPIOOUTDATA_18(0xFF);
	SSD1963GPIOOUTDATA_18(0x7D);

	////////SET PANEL
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0002);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xcC);	  //
	SSD1963GPIOOUTDATA_18(0x00);	 //00

	////////SET MESSI
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0002);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xEA);	  //
	SSD1963GPIOOUTDATA_18(0x7A);
	
	////////Gamma Setting
	SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0024);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0xE0);	  //
	SSD1963GPIOOUTDATA_18(0x00);	//1
	SSD1963GPIOOUTDATA_18(0x10);	//2
	SSD1963GPIOOUTDATA_18(0x18);	//3
	SSD1963GPIOOUTDATA_18(0x0E);	//4
	SSD1963GPIOOUTDATA_18(0x10);	//5
	SSD1963GPIOOUTDATA_18(0x3C);	//6
	SSD1963GPIOOUTDATA_18(0x26);	//7
	SSD1963GPIOOUTDATA_18(0x32);	//8
	SSD1963GPIOOUTDATA_18(0x07);	//9
	SSD1963GPIOOUTDATA_18(0x0C);	//10
	SSD1963GPIOOUTDATA_18(0x10);	//11
	SSD1963GPIOOUTDATA_18(0x13);	//12
	SSD1963GPIOOUTDATA_18(0x16);	//13
	SSD1963GPIOOUTDATA_18(0x14);	//14
	SSD1963GPIOOUTDATA_18(0x15);	//15
	SSD1963GPIOOUTDATA_18(0x13);	//16
	SSD1963GPIOOUTDATA_18(0x15);	//17
	SSD1963GPIOOUTDATA_18(0x00);	//18
	SSD1963GPIOOUTDATA_18(0x10);	//19
	SSD1963GPIOOUTDATA_18(0x18);	//20
	SSD1963GPIOOUTDATA_18(0x0E);	//21
	SSD1963GPIOOUTDATA_18(0x10);	//22
	SSD1963GPIOOUTDATA_18(0x3C);	//23
	SSD1963GPIOOUTDATA_18(0x26);	//24
	SSD1963GPIOOUTDATA_18(0x32);	//25
	SSD1963GPIOOUTDATA_18(0x07);	//26
	SSD1963GPIOOUTDATA_18(0x0C);	//27
	SSD1963GPIOOUTDATA_18(0x10);	//28
	SSD1963GPIOOUTDATA_18(0x13);	//29
	SSD1963GPIOOUTDATA_18(0x16);	//30
	SSD1963GPIOOUTDATA_18(0x14);	//31
	SSD1963GPIOOUTDATA_18(0x15);	//32
	SSD1963GPIOOUTDATA_18(0x13);	//33
	SSD1963GPIOOUTDATA_18(0x15);	//34
	SSD1963GPIOOUTDATA_18(0x01);	//35

	////////sleep out
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0001);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x11);	  //
	Delay(20); 			  ////delay 200ms
	
	////////display on
			SSD1963GPIOOUTCOM(0xBC);		                  // Packet Size Control Register
	SSD1963GPIOOUTDATA_16(0x0001);						  //接下来写入的数据
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x29);	  //
	Delay(5); 			   ///delay 50ms
}




void enter_sleep_mode(void)
{
	ENTER_LP_mode(); //enter  LP mode
////////ic  display off  and  in sleep mode  setting

//	SSD1963GPIOOUTCOM(0xBC);
//	SSD1963GPIOOUTDATA_8(0x01,0x00);
//			SSD1963GPIOOUTCOM(0xbf); 
//	SSD1963GPIOOUTDATA_8(0x28,0x00);
//	Delay(15);

	SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0001);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x28);
	SSD1963GPIOOUTDATA_18(0x00);
	Delay(15);

	SSD1963GPIOOUTCOM(0xBC);		               
	SSD1963GPIOOUTDATA_16(0x0001);						 
			SSD1963GPIOOUTCOM(0xbf);
	SSD1963GPIOOUTDATA_18(0x10);
	SSD1963GPIOOUTDATA_18(0x00);
	Delay(5);

//	SSD1963GPIOOUTCOM(0xBC);
//	SSD1963GPIOOUTDATA_8(0x01,0x00);
//			SSD1963GPIOOUTCOM(0xbf); 
//	SSD1963GPIOOUTDATA_8(0x10,0x00);
//	Delay(5);

	 SSD1963GPIOOUTCOM(0xBC);				 	//soft reset  need mini 5ms
	SSD1963GPIOOUTDATA_8(0x01,0x00);
			SSD1963GPIOOUTCOM(0xbf); 
	SSD1963GPIOOUTDATA_8(0x01,0x00);
	Delay(20); 


  //2825 CHANGE MODE  IN SLEEP
	SSD1963GPIOOUTCOM(0xc0);		  //SSD2825 SOFT RESET
		SSD1963GPIOOUTDATA_16(0x0100);
		Delay(20);

			SSD1963GPIOOUTCOM(0xB6);		//Video mode and video pixel format
		SSD1963GPIOOUTDATA_16(0x0006);		//18bit 

			SSD1963GPIOOUTCOM(0xDE);		
		SSD1963GPIOOUTDATA_16(0x0001);		//MIPI lane select  

			SSD1963GPIOOUTCOM(0xd6);	
		SSD1963GPIOOUTDATA_16(0x0000);		//Color order and endianess 

			SSD1963GPIOOUTCOM(0xb7);	
		SSD1963GPIOOUTDATA_16(0x0342);		//DCS mode 

			SSD1963GPIOOUTCOM(0xb8);		//VC register 
		SSD1963GPIOOUTDATA_16(0x0080);

		SSD1963GPIOOUTCOM(0xb9);		//PLL disable 
		SSD1963GPIOOUTDATA_16(0x0000);

			SSD1963GPIOOUTCOM(0xba);		//PLL setting 
		SSD1963GPIOOUTDATA_16(0x400A);				//Fout=(Fin/MS)*NF,MS=0,NF=8,Fout=(12/1)*8=96MHZ

			SSD1963GPIOOUTCOM(0xbb);		//LP clock divider 
		SSD1963GPIOOUTDATA_16(0x0001);				//LP clock=Fout/LPD/8,LPD=2,LP clock=96/2/8=6MHZ

			SSD1963GPIOOUTCOM(0xb9);		//PLL enable 
		SSD1963GPIOOUTDATA_16(0x0001);

			SSD1963GPIOOUTCOM(0xb8);		//VC register 
		SSD1963GPIOOUTDATA_16(0x0000);

			SSD1963GPIOOUTCOM(0xb7);		//Generic mode, HS video mode
		SSD1963GPIOOUTDATA_16(0x0314);		//

		Delay(20);

		//ssd1963 into sleep mode

		SSD1963Command_8(0x01);
	   Delay(20);

		SSD1963Command_8(0x10);

	   Delay(2);
	   SSD1963Command_8(0xe5); 

}

void exit_sleep_mode(void)
{	
		STM32_SSD1963_Init();
}

void Soft_reset(void)
{
	SSD1963Command_8(0xB8);  //Set the GPIOs configuration. If the GPIOs are not used for LCD, set the direction. Otherwise, they are toggled with  LCD signals by 0xC0 – 0xCF.
	    SSD1963Data_8(0x0F);     //
	    SSD1963Data_8(0x01);     //
	  
	   SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0e);
	   Delay(1);   
	   
	   SSD1963Command_8(0xba);  //Set GPIO value for GPIO configured as output,4个GPIO全部输出1
	   SSD1963Data_8(0x0F); 
	   Delay(15);

////////////=============================================================================
////////////在SSD1963的I/O工作正常后可以对SSD2825进行配置。以便后续对IC进行初始化配置
	
	//对HX8369 IC进行初始化
	SSD1963INITSSD2825(); 

	INITIALOTM8018B_QC();					   /////GPM1461A0  USE
//	SSD2825_CLAN043LL_SC7789_A();
//	SSD2825INITIAL_HX8369B();
///////////此处对SSD2825重新配置为VIDEO模式，一定要在SSD1963时序生成OK后进行配置2825，
	VIDEO_ON();
}



/////////////////////////////////////////////
/////////////////////////////////////////////
////////////////////FOR  TFT9K2353 USE
////////////////////////////////////////////
////////////////////////////////////////////
void LCDSPI_InitCMD(char cmd)
{
    u8 i=0;
    u32 instru_H=0;
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
    instru_H  = (cmd&0xFFFF) + ((0x70+(LCD_ID_SPI<<2)+(LCD_COM_SPI<<1)+(LCD_WRITE_SPI))<<16);

	
	for (i=0;i<24;i++)     // 
	{ 
		if( !(instru_H & 0x00800000))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}

		instru_H=instru_H<<1;
	} 
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);
}
    
void LCDSPI_InitDAT(int Data)
{
    u8 i=0;
    u32 instru_H=0;
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x07);   //CS=0
	
    instru_H  = (Data&0xFFFF) + ((0x70+(LCD_ID_SPI<<2)+(LCD_DATA_SPI<<1)+(LCD_WRITE_SPI))<<16);

	
	for (i=0;i<24;i++)     // 
	{ 
		if( !(instru_H & 0x00800000))
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x05);    //SDI=0
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x01);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x05);     //sck=1
		}
		else
		{ 
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x07);    //SDI=1
			SSD1963Command_8(0xBa); 
			SSD1963Data_8(0x03);    //sck=0
			
			SSD1963Command_8(0xBa);  
			SSD1963Data_8(0x07);     //sck=1
		}

		instru_H=instru_H<<1;
	} 
	
	SSD1963Command_8(0xBa); 
	SSD1963Data_8(0x0F);
}

void LCD2353INITTION(void)
{
//    LCDSPI_InitCMD(0x2d);LCDSPI_InitDAT(0x3F44);//////add   shut
    
    LCDSPI_InitCMD(0x02);LCDSPI_InitDAT(0x0c02);
    LCDSPI_InitCMD(0x01);LCDSPI_InitDAT(0x2B0f);////0x630F
    LCDSPI_InitCMD(0x0f);LCDSPI_InitDAT(0x0000);
    LCDSPI_InitCMD(0x03);LCDSPI_InitDAT(0x4144);
    LCDSPI_InitCMD(0x05);LCDSPI_InitDAT(0x0557);
    LCDSPI_InitCMD(0x0c);LCDSPI_InitDAT(0x119c);
    LCDSPI_InitCMD(0x0d);LCDSPI_InitDAT(0x060b);
    LCDSPI_InitCMD(0x0b);LCDSPI_InitDAT(0x8c05);
    LCDSPI_InitCMD(0x15);LCDSPI_InitDAT(0x00D0);
    
    LCDSPI_InitCMD(0x16);LCDSPI_InitDAT(0xefa6);/////38-hbp
//    LCDSPI_InitCMD(0x17);LCDSPI_InitDAT(0x0001);/////1-vbp
    
    LCDSPI_InitCMD(0x30);LCDSPI_InitDAT(0xa0a0);
    LCDSPI_InitCMD(0x31);LCDSPI_InitDAT(0x7dd5);
    LCDSPI_InitCMD(0x32);LCDSPI_InitDAT(0x1205);
    LCDSPI_InitCMD(0x33);LCDSPI_InitDAT(0x4188);
    LCDSPI_InitCMD(0x34);LCDSPI_InitDAT(0x65a5);
    
        LCDSPI_InitCMD(0x35);LCDSPI_InitDAT(0x8cc6);
    LCDSPI_InitCMD(0x36);LCDSPI_InitDAT(0x1185);
    LCDSPI_InitCMD(0x37);LCDSPI_InitDAT(0x2872);
    
    LCDSPI_InitCMD(0x1e);LCDSPI_InitDAT(0x02aa);        ///VCOM
    
    
//    GPIO_SetBits(GPIOA, GPIO_Pin_8);      ////////exit sleep
//    LCDSPI_InitCMD(0x2d);LCDSPI_InitDAT(0x3F46);//////add
    Delay(10);
//    LCDSPI_InitCMD(0x04);LCDSPI_InitDAT(0xE400);//////add
}





void Forward_scan(void)
{
	SEND_MIPI_DATA(2,SC7789_A_CC_F);
}

void Backward_scan(void)
{
	SEND_MIPI_DATA(2,SC7789_A_CC_B);	
}

/*******************************************************************************
* Function Name  : LCD_SetDisplayWindow
* Description    : Sets a display window
* Input          : - Xpos: specifies the X buttom left position.
*                  - Ypos: specifies the Y buttom left position.
*                  - Height: display window height.
*                  - Width: display window width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetDisplayWindow(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey)
{            //此函数非常重要
  u16 sx= add_sx;
  u16 ex= add_ex;
  u16 sy= add_sy;
  u16 ey= add_ey;
  
       SSD1963Command_8(0x2a); 
       SSD1963Data_8(sx>>8);
       SSD1963Data_8(sx);
       SSD1963Data_8(ex>>8);
       SSD1963Data_8(ex);// 

       //row start_end
       SSD1963Command_8(0x2b);  
       SSD1963Data_8(sy>>8);
       SSD1963Data_8(sy);
       SSD1963Data_8(ey>>8);
       SSD1963Data_8(ey);// 
}
/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{//此函数非常重要
  SSD1963Command_8(0x2c);
}
void LCD_DrawPoint(u32 point)  //向Display RAM 写入1点的数据，此函数与“LCD_WriteRAM_Prepare”配合使用
{ //此函数非常重要
	u8 rr,gg,bb;
	//u8 tep;
//	rr=(point>>16);  
//	gg=(point>>8);  
//	bb=point;
  	
	LCD_RS(1);
	SSD1963_CS(0);

	if(SDShowTimes == 1)
	{// SSD1963设定为24bits色彩输出，
//		SSD1963Data_8(rr);SSD1963Data_8(gg);SSD1963Data_8(bb);
		SSD1963Data_8tt(point>>16);
		SSD1963Data_8tt(point>>8);
		SSD1963Data_8tt(point);
	}
	else
	{	;	}
}



void LCD_WriteArea(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey, u8 corr,u8 corg,u8 corb)
{
  u16 m,n;
  
  LCD_SetDisplayWindow(add_sx, add_ex, add_sy, add_ey);
  LCD_WriteRAM_Prepare();
	
	LCD_RS(1);
    SSD1963_CS(0);
			 
	for(m = 0; m <(add_ex-add_sx+1); m++)
	{
		for(n = 0; n <(add_ey-add_sy+1); n++)
		{
			SSD1963Data_8tt(corr);SSD1963Data_8tt(corg);SSD1963Data_8tt(corb);
  		}
	}
  	
   SSD1963_CS(1);
}

void Flicker_sub_pixel(void)
{
	u16 m,n;
  
  LCD_SetDisplayWindow(0, XDP-1, 0, YDP-1);
  LCD_WriteRAM_Prepare();

	LCD_RS(1);
	SSD1963_CS(0);

	for(m = 0; m <(YDP); m++)
	{
		for(n = 0; n <(XDP/2); n++)
		{
	         SSD1963Data_8tt(0X80);	 SSD1963Data_8tt(0X00);	  SSD1963Data_8tt(0X80);	
			 SSD1963Data_8tt(0X00);	 SSD1963Data_8tt(0X80);	  SSD1963Data_8tt(0X00);
//			 SSD1963Data_8(0X80);SSD1963Data_8(0X00);SSD1963Data_8(0X80);
//			 SSD1963Data_8(0X00);SSD1963Data_8(0X80);SSD1963Data_8(0X00);
		}
		
	}
	SSD1963_CS(1);	
}

void Flicker_PIXEL(void)
{
	u16 m,n;
  
  LCD_SetDisplayWindow(0, XDP-1, 0, YDP-1);
  LCD_WriteRAM_Prepare();
 
	LCD_RS(1);
            SSD1963_CS(0);

	for(m = 0; m <(YDP); m++)
	{
		for(n = 0; n <(XDP/2); n++)
		{
	         SSD1963Data_8tt(0X80);	 SSD1963Data_8tt(0X80);  SSD1963Data_8tt(0X80);
			 SSD1963Data_8tt(0X00);	 SSD1963Data_8tt(0X00);  SSD1963Data_8tt(0X00);
//			 SSD1963Data_8(0X7f);SSD1963Data_8(0X7f);SSD1963Data_8(0X7f);
//			 SSD1963Data_8(0X00);SSD1963Data_8(0X00);SSD1963Data_8(0X00);
		}
		
	}
	SSD1963_CS(1);
}

void Flicker_COLUMN(void)
{
	u16 m,n;
  
  LCD_SetDisplayWindow(0, XDP-1, 0, YDP-1);
  LCD_WriteRAM_Prepare();
 
	for(m = 0; m <(YDP/2); m++)
	{
		for(n = 0; n <(XDP); n++)
		{
	         SSD1963Data_8(0X80);SSD1963Data_8(0X80);SSD1963Data_8(0X80);
		}
		for(n = 0; n <(XDP); n++)
		{
	         
			 SSD1963Data_8(0X00);SSD1963Data_8(0X00);SSD1963Data_8(0X00);
		}
	}
}

void RGB_color(void)
{
	u16 m,n;
	u8 corr,corg,corb;

	
  
	LCD_SetDisplayWindow(0,(XDP-1),0,(YDP-1));
	LCD_WriteRAM_Prepare();

	LCD_RS(1);
    SSD1963_CS(0);
 	corr = corg = corb = 0;
	for(m = 0; m <(YDP); m++)
	{
		
  	  for(n = 0; n <XDP/4; n++)
  	     {
//	         SSD1963Data_8(corr);SSD1963Data_8(0);SSD1963Data_8(0);
			 SSD1963Data_8tt(corr);		SSD1963Data_8tt(0);			SSD1963Data_8tt(0);
	     }
		 corr = (m*255)/YDP;

		 for(n = 0; n <XDP/4; n++)
  	     {
//	         SSD1963Data_8(0);SSD1963Data_8(corg);SSD1963Data_8(0);
			 SSD1963Data_8tt(0);		SSD1963Data_8tt(corg);			SSD1963Data_8tt(0);
	     }
		 corg = (m*255)/YDP;

		 for(n = 0; n <XDP/4; n++)
  	     {
//	         SSD1963Data_8(0);SSD1963Data_8(0);SSD1963Data_8(corb);
			 SSD1963Data_8tt(0);		SSD1963Data_8tt(0);			SSD1963Data_8tt(corb);
	     }
		 corb = (m*255)/YDP;

		 for(n = 0; n <XDP/4; n++)
  	     {
//	         SSD1963Data_8(corr);SSD1963Data_8(corg);SSD1963Data_8(corb);
			 SSD1963Data_8tt(corr);		SSD1963Data_8tt(corg);			SSD1963Data_8tt(corb);
	     }

	}
	SSD1963_CS(1);
}

void CT(void)
{
	u16 m,n;

	LCD_SetDisplayWindow(0,(XDP-1),0,(YDP-1));
	LCD_WriteRAM_Prepare();

	for(m = 0; m <(YDP); m++)
	{
		
  	  for(n = 0; n <XDP; n++)
  	     {
	         
			 if((270<m<530)&&(160<n<320))
			{
				SSD1963Data_8(0x00);SSD1963Data_8(0x00);SSD1963Data_8(0x00);	
			}
			else
			{
				SSD1963Data_8(0x80);SSD1963Data_8(0x80);SSD1963Data_8(0x80);	
			}
	     }
	
		 
	}	
}

void All_Color(u8 corr,u8 corg,u8 corb)
{
  vu32 index =0 ;
  LCD_SetDisplayWindow(0,(XDP-1),0,(YDP-1));
  LCD_WriteRAM_Prepare();
 
  LCD_RS(1);
            SSD1963_CS(0);//asm("NOP");asm("NOP");

  for(index = 0; index < (YDP*XDP); index++)
  {
	 SSD1963Data_8tt(corr);SSD1963Data_8tt(corg);SSD1963Data_8tt(corb);
  }
   SSD1963_CS(1);
}
void LCD_Clear(void)
{
  All_Color(0,128,255);
}
/*******************************************************************************
* Function Name  : LCD_WriteBMP
* Description    : Displays a bitmap picture loaded in the internal Flash.
* Input          : - BmpAddress: Bmp picture address in the internal Flash.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteBMP(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey, const u8 *bitmap)
{
	u16 m,n;
	u8 *bitmap_ptr = (u8 *)bitmap;
	u16 point;
  
	LCD_SetDisplayWindow(add_sx, add_ex, add_sy, add_ey);
	LCD_WriteRAM_Prepare();
 
	for(m = 0; m <(add_ex-add_sx+1); m++)
	{
		for(n = 0; n <(add_ey-add_sy+1); n++)
  	    {
	         point= *bitmap_ptr++;  
			 SSD1963Data_8(point);

			 point= *bitmap_ptr++;
			 SSD1963Data_8(point);

			 point= *bitmap_ptr++;
			 SSD1963Data_8(point);
		}
	}
}



  /****************************************************************************
* 名    称：LCD_PutChar(u16 adx,u16 ady,const uint  *p)
* 功    能：在指定座标显示一个16x24点阵的ascii字符
* 入口参数：adx          列座标
*           ady          行座标
* 说    明：显示范围限定为可显示的ascii码
****************************************************************************/
void LCD_PutChar(u16 adx,u16 ady,const uint  *p)
{ //此函数作用为写入16*24大小字符
    u8 i,j;       u16 temp;
    u16 sx=adx;   u16 sy=ady;     
           
    LCD_SetDisplayWindow(sx, sx+16-1,sy, sy+24-1)	; 
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    
	for(i=0;i<24;i++)  //字体高度：24 Dots
	{
	    temp=*p;
	    for(j=0;j<16;j++) ////字体宽度：16 Dots
	       {                 
	           if(temp&1) 
                 {  SSD1963Data_8(FontR);SSD1963Data_8(FontG);SSD1963Data_8(FontB);} //字体色
	           else
                  { 
				  SSD1963Data_8(255);SSD1963Data_8(255);SSD1963Data_8(255);
				   }  //背景色    
	           temp>>=1;
	       }
      p++;
     }
}
/****************************************************************************
* 名    称：DisplayStringLine
* 功    能：显示n个字符一行在LCD上  ;字库大小为16*24，故n=(行像素个数/16)
* 入口参数：addx：起始X地址，addy：起始Y地址;  *ptr指向字符串的指针 
* 出口参数：无
* 说    明：
* 调用方法：LCD_DisplayStringLine(Line0,"I Love you...",White,Blue);  
****************************************************************************/
void LCD_DisplayStringLine(u16 addx,u16 addy, unsigned char *p )
{
  unsigned char tep;    
    const u16 *fp;
    while(*p!='\0')
    {       
      tep=*p;
      tep-=32;
      fp=&ASCII_Table[tep * 24];
      LCD_PutChar(addx,addy,fp);
        addx+=16;
        p++;
    }  
}

void LCD_PutChar_A(u16 adx,u8 times_X,u16 ady,u8 times_Y,const uint  *p)
{ 
    u8 Font_H,Font_W;
	u8 i,j,k,k1;       u16 temp,temp1;
    u16 sx=adx;   u16 sy=ady; 
  
	Font_H = times_Y;	Font_W = times_X;
   
    LCD_SetDisplayWindow(sx, sx+16*Font_W-1,sy, sy+24*Font_H-1)	; 
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    
	for(i=0;i<24;i++)  		   //////////字体高度：24 Dots
	{
	    temp=*p;
		temp1 = temp;
		for(k1=0;k1<Font_H;k1++)
		{
	    temp = temp1;
		for(j=0;j<16;j++) ///		////字体宽度：16 Dots
	       {                 
	           if(temp&1) 
                 {  								 
					for(k=0;k<Font_W;k++)
					{
						SSD1963Data_8(FontR);SSD1963Data_8(FontG);SSD1963Data_8(FontB);		 ////////字体色
					}
				 } 
	           else
                  { 
						for(k=0;k<Font_W;k++)
						{
							SSD1963Data_8(255);SSD1963Data_8(255);SSD1963Data_8(255);		/////////背景色
						}
				   }     
	           temp>>=1;
	       }
		}
      p++;
     }
}

void LCD_DisplayStringLine_A(u16 addx,u16 addy, unsigned char *p )
{
	u16 FontFlag; 		u8 fontH,fontW;
	unsigned char tep;    		const u16 *fp;

	FontFlag=1;		
	fontH = 2; 			  /////// height *3
	fontW = 2;			  ///////  width *3
    while((*p!='\0')&&(FontFlag<(XDP-1)))
    {       
      tep=*p;
      tep-=32;
      fp=&ASCII_Table[tep * 24];
      LCD_PutChar_A(addx,fontW,addy,fontH,fp);
        addx+=16*fontW;
        p++;
		FontFlag += 16*fontW;;
    }  
}
/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC 
                                             | RCC_APB2Periph_AFIO, ENABLE); 
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//禁止jtag，以空出PB3,PB4,PA15
 
  
  /*[把GPIOB配置成输出模式] */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO最高速度50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /*[把PA4678配置成输出模式] */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO最高速度50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    /*[把PA4678配置成输出模式] */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO最高速度50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  GPIO_ResetBits(GPIOA, GPIO_Pin_8);  /////shut down
//  GPIO_SetBits(GPIOA, GPIO_Pin_8);      ////////exit sleep
}


/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void LCD_PutChar1(u16 adx,u16 ady,u8 *p,u8 YY)
{ //此函数作用为写入32*40大小字符
    u8 i,j,k,x,y;       u16 temp;
    u16 sx=adx;   u16 sy=ady;     
      
    LCD_SetDisplayWindow(sx, sx+16*YY-1,sy, sy+24*YY-1)	; /////////YY
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    
	for(i=0;i<24;i++)  //字体高度：24 Dots
	{ 	

	for(y=0;y<YY;y++)   ///////////YY
	{ 
	  temp=*p;	  
 /**************************************/       //显示一薄条
	for(k=0;k<2;k++)      
	{
		temp=*p;		    
	    for(j=0;j<8;j++) ////字体宽度：16 Dots
	       {                 
	           if(temp&1) 
                 {  
				   for(x=0;x<YY;x++)        //////////////////YY
				  {SSD1963Data_8(FontR);SSD1963Data_8(FontG);SSD1963Data_8(FontB);}} //字体色
	           else
                  { 
				   for(x=0;x<YY;x++)     //////////////////YY
				  {SSD1963Data_8(255);SSD1963Data_8(255);SSD1963Data_8(255); }}  //背景色    
	           temp>>=1;
	       }
      p++;
	  }
 /**********************/
	   p-=2;    
	  }
	  p+=2;   
     }
}



void LCD_DisplayChinese(u16 addx,u16 addy, u8 *number,u8 YY )
{
    u8 tep,temp,aa;    
    u8 *fp;		//同unsigned char 效果一致
	aa=addx;
	  temp=*number;
      while(temp!='\0')
	  {

      tep=temp;
      fp=&ASCII_Table1[tep * 48];  //单个汉字占用48比特
      LCD_PutChar1(addx,addy,fp,YY);    //////////
	  addx+=16*YY;   //////////
	  number++;
	  if(addx+16*YY>XDP)
	  {
	  addy+=24*YY;   ////////////
	  addx=aa;
	  }
	  temp=*number;
	  }
    
}







/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
