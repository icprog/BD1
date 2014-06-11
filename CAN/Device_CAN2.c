/*
       Device  CAN    2  
*/
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
#include "Device_CAN2.h"
#include  "rtdevice.h "


#define  Beam_power_numb 1
#define  Version_Info    10   


#define  bd_ic_ok        2
#define  bd_terminal_ok  1
#define  bobao_ing			3
u8   U3_Rx[300];

u8   U3_content[600];
u16   U3_content_len=0; 
u8   U3_flag=0;
u16   U3_rxCounter=0;
u8    Version[Version_Info];
BD_ONE	BD1_struct; //  ����1  ���

u8	U3_Tx[260];
u8  you_can_deial =0;
BD_COMMAND BD1_control;
ELEVA_PRESS Aerial_air;
	
#ifdef RT_USING_DEVICE 
 struct rt_device  Device_CAN2;  
#endif
//+++++++++++++++++++
BD_SELF_CHECK BD_IC;//IC���
BD_SELF_CHECK BD_ZJ;//�Լ�
BD_SELF_CHECK BD_TX;//ͨ��
BD_SELF_CHECK BD_FRE;//��վƵ��
BD_SELF_CHECK BD_FK;//����
static rt_timer_t BD_Timer; 
extern int number_txt;
extern char *trans[];
extern char *number[];
//+++++++++++++++++++++++���ջ�����һ��ringbuffer++++++++++++

#define BD_DATA_SIZE 128
static unsigned char	bd_data[BD_DATA_SIZE];

static struct rt_ringbuffer bd1_pool;


u8 dwlcd =0;//0ΪС������ 1Ϊ��������
u8 tts_play_flag =0;//0��ʾ����״̬ 3��ʾҪ��������
//������ʾ���
 u8 index_addre[12]={0};
 u8 index_len[12]={0};
#define  Max_value     0xffffffff 

/******************************************************************************
 * bo_bao() 
 * DESCRIPTION: ��ֹ�����Ͳ��ŵĳ�ͻ  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *  05 6 2014, wxg written
 * --------------------
 ******************************************************************************/
u8 bo_bao(u8 input)
{
	if(input!=bobao_ing)
	{
		switch(input)
		{
			case 1:
				tts_play_flag = bobao_ing;
				rt_kprintf("input2 %d\n",tts_play_flag);
				TTS_play("�����ն�����");
				
			break;
			case 2:
				tts_play_flag = bobao_ing;;
				rt_kprintf("input1 %d\n",tts_play_flag);
				TTS_play("�����û�������");
				
			break;
			default:
				break;
		}
	}
	return tts_play_flag;
}




/******************************************************************************
 * Function: index_txt () 
 * DESCRIPTION: - �������������ֶ�����ĺ��ֺ�ascii�ַ� 
 * Input: str��������
 * Input: length���ĳ���   index�洢����
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 16-jan-2014
* -------------------------------
 ******************************************************************************/
void index_txt(u8*str,u8 length)
{
#if 1
	u8 counter =0;//��¼��ѯ�˶��ٸ��ַ�
	u8 mark =0;//��ǵ�ַ
	u8 flag=0;//��Ǹ���
	u8 i=0;
	u8 number=0;//��¼��ѯ�ַ������ж��ٸ�ascii
	memset(index_addre,0,12);
	memset(index_len,0,12);
	//OutPrint_HEX("input_data:",str, length);
	if(length<20)//����С��20����һ����ʾ
	{
		index_len[flag] =length;
	}
	else
	{
	    
		for(i=0;i<length;i++)
		{
		    
			if(str[i]<0x7e)
			{
				number=number+1;
				rt_kprintf("ascii---%x--%d\r\n",str[i],i);
			}
			counter=counter+1;
			//���ֺ��ַ�����20���Ժ�Ĵ���
			if(counter%20==0)
			{
				//�ж��ַ��Ƿ���ż����ֻ���20С��ż��
				if(number%2==0)
				{
					index_addre[mark++] = i;
					index_len[flag++] = counter;
					//rt_kprintf("index[mark]---%d--%d\r\n",index_addre[mark-1],index_len[flag-1]);

				}
				//������治��ż������ֻ������ʾ19��
				else
				{
					index_addre[mark++] =i-1;
					index_len[flag++] = counter-1;
					//rt_kprintf("hunfa:index[mark]---%d--%d\r\n",index_addre[mark-1],index_len[flag-1]);
				}
				counter =0;
				number =0;
			}
		}
		
		if( length%20!=0)
		{
			index_len[flag]=(length%20);
		}
		
	}
	number =0;
	counter =0;
	//input = mark-1;
#endif
}


/******************************************************************************
 * Function: find_max_id () 
 * DESCRIPTION: - �ҵ�����IDֵ 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 15-jan-2014
* -------------------------------
 ******************************************************************************/
u32  find_max_id(u16 addre)
{
#if 1
	u8 temp[4];
	u32 tmp_id=0;
	u32 tran_id=0;
	u32 i=0;
	memset(temp,0,4);
	for(i=0;i<16*512;i=i+256)
	{
		DF_ReadFlash(addre,i,temp,4);
		tmp_id = (temp[0]<<24)+(temp[1]<<16)+(temp[2]<<8)+temp[3];
		if(tmp_id!=Max_value)
		{
		    if(tmp_id>tran_id)
			{
				tran_id = tmp_id;
			}
			/*
			else
			{
				tran_id = 0;
			}
			*/
		}
		
		//tran_id =tran_id +1;
	}
	rt_kprintf("the max ID --%d\r\n",tran_id+1);
	return tran_id ;
	#endif
}



/******************************************************************************
 * Function: Sort_ID () 
 * DESCRIPTION: - ����������ǰ�洢����������ID�͵�ַ 
 * Input: addre�洢��ʼ��ַ offset����ƫ����
 * Input: rank:��ѯID�ĵ�ַ
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 16-jan-2014
* -------------------------------
 ******************************************************************************/
u16 Sort_ID(u16 addre,u16 offset,u8 rank)
{
#if 1
	u8 temp[4];
	u32 i;
	u32 max_addre =0;
	u32 tmp_addre =0;
	memset(temp,0,4);
	max_addre = find_max_id(DF_BD_data_rx);
	#if 1
	if(max_addre==0)
	{
		return RT_ERROR;	
	}
	if(max_addre>16)
	{
		rank =max_addre-(16 -rank); 
	}
	#endif
	
	for(i=0;i<16*512;i=i+256)
	{
		DF_ReadFlash(addre,i,temp,4);
		tmp_addre= (temp[0]<<24)+(temp[1]<<16)+(temp[2]<<8)+temp[3];
		if(tmp_addre==rank)
		{
			offset =i;
			
			return offset;
		}
	}
	rt_kprintf("debug:the offset is %d--%d\r\n",offset,rank);
	return RT_ERROR;
	#endif
}

/******************************************************************************
 * Function: First_show () 
 * DESCRIPTION: - ÿһ�δ�����Ϣ���ŵ���ʾ 
 * Input: str ����
 * Input: str2 ��Ϣ����
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 15-jan-2014
* -------------------------------
 ******************************************************************************/
void  First_show(u8* str2 ,u8 Screen,u8 mode)
{
#if 1
    u32   addre=0;
	u8  show_buf[20];
	char T_date[22]={"00/00/00 00:00:00"};
	u8 i=0;
	memset(show_buf,0,20);
	lcd_fill(0);
	lcd_text12(0,5,number[Screen-1],strlen(number[Screen-1]),mode);
	for(i=0;i<3;i++)
		T_date[i*3]=str2[i+5]/10+'0';
	for(i=0;i<3;i++)
		T_date[1+i*3]=str2[i+5]%10+'0';

	for(i=0;i<3;i++)
		T_date[9+i*3]=str2[i+5+3]/10+'0';
	for(i=0;i<3;i++)
		T_date[10+i*3]=str2[i+5+3]%10+'0';
	lcd_text12(strlen(number[Screen-1])*6,5,T_date,17,mode);
	//rt_kprintf("mian_screen --%d---%d\r\n",strlen(number[Screen]),Screen);
	lcd_text12(0,19,"���ŵ�ַ:",8,mode);
	OutPrint_HEX("addre", str2+12, 3);
	addre=(str2[12]<<16)+(str2[13]<<8)+str2[14];
	sprintf((char*)show_buf,"%d",addre);
	lcd_text12(60,19,(char*)show_buf,strlen((char*)show_buf),mode);
	lcd_update_all();
	#endif
}
#if 0
/******************************************************************************
 * Function: Auto_line_show () 
 * DESCRIPTION: - �Զ�������ʾ���� 
 * Input: str ����
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 15-jan-2014
* -------------------------------
 ******************************************************************************/
