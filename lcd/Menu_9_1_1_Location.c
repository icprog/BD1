#include "Menu_Include.h"
#include <stdio.h>
#include <string.h>
#include "Device_CAN2.h"
	   
static unsigned char Menu_Logout=0;  //  
static unsigned char Highmode_screen=0;
static unsigned char Highmode=0;//  1:�и߳�	2:�޲��
extern u8   send_applicate ;
static u8 info_data[260];
static void confirm_high(unsigned char par)
{
lcd_fill(0);
lcd_text12(0, 10,"�и߳�    �޲��",16,LCD_MODE_SET);
if(par==1)
	lcd_text12( 0,  10,"�и߳�",6,LCD_MODE_INVERT);
else if(par==2)
	lcd_text12(60,  10,"�޲��",6,LCD_MODE_INVERT);

lcd_update_all();
}
static void msg( void *p)
{
}
static void show(void)//��ʼ����״̬
   {
   confirm_high(1);
   Highmode_screen=1;
   Highmode=1;
   }

static void keypress(unsigned int key)
{
    // u8  Reg_buf[40];
	 
   switch(KeyValue)
	   {
	   case KeyValueMenu:
		   pMenuItem=&Menu_9_Urgent;
		   pMenuItem->show();
		   CounterBack=0;

		   Menu_Logout=0;
		   Highmode_screen=0;
		   Highmode=0;//
		   break;
	   case KeyValueOk:
	   	   if(Highmode==1)//�и߳�
			   {
			   Highmode_screen=0;
			   Highmode=0;
			   lcd_fill(0);
			   //lcd_text12(6,10,"��ȷ�ϼ�ѡ���߷�ʽΪ",18,LCD_MODE_SET);
			   lcd_text12(0,2,"��ȷ�ϼ�ѡ���߷�",18,LCD_MODE_SET);
				lcd_text12(0,18,"ʽΪ�и߳�",10,LCD_MODE_SET);
			   lcd_update_all();
			   BD1_control.data_altimeter = 0;
			   Menu_Logout=1;//������һ����
			   }
		   else if(Menu_Logout==1)
			   	{
			   	Menu_Logout=0;
				memset(info_data,0,sizeof(info_data));
				lcd_fill(0); 
				lcd_text12(24,10,"��λ���뷢��",12,LCD_MODE_SET);
				BD1_Tx(BD1_TYPE_DWSQ,info_data,11);
				lcd_update_all();
			   	}
		  else if(Highmode==2)//�޲��
			   {
			   Highmode_screen=0;
			   Highmode=0;
			   lcd_fill(0);
			   	lcd_text12(0,2,"��ȷ�ϼ�ѡ���߷�",18,LCD_MODE_SET);
				lcd_text12(0,18,"ʽΪ�޲��",10,LCD_MODE_SET);
			   lcd_update_all();
			   BD1_control.data_altimeter =0x04;
			   Menu_Logout=2;
			   }
		   else if(Menu_Logout==2)//
			   {
			   Menu_Logout=0;

			   lcd_fill(0);
			   lcd_text12(24,10,"��λ���뷢��",12,LCD_MODE_SET);
			   BD1_Tx(BD1_TYPE_DWSQ,info_data,11);
			   lcd_update_all();
			   
			   }

		   break;
	   case KeyValueUP:
		   if(Highmode_screen==1)
			   {
			   Highmode--;
			   if(Highmode<1)
				   Highmode=2;
			   confirm_high(Highmode);
			   }
		   break;
		   
	   case KeyValueDown:
		   if(Highmode_screen==1)
			   {
			   Highmode++;
			   if(Highmode>2)
				   Highmode=1;
			   confirm_high(Highmode);
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

	   Menu_Logout=0;
	   Highmode_screen=0;
	   Highmode=0;//	
	   }
}

ALIGN(RT_ALIGN_SIZE)
MENUITEM    Menu_9_1_1_Location=
{
   "��λ����",
   8,
   &show,
   &keypress,
   &timetick,
   &msg,
   (void*)0
};

