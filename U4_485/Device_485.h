/*
     Device_485_uart4.h
*/
#ifndef  _RT_485_U4
#define _RT_485_U4

#include <rthw.h>
#include <rtthread.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "board.h"
#include <serial.h>





#define  RX_485const         GPIO_ResetBits(GPIOC,GPIO_Pin_4)
#define  TX_485const         GPIO_SetBits(GPIOC,GPIO_Pin_4)  
#define  Power_485CH1_ON     GPIO_SetBits(GPIOB,GPIO_Pin_8)  // 第一路485的电	       上电工作
#define  Power_485CH1_OFF    GPIO_ResetBits(GPIOB,GPIO_Pin_8)

#define _485_dev_SIZE   600
#define  normal                 2

typedef  struct _485_REC
{
    u8     _485_receiveflag;
    u16   _485_RxLen;  	

}_485REC_Struct;   

//++++++++++++++++++++++++++++++++++++++++++DWLCD相关数据++++++++++++++++++

#define		LCD_SETTIME		1
#define     LCD_SETSPD		2
#define     LCD_SDTXT			3
#define     LCD_GPSNUM		5
#define     LCD_VechInfo         6
#define     LCD_IDLE              7
#define    LCD_PAGE				8
#define    LCD_GSMNUM			9
#define    LCD_BUSY				10
#define    LCD_Text_clear		11

typedef struct _DWLCD
{
	u8	time;			//当前用时
	u8  status;			//当前lcd状态
   u16  CounterTimer;  // 计数器
   u8   Txinfo[100];   //  寄存器内容
   u16  TxInfolen;    //   发送 信息长度
   u8   TXT_content[256]; 
   u8   RxInfo[220];    // 接收内容 
   u16  RxInfolen;     // 接收信息长度 
   u8   RX_enable;
}_Big_lcd;

extern _Big_lcd Big_lcd;

//------------------------------变量声明---------------------------------------------------
extern u8   Take_photo[10];   //----  报警拍照命令 
extern u8   Fectch_photo[10];   //----- 报警取图命令  

 extern u8 	 _485_content[600];
 extern u16	 _485_content_wr;
 extern u8    _485_CameraData_Enable;// 有图片数据过来	1: data come  0:   no data 

 extern  _485REC_Struct 	 _485_RXstatus;	  

 extern  struct rt_device  Device_485;



//-----------------------------
extern void  _485_RxHandler(u8 data);  

//-----------------------------
extern void  Photo_TakeCMD_Update(u8 CameraNum);
extern void  Photo_FetchCMD_Update(u8 CameraNum);

extern void  OpenDoor_TakePhoto(void);

//----------------------------
extern void _485_delay_us(u16 j);
extern void _485_delay_ms(u16 j);
extern void _485_startup(void);
extern  void rt_hw_485_putc(const char c);
extern  void rt_hw_485_output(const char *str);
extern void rt_hw_485_Output_Data(const char *Instr, unsigned int len) ;
//++++++++++++++++++++++++++++++++++++++++++++
extern void  Lcd_Data_Process(void);
extern void  Lcd_init(void);
extern u8  Lcd_write(u8 statu,u8 type,u8 data);
extern void  _485_RxHandler_01(void);


#endif 