u8  Auto_line_show( u8 *str , u8 len )
{
	u8 remain_len=0;
    if(len <=20)
	{

		lcd_fill(0);
		lcd_text12(0,5,str,len-Find_ascii(str ),LCD_MODE_SET);
		lcd_update_all();
		remain_len =Find_ascii(str );
		lcd_update_all();
	}
	if(20<len<=40 )
	{
		lcd_fill(0);
		lcd_text12(0,5,str,20- Find_ascii(str ),LCD_MODE_SET);
		lcd_text12(0,5,str+20- Find_ascii(str ),len-(20- Find_ascii(str+20- Find_ascii(str ))),LCD_MODE_SET);
		lcd_update_all();
		remain_len =Find_ascii(str )+Find_ascii(str+20- Find_ascii(str ));
		lcd_update_all();
	}
	if(len>40)
	{
		lcd_fill(0);
		lcd_text12(0,5,str,20-Find_ascii(str ),LCD_MODE_SET);
		lcd_text12(0,5,str+20-Find_ascii(str ),20-Find_ascii(str+20- Find_ascii(str )),LCD_MODE_SET);
		lcd_update_all();
		remain_len =len-(40-Find_ascii(str )-Find_ascii(str+20- Find_ascii(str )));
		lcd_update_all();
	}
	
	return remain_len ;
}
#endif 
#if 0
/******************************************************************************
 * Function: show_text () 
 * DESCRIPTION: - ��ʾ���� 
 * Input: n_screen �ǵڼ�������
 * Input: n_sub_screen  ��ǰ���ݿ���ʾ����
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 15-jan-2014
* -------------------------------
 ******************************************************************************/
show_text(u8 n_screen,u8 n_sub_screen)
{
	u8 temp[2];//���������ͳ���
	u8 tmp[256];//����ȫ������
	u8 timer[6];//ʱ��buffer
	u8 text_data[210];//��Ϣ����
	u16 len_txt;//�ı�����

	//����flash����
	if(mainshow==show_first)
	{
		memset(temp,0,10);
		memset(tmp,0,256);
		memset(timer,0,6);
		memset(text_data,0,210);
		mainshow=0;
		now_show=0;
		subshow=0;
		wait_show=0;
		len_txt=0;
		if(Erase_flag ==Top_15)
		{
			DF_ReadFlash(DF_BD_data_rx,n_screen*256,temp,2);
		}
		if(Erase_flag ==After_15)
		{
			DF_ReadFlash(DF_BD_data_rx+8,n_screen*256,temp,2);
		}
		if(1<=temp[0]<=15)
		{
			if(mainshow==show_first)
			{
				DF_ReadFlash(DF_BD_data_rx,n_screen*256,tmp,temp[1]);
				memcpy(timer,tmp+2,6);//ʱ��
				//��������ݳ���
				len_txt = tmp[14]<<8+tmp[15];
				len_txt=len_txt/8;
				memcpy( text_data,tmp+16,len_txt);
				now_show =len_txt;
			}
		}
		else
		{
			lcd_fill(0);
			lcd_text12(0,10,"�����ݿ���ʾ...",15,LCD_MODE_SET);
			lcd_update_all();	
		}
	}	
	//��ʼ��ʾ
			
				if(mainshow==show_first )
				{
					First_show(tmp ,n_screen);//��ʾʱ�������
					mainshow=show_end;//���showing==show_end������++,�ȵ�Auto_line_show����ֵΪ0 �����++;
					subshow=show_first;//������ʾ++;
				}
				wait_show =Auto_line_show(text_data, now_show);
				if(wait_show!=0)
				{
					now_show= wait_show;
				}
				else
				{
					mainshow=show_first ;//����������ʾ
					subshow=show_end;//������������ʾ
				}
		

	
}


#endif


/******************************************************************************
 * Function: Read_save_data () 
 * DESCRIPTION: - ��ȡ�洢���� 
 * Input: str�洢��������  input�ڼ����ĵ��� length���ĳ���
 * Input: mode ��ʾģʽ
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 16-jan-2014
* -------------------------------
 ******************************************************************************/
 u8 Read_save_data(u8*str,u8 input,u8 mode)
 {
  #if 1
	u8 temp[10];
	u8 data[256];//�Լ�����Ĵ洢��ʽ
	u16  Addre_offset=0;//�洢��ƫ�Ƶ�ַ
	u8 data_len =0;//
	memset(temp,0,10);
	memset(data,0,256);
	memset(str,0,210);//����Ϣ����
	
	Addre_offset =Sort_ID(DF_BD_data_rx, Addre_offset,input);
	rt_kprintf("Addre_offset--%d\r\n",Addre_offset);
    if(Addre_offset==RT_ERROR)
    {
		lcd_fill(0);
		lcd_text12(0,10,"�����ݿ���ʾ...",15,LCD_MODE_SET);
		lcd_update_all();
		return RT_ERROR;
    }
	
	//���ȶ��������ͳ��ȵ�����
	DF_ReadFlash(DF_BD_data_rx,Addre_offset,temp,5);
	//���ݳ����ٶ�����ʵ������
	DF_ReadFlash(DF_BD_data_rx,Addre_offset,data,temp[4]);
	//�����Լ�����ĸ�ʽ17����18�洢�Ž��յ��ĵĳ�����Ϣ//���յĴ洢���ȳ���8
	data_len = ((data[17]<<8) +data[18])/8;
	rt_kprintf("du chu shu ju len-- %d\r\n",temp[4]);
	OutPrint_HEX("Read", data,temp[4]);
	//length =data_len;
	//data_len=data_len/8;
	if(data[11]==0x60)//����ǻ췢�Ļ�
	{
		
		memcpy(str,data+20,data_len-1);
		index_txt(str,data_len-1);
		OutPrint_HEX("Read DATA ", str,data_len);
		OutPrint_HEX("index_addre",index_addre,12);
		OutPrint_HEX("index_len",index_len,12);
	}
	else
	{
		//���鿽�������str����
		memcpy(str,data+19,data_len);
		
		index_txt(str,data_len);
		OutPrint_HEX("Read DATA1 ", str,data_len);
		OutPrint_HEX("index_addre",index_addre,12);
		OutPrint_HEX("index_len",index_len,12);
	}
	
	//�ӵ��ĳ�����
	First_show(data ,input,mode);
	return data_len;
	#endif
 }
/******************************************************************************
 * Function: Rx_data_save () 
 * DESCRIPTION: - ѭ���洢15������ 
 * Input: str����������ͨ����Ϣ
 * Input: 
 * Output: 
 * Returns: 
 * 
 * --------------------------------------------------------------------------
* Created By wxg 14-jan-2014   1�ֽ�:���� 2�ֽ�:����
* -------------------------------
 ******************************************************************************/
