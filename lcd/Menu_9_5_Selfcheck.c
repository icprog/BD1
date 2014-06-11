#include "Menu_Include.h"

static void msg( void *p)
{
}
static void show(void)
{
	lcd_fill(0);
	lcd_text12(0,2,"按确认键发系统自检",18,LCD_MODE_SET);
	lcd_text12(0,18,"指令",4,LCD_MODE_SET);
	lcd_update_all();
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
		
		BD1_Tx(BD1_TYPE_XTZJ,0,2);
		lcd_fill(0);
		lcd_text12(4,4,"系统检测指令已发出",18,LCD_MODE_SET);
		lcd_update_all();
		//SD_ACKflag.f_Worklist_SD_0701H=1;//按键选择发送电子运单标志
		/*
		  这里添加发送系统检测的标志
		*/
		break;
	case KeyValueUP:
		break;
	case KeyValueDown:
		break;
	}
KeyValue=0;
}


static void timetick(unsigned int systick)
{
    CounterBack++;
	if(CounterBack!=MaxBankIdleTime)
		return;
	pMenuItem=&Menu_1_Idle;
	pMenuItem->show();
	CounterBack=0;

}

ALIGN(RT_ALIGN_SIZE)
MENUITEM	Menu_9_5_Selfcheck=
{
    "系统检测",
	8,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};