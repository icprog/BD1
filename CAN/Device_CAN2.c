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

#define  Beam_power_numb 1
#define  Version_Info    10   

u8   U3_Rx[300];

u8   U3_content[300];
u16   U3_content_len=0; 
u8   U3_flag=0;
u16   U3_rxCounter=0;
u8    Version[Version_Info];
BD_ONE	BD1_struct; //  ����1  ���

u8	U3_Tx[260];
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
rt_timer_t BD_Timer; 
extern int number_txt;
extern char *trans[];
extern char *number[];

u8 dwlcd =0;//0ΪС������ 1Ϊ��������
//������ʾ���
 u8 index_addre[12]={0};
 u8 index_len[12]={0};
#define  Max_value     0xffffffff 

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
	u8 counter =0;
	u8 mark =0;//��ǵ�ַ
	u8 flag=0;//��ǳ���
	u8 i=0;
	u8 number=0;
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
		    /*
			if((str[i]<=9)&&(str[i]>=0))
			{
				str[i]=str[i]+'0';
			}
			*/
			if(str[i]<0x7e)
			{
				number=number+1;
				rt_kprintf("ascii---%x--%d\r\n",str[i],i);
			}
			counter=counter+1;
			if(counter%20==0)
			{
				
				if(number%2==0)
				{
					index_addre[mark++] = i;
					index_len[flag++] = counter;
					//rt_kprintf("index[mark]---%d--%d\r\n",index_addre[mark-1],index_len[flag-1]);

				}
				else
				{
					index_addre[mark++] =i-1;
					index_len[flag++] = counter-1;
					//rt_kprintf("hunfa:index[mark]---%d--%d\r\n",index_addre[mark-1],index_len[flag-1]);
				}
				counter =0;
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
			else
			{
				tran_id = 0;
			}
		}
		
		//tran_id =tran_id +1;
	}
	rt_kprintf("the max ID --%d\r\n",tran_id);
	return tran_id ;
	#endif
}