#if 1
u8 Rx_data_save(u8 *str,u8 length)
{
	u8 temp[4];
	u8 trans[256];
	u8 len =0;
	u32 i= 0;
	u32 test_head=0;
	memset(trans,0,256);
	memset(temp,0,4);
	//OutPrint_HEX("str_test",str,length);
	for(i=0;i<16*512;i=i+256)
	{
	    
		DF_ReadFlash(DF_BD_data_rx,i,temp,4);
		
		test_head = (temp[0]<<24)+(temp[1]<<16)+(temp[2]<<8)+temp[3];
		//rt_kprintf(" %d--%d--%d--%d--%d--%x\r\n",temp[0],temp[1],temp[2],temp[3],i,test_head);
		if(test_head==Max_value)
		{
			
			rt_kprintf("watch i %d\n",i/256);
			test_head = find_max_id(DF_BD_data_rx)+1;//���ID�Լ�1+1;//���ID�Լ�1	
			
			
		    //�ҳ���һ��IDֵ
	    	trans[0]=test_head>>24;
			trans[1]=(test_head>>16)&0xff;
			trans[2]=(test_head>>8)&0xff;
			trans[3]=test_head&0xff;
			//�յ��ĵ��ĵĳ���
			//len =(((str[16]<<8)+str[17])/8);
			rt_kprintf("�յ����ĳ���---%d",length);
			//����Э�����7���ֽ�(��Ϣ��𵽵������ݽ����ĳ���)
			len =length+8;
			//+4�Ǽ��ϼ���+1����+��ʱ�䳤��=�ܳ�
		    trans[4] =len+4+6+1;

			if(UDP_dataPacket_flag==0x02)
			{
				trans[5]=Temp_Gps_Gprs.Date[0];
				trans[6]=Temp_Gps_Gprs.Date[1];
				trans[7]=Temp_Gps_Gprs.Date[2]; 

				trans[8]=Temp_Gps_Gprs.Time[0]; 
				trans[9]=Temp_Gps_Gprs.Time[1]; 
				trans[10]=Temp_Gps_Gprs.Time[2]; 
			}
			else
			{
				time_now=Get_RTC(); 

				trans[5]= time_now.year;
				trans[6]= time_now.month;
				trans[7]= time_now.day;

				trans[8]= time_now.hour;
				trans[9]= time_now.min;
				trans[10]= time_now.sec;
			}
			/*
		    trans[5]=(((Gps_Gprs.Date[0])/10)<<4)+((Gps_Gprs.Date[0])%10);		
			trans[6]=((Gps_Gprs.Date[1]/10)<<4)+(Gps_Gprs.Date[1]%10); 
			trans[7]=((Gps_Gprs.Date[2]/10)<<4)+(Gps_Gprs.Date[2]%10);
			trans[8]=((Gps_Gprs.Time[0]/10)<<4)+(Gps_Gprs.Time[0]%10);
			trans[9]=((Gps_Gprs.Time[1]/10)<<4)+(Gps_Gprs.Time[1]%10);
			trans[10]=((Gps_Gprs.Time[2]/10)<<4)+(Gps_Gprs.Time[2]%10);	
			*/
			OutPrint_HEX("Time", trans+5, 6);//wxg_test
		    if(trans[4]>=256)
		    {
		    rt_kprintf("-------������������-----\r\n");
			return RT_ERROR;
		    }
			memcpy(trans+11,str+10,trans[4]);//����Ϣ���ݿ���
			//+++++++++++++++++++++++++++++++++++++++++++++
			if(i==15*256)
		    {
				rt_kprintf("������벿��\n");
		    	//�����ϱ���ǰ�˸���������˸�
		    	SST25V_SectorErase_4KByte((8*(((u32)DF_BD_data_rx+8)/8))*PageSIZE);
				
		    }
			
			//++++++++++++++++++++++++++++++++++++++++++++
			DF_WriteFlashDirect(DF_BD_data_rx,i,trans,trans[4]);
			OutPrint_HEX("saved", trans, trans[4]);//wxg_test
			memset(temp,0,4);//����
			return RT_EOK;
		}
		
	if(i==31*256)//��֤��ʱ���п���д�Ŀռ�
	{
		rt_kprintf("����ǰ�벿��\n");
	    
		//�����ϰ벿��
		SST25V_SectorErase_4KByte((8*((u32)DF_BD_data_rx/8))*PageSIZE);	
		
		rt_kprintf("erase the 8016-8023\r\n");
	}
			
	}
	return RT_EOK;
}
#endif


/******************************************************************************
 * Function: show_screen () 
 * DESCRIPTION: - �Զ�������ʾ�ַ���������������� 
 * Input: str �������� input��ʾ�ڼ�����
 * Input: length ���ĳ���
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 16-jan-2014
* -------------------------------
 ******************************************************************************/
u8  show_screen(u8 *str ,u8 input,u16 length)
{
  #if 1
    u8 max_n=0;
  
	max_n =(length/40)+1;
	if(max_n>1)
	{
		if(length%40>0)
		{
			max_n=max_n+1;
		}
	}
	rt_kprintf("max_n --%d\r\n",max_n);
	
	OutPrint_HEX("Index:",index_addre, 12);
	if(input>max_n)
	{
	lcd_fill(0);
	lcd_text12(0,10,"�����ݿ���ʾ...",15,LCD_MODE_SET);
	lcd_update_all();	
	return  RT_ERROR;
	}
	switch(input)
	{
	case 1:
		lcd_fill(0);
		lcd_text12(0,5,str,index_len[0],LCD_MODE_SET);
		lcd_text12(0,19,str+index_addre[0]+1,index_len[1],LCD_MODE_SET);
		lcd_update_all();
	break;
	case 2:
		lcd_fill(0);
		lcd_text12(0,5,str+index_addre[1]+1,index_len[2],LCD_MODE_SET);
		lcd_text12(0,19,str+index_addre[2]+1,index_len[3],LCD_MODE_SET);
		lcd_update_all();
	break;
	case 3:
		lcd_fill(0);
		lcd_text12(0,5,str+index_addre[3]+1,index_len[4],LCD_MODE_SET);
		lcd_text12(0,19,str+index_addre[4]+1,index_len[5],LCD_MODE_SET);
		lcd_update_all();
	break;
	case 4:
		lcd_fill(0);
		lcd_text12(0,5,str+index_addre[5]+1,index_len[6],LCD_MODE_SET);
		lcd_text12(0,19,str+index_addre[6]+1,index_len[7],LCD_MODE_SET);
		lcd_update_all();
	break;
	case 5:
		lcd_fill(0);
		lcd_text12(0,5,str+index_addre[7]+1,index_len[8],LCD_MODE_SET);
		lcd_text12(0,19,str+index_addre[8]+1,index_len[9],LCD_MODE_SET);
		lcd_update_all();
	break;
		case 6:
		lcd_fill(0);
		lcd_text12(0,5,str+index_addre[9]+1,index_len[10],LCD_MODE_SET);
		lcd_text12(0,19,str+index_addre[10]+1,index_len[11],LCD_MODE_SET);
		lcd_update_all();
	break;
	
	}
	return  RT_EOK;
	#endif
}

/******************************************************************************
 * Function: Info_to_fill () 
 * DESCRIPTION: - 0200��Ϣ����� 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 12-jan-2014
* -------------------------------
 ******************************************************************************/
