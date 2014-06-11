#ifndef   _CAN2_DEV
#define    _CAN2_DEV
#include <rtthread.h>
#include <rthw.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "board.h"
#include <serial.h>

#include  <stdlib.h>//����ת�����ַ���
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
   u16 infolen; // ��Ϣ����
   u8 UserAddr[3]; // �û���ַ
   u8 info_Type;  // ��Ϣ���
   u8 High_antena[4]; //  �߳����ݺ����߸�
   u32 air_pressure; //  ��ѹ����
   u16 in_freq; // ��վƵ��
   u16 Text_len;// ���ĳ���
   u8  Ack_state; // Ӧ��״̬
   u8  Text_info[210];//  ��������     1680bit 
   u8  Trans_rate; // ��������
   u8  Frame_num; //֡��
   u8  Broadcast_ID[3]; //ͨ��ID
   u16 Self_chk_freq;  //�Լ�Ƶ��
   u16 Output_freq;  // ���Ƶ��
   u8  check_add[3];  // ��ѯ��ַ
   u8  IC_state;      // IC ״̬
   u8  Hrdware_sate;  // Ӳ��״̬
   u8  Battery_stae;  //  ���״̬
   u8  User_status;  //  �û�����
   u16 Service_freq; //  ����Ƶ��
   u8  Communicate_grade; // ͨ�ŵȼ�
   u8  Encode_state;  // ����״̬ 
   u16 Sub_user;     //�����û���
   u8  Inbound_status; // ��վ״̬
   u8  Beam_power[6];  //��������
   
   //  position segments
   u8  Day;
   u8  hour;
   u8  minute;
   u8  second;
   u8  little_second;
   u8  Month;
   u16 year;           //��� 
   
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
   u8 Rx_enable; //��Ϣ����ʹ��


}BD_ONE;
   
typedef struct _BD_COMMAND
{
u8	data_type;        //��Ϣ���
//u8 	data_high[4];		//�߳�����
//u8	data_press[4];		//��ѹ����
u16	data_freq;			//��վƵ��
u8	data_user_addr[3];		//ͨ�ŵ������ŷ���ַ
u16	data_len;			//���ĳ���
u8	data_ask;			//Ӧ��
u8	data_message[210];	//��������
u8	data_frame;			//֡��
u16 data_freq_test;		//�Լ�Ƶ��
u16	data_outfreq;		//���Ƶ��
u8	data_altimeter;		//��߷�ʽ 0�и߳� 1�޲�� 2���1  3���2
u8	data_mode;			//�߳�ָʾ 0��ͨ 1�߿�
u8  message_mode;		//���Ĵ��䷽ʽ0x44 ���� 0x46����
u8  BD_Signal;
}BD_COMMAND;

typedef struct _ELEVA_PRESS
{
vs16	ground_high;	//�������ڵ�Ĵ�ظ߳� ��λ1��
vs16	aerial_ground;	//���߾������ĸ߶�   ��λ0.1��/0.5��
vs16	aerial_barograph;	//���߾����û�������ѹ�ǵĸ߶� ��λ0.1��
vs16	barograph_high;     //��λΪ��
vs32 	press_value;		//��ѹ���ݵ�λΪ0.1HP
}ELEVA_PRESS;

typedef struct _BD_SELF_CHECK
{
u8  Time_consum;
u8  Frequency;
u8  result;
u8  status;
u8  flag_send;//7�����ܷ��� 0���Է���
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