/******************************************************************************
 * Function: Sort_ID () 
 * DESCRIPTION: - ����������ǰ�洢����������ID�͵�ַ 
 * Input: addre�洢��ʼ��ַ offset����ƫ����
 * Input: rank:��ѯID����������
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
	if(max_addre==0)
	{
		return RT_ERROR;	
	}
	for(i=0;i<16*512;i=i+256)
	{
		DF_ReadFlash(addre,i,temp,4);
		tmp_addre= (temp[0]<<24)+(temp[1]<<16)+(temp[2]<<8)+temp[3];
		if(tmp_addre==(max_addre-rank))
		{
			offset =i;
			//rt_kprintf("the offset is ---%d\r\n",offset);
			return offset;
		}
	}
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
	//Dis_date[22]={"2000/00/00  00:00:00"};
	char T_date[22]={"00/00/00 00:00:00"};
	u8 i=0;
	memset(show_buf,0,20);
	lcd_fill(0);
	lcd_text12(0,5,number[Screen],strlen(number[Screen]),mode);
	for(i=0;i<3;i++)
		T_date[i*3]=str2[i+5]/10+'0';
	for(i=0;i<3;i++)
		T_date[1+i*3]=str2[i+5]%10+'0';

	for(i=0;i<3;i++)
		T_date[9+i*3]=str2[i+5+3]/10+'0';
	for(i=0;i<3;i++)
		T_date[10+i*3]=str2[i+5+3]%10+'0';
	lcd_text12(strlen(number[Screen])*6,5,T_date,17,mode);
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
    if(Addre_offset==RT_ERROR)
    {
		lcd_fill(0);
		lcd_text12(0,10,"�����ݿ���ʾ...",15,LCD_MODE_SET);
		lcd_update_all();
		return RT_ERROR;
    }
	
	rt_kprintf("Addre_offset--%d\r\n",Addre_offset);
	DF_ReadFlash(DF_BD_data_rx,Addre_offset,temp,5);
	DF_ReadFlash(DF_BD_data_rx,Addre_offset,data,temp[4]);
	
	data_len = ((data[17]<<8) +data[18])/8;//���յĴ洢���ȳ���8
	//rt_kprintf("du chu shu ju len-- %d\r\n",temp[4]);
	//OutPrint_HEX("Read", data,temp[4]);
	//length =data_len;
	//data_len=data_len/8;
	if(data[11]==0x60)//����ǻ췢�Ļ�
	{
		memcpy(str,data+20,data_len);
		index_txt(str,data_len-1);
		OutPrint_HEX("Read DATA ", str,data_len);
	}
	else
	{
		memcpy(str,data+19,data_len);
		index_txt(str,data_len);
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
u8 Rx_data_save(u8 *str)
{
	u8 temp[4];
	u8 trans[256];
	u8 len =0;
	u32 i= 0;
	u32 test_head=0;
	memset(trans,0,256);
	memset(temp,0,4);
	
	for(i=0;i<16*512;i=i+256)
	{
	    if(i>=8*512)
	    {
	    	//�°벿����ʾʱ����ʾ�ϰ벿��
	    	SST25V_SectorErase_4KByte((8*(((u32)DF_BD_data_rx+8)/8))*PageSIZE);
			//Erase_flag =Top_15;
	    }
		if(i>=16*512)//��֤��ʱ���п���д�Ŀռ�
		{
		    i=0;//���±���
			//�����ϰ벿����ʾ�°벿��
			SST25V_SectorErase_4KByte((8*((u32)DF_BD_data_rx/8))*PageSIZE);	
			//Erase_flag = After_15;
			rt_kprintf("erase the 8016-8023\r\n");
		}
		DF_ReadFlash(DF_BD_data_rx,i,temp,4);
		
		test_head = (temp[0]<<24)+(temp[1]<<16)+(temp[2]<<8)+temp[3];
		//rt_kprintf(" %d--%d--%d--%d--%d--%x\r\n",temp[0],temp[1],temp[2],temp[3],i,test_head);
		if(test_head==Max_value)
		{
	
			test_head = find_max_id(DF_BD_data_rx)+1;//���ID�Լ�1
		    //�ҳ���һ��IDֵ
	    	trans[0]=test_head>>24;
			trans[1]=(test_head>>16)&0xff;
			trans[2]=(test_head>>8)&0xff;
			trans[3]=test_head&0xff;
			len =(((str[16]<<8)+str[17])/8);
			//rt_kprintf("�յ����ĳ���---%d",len);
			len =len+8;//����Э�����7���ֽ�(��Ϣ��𵽵��ĳ���)
		    trans[4] =len+4+6+1;//+4�Ǽ��ϼ���+1����+��ʱ�䳤��=�ܳ�

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
			memcpy(trans+11,str+10,len);//����Ϣ���ݿ���
			
			DF_WriteFlashDirect(DF_BD_data_rx,i,trans,trans[4]);
			//OutPrint_HEX("saved", trans, trans[4]);//wxg_test
			memset(temp,0,4);//����
			return RT_EOK;
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
  
	max_n =(length/40);
	if(max_n<=1)
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



		if(Warn_Status[1]&0x10)
		{

		U3_Tx[U3_Tx_Wr++]=0x12; 

		U3_Tx[U3_Tx_Wr++]=6;

		U3_Tx[U3_Tx_Wr++]=InOut_Object.TYPE;
		U3_Tx[U3_Tx_Wr++]=(InOut_Object.ID>>24);
		U3_Tx[U3_Tx_Wr++]=(InOut_Object.ID>>16);
		U3_Tx[U3_Tx_Wr++]=(InOut_Object.ID>>8);
		U3_Tx[U3_Tx_Wr++]=InOut_Object.ID;
		U3_Tx[U3_Tx_Wr++]=InOut_Object.InOutState;  
		rt_kprintf("\r\n ----- 0x0200 current ������Ϣ \r\n");    
		}


		if(Warn_Status[3]&0x02)
		{      

		U3_Tx[U3_Tx_Wr++]=0x11;

		U3_Tx[U3_Tx_Wr++]=1; 

		U3_Tx[U3_Tx_Wr++]=0; 

		}

		rt_kprintf("\r\n ----- 0x0200 current ������Ϣ \r\n"); 

		U3_Tx[U3_Tx_Wr++]=0x25; 

		U3_Tx[U3_Tx_Wr++]=4; 

		// U3_Tx[U3_Tx_Wr++]= BD_EXT.FJ_SignalValue; 

		U3_Tx[U3_Tx_Wr++]=0x00;
		U3_Tx[U3_Tx_Wr++]=0x00;
		U3_Tx[U3_Tx_Wr++]=0x00;
		U3_Tx[U3_Tx_Wr++]=BD_EXT.Extent_IO_status; 


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
	
	if(BD_ZJ.Time_consum>10)//�����������
	{
		BD_ZJ.result=result_failed;
		BD_ZJ.status=result_failed;
		BD_ZJ.Time_consum =0;
		//TTS_play("�����û����޷�����");
	}
 }
 if(((GSM_PWR.result == result_success)&&(BD_ZJ.result==result_success)&&(BD_IC.result==result_pending))||(BD_IC.status==result_Artificial))
 {
 	
	BD_IC.Time_consum++;
	if((BD_IC.Time_consum%2)==0)//ÿ���뷢��һ��
	BD1_Tx(BD1_TYPE_ICJC, 0, 1);
	if(BD_IC.Time_consum>10)
	{
	    BD_IC.status=result_failed;
		BD_IC.result = result_failed;
		BD_IC.Time_consum =0;
		TTS_play("�����û������ʧ��");
	}
	
	
 }
 #endif
	
}
/******************************************************************************
 * Function:  () 
 * DESCRIPTION: - ��ʱ��������һ����վƵ�ȼ���Ĵ��� 
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
#if 1
    Check_bd_data();
	//rt_kprintf("test----%d--%d\r\n",BD1_struct.in_freq,BD_FRE.Frequency);
    if((BD_IC.result==result_success)||(BD1_struct.in_freq!=0))
    {
		BD_FRE.Frequency++;
		//rt_kprintf("BD_FK.result=%d\r\n",BD_FK.result);
		if(BD_FK.result==result_pending)
		{
			if(BD_FRE.Frequency==(BD1_struct.in_freq-3))
			{
				TTS_play("�����ظ���ʱ��������");
				BD_FK.result=result_failed;
				//����ʧ����
				dwlcd =0;//С������
				BD_TX.status = bd1_send_auto;
			}
			
		}
		if(BD_FRE.Frequency ==(BD1_struct.in_freq+1))
		{
			
			
		   if(ModuleStatus&Status_GPS )//�����λ�ͷ���0200��Ϣ
		   	{
				
				//����ʲô��ʽ�Ϸ���λ��Ϣ
				memset(U3_Tx,0,sizeof(U3_Tx));
				if(BD_TX.status == bd1_send_auto)//�Զ�����
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
				else
				{
					BD1_Tx(BD1_TYPE_TXSQ,Big_lcd.TXT_content,strlen(Big_lcd.TXT_content));//lcd��������
					Lcd_write(Big_lcd.status, LCD_PAGE, 1);
					BD_FRE.Frequency =0;//���¼���
				}
					
					BD_TX.Time_consum =1;//����С��Ļ�л�ʱ��ü�ʱ
					BD_TX.flag_send=RT_EBUSY;//�ڷ��ͼ���д���æ״̬
					BD_TX.Frequency=0;//��������ص���������
					lcd_fill(0);
					lcd_text12(10,10,"ͨ�������Ѿ�����",16,LCD_MODE_SET);
					lcd_update_all();
				
			}
			BD_FRE.Frequency =0;
			BD_TX.flag_send=RT_EOK;
		}
    }
	//rt_kprintf("ʱ������---%d\r\n",BD_TX.Time_consum);
	if(BD_TX.Time_consum ==1)
	{
		BD_TX.Frequency++;
		if(BD_TX.Frequency==5)
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
	BD_IC.Frequency=0;
	BD_FRE.Frequency =0;
	BD_IC.result = result_pending;
	BD_ZJ.result=result_pending;
	BD_FRE.status =0;
	BD_TX.status =bd1_send_auto;
	BD_TX.flag_send=RT_EOK;//RT_ERROR
	BD1_struct.Rx_enable =RT_EBUSY;
	BD_FK.Time_consum=0;
	BD_ZJ.flag_send=0;
	//mainshow=show_first;
	memset(BD1_control.data_user_addr,0,3);
	memset(BD1_struct.UserAddr,0,3);
	
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
		rt_kprintf("ͨ�ŵ�ַ%d--%d--%d\r\n",data[0],data[1],data[2]);
	}
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
	memset(data,0,sizeof(data));
	
}

void u3_send_len(u8 *instr,u16 infolen);


void  BD1_Tx(u8 Type,void *str,u8 stuff_len)
{
	u8  bd_send[300]; 
	u16  d_len,len=0;
	u8  fcs=0;
	u8  stuff[260];
	u16  BD_temp =0;
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
									stuff[4] = 0;//��ʽʱ����Ϊ0.1��
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
							stuff[0] = 0x46;
							//stuff[0] = BD1_control.message_mode;
							//ͨ�ŵ�ַ
							stuff[1] = BD1_control.data_user_addr[0];
							stuff[2] = BD1_control.data_user_addr[1];
							stuff[3] = BD1_control.data_user_addr[2];
							//ͨ������
							stuff_len=stuff_len+1;//���ȼ��ϻ췢��ͷ
							BD_temp =stuff_len*8;
							stuff[4] = BD_temp >>8;
							stuff[5] = BD_temp &0xff;
							//�Ƿ�Ӧ��
							stuff[6] = BD1_control.data_ask;
							stuff[7] = 0xA4;
							memcpy(stuff+8, str, stuff_len-1);//�������ĵ��Ĳ�Ӧ�ü���A4
							//rt_kprintf("���ĳ���%d\r\n",stuff_len-1);
							stuff_len=(stuff_len-1)+8;
							
						
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
							rt_kprintf("wxg----xtzj\r\n");
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
		bd_send[5]=len>>8;
		bd_send[6] = len & 0xff;//���ݳ���
		bd_send[6+1]=BD1_struct.UserAddr[0];//�û���ַ��ô����?
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
		
		bd_send[d_len+1] =0x0D;
		bd_send[d_len+2] =0x0A;
		//rt_kprintf("У��͵�ֵ%d---%d\r\n",bd_send[d_len],d_len);


	// send
	
	u3_send_len(bd_send,len+2);
	OutPrint_HEX("write_BD1:", bd_send, len+2);
	//memset(str,0,sizeof(str));
	 //һ�����;�ֵλ;

}


void BD1_RxProcess(void)  
{
   //u8  iRX;
  // u8  Rx_userAdd[8];
   u8  Broadcast[260];
   u8 i=0;
     //  Debug
     OutPrint_HEX("U3_rx_hex",U3_content,U3_content_len);
     //rt_kprintf("\r\n %s \r\n",U3_content);
	   memset(Broadcast,0,sizeof(Broadcast));
     //  Check UserID  filter
     //if((U3_content[6]==BD1_struct.UserAddr[0])&&(U3_content[7]==BD1_struct.UserAddr[1])&&(U3_content[8]==BD1_struct.UserAddr[2]))
     //------------------------------------------------------------
     if(strncmp(U3_content,"$GLJC",5)==0)    // ���ʼ��
     	{
     		
         BD1_struct.Output_freq = U3_content[10];
		 /*
		 sprintf(Broadcast,"%d",BD1_struct.Output_freq);
		 TTS_play("���ʼ���ֵΪ");
		 TTS_play(Broadcast);
		 memset(Broadcast,0,sizeof(Broadcast));
		 */
     	}
     
     if(strncmp(U3_content,"$DWXX",5)==0)  // ��λ��Ϣ
     	{
     	   // check info_type    U3_content[9]
     	   // position    U3_content[13]
     	   BD1_struct.hour= U3_content[13+1];
		   BD1_struct.minute= U3_content[14+1];
		   BD1_struct.second= U3_content[15+1];
		   BD1_struct.little_second= U3_content[16+1];

		   BD1_struct.longi_Du= U3_content[17+1];
		   BD1_struct.longi_Fen= U3_content[18+1];
		   BD1_struct.longi_Miao= U3_content[19+1];
		   BD1_struct.longi_Little_Miao= U3_content[20+1];
		   

		   BD1_struct.lati_Du= U3_content[21+1];
		   BD1_struct.lati_Fen= U3_content[22+1];
		   BD1_struct.lati_Miao= U3_content[23+1];
		   BD1_struct.lati_Little_Miao= U3_content[24+1]; 
			rt_sprintf(Broadcast,"��λ��Ϣ:����%d��%d��%d��%d��,γ��:%d��%d��%d��%d��",BD1_struct.longi_Du,BD1_struct.longi_Fen,BD1_struct.longi_Miao,\
			BD1_struct.longi_Little_Miao,BD1_struct.lati_Du,BD1_struct.lati_Fen,BD1_struct.lati_Miao,BD1_struct.lati_Little_Miao);
		   TTS_play(Broadcast);
		   BD1_struct.Ground_height= (U3_content[25+1]<<8)+U3_content[26+1];
           
		   BD1_struct.User_height= (U3_content[27+1]<<8)+U3_content[28+1]; 

		   // debug
		   rt_kprintf("\r\n ����1 λ����Ϣ: %d:%d:%d.%d   Longi  %d��%d��%d.%d��  lati  %d��%d��%d.%d��  ��ظ߳� %d  �û��߳� %d \r\n",\
		   BD1_struct.hour,BD1_struct.minute,BD1_struct.second,BD1_struct.little_second,\
		   BD1_struct.longi_Du,BD1_struct.longi_Fen,BD1_struct.longi_Miao,BD1_struct.longi_Little_Miao,\
		   BD1_struct.lati_Du,BD1_struct.lati_Fen,BD1_struct.lati_Miao,BD1_struct.lati_Little_Miao,\
		   BD1_struct.Ground_height&0x3f,BD1_struct.User_height&0x3f );
		   


     	}
	 if(strncmp(U3_content,"$TXXX",5)==0)  // ͨ����Ϣ 
		 {

		     BD1_struct.Text_len=(U3_content[16]<<8)+U3_content[17];
			 BD1_struct.Text_len =BD1_struct.Text_len/8;
			 memset(BD1_struct.Text_info,0,sizeof(BD1_struct.Text_info));
			 if(U3_content[10]==0x60)//����ǻ췢
             {
             	memcpy(BD1_struct.Text_info,U3_content+19,BD1_struct.Text_len);
			 	TTS_play(BD1_struct.Text_info);
             }
			 else
			 {
			 	memcpy(BD1_struct.Text_info,U3_content+18,BD1_struct.Text_len);
				TTS_play(BD1_struct.Text_info);
			 }
			 Rx_data_save(U3_content);
			 //+++++++++++++++������0-9ת��Ϊ�ַ�������++++++++
			 /*
			 for(i=0;i<BD1_struct.Text_len;i++)
			 {
			 	if((BD1_struct.Text_info[i]<=9)&&(BD1_struct.Text_info[i]>=0))
		 		{
					BD1_struct.Text_info[i]=BD1_struct.Text_info[i]+'0';
		 		}
			 }
			 */
			 //TTS_play(BD1_struct.Text_info);
	         rt_kprintf("\r\n   ����1 �����ı���Ϣ:%s\r\n", BD1_struct.Text_info); 
		 }
	  
	  if(strncmp(U3_content,"$ICXX",5)==0) // IC  ��Ϣ 
		 {
		 	memcpy(BD1_struct.UserAddr,U3_content+7,3);
			BD1_struct.Frame_num = U3_content[10];
		    memset(BD1_struct.Broadcast_ID,0,sizeof(BD1_struct.Broadcast_ID));
			memcpy(BD1_struct.Broadcast_ID,U3_content+11,3);
			BD1_struct.User_status = U3_content[14];
			BD1_struct.Service_freq = (U3_content[15]<<8)+U3_content[16];
			if(BD1_struct.Service_freq>=BD1_struct.in_freq)
			{
				BD1_struct.in_freq= BD1_struct.Service_freq;
				
			}

			BD1_struct.Communicate_grade = 	U3_content[17];
			BD1_struct.Encode_state = U3_content[18];
			BD1_struct.Sub_user = (U3_content[19]<<8)+U3_content[20];
			BD_IC.result=result_success;
			BD_IC.status=result_success;
			TTS_play("�����û�������");
			
		 }
	  if(strncmp(U3_content,"$ZJXX",5)==0)	// �Լ���Ϣ
		{
			BD1_struct.IC_state =  U3_content[10];
			BD1_struct.Hrdware_sate = U3_content[11];
			BD1_struct.Battery_stae = U3_content[12];
			BD1_struct.Inbound_status = U3_content[13];
			#ifdef  Beam_power_numb//���幦��״��
			memset(BD1_struct.Beam_power,0,sizeof(BD1_struct.Beam_power));
			memcpy(BD1_struct.Beam_power,U3_content+14,6);
			#endif
			if((0==BD1_struct.IC_state)&&(0==BD1_struct.Hrdware_sate))
			{
				BD_ZJ.result = result_success;
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
	   
	   if(strncmp(U3_content,"$SJXX",5)==0) // ʱ����Ϣ
		  {
			BD1_struct.year = (U3_content[10]<<8)+U3_content[11];
			BD1_struct.Month = U3_content[12];
			BD1_struct.Day = U3_content[13];
			BD1_struct.hour = U3_content[14];
			BD1_struct.minute = U3_content[15];
			BD1_struct.second = U3_content[16];
			
		  }
	   if(strncmp(U3_content,"$BBXX",5)==0) // �汾��Ϣ 
		   {
	   #ifdef  Version_Info
	        memset(Version,0,Version_Info);
	        memcpy(Version,(char*)U3_content[10],Version_Info);
	   #endif	
			 
		   }
		
		if(strncmp(U3_content,"$FKXX",5)==0) // ������Ϣ
		   {
             BD1_struct.Feedback_flag = U3_content[10];
			 if(BD1_struct.Feedback_flag==0x00)
			 {
			 	BD_FK.result =result_success; 
				memset(Big_lcd.TXT_content,0,sizeof(Big_lcd.TXT_content));//�����������
				Lcd_write(Big_lcd.status,LCD_Text_clear,0);//����������
				delay_ms(10);
				Lcd_write(Big_lcd.status,LCD_PAGE,1);
				//���ͳɹ���
				dwlcd =0;//С������
				BD_TX.status = bd1_send_auto;
				
			 }
			 else
			 {
			 	BD_FK.result = result_failed;
				rt_kprintf("Reasons for failure--%d\r\n",BD1_struct.Feedback_flag);
				 memset(BD1_struct.Add_Info,0,4);
		 		memcpy(BD1_struct.Add_Info,U3_content+11,4);
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
     //-------------------------------------------------------------
     memset(U3_content,0,sizeof(U3_content));
	 //rt_kprintf("test----1\r\n");
	 BD1_struct.Rx_enable=RT_EBUSY;
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
	
	 if(U3_rxCounter==7)
	 {
	   
	   U3_content_len=(U3_Rx[U3_rxCounter-2]<<8)+U3_Rx[U3_rxCounter-1];	// byte 5   �ǳ����ֽ�����				
	 }
	 
    
	 if(U3_content_len==U3_rxCounter)
	 	{
	 	   
	 	   memcpy(U3_content,U3_Rx,U3_content_len);				  
           BD1_struct.Rx_enable=RT_EOK;
		   //------------------
           U3_rxCounter=0;
	 	}
   //-----------------------------------------------------------------------
 
	 
    
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
		 
	 while (len)
	{
		CAN2_putc (*instr++);   
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
		reg_str[3] = BD1_control.data_freq>>8;
		reg_str[4] =BD1_control.data_freq&0xff;
		BD1_struct.in_freq = BD1_control.data_freq;
	}
	else
	{
		reg_str[3] = BD1_struct.Service_freq>>8;
		reg_str[4] =BD1_struct.Service_freq&0xff;
		BD1_struct.in_freq = BD1_struct.Service_freq;
		
	}
	
	reg_str[0] = BD1_control.data_user_addr[0];	
	reg_str[1] = BD1_control.data_user_addr[1];	
	reg_str[2] = BD1_control.data_user_addr[2];
	DF_WriteFlashSector(DF_BD_first_target ,0,reg_str,5);
	#endif
}
FINSH_FUNCTION_EXPORT(Freq_in, Freq); 