u16 Info_to_fill(void ) 
{
		u16  U3_Tx_Wr = 0;
		u16  info_len =0;
		u32  Dis_01km=0;
		//------------------------------- Stuff --��λ�������--------------------------------------
		//memcpy( ( char * ) U3_Tx+ U3_Tx_Wr, "#TCB",4 );
		//U3_Tx_Wr += 4;
		
		memcpy( ( char * ) U3_Tx+ U3_Tx_Wr, ( char * )Warn_Status,4 );    
		U3_Tx_Wr += 4;

		memcpy( ( char * ) U3_Tx+ U3_Tx_Wr, ( char * )Car_Status,4 );   
		U3_Tx_Wr += 4;

		memcpy( ( char * ) U3_Tx+ U3_Tx_Wr,( char * )  Gps_Gprs.Latitude, 4 );
		U3_Tx_Wr += 4;

		memcpy( ( char * ) U3_Tx+ U3_Tx_Wr, ( char * )  Gps_Gprs.Longitude, 4 );	  
		U3_Tx_Wr += 4;

		U3_Tx[U3_Tx_Wr++]=(u8)(GPS_Hight<<8);
		U3_Tx[U3_Tx_Wr++]=(u8)GPS_Hight;

		U3_Tx[U3_Tx_Wr++]=(u8)(Speed_gps>>8);//(Spd_Using>>8); 
		U3_Tx[U3_Tx_Wr++]=(u8)(Speed_gps);//Spd_Using;     

		U3_Tx[U3_Tx_Wr++]=(GPS_direction>>8);  //High 
		U3_Tx[U3_Tx_Wr++]=GPS_direction; // Low

		U3_Tx[U3_Tx_Wr++]=(((Gps_Gprs.Date[0])/10)<<4)+((Gps_Gprs.Date[0])%10);		
		U3_Tx[U3_Tx_Wr++]=((Gps_Gprs.Date[1]/10)<<4)+(Gps_Gprs.Date[1]%10); 
		U3_Tx[U3_Tx_Wr++]=((Gps_Gprs.Date[2]/10)<<4)+(Gps_Gprs.Date[2]%10);
		U3_Tx[U3_Tx_Wr++]=((Gps_Gprs.Time[0]/10)<<4)+(Gps_Gprs.Time[0]%10);
		U3_Tx[U3_Tx_Wr++]=((Gps_Gprs.Time[1]/10)<<4)+(Gps_Gprs.Time[1]%10);
		U3_Tx[U3_Tx_Wr++]=((Gps_Gprs.Time[2]/10)<<4)+(Gps_Gprs.Time[2]%10);	 
		U3_Tx[U3_Tx_Wr++]=0x03; 

		U3_Tx[U3_Tx_Wr++]=2;

		U3_Tx[U3_Tx_Wr++]=(u8)(Speed_cacu>>8); 
		U3_Tx[U3_Tx_Wr++]=(u8)(Speed_cacu);	     

		U3_Tx[U3_Tx_Wr++]=0x01; 

		U3_Tx[U3_Tx_Wr++]=4; 

		Dis_01km=JT808Conf_struct.Distance_m_u32/100;
		U3_Tx[U3_Tx_Wr++]=(Dis_01km>>24); 
		U3_Tx[U3_Tx_Wr++]=(Dis_01km>>16); 
		U3_Tx[U3_Tx_Wr++]=(Dis_01km>>8); 
		U3_Tx[U3_Tx_Wr++]=Dis_01km; 

		U3_Tx[U3_Tx_Wr++]=0x25; 

		U3_Tx[U3_Tx_Wr++]=4; 

		// U3_Tx[U3_Tx_Wr++]= BD_EXT.FJ_SignalValue; 

		U3_Tx[U3_Tx_Wr++]=0x00;
		U3_Tx[U3_Tx_Wr++]=0x00;
		U3_Tx[U3_Tx_Wr++]=0x00;
		U3_Tx[U3_Tx_Wr++]=BD_EXT.Extent_IO_status; 

#if 0
		U3_Tx[U3_Tx_Wr++]=0xFE; 

		U3_Tx[U3_Tx_Wr++]=2; 

		U3_Tx[U3_Tx_Wr++]= BD_EXT.FJ_SignalValue; 
		U3_Tx[U3_Tx_Wr++]=0x00;  


		U3_Tx[U3_Tx_Wr++]=0xFF; 

		U3_Tx[U3_Tx_Wr++]=6; 

		U3_Tx[U3_Tx_Wr++]= BD_EXT.FJ_IO_1; 
		U3_Tx[U3_Tx_Wr++]= BD_EXT.FJ_IO_2;  
		U3_Tx[U3_Tx_Wr++]=(BD_EXT.AD_0>>8); 
		U3_Tx[U3_Tx_Wr++]=BD_EXT.AD_0;
		U3_Tx[U3_Tx_Wr++]=(BD_EXT.AD_1>>8);  
		U3_Tx[U3_Tx_Wr++]=BD_EXT.AD_1;
		info_len = U3_Tx_Wr-1;
#endif	
		info_len = U3_Tx_Wr;
		return info_len;
		
}


/******************************************************************************
 * Function:  () 
 * DESCRIPTION: - BD1����Ϣ��� 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 12-jan-2014
* -------------------------------
 ******************************************************************************/

void Check_bd_data(void)
{
#if 1
 if(((GSM_PWR.result == result_success)&&(BD_ZJ.result==result_pending))||(BD_ZJ.status==result_Artificial))
 {
 	
 	BD_ZJ.Time_consum++;
	if((BD_ZJ.Time_consum%2)==0)//ÿ���뷢��һ��
 	BD1_Tx(BD1_TYPE_XTZJ,0,2);
	#if 0
	if(BD_ZJ.Time_consum>10)//�����������
	{
		BD_ZJ.result=result_failed;
		BD_ZJ.status=result_failed;
		BD_ZJ.Time_consum =0;
		TTS_play("�����û����޷�����");
	}
	#endif
 }
 if(0==tts_play_flag)
 {
	 if(((GSM_PWR.result == result_success)&&(BD_ZJ.result==result_success)&&(BD_IC.result==result_pending))||(BD_IC.status==result_Artificial))
	 {

		
		
			BD_IC.Time_consum++;
		
		if((BD_IC.Time_consum%2)==0)//ÿ���뷢��һ��
		BD1_Tx(BD1_TYPE_ICJC, 0, 1);
		#if 0
		if(BD_IC.Time_consum>10)
		{
		    BD_IC.status=result_failed;
			BD_IC.result = result_failed;
			BD_IC.Time_consum =0;
			TTS_play("�����û������ʧ��");
		}
		#endif
		
		
	 }
 }
 #endif
	
}
/******************************************************************************
 * data_recv() 
 * DESCRIPTION: ��Ϣ���н��յĺ���  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *  05 6 2014, wxg written
 * --------------------
 ******************************************************************************/
u8 data_recv(void)
{
	rt_err_t result;
	if((BD_IC.result==result_success)&&(BD1_struct.in_freq!=0))
	{
		if(BD_TX.flag_send==RT_EOK)
		{
			
				
			memset(U3_Tx,0,sizeof(U3_Tx));
			result=rt_mq_recv(&mq_bd1,&U3_Tx[0],sizeof(U3_Tx),RT_WAITING_NO);
			rt_kprintf("���ս�� %d\n",result);
			if(result==RT_EOK)
			{
				u3_send_len(U3_Tx,BD_TX.length);
				OutPrint_HEX("write_BD1:", U3_Tx, BD_TX.length);
				BD_TX.flag_send =RT_EBUSY;
				//ÿ�η��������Ҫ����������,����վ�������¿�ʼ����,ģʽΪ�Զ�����
				BD_TX.length=0;
				BD_FRE.llcd_time_wait =0;
				BD_TX.status=bd1_send_auto;
			}
			else
			{
				if(BD_TX.status==bd1_send_auto)
				{
					memset(U3_Tx,0,sizeof(U3_Tx));
					//Ĭ�ϵ�����·���0200��Ϣ����Ӫƽ̨
					BD1_Tx(BD1_TYPE_TXSQ,U3_Tx,Info_to_fill());	
				}
			}
		}
		
	}
	
}

/******************************************************************************
 * Function:  () 
 * DESCRIPTION: - ��ʱ��������һ����վƵ�ȼ���Ĵ����׼ʱ��Ϊ1s 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 14-jan-2014
* -------------------------------
 ******************************************************************************/

