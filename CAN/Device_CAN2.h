#ifndef   _CAN2_DEV
#define    _CAN2_DEV
#include <rtthread.h>
#include <rthw.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "board.h"
#include <serial.h>

#include  <stdlib.h>//数字转换成字符串
#include  <stdio.h>
#include  <string.h>
#include "App_moduleConfig.h"
#include "Device_CAN.h"
#include <finsh.h>


#define      U3_OUT_PWR_ON       GPIO_SetBits(GPIOE,GPIO_Pin_7)       
#define      U3_OUT_PWR_OFF      GPIO_ResetBits(GPIOE,GPIO_Pin_7)    


#define BD1_TYPE_DWSQ             1 
#define BD1_TYPE_TXSQ             2 
#define BD1_TYPE_CKSC             3 
#define BD1_TYPE_ICJC             4
#define BD1_TYPE_XTZJ             5    
#define BD1_TYPE_SJSC			 6 
#define BD1_TYPE_BBDQ             7 


#define result_success   	2
#define result_failed	 	3
#define result_pending   	5
#define result_Artificial 	6
#define bd1_send_auto    	7
#define bd1_send_Artificial    9

#define TXCF     2
#define DWCF     3


typedef struct _BD_ONE
 {
   u16 infolen; // 信息长度
   u8 UserAddr[3]; // 用户地址
   u8 info_Type;  // 信息类别
   u8 High_antena[4]; //  高程数据和天线高
   u32 air_pressure; //  气压数据
   u16 in_freq; // 入站频度
   u16 Text_len;// 电文长度
   u8  Ack_state; // 应答状态
   u8  Text_info[210];//  电文内容     1680bit 
   u8  Trans_rate; // 传输速率
   u8  Frame_num; //帧号
   u8  Broadcast_ID[3]; //通播ID
   u16 Self_chk_freq;  //自检频度
   u16 Output_freq;  // 输出频度
   u8  check_add[3];  // 查询地址
   u8  IC_state;      // IC 状态
   u8  Hrdware_sate;  // 硬件状态
   u8  Battery_stae;  //  电池状态
   u8  User_status;  //  用户特征
   u16 Service_freq; //  服务频度
   u8  Communicate_grade; // 通信等级
   u8  Encode_state;  // 加密状态 
   u16 Sub_user;     //下属用户数
   u8  Inbound_status; // 入站状态
   u8  Beam_power[6];  //波束功率
   
   //  position segments
   u8  Day;
   u8  hour;
   u8  minute;
   u8  second;
   u8  little_second;
   u8  Month;
   u16 year;           //年份 
   
   u8  longi_Du;  
   u8  longi_Fen;
   u8  longi_Miao;
   u8  longi_Little_Miao;

   u8  lati_Du;  
   u8  lati_Fen;
   u8  lati_Miao;
   u8  lati_Little_Miao;

   u16 Ground_height;
   u16 User_height; 

   u8 Feedback_flag;
   u8 Add_Info[3];
   u8 Rx_enable; //信息接收使能


}BD_ONE;
   
typedef struct _BD_COMMAND
{
u8	data_type;        //信息类别
//u8 	data_high[4];		//高程数据
//u8	data_press[4];		//气压数据
u16	data_freq;			//入站频率
u8	data_user_addr[3];		//通信电文收信方地址
u16	data_len;			//电文长度
u8	data_ask;			//应答
u8	data_message[210];	//电文内容
u8	data_frame;			//帧号
u16 data_freq_test;		//自检频度
u16	data_outfreq;		//输出频率
u8	data_altimeter;		//测高方式 0有高程 1无测高 2测高1  3测高2
u8	data_mode;			//高程指示 0普通 1高空
u8  message_mode;		//电文传输方式0x44 汉字 0x46代码
u8  BD_Signal;
}BD_COMMAND;

typedef struct _ELEVA_PRESS
{
vs16	ground_high;	//天线所在点的大地高程 单位1米
vs16	aerial_ground;	//天线距离地面的高度   单位0.1米/0.5米
vs16	aerial_barograph;	//天线距离用户机中气压仪的高度 单位0.1米
vs16	barograph_high;     //单位为米
vs32 	press_value;		//气压数据单位为0.1HP
}ELEVA_PRESS;

typedef struct _BD_SELF_CHECK
{
u8  Time_consum;
u8  Frequency;
u8  result;
u8  status;
u8  flag_send;//7、不能发送 0可以发送
}BD_SELF_CHECK;

extern  ELEVA_PRESS Aerial_air;
extern  BD_COMMAND	BD1_control;
extern  BD_ONE  BD1_struct;
extern  BD_SELF_CHECK BD_FRE;
extern  BD_SELF_CHECK BD_IC;
extern  BD_SELF_CHECK BD_ZJ;
extern  BD_SELF_CHECK BD_TX;
extern  BD_SELF_CHECK BD_FK;

   
extern  struct rt_device  Device_CAN2;

extern void  CAN2_RxHandler(unsigned char rx_data);   
extern void  Device_CAN2_regist(void );    
extern void Read_bd1_data(void);
extern void Freq_in(u8 *input);
extern void Tx_in(u8* instr,u16 len);
extern void Target_id(u8* input);
extern void user_id(u8* input);
extern void  BD1_Tx(u8 Type,void *str,u8 stuff_len);
extern void BD1_init(void);
extern u16 Info_to_fill(void ); 
//+++++++++++++++++++++++++++++++++++
extern u8 Rx_data_save(u8 *str);
extern u8  show_screen(u8 *str ,u8 input,u16 length);
extern u8 Read_save_data(u8*str,u8 input,u8 mode);
extern void  First_show(u8* str2 ,u8 Screen,u8 mode);
extern u16 Sort_ID(u16 addre,u16 offset,u8 rank);
extern u32  find_max_id(u16 addre);
extern void index_txt(u8*str,u8 length);
extern void BD_Timer_out(void *  parameter);
#endif 
