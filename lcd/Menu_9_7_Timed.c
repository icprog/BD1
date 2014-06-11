#include "Menu_Include.h"

#define shuzu_num   210
#define Locking      1
#define Unlocking    2

#define the_main_screen  1
#define the_sub_screen   2
#define screen_cannot_add 3
#define screen_could_add  4

char *number[]={"1.","2.","3.","4.","5.","6.","7.","8.","9.","10.","11.","12.","13.","14.","15.","16.",};
static u8 main_screen=screen_could_add;
static u8 sub_screen=screen_cannot_add;
static u8 main_add =1;
static u8 sub_add=0;

static u8 screen_status =0;
static u8 screen_add =0;
u8 info_cont[shuzu_num];
u8 len_txt=0;
u8 screen_switch =0;


static void msg( void *p)
{
}
static void show(void)
{
	lcd_fill(0);
	lcd_text12(0,5,"按确认键查已通信内容",20,LCD_MODE_SET);
	//lcd_text12(0,18,"??",4,LCD_MODE_SET);
	lcd_update_all();
	//初始化为最初状态
	main_screen=screen_could_add;
	sub_screen=screen_cannot_add;
	main_add =1;
	sub_add=1;
	screen_status =the_main_screen;
	
	
}


static void keypress(unsigned int key)
{
switch(KeyValue)
	{
	case KeyValueMenu:
		//当主屏可加或是子屏幕可加的时候菜单键返回主菜单
		if(main_screen==screen_could_add)
		{
			pMenuItem=&Menu_9_Urgent;//scr_CarMulTrans;
			pMenuItem->show();
			CounterBack=0;
			screen_switch =0;
		}
		if(sub_screen==screen_could_add)//从子屏里面挑出来
		{
			
			screen_status =the_main_screen;
			sub_screen=screen_cannot_add;
			main_screen=screen_could_add;
			len_txt=Read_save_data(info_cont, main_add,LCD_MODE_SET);
			screen_switch =0;
		}
		break;
	case KeyValueOk:
		/*
		BD1_Tx(BD1_TYPE_SJSC,0,2);
		lcd_fill(0);
		lcd_text12(2,8,"时间获取指令已发出",18,LCD_MODE_SET);
		lcd_update_all();
		//SD_ACKflag.f_Worklist_SD_0701H=1;//按键选择发送电子运单标志
		
		  //这里添加发送系统检测的标志
		*/
		if((main_screen==screen_could_add)&&(screen_status ==the_main_screen))
		{
			len_txt=Read_save_data(info_cont, main_add,LCD_MODE_SET);
			screen_switch++;
			
		}
		if((screen_status ==the_main_screen)&&(screen_switch==2))
		{
			
			len_txt=Read_save_data(info_cont,main_add,LCD_MODE_INVERT);
			screen_status =the_sub_screen;
			sub_screen =screen_could_add; 
			main_screen = screen_cannot_add;
			sub_add=0;
		}
		break;
	case KeyValueUP:
		
		if((main_screen==screen_could_add)&&(screen_status ==the_main_screen))
		{
			screen_switch =0;
			main_add=main_add-1;
			if(main_add<1)
			{
				main_add =1;
			}
			len_txt=Read_save_data(info_cont, main_add,LCD_MODE_SET);
			rt_kprintf("main_screen--%d\r\n",main_add);
		}
		if((sub_screen ==screen_could_add)&&(screen_status ==the_sub_screen))
		{
			sub_add=sub_add-1;
			if(sub_add<1)
			{
				sub_add=1;
			}
			show_screen(info_cont, sub_add, len_txt);
			rt_kprintf("sub_screen--%d\r\n",sub_add);
		}
		break;
	case KeyValueDown:
		
		if((main_screen==screen_could_add)&&(screen_status ==the_main_screen))
		{
		  screen_switch =0;
			main_add=main_add+1;
			if(main_add>16)
			{
				main_add =16;
			}
			len_txt=Read_save_data(info_cont, main_add,LCD_MODE_SET);
			rt_kprintf("main_screen--%d\r\n",main_add);
		}
		if((sub_screen ==screen_could_add)&&(screen_status ==the_sub_screen))
		{
			sub_add=sub_add+1;
			if(sub_add>6)
			{
				sub_add=6;
			}
			show_screen(info_cont, sub_add, len_txt);
			rt_kprintf("sub_screen--%d\r\n",sub_add);
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
	pMenuItem=&Menu_1_Idle;
	pMenuItem->show();
	CounterBack=0;

}

ALIGN(RT_ALIGN_SIZE)
MENUITEM	Menu_9_7_Timed=
{
    "通信信息查看",
	12,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};