void BD_Timer_out(void *  parameter)
{


  	 Check_bd_data();
	if(BD_TX.flag_send!=RT_EOK)
		{
			BD_FRE.llcd_time_wait++;
			if(BD_FRE.llcd_time_wait==(BD1_struct.in_freq+1))
			{
				BD_FRE.llcd_time_wait=0;
				
				
				BD_TX.flag_send=RT_EOK;
			}
		}
#if 0	
	if((GSM_PWR.result == result_success)&&(get_fist==0))
	{
		bd1_link_time++;
		if(bd1_link_time>60)
		{
			you_can_deial =1;
			get_fist =1;
			bd1_link_time =0;
		}
	}
#endif	
#if 0	
	//rt_kprintf("test----%d--%d\r\n",BD1_struct.in_freq,BD_FRE.Frequency);
	if((BD_IC.result==result_success)&&(BD1_struct.in_freq!=0))
	{
		if(BD_TX.flag_send==RT_EOK)
		{
			
				
			memset(U3_Tx,0,sizeof(U3_Tx));
			result=rt_mq_recv(&mq_bd1,&U3_Tx[0],sizeof(U3_Tx),RT_WAITING_NO);
			rt_kprintf("���ս�� %d\n",result);
			if(result==RT_EOK)
			{
				u3_send_len(U3_Tx,BD_TX.length);
				OutPrint_HEX("write_BD1:", U3_Tx, BD_TX.length);
				BD_TX.flag_send =RT_EBUSY;
				//ÿ�η��������Ҫ����������,����վ�������¿�ʼ����,ģʽΪ�Զ�����
				BD_TX.length=0;
				BD_FRE.llcd_time_wait =0;
				BD_TX.status=bd1_send_auto;
			}
			else
			{
				if(BD_TX.status==bd1_send_auto)
				{
					memset(U3_Tx,0,sizeof(U3_Tx));
					//Ĭ�ϵ�����·���0200��Ϣ����Ӫƽ̨
					BD1_Tx(BD1_TYPE_TXSQ,U3_Tx,Info_to_fill());	
				}
			}
		}
		else
		{
			BD_FRE.llcd_time_wait++;
			if(BD_FRE.llcd_time_wait==(BD1_struct.in_freq+1))
			{
				BD_FRE.llcd_time_wait=0;
				
				
				BD_TX.flag_send=RT_EOK;
			}
		}
	}
#endif	
#if 0	
    if((BD_IC.result==result_success)&&(BD1_struct.in_freq!=0))
    {
		BD_FRE.llcd_time_wait++;
		//rt_kprintf("BD_FK.result=%d\r\n",BD_FK.result);
		if(BD_FK.result==result_pending)
		{
			if(BD_FRE.llcd_time_wait==(BD1_struct.in_freq-3))
			{
				TTS_play("�����ظ���ʱ��������");
				BD_FK.result=result_failed;
				//����ʧ����
				dwlcd =0;//С������
				BD_TX.status = bd1_send_auto;
			}
			
		}
		if(BD_FRE.llcd_time_wait==(BD1_struct.in_freq+1))
		{
			//���ڿ��Է����˵ȴ�����
			BD_TX.flag_send=RT_EOK;
			//������¼���ģʽ
			BD_FRE.llcd_time_wait=0;
		  // if(ModuleStatus&Status_GPS )//�����λ�ͷ���0200��Ϣ
		   	{
				
				
				memset(U3_Tx,0,sizeof(U3_Tx));
				//�Զ�����״̬�·���Ĭ����Ϣ
				if(BD_TX.status == bd1_send_auto)
				{
					BD1_Tx(BD1_TYPE_TXSQ,U3_Tx,Info_to_fill());	
					BD_TX.flag_send=RT_EBUSY;
					memset(U3_Tx,0,sizeof(U3_Tx));
					BD_FK.result=0;//���Ͷ�λ����ʱ��ʹ�ܳ�ʱ����
					
				}
		   	}
		   
			if(BD_TX.status==bd1_send_Artificial)
			{
				if(dwlcd==0)//С������
				{
					BD1_Tx(BD1_TYPE_TXSQ,trans[number_txt],strlen(trans[number_txt]));//��̨��������
				}
				
					//����С��Ļ�л�ʱ��ü�ʱ
					BD_TX.Time_consum =1;
					//�ڷ��ͼ���д���æ״̬
					BD_TX.flag_send=RT_EBUSY;
					//��������ص���������
					BD_TX.llcd_time_wait=0;
					lcd_fill(0);
					lcd_text12(10,10,"ͨ�������Ѿ�����",16,LCD_MODE_SET);
					lcd_update_all();
				
			}
			
			
		}
    }
	//С��Ļ����ʾ�����������л�
	if(BD_TX.Time_consum ==1)
	{
		BD_TX.llcd_time_wait++;
		if(BD_TX.llcd_time_wait==5)
		{
			pMenuItem=&Menu_1_Idle;
			BD_TX.Time_consum =0;
		}
	}
	//rt_kprintf("-----test bd1 timer output---\r\n");
	#endif
}


/******************************************************************************
 * Function: BD1_init(void) 
 * DESCRIPTION: - ���ݳ�ʼ�� 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 06-jan-2014
* -------------------------------
 ******************************************************************************/

void BD1_init(void)
{
	BD1_control.data_ask =0;
	//BD1_struct.IC_state =0;
	BD1_control.data_frame =0;//IC���֡��
	BD1_control.data_freq_test =0;//�Լ�Ƶ��
	BD1_control.data_outfreq =0;//ʱ�����Ƶ��
	BD_IC.Time_consum =0;
	
	BD_FRE.llcd_time_wait=0;
	BD_IC.result = result_pending;
	BD_ZJ.result=result_pending;
	BD_FRE.status =0;
	BD_TX.status =bd1_send_auto;
	BD_TX.flag_send=RT_EOK;//RT_ERROR
	BD1_struct.Rx_enable =RT_EBUSY;
	BD_FK.Time_consum=0;
	BD_ZJ.flag_send=0;
	BD1_struct.in_freq = 20;
	BD_TX.info_source =0;
	//mainshow=show_first;
	memset(BD1_control.data_user_addr,0,3);
	memset(BD1_struct.UserAddr,0,3);
	//���ڲ��Բ�����ɺ�ɾ��++++++++++++++++++++++++

}
/******************************************************************************
 * Function: Read_bd1_data () 
 * DESCRIPTION: - ��ȡ����1�洢��������� 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 27-dec-2013
* -------------------------------
 ******************************************************************************/
void Read_bd1_data(void)
{

	u8 data[260];
	memset(data,0,sizeof(data));
	//********************��ȡͨ��Ŀ���ַ����վƵ��******************
	DF_ReadFlash(DF_BD_first_target,0,data,5);
	//OutPrint_HEX("�������", data, 5);
	if(0xFF==data[0])
	{
	    memset(BD1_control.data_user_addr,0,3);
		rt_kprintf("NO Target address,default number 0\r\n");
	}
	else
	{
		BD1_control.data_user_addr[0] = data[0];
		BD1_control.data_user_addr[1] = data[1];
		BD1_control.data_user_addr[2] = data[2];
		rt_kprintf("ͨ�ŵ�ַ%02x--%02x--%02x\r\n",data[0],data[1],data[2]);
	}
	/*
	if(0xff!=data[3])
	{
		BD1_struct.in_freq= (data[3]<<8) +data[4];
		rt_kprintf("Positioning frequency %ds\r\n",BD1_struct.in_freq);
	}
	else
	{
		
		rt_kprintf("NO Positioning frequency \r\n");
		BD1_struct.in_freq =0;
	}
	*/
	memset(data,0,sizeof(data));
	
}




