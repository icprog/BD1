#include "Menu_Include.h"
#include <stdio.h>
#include <string.h>
#include "Device_CAN2.h"
	   
 unsigned char Addr_fre=0;  // 控制输入什么地址 
static unsigned char AddreorFre_screen=0;
static unsigned char AddreorFre=0;//
extern u8 Addr_Code[6];

static void confirm_fre(unsigned char par)
{
lcd_fill(0);
lcd_text12(0, 5,"通信地址  入站频度",18,LCD_MODE_SET);
lcd_text12(25,19,"信息查看",8,LCD_MODE_SET);
if(par==1)
	lcd_text12( 0,  5,"通信地址",8,LCD_MODE_INVERT);
else if(par==2)
	lcd_text12(60,  5,"入站频度",8,LCD_MODE_INVERT);
else if(par==3)
	lcd_text12( 25, 19,"信息查看",8,LCD_MODE_INVERT);
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
	sprintf((char*)str, "%7d", temp_addr);//通信地址
	sprintf(((char*)str)+15, "%7d", temp_usr);//用户地址

	lcd_fill(0);
	lcd_text12(0, 5,"通信地址:",9,LCD_MODE_SET);
	lcd_text12(60, 5,((char*)str),7,LCD_MODE_SET);
	lcd_text12(0, 19,"用户地址:",9,LCD_MODE_SET);
	lcd_text12(60, 19,((char*)str)+15,7,LCD_MODE_SET);
	lcd_update_all();
	AddreorFre_screen =input;	
}
static void msg( void *p)
{
}
static void show(void)//初始化的状态
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
	   	   if(AddreorFre==1)//通信地址
			   {
			  AddreorFre_screen=0;
			  AddreorFre=0;
			   memset(Addr_Code,0,sizeof(Addr_Code));
			    Addr_fre=1;
				rt_kprintf("AddreorFre = %d\r\n",AddreorFre);
				
				pMenuItem=&Menu_9_0_1adress;//修改为进入设置地址界面
				pMenuItem->show();
			  //进入下一界面
			   }
		   /*
			  
		  else if(AddreorFre==2)//通信地址
			   {
			  AddreorFre_screen=0;
			  AddreorFre=0;
			    memset(Addr_Code,0,sizeof(Addr_Code));
			    Addr_fre=2;
				pMenuItem=&Menu_9_0_1adress;//修改为进入设置地址界面
				pMenuItem->show();
			  
			   
			   }
		        */
		 else if(AddreorFre==2)//入站频度
			   {
				 AddreorFre_screen=0;
				 AddreorFre=0;
			    memset(Addr_Code,0,sizeof(Addr_Code));
				Addr_fre=2;
				pMenuItem=&Menu_9_0_1adress;//修改为进入设置地址界面
				pMenuItem->show();
			   }
		 else if(AddreorFre==3)
		 	{
		 	/*
		 	    memset(Reg_buf,0,sizeof(Reg_buf));
		 		DF_ReadFlash(DF_BD_first_target,0,Reg_buf,3);
				temp_addr = (Reg_buf[0]<<24)+(Reg_buf[1]<<16)+(Reg_buf[2]);
				temp_usr = (BD1_struct.UserAddr[0]<<24)+(BD1_struct.UserAddr[1]<<16)+BD1_struct.UserAddr[2];
				sprintf(Reg_buf, "%7d", temp_addr);//通信地址
				sprintf(Reg_buf+15, "%7d", temp_usr);//用户地址
				
				lcd_fill(0);
				lcd_text12(0, 5,"通信地址:",8,LCD_MODE_SET);
				lcd_text12(60, 5,Reg_buf,7,LCD_MODE_SET);
				lcd_text12(0, 19,"用户地址:",8,LCD_MODE_SET);
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
				sprintf((char*)Reg_buf, "%7d", BD1_struct.in_freq);//入站频度
				lcd_fill(0);
				lcd_text12(0, 5,"入站频度:",8,LCD_MODE_SET);
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
	  AddreorFre=0;//	1:鉴权	 2:注册
	   }
}

ALIGN(RT_ALIGN_SIZE)
MENUITEM    Menu_9_0_user=
{
   "配置与查看",
   10,
   &show,
   &keypress,
   &timetick,
   &msg,
   (void*)0
};

