#include "Menu_Include.h"
#include <string.h>
#include "App_moduleConfig.h"

static unsigned char noselect_BD[]={0x3C,0x7E,0xC3,0xC3,0xC3,0xC3,0x7E,0x3C};//空心
static unsigned char select_BD[]={0x3C,0x7E,0xFF,0xFF,0xFF,0xFF,0x7E,0x3C};//实心

static DECL_BMP(8,8,select_BD); 
static DECL_BMP(8,8,noselect_BD); 

static unsigned char menu_pos=0;
static PMENUITEM psubmenu[6]=
{
	&Menu_9_0_user,
	&Menu_9_1_1_Location,
	&Menu_9_2_TongXin,		//通信申请
	&Menu_9_4_IC_test,	                  
	//&Menu_9_5_Selfcheck,	//系统自检(可以作为开机执行)
	&Menu_9_7_Timed,   //时间输出
	//&Menu_9_6_Version,	//版本读取
};
static void menuswitch(void)
{
u8 i=0;
	
lcd_fill(0);
lcd_text12(0,3,"紧急",4,LCD_MODE_SET);
lcd_text12(0,17,"通信",4,LCD_MODE_SET);
for(i=0;i<5;i++)
lcd_bitmap(30+i*11, 5, &BMP_noselect_BD, LCD_MODE_SET);
lcd_bitmap(30+menu_pos*11,5,&BMP_select_BD,LCD_MODE_SET);
lcd_text12(30,19,(char *)(psubmenu[menu_pos]->caption),psubmenu[menu_pos]->len,LCD_MODE_SET);
lcd_update_all();
}
static void msg( void *p)
{
}
static void show(void)
{
    menu_pos=0;
	menuswitch();
}


static void keypress(unsigned int key)
{
switch(KeyValue)
	{
	case KeyValueMenu:
		//允许退出
		//Password_correctFlag=1;
		
		pMenuItem=&Menu_1_Idle;
		pMenuItem->show();
		CounterBack=0;
		break;
	case KeyValueOk:
			pMenuItem=psubmenu[menu_pos];
			pMenuItem->show();
		break;
	case KeyValueUP:
		if(menu_pos==0) 
			menu_pos=4;
		else
			menu_pos--;
		menuswitch();		
		break;
	case KeyValueDown:
		menu_pos++;
		if(menu_pos>4)
			menu_pos=0;
		menuswitch();
		break;
	}
KeyValue=0;
}


static void timetick(unsigned int systick)
{

	CounterBack++;
	if(CounterBack!=MaxBankIdleTime)
		return;
	CounterBack=0;
	pMenuItem=&Menu_1_Idle;
	pMenuItem->show();
}

ALIGN(RT_ALIGN_SIZE)
	
MENUITEM	Menu_9_Urgent =
{
    "紧急通信",
	8,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};