u8  BD1_Tx(u8 Type,void *str,u8 stuff_len)
{
	u8  bd_send[300]; 
	u16  d_len,len=0;
	u8  fcs=0;
	u8  stuff[260];
	u16  BD_temp =0;
	static u8  flag_mq =0;
	rt_err_t result;
	memset(bd_send,0,sizeof(bd_send));
	memset(stuff,0,sizeof(stuff));
	// stuff 
	switch(Type)
		{
		case  BD1_TYPE_DWSQ:
	                          memcpy(bd_send,"$DWSQ",5);
							  BD_FK.result=result_pending;
							  //�и߳� 
							  if(BD1_control.data_altimeter ==0)
							  {
							  	if(BD1_control.data_mode ==0)
							  	{
							  		stuff[0] = 0;//��ͨ
							  		stuff[1] = 0x80;
									stuff[2] = 0;
									stuff[3] = 0;
									stuff[4] = 0;
									stuff[5] = 0;
									stuff[6] = 0;
									stuff[7] = 0;
									stuff[8] = 0;
									stuff[9] = BD1_control.data_freq>>8;
									stuff[10] = BD1_control.data_freq&0XFF;
							  	}
								/*
								else
								{
									stuff[0] = 1;//�߿�
							  		stuff[1] = Aerial_air.ground_high>>8;
									stuff[2] = Aerial_air.ground_high&0xff;
									stuff[3] = 0;
									stuff[4] = 0;
								}
								*/
							  }
							  //�޲�ߺͲ��1һ��
							  if((BD1_control.data_altimeter ==1)||(BD1_control.data_altimeter ==2))
							  {
							  	if(BD1_control.data_mode ==0)
							  	{
							  		stuff[0] = 0x04;//��ͨ
							  		stuff[1] = 0;
									stuff[2] = 0;
									stuff[3] = 0;
									stuff[4] = 0;
									//��ʽʱ����Ϊ0.1��
									stuff[5] = 0;
									stuff[6] = 0;
									stuff[7] = 0;
									stuff[8] = 0;
									stuff[9] = BD1_control.data_freq>>8;
									stuff[10] = BD1_control.data_freq&0XFF;
							  	}
								/*
								else
								{
									stuff[0] = 0x05;//�߿�
									stuff[1] = 0;
									stuff[2] = 0;
									stuff[3] = Aerial_air.ground_high>>8;;
									stuff[4] = Aerial_air.ground_high&0xff;//��ʽʱ����Ϊ0.5��
								}
								*/
							  }
							  //���2
							  if(BD1_control.data_altimeter ==3)
							  {
							  	if(BD1_control.data_mode ==0)
							  	{
							  		stuff[0] = 0x0c;//��ͨ
							  		stuff[1] = 0;
									stuff[2] = 0;
									stuff[3] = Aerial_air.aerial_barograph>>8;;
									stuff[4] = (Aerial_air.aerial_barograph)&0xff;//��ʽʱ����Ϊ0.5��
									//��ѹ����Ŀǰ������Ϊ0
									stuff[5] = 0;
									stuff[6] = 0;
									stuff[7] = 0;
									stuff[8] = 0;
									stuff[9] = BD1_control.data_freq>>8;
									stuff[10] = BD1_control.data_freq&0XFF;
							  	}/*
								else
								{
									stuff[0] = 0x0d;//�߿�
								}*/
							  }
		                      break;

		case  BD1_TYPE_TXSQ:
							memcpy(bd_send,"$TXSQ",5);
							BD_FK.result=result_pending;
							
							if(0x46!=BD1_control.message_mode)
							{
								BD1_control.message_mode =0x44;//����
							}
							else
							{
								BD1_control.message_mode =0x46;//����
							}
							stuff[0] = 0x46;//Ĭ��Ϊ�췢ģʽ
							//stuff[0] = BD1_control.message_mode;
							//ͨ�ŵ�ַ
							stuff[1] = BD1_control.data_user_addr[0];
							stuff[2] = BD1_control.data_user_addr[1];
							stuff[3] = BD1_control.data_user_addr[2];
							//ͨ������
							
							stuff_len=stuff_len+1;//���ȼ��ϻ췢��ͷ(A4)
							//rt_kprintf("stuff_len+1  %d\n",stuff_len+1);
							BD_temp =stuff_len*8;
							//���ĳ���
							stuff[4] = BD_temp >>8;
							stuff[5] = BD_temp &0xff;
							//�Ƿ�Ӧ��
							stuff[6] = BD1_control.data_ask;
							stuff[7] = 0xA4;
							memcpy(stuff+8, str, stuff_len-1);//�������ĵ�����ʵ����(û�мӻ췢��ͷ)
							//rt_kprintf("���ĳ���%d\r\n",stuff_len-1);
							stuff_len=(stuff_len-1)+8;//����ʵ�ʳ��ȼ���һ��A4+ǰ��7λ
							flag_mq =1;
						
							break;
			   
		case  BD1_TYPE_CKSC: 
			                  memcpy(bd_send,"$CKSC",5);
							 // stuff[0]=0;
				             break;
			   
	    case  BD1_TYPE_ICJC:
			                 memcpy(bd_send,"$ICJC",5);
							 stuff[0]=BD1_control.data_frame;
				             break;

		case  BD1_TYPE_XTZJ:
							
	                         memcpy(bd_send,"$XTZJ",5);
							 stuff[0] = BD1_control.data_freq_test>>8;
							 stuff[1] = (BD1_control.data_freq_test)&0xff;
		                     break;

		case  BD1_TYPE_SJSC:
	                         memcpy(bd_send,"$SJSC",5);
							 stuff[0] = BD1_control.data_outfreq>>8;
							 stuff[1] = (BD1_control.data_outfreq) & 0xff;
			                 break;
		case  BD1_TYPE_BBDQ:
	                         memcpy(bd_send,"$BBDQ",5); 
		                     break;

		}
		//clear 
		
		len=10+stuff_len+1;  // �ܵ����ݳ���
		//rt_kprintf("len... %d\n",len);
		//���ݳ���
		bd_send[5]=len>>8;
		bd_send[6] = len & 0xff;
		//�û���ַ��ô����?
		bd_send[6+1]=BD1_struct.UserAddr[0];
		bd_send[7+1]=BD1_struct.UserAddr[1];
		bd_send[8+1]=BD1_struct.UserAddr[2]; 	

		memcpy(bd_send+10,stuff,stuff_len); 

	//   caculate  fcs
	fcs=0;
	for(d_len=0;d_len<len-1;d_len++)
	{
  		fcs^=bd_send[d_len];
	}
	bd_send[d_len]=fcs;
	//rt_kprintf("d_len %d\n",d_len);
#if 1		
		bd_send[d_len+1] =0x0D;
		bd_send[d_len+2] =0x0A;

		//rt_kprintf("У��͵�ֵ%d---%d\r\n",bd_send[d_len],d_len);
	if(flag_mq==1)
	{
		//����н�����������λ��־
		if(WARN_StatusGet())
		{
			StatusReg_WARN_Enable();
			result = rt_mq_urgent(&mq_bd1,&bd_send[0],len+2);
			rt_kprintf("bd1 jin ji bao jing fa song\n");
		}
		else
		{
			StatusReg_WARN_Clear();
			if(BD_TX.status ==bd1_send_Artificial)
			{
				result = rt_mq_urgent(&mq_bd1,&bd_send[0],len+2);
				rt_kprintf("shou dong fa song  \n");
			}
			else
			{
				result =rt_mq_send(&mq_bd1,&bd_send[0],len+2);
			}
			
		}
		rt_kprintf("fa song jie guo %d\n",result);
		BD_TX.length =len+2;//���ȸ�ֵ��ȫ�ֵĳ���,���ڽ��ղü�;
		if(result==-RT_EFULL)
		{
			rt_kprintf("��Ϣ�����Ѿ�����,�ȴ���Ϣ\n");
			return RT_ERROR;
		}
	}
	else
	{
		OutPrint_HEX("write_BD1:", bd_send, len+2);
		u3_send_len(bd_send,len+2);
	}
	return RT_EOK;
#endif
		//u3_send_len(bd_send,len);
		//OutPrint_HEX("write_BD1:", bd_send, len);

	//memset(str,0,sizeof(str));
	 //һ�����;�ֵλ;

}
/******************************************************************************
 * BD1_RxProcess() 
 * DESCRIPTION: ����ӱ���1�ն˷�����������  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *  08 5 2014, wxg written
 * --------------------
 ******************************************************************************/
