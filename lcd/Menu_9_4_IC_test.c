#include "Menu_Include.h"
#include "Device_CAN2.h"
static u8 IC_or_test =0;


static void ic_self(unsigned char par)
{
lcd_fill(0);
lcd_text12(0, 5,"IC检测  系统自检",16,LCD_MODE_SET);
if(par==1)
	lcd_text12( 0,  5,"IC检测",6,LCD_MODE_INVERT);
else if(par==2)
	lcd_text12(48,  5,"系统自检",8,LCD_MODE_INVERT);
lcd_update_all();
}
static void msg( void *p)
{
}
static void show(void)
{
	ic_self(1);
    IC_or_test =1;
	/*
	lcd_fill(0);
	if(0==IC_or_test)
    {
	lcd_text12(0,2,"按确认键发出IC检测",18,LCD_MODE_SET);
	lcd_text12(0,18,"指令",4,LCD_MODE_SET);
	}
	if(1==IC_or_test)
	{
	lcd_text12(0,2,"按确认键发系统自检",18,LCD_MODE_SET);
	lcd_text12(0,18,"指令",4,LCD_MODE_SET);	
	}
	lcd_update_all();
	*/
}


static void keypress(unsigned int key)
{
switch(KeyValue)
	{
	case KeyValueMenu:
		pMenuItem=&Menu_9_Urgent;//scr_CarMulTrans;
		pMenuItem->show();
		CounterBack=0;

		break;
	case KeyValueOk:
		
		
		if(1==IC_or_test)
		{
		   lcd_fill(0);
			//BD1_Tx(BD1_TYPE_ICJC, 0, 1);
			BD_IC.status=result_Artificial;
			lcd_text12(10,10,"IC检测指令已发出",16,LCD_MODE_SET);
			lcd_update_all();
			//rt_kprintf("tset-----BD_IC.status %d\r\n",BD_IC.status);
			
		}
		if(2==IC_or_test)
		{
			lcd_fill(0);
			//BD1_Tx(BD1_TYPE_XTZJ,0,2);
			BD_ZJ.status=result_pending;
			lcd_text12(4,4,"系统检测指令已发出",18,LCD_MODE_SET);
			lcd_text12(4,19,"等待回复.....",13,LCD_MODE_SET);
			lcd_update_all();
			BD_ZJ.status=result_Artificial;
			//rt_kprintf("tset-----BD_ZJ.status %d\r\n",BD_ZJ.status);
		}
		
		//SD_ACKflag.f_Worklist_SD_0701H=1;//按键选择发送电子运单标志
		/*
		  这里添加发送IC检测的标志
		*/
		break;
	case KeyValueUP:
		IC_or_test++;
		if(IC_or_test>2)
		IC_or_test=1;
		ic_self(IC_or_test);
		break;
	case KeyValueDown:
		IC_or_test--;
		if(IC_or_test<1)
		IC_or_test=2;
		ic_self(IC_or_test);
		break;
	}
KeyValue=0;
}


static void timetick(unsigned int systick)
{
    CounterBack++;
	if(BD_ZJ.flag_send ==result_success)
	{
		lcd_fill(0);
		lcd_text12(10,5,"波束状况  ",10,LCD_MODE_SET);
		BD1_control.BD_Signal=BD1_control.BD_Signal+'0';
		lcd_text12(84,5,(char*)(&(BD1_control.BD_Signal)),1,LCD_MODE_SET);
		lcd_text12(10,19,"信号强度  ",10,LCD_MODE_SET);
		if(((BD1_control.BD_Signal-'0')>0)&&(BD1_control.BD_Signal-'0')<=2)
		lcd_text12(84,19,"较差",4,LCD_MODE_SET);
		if(((BD1_control.BD_Signal-'0')>2)&&(BD1_control.BD_Signal-'0')<=4)
		lcd_text12(84,19,"良好",4,LCD_MODE_SET);	
		if(((BD1_control.BD_Signal-'0')>4)&&(BD1_control.BD_Signal-'0')<=6)
		lcd_text12(84,19,"优",2,LCD_MODE_SET);	
		if(((BD1_control.BD_Signal-'0')==0))
		{
			lcd_text12(84,19,"无信号",6,LCD_MODE_SET);	
		}
		
		lcd_update_all();
		BD_ZJ.flag_send=0;
	}
	if(CounterBack!=MaxBankIdleTime)
		return;
	pMenuItem=&Menu_1_Idle;
	pMenuItem->show();
	CounterBack=0;

}

ALIGN(RT_ALIGN_SIZE)
MENUITEM	Menu_9_4_IC_test=
{
    "IC和系统检测",
	12,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};

