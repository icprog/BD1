#include "Menu_Include.h"
#include <stdio.h>
#include <string.h>
#include "Device_CAN2.h"
	   
 unsigned char Addr_fre=0;  // ��������ʲô��ַ 
static unsigned char AddreorFre_screen=0;
static unsigned char AddreorFre=0;//
extern u8 Addr_Code[6];

static void confirm_fre(unsigned char par)
{
lcd_fill(0);
lcd_text12(0, 5,"ͨ�ŵ�ַ  ��վƵ��",18,LCD_MODE_SET);
lcd_text12(25,19,"��Ϣ�鿴",8,LCD_MODE_SET);
if(par==1)
	lcd_text12( 0,  5,"ͨ�ŵ�ַ",8,LCD_MODE_INVERT);
else if(par==2)
	lcd_text12(60,  5,"��վƵ��",8,LCD_MODE_INVERT);
else if(par==3)
	lcd_text12( 25, 19,"��Ϣ�鿴",8,LCD_MODE_INVERT);
//else if(par==4)
//	lcd_text12(84, 18,"??",4,LCD_MODE_INVERT);
lcd_update_all();
}
static Look_data(u8 *str,u8 input)
{
	u32 temp_usr=0;
	u32 temp_addr=0;
	memset(str,0,sizeof(str));
	DF_ReadFlash(DF_BD_first_target,0,str,3);
	temp_addr = (BD1_control.data_user_addr[0]<<16)+(BD1_control.data_user_addr[1]<<8)+(BD1_control.data_user_addr[2]);
	temp_usr = (BD1_struct.UserAddr[0]<<16)+(BD1_struct.UserAddr[1]<<8)+BD1_struct.UserAddr[2];
	rt_kprintf("temp_addr:%d---temp_usr:%d\r\n",temp_addr,temp_usr);
	sprintf((char*)str, "%7d", temp_addr);//ͨ�ŵ�ַ
	sprintf(((char*)str)+15, "%7d", temp_usr);//�û���ַ

	lcd_fill(0);
	lcd_text12(0, 5,"ͨ�ŵ�ַ:",9,LCD_MODE_SET);
	lcd_text12(60, 5,((char*)str),7,LCD_MODE_SET);
	lcd_text12(0, 19,"�û���ַ:",9,LCD_MODE_SET);
	lcd_text12(60, 19,((char*)str)+15,7,LCD_MODE_SET);
	lcd_update_all();
	AddreorFre_screen =input;	
}
static void msg( void *p)
{
}
static void show(void)//��ʼ����״̬
   {
   confirm_fre(1);
  AddreorFre_screen=1;
  AddreorFre=1;
   }

static void keypress(unsigned int key)
{
    u8  Reg_buf[40];

   switch(KeyValue)
	   {
	   case KeyValueMenu:
		   pMenuItem=&Menu_9_Urgent;
		   pMenuItem->show();
		   CounterBack=0;

		   Addr_fre=0;
		  AddreorFre_screen=0;
		  AddreorFre=0;//
		   break;
	   case KeyValueOk:
	   	   if(AddreorFre==1)//ͨ�ŵ�ַ
			   {
			  AddreorFre_screen=0;
			  AddreorFre=0;
			   memset(Addr_Code,0,sizeof(Addr_Code));
			    Addr_fre=1;
				rt_kprintf("AddreorFre = %d\r\n",AddreorFre);
				
				pMenuItem=&Menu_9_0_1adress;//�޸�Ϊ�������õ�ַ����
				pMenuItem->show();
			  //������һ����
			   }
		   /*
			  
		  else if(AddreorFre==2)//ͨ�ŵ�ַ
			   {
			  AddreorFre_screen=0;
			  AddreorFre=0;
			    memset(Addr_Code,0,sizeof(Addr_Code));
			    Addr_fre=2;
				pMenuItem=&Menu_9_0_1adress;//�޸�Ϊ�������õ�ַ����
				pMenuItem->show();
			  
			   
			   }
		        */
		 else if(AddreorFre==2)//��վƵ��
			   {
				 AddreorFre_screen=0;
				 AddreorFre=0;
			    memset(Addr_Code,0,sizeof(Addr_Code));
				Addr_fre=2;
				pMenuItem=&Menu_9_0_1adress;//�޸�Ϊ�������õ�ַ����
				pMenuItem->show();
			   }
		 else if(AddreorFre==3)
		 	{
		 	/*
		 	    memset(Reg_buf,0,sizeof(Reg_buf));
		 		DF_ReadFlash(DF_BD_first_target,0,Reg_buf,3);
				temp_addr = (Reg_buf[0]<<24)+(Reg_buf[1]<<16)+(Reg_buf[2]);
				temp_usr = (BD1_struct.UserAddr[0]<<24)+(BD1_struct.UserAddr[1]<<16)+BD1_struct.UserAddr[2];
				sprintf(Reg_buf, "%7d", temp_addr);//ͨ�ŵ�ַ
				sprintf(Reg_buf+15, "%7d", temp_usr);//�û���ַ
				
				lcd_fill(0);
				lcd_text12(0, 5,"ͨ�ŵ�ַ:",8,LCD_MODE_SET);
				lcd_text12(60, 5,Reg_buf,7,LCD_MODE_SET);
				lcd_text12(0, 19,"�û���ַ:",8,LCD_MODE_SET);
				lcd_text12(60, 19,Reg_buf+15,8,LCD_MODE_SET);
				lcd_update_all();
				AddreorFre_screen =2;
				*/
				 memset(Reg_buf,0,sizeof(Reg_buf));
				Look_data(Reg_buf,2);
				

		 	}
		  	
		   break;
	   case KeyValueUP:
	   
		   if(AddreorFre_screen==1)
			   {
			  AddreorFre--;
			   if(AddreorFre<1)
				  AddreorFre=3;
			   confirm_fre(AddreorFre);
			   }
		   if(AddreorFre_screen==2)
		   	{
				memset(Reg_buf,0,sizeof(Reg_buf));
				Look_data(Reg_buf,2);
		   	}
			   
		   break;
		   
	   case KeyValueDown:
	   
		   if(AddreorFre_screen==1)
			   {
			  AddreorFre++;
			   if(AddreorFre>3)
				  AddreorFre=1;
			   rt_kprintf("AddreorFre = %d\r\n",AddreorFre);
			   confirm_fre(AddreorFre);
			   }
			if(AddreorFre_screen==2)
		   	{
			   	memset(Reg_buf,0,40);
				sprintf((char*)Reg_buf, "%7d", BD1_struct.in_freq);//��վƵ��
				lcd_fill(0);
				lcd_text12(0, 5,"��վƵ��:",8,LCD_MODE_SET);
				lcd_text12(60, 5,(char*)Reg_buf,7,LCD_MODE_SET);
				lcd_update_all();
		   	}
		   break;
		   
	   }
KeyValue=0;
}
   

static void timetick(unsigned int systick)
{
   CounterBack++;
   if(CounterBack!=MaxBankIdleTime)
	   return;
   else
	   {
	   pMenuItem=&Menu_1_Idle;
	   pMenuItem->show();
	   CounterBack=0;

	   Addr_fre=0;
	  AddreorFre_screen=0;
	  AddreorFre=0;//	1:��Ȩ	 2:ע��
	   }
}

ALIGN(RT_ALIGN_SIZE)
MENUITEM    Menu_9_0_user=
{
   "������鿴",
   10,
   &show,
   &keypress,
   &timetick,
   &msg,
   (void*)0
};