void BD1_RxProcess(void)  
{

u8  Broadcast[260];
u8 i=0,j=0,cycles =0;
u8 boundary[10] ={0};
u8 tran[300]={0};
u16 data_len =0;
//+++++++++++++++++++++++++++++++++++++++++++ring_buffer+++++++++++++
unsigned char CH;
rt_err_t result;
//  Debug

#if 1
while((rt_ringbuffer_getchar(&bd1_pool,&CH))==1)
{
	rt_kprintf("%02X ",CH);
	 tran[U3_rxCounter++]=CH;
	  if(tran[0]!='$')
	 { 	
	     U3_rxCounter=0;
     }
	  if(U3_rxCounter==7)
	 {
	   //5��6�ֽ��ǳ�����Ϣ
	   U3_content_len=(tran[5]<<8)+tran[6];	// byte 5   �ǳ����ֽ�����	
	   rt_kprintf("U3_content_len %d\n",U3_content_len);
	 }
	 
    //���յ������ݳ��ȵ���lenĬ��Ϊ������
	 if(U3_content_len==U3_rxCounter)
	 	{
	 	   BD1_struct.Rx_enable=RT_EOK;//�յ�����Ϣ�Ŵ���			  
          U3_rxCounter =0;
		   break;
	 	} 
}
#endif
if( BD1_struct.Rx_enable==RT_EOK)
{
OutPrint_HEX("U3_rx_hex",tran,U3_content_len);	
memset(Broadcast,0,sizeof(Broadcast));
if(strncmp(tran,"$GLJC",5)==0)	  // ���ʼ��
{
		
 BD1_struct.Output_freq = tran[10];
 /*
 sprintf(Broadcast,"%d",BD1_struct.Output_freq);
 TTS_play("���ʼ���ֵΪ");
 TTS_play(Broadcast);
 memset(Broadcast,0,sizeof(Broadcast));
 */
}

if(strncmp(tran,"$DWXX",5)==0)	// ��λ��Ϣ
	{
	   // check info_type	 tran[9]
	   // position	  tran[13]
	   BD1_struct.hour= tran[13+1];
   BD1_struct.minute= tran[14+1];
   BD1_struct.second= tran[15+1];
   BD1_struct.little_second= tran[16+1];

   BD1_struct.longi_Du= tran[17+1];
   BD1_struct.longi_Fen= tran[18+1];
   BD1_struct.longi_Miao= tran[19+1];
   BD1_struct.longi_Little_Miao= tran[20+1];
   

   BD1_struct.lati_Du= tran[21+1];
   BD1_struct.lati_Fen= tran[22+1];
   BD1_struct.lati_Miao= tran[23+1];
   BD1_struct.lati_Little_Miao= tran[24+1]; 
	rt_sprintf(Broadcast,"��λ��Ϣ:����%d��%d��%d��%d��,γ��:%d��%d��%d��%d��",BD1_struct.longi_Du,BD1_struct.longi_Fen,BD1_struct.longi_Miao,\
	BD1_struct.longi_Little_Miao,BD1_struct.lati_Du,BD1_struct.lati_Fen,BD1_struct.lati_Miao,BD1_struct.lati_Little_Miao);
   TTS_play(Broadcast);
   BD1_struct.Ground_height= (tran[25+1]<<8)+tran[26+1];
   
   BD1_struct.User_height= (tran[27+1]<<8)+tran[28+1]; 

   // debug
   rt_kprintf("\r\n ����1 λ����Ϣ: %d:%d:%d.%d   Longi  %d��%d��%d.%d��  lati	%d��%d��%d.%d��  ��ظ߳� %d  �û��߳� %d \r\n",\
   BD1_struct.hour,BD1_struct.minute,BD1_struct.second,BD1_struct.little_second,\
   BD1_struct.longi_Du,BD1_struct.longi_Fen,BD1_struct.longi_Miao,BD1_struct.longi_Little_Miao,\
   BD1_struct.lati_Du,BD1_struct.lati_Fen,BD1_struct.lati_Miao,BD1_struct.lati_Little_Miao,\
   BD1_struct.Ground_height&0x3f,BD1_struct.User_height&0x3f );

	}
if((strncmp(tran,"$TXXX",5)==0))	// ͨ����Ϣ 
 {

	 BD1_struct.Text_len=(tran[16]<<8)+tran[17];
	 BD1_struct.Text_len =BD1_struct.Text_len/8;
	 memset(BD1_struct.Text_info,0,sizeof(BD1_struct.Text_info));
	 OutPrint_HEX("mgic_number",tran+18,5);
	 if(tran[10]==0x60)//����ǻ췢
	 {

		memcpy(BD1_struct.Text_info,tran+19,BD1_struct.Text_len);
		
		//if(strcmp(tran+19,"#TCB")!=0)
		if(BD_TX.info_source==1)
		{
			TTS_play(BD1_struct.Text_info);
			Rx_data_save(tran,BD1_struct.Text_len);
			BD_TX.info_source =0;
		}
		
	 }
	 else
	 {
		memcpy(BD1_struct.Text_info,tran+18,BD1_struct.Text_len);
		//if(strcmp(tran+19,"#TCB")!=0)
		if(BD_TX.info_source==1)
		{
			TTS_play(BD1_struct.Text_info);
			Rx_data_save(tran,BD1_struct.Text_len);
			BD_TX.info_source =0;
		}
		
	 }
	 
	 
	 rt_kprintf("	����1 �����ı���Ϣ:%s\n", BD1_struct.Text_info); 
 }

if(strncmp(tran,"$ICXX",5)==0) // IC  ��Ϣ 
 {
	memcpy(BD1_struct.UserAddr,tran+7,3);
	BD1_struct.Frame_num = tran[10];
	memset(BD1_struct.Broadcast_ID,0,sizeof(BD1_struct.Broadcast_ID));
	memcpy(BD1_struct.Broadcast_ID,tran+11,3);
	BD1_struct.User_status = tran[14];
	BD1_struct.Service_freq = (tran[15]<<8)+tran[16];
	if(BD1_struct.Service_freq>=BD1_struct.in_freq)
	{
		BD1_struct.in_freq= BD1_struct.Service_freq;
	}

	BD1_struct.Communicate_grade =	tran[17];
	BD1_struct.Encode_state = tran[18];
	BD1_struct.Sub_user = (tran[19]<<8)+tran[20];
	BD_IC.result=result_success;
	BD_IC.status=result_success;
	//tts_play_flag =2;//��ʾ��Ҫ����������
	TTS_play("�����û�������");
	
 }
if(strncmp(tran,"$ZJXX",5)==0)	// �Լ���Ϣ
{
	BD1_struct.IC_state =  tran[10];
	BD1_struct.Hrdware_sate = tran[11];
	BD1_struct.Battery_stae = tran[12];
	BD1_struct.Inbound_status = tran[13];
#ifdef  Beam_power_numb//���幦��״��
	memset(BD1_struct.Beam_power,0,sizeof(BD1_struct.Beam_power));
	memcpy(BD1_struct.Beam_power,tran+14,6);
#endif
	if((0==BD1_struct.IC_state)&&(0==BD1_struct.Hrdware_sate))
	{
		BD_ZJ.result = result_success;
		//tts_play_flag=1;
		TTS_play("�����ն�����");
		BD_ZJ.status=result_success;
	}
	BD1_control.BD_Signal =0;
	for(i=0;i<6;i++)
	{
		if((BD1_struct.Beam_power[i])!=0)
		{
			 BD1_control.BD_Signal=BD1_control.BD_Signal+1;
		}
		//rt_kprintf("��������--%d",BD1_struct.Beam_power[i]);
	}
	BD_ZJ.flag_send =result_success;
	//rt_kprintf("BD1_control.BD_Signal--%d\r\n",BD1_control.BD_Signal);
}

if(strncmp(tran,"$SJXX",5)==0) // ʱ����Ϣ
  {
	BD1_struct.year = (tran[10]<<8)+tran[11];
	BD1_struct.Month = tran[12];
	BD1_struct.Day = tran[13];
	BD1_struct.hour = tran[14];
	BD1_struct.minute = tran[15];
	BD1_struct.second = tran[16];
	
  }
if(strncmp(tran,"$BBXX",5)==0) // �汾��Ϣ 
   {
#ifdef  Version_Info
	memset(Version,0,Version_Info);
	memcpy(Version,(char*)tran[10],Version_Info);
#endif	
	 
   }

if(strncmp(tran,"$FKXX",5)==0) //������Ϣ
   {
	 BD1_struct.Feedback_flag = tran[10];
	 if(BD1_struct.Feedback_flag==0x00)
	 {
		BD_FK.result =result_success; 
		#if 0
		memset(Big_lcd.TXT_content,0,sizeof(Big_lcd.TXT_content));//�����������
		Lcd_write(Big_lcd.status,LCD_Text_clear,0);//����������
		delay_ms(10);
		Lcd_write(Big_lcd.status,LCD_PAGE,1);
		//���ͳɹ���
		#endif
		//TTS_play("ͨ�ųɹ�");
		dwlcd =0;//С������
		BD_TX.status = bd1_send_auto;
		
	 }
	 else
	 {
		BD_FK.result = result_failed;
		rt_kprintf("Reasons for failure--%d\r\n",BD1_struct.Feedback_flag);
		 memset(BD1_struct.Add_Info,0,4);
		memcpy(BD1_struct.Add_Info,tran+11,4);
		if(strncmp(BD1_struct.Add_Info,"DWSQ",4)==0)
		{
			
			//BD_FK.flag_send =DWCF;
			TTS_play("��λʧ��");
		}
		if(strncmp(BD1_struct.Add_Info,"TXSQ",4)==0)
		{
			//BD_FK.flag_send =TXCF;
			TTS_play("ͨ��ʧ��");
		}
	
	  }

	}
memset(tran,0,sizeof(tran));
}


BD1_struct.Rx_enable=RT_EBUSY;//�����busy��ָ���ڷ���ʱ�����ȴ���
}

u16  Protocol_808_Decode_Good(u8 *Instr ,u8* Outstr,u16  in_len)  // ����ָ��buffer :  UDP_HEX_Rx  
{
	//-----------------------------------
	  u16 i=0, decode_len=0;

    // 1.  clear  write_counter
	  decode_len=0;//clear DecodeLen

	// 2   decode process   
	for(i=0;i<in_len;i++)
	 {
		if((Instr[i]==0x7d)&&(Instr[i+1]==0x02))
		{
		   Outstr[decode_len]=0x7e;
		   i++;
		}
		else
		if((Instr[i]==0x7d)&&(Instr[i+1]==0x01))
		{
		   Outstr[decode_len]=0x7d;
		   i++;
		}
		else  
		{
		  Outstr[decode_len]=Instr[i];    
		}
	    decode_len++;
	 }	
    //  3.  The  End
        return decode_len;
}

void CAN2_RxHandler(unsigned char rx_data)
{

   //-----------------------------------------------------------------------
   /*
            �û�����������Ϣ�����ʽ
     */
     
     U3_Rx[U3_rxCounter++]=rx_data;
  
     if(U3_Rx[0]!='$')
	 { 	
	     U3_rxCounter=0;
	     return;
     }
	else
	{
		//�յ�����Ϣ�Ŵ���
		BD1_struct.Rx_enable=RT_EOK;
	}
   //-----------------------------------------------------------------------
 
	 
}

void CAN2_RxHandler_01(unsigned char rx_data)
{
#if 1	
	rt_ringbuffer_putchar(&bd1_pool,rx_data);
#endif

}


void CAN2_putc(char c)
{
#if 0
	USART_SendData(USART3,  c); 
	while (!(USART3->SR & USART_FLAG_TXE));  
	USART3->DR = (c & 0x1FF);   
#endif	
       
	   	USART_SendData( USART3, c ); 
		while( USART_GetFlagStatus( USART3, USART_FLAG_TXE ) == RESET )  
		{
		
		}
}
static  void   U3_Ctrl_IO(void)
{
#if 0
    GPIO_InitTypeDef        gpio_init;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	 

    gpio_init.GPIO_Mode = GPIO_Mode_AF;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz; 
    gpio_init.GPIO_OType = GPIO_OType_PP;  
    gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL; 	 

    // 		Out
	//------------------- PE7 -----------------------------
	gpio_init.GPIO_Pin	 = GPIO_Pin_7;				//------���ſ���״̬  0 ��Ч  ��̬��Ϊ��   
	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;   //���ֻ��ɲ�����Ǿ���PE5��ɲ������ 
	GPIO_Init(GPIOE, &gpio_init); 

	//U3_OUT_PWR_OFF;

#endif


}

void u3_power(u8 i)
{
  #if 0
    if(i)
	{   
	     U3_OUT_PWR_ON;
		 rt_kprintf("\r\n---------U3_power ON\r\n");
    }
	else
	{
		U3_OUT_PWR_OFF;
        rt_kprintf("\r\n---------U3_power OFF\r\n"); 
	}
	#endif
}
//FINSH_FUNCTION_EXPORT(u3_power, u3_power[1|0]);    


void u3_send_len(u8 *instr,u16 infolen)
{
    u16  len=infolen;
	u8 *p=instr;	
	//rt_kprintf("\nlen=%d p=%p",len,p);
	 while (len)
	{
		CAN2_putc (*p++);   
		len--; 
	}
    
}

void u3_send(u8 *input)
{
   u16 len=0;

    len=strlen((const char*)input);  
	rt_kprintf("len=%d\r\n",len);
	while (len)
	{
		CAN2_putc (*input);
		input++;
		len--; 
	}
    rt_kprintf("\r\n u3_output:%s %d\r\n",input,len);
}
FINSH_FUNCTION_EXPORT(u3_send, str);     


static rt_err_t   Device_CAN2_init( rt_device_t dev )
{
      GPIO_InitTypeDef  GPIO_InitStructure;
      USART_InitTypeDef USART_InitStructure;    
     NVIC_InitTypeDef NVIC_InitStructure;	  


       //  1 . Clock	  
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Enable USART3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

      //   2.  GPIO    
       	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	    /* Connect alternate function */
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);  

     //  3.  Interrupt
	/* Enable the USART3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	
    //   4.  uart  Initial
    USART_InitStructure.USART_BaudRate = 115200;    //19200    BD1    Module    ͨѶ����  
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);         




	return RT_EOK;
}

static rt_err_t Device_CAN2_open( rt_device_t dev, rt_uint16_t oflag )  
{
         return RT_EOK;
}
static rt_err_t Device_CAN2_close( rt_device_t dev )
{
        return RT_EOK;
}

static rt_size_t Device_CAN2_read( rt_device_t dev, rt_off_t pos, void* buff, rt_size_t count )
{

        return RT_EOK;
}

static rt_size_t Device_CAN2_write( rt_device_t dev, rt_off_t pos, const void* buff, rt_size_t count )
 {
        unsigned int  Info_len485=0;
	 const char		*p	= (const char*)buff;
	

	Info_len485=(unsigned int)count;
    	/* empty console output */
		//--------  add by  nathanlnw ---------
  while (Info_len485)
	{
		CAN2_putc (*p++);   
		Info_len485--;
	}
       //--------  add by  nathanlnw  --------	
        return RT_EOK;
  }
static rt_err_t Device_CAN2_control( rt_device_t dev, rt_uint8_t cmd, void *arg )
{
     return RT_EOK;
}


void  Device_CAN2_regist(void ) 
{
    Device_CAN2.type	= RT_Device_Class_Char;
	Device_CAN2.init	=   Device_CAN2_init;
	Device_CAN2.open	=  Device_CAN2_open; 
	Device_CAN2.close	=  Device_CAN2_close;
	Device_CAN2.read	=  Device_CAN2_read;
	Device_CAN2.write	=  Device_CAN2_write;
	Device_CAN2.control =Device_CAN2_control;
	

//++++++++++++++++++++++++++++++++++++++++BD1++++++
#if 1
	 BD_Timer=rt_timer_create("BD_Timer",BD_Timer_out,RT_NULL,100,RT_TIMER_FLAG_PERIODIC);
	 if(BD_Timer!=RT_NULL)
	 rt_timer_start(BD_Timer); 
#endif
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
	rt_ringbuffer_init( &bd1_pool, bd_data,BD_DATA_SIZE );

	rt_device_register( &Device_CAN2, "CAN2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE );
	rt_device_init( &Device_CAN2 );
	
}


void user_id(u8* input)
{
	#if 1
	u16 len =0;
	u8 reg_str[10];
	memset(reg_str,0,sizeof( reg_str));
	len = strlen((const char*)input);//��ӳ����ж�
	//---- check -------
	//rt_kprintf("%d-----",len);
	GSM_AsciitoHEX_Convert(input,len,reg_str);
	//reg_str[0]=reg_str[0]&0x1F;
	BD1_struct.UserAddr[0] = reg_str[0]&0x1F;	
	BD1_struct.UserAddr[1] = reg_str[1];	
	BD1_struct.UserAddr[2] = reg_str[2];
	rt_kprintf("%d--%d---%d\r\n",reg_str[0]&0x1F,reg_str[1],reg_str[2]);
	#endif
	

}	
FINSH_FUNCTION_EXPORT(user_id, user);    
	
void Target_id(u8* input)
{
   #if 1
	u16 len =0;
	u8 reg_str[10];
	memset( reg_str,0,sizeof( reg_str));
	len = strlen((const char*)input);//��ӳ����ж�
	//---- check -------
	//rt_kprintf("%d-----",len);
	GSM_AsciitoHEX_Convert(input,len,reg_str);	
	BD1_control.data_user_addr[0] = reg_str[0]&0x1F;	
	BD1_control.data_user_addr[1] = reg_str[1];	
	BD1_control.data_user_addr[2] = reg_str[2];
	OutPrint_HEX("BD1_control.data_user_addr", BD1_control.data_user_addr, 3);
	reg_str[3]=BD1_struct.in_freq>>8;
	reg_str[4]=BD1_struct.in_freq&0xff;
	DF_WriteFlashSector(DF_BD_first_target ,0,reg_str,5);
	#endif

}	
FINSH_FUNCTION_EXPORT(Target_id, Target); 

void Freq_in(u8 *input)
{
	#if 1
	u16 value=0;
	u8 reg_str[10];
	memset(reg_str,0,sizeof(reg_str));
 //len = strlen((const char*)input);
	//---- check -------
	value = atoi((const char *)input);
	BD1_control.data_freq = value;
	if(BD1_control.data_freq>=BD1_struct.Service_freq)
	{
		BD1_struct.in_freq = BD1_control.data_freq;
	}
	/*
	else
	{
		reg_str[3] = BD1_struct.Service_freq>>8;
		reg_str[4] =BD1_struct.Service_freq&0xff;
		BD1_struct.in_freq = BD1_struct.Service_freq;
		
	}
	*/
	reg_str[0] = BD1_control.data_user_addr[0];	
	reg_str[1] = BD1_control.data_user_addr[1];	
	reg_str[2] = BD1_control.data_user_addr[2];
	reg_str[3] = BD1_control.data_freq>>8;
	reg_str[4] =BD1_control.data_freq&0xff;
	DF_WriteFlashSector(DF_BD_first_target ,0,reg_str,5);
	#endif
}
FINSH_FUNCTION_EXPORT(Freq_in, Freq); 

void test(u8 input)
{
	u16 i=0,m=0;
	u8 data[200] ={0};
	if(input == 2)
	{
		for(i=0;i<16*512;i=i+256)
		{
			DF_ReadFlash(DF_BD_data_rx,i,data,50);
			for( m=0;m<50;m++)
			 {
			 	rt_kprintf("%02X",data[m]);
			 }
			 rt_kprintf("\n");
			memset(data,0,50);
		}
	}
}
FINSH_FUNCTION_EXPORT(test, da yin); 




