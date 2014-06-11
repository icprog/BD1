#include "Menu_Include.h"
/*
static char trans_1[]="遇到危险，紧急救援";
static char trans_2[]="车辆进入通信不畅地区";
static char trans_3[]="已经安全到达";
static char trans_4[]="车辆状态正常";
*/
#define   Have_txt    2
static u8 select =0;
char *trans[]={
"遇到危险，紧急救援",
"车辆进入通信不畅地区",
"已经安全到达",
"车辆状态正常",
};


int number_txt=0;
static void msg( void *p)
{
}
static void show(void)
{
	lcd_fill(0);
	lcd_text12(0,2,"按上下翻键选择通信",18,LCD_MODE_SET);
	lcd_text12(0,20,"内容",4,LCD_MODE_SET);
	lcd_update_all();
}
/******************************************************************************
 * Function:trans_data() 
 * DESCRIPTION: - 显示文本 
 * Input: input 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 04-jan-2014  
	先简单显示 待得到具体要求时候改进显示方法
* -------------------------------
 ******************************************************************************/

static void dis_data(int input)
{


	lcd_fill(0);
	//lcd_text12(0,10,"1.",2,LCD_MODE_SET);
	lcd_text12(2,10,trans[input],strlen(trans[input]),LCD_MODE_SET);
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
		//只有选择过内容才可以通信
		if(select !=0)
		{
			BD_TX.status = bd1_send_Artificial;
			if(BD_TX.flag_send==RT_EOK)
			{
				BD1_Tx(BD1_TYPE_TXSQ,trans[number_txt],strlen(trans[number_txt]));
				BD_TX.status=bd1_send_auto;
				BD_FRE.Frequency=0;
				BD_TX.flag_send=RT_EBUSY;
				lcd_fill(0);
				lcd_text12(10,10,"信息已经发送",12,LCD_MODE_SET);
				lcd_update_all();
			}
			else
			{
				lcd_fill(0);
				lcd_text12(10,10,"等待入站.....",13,LCD_MODE_SET);
				lcd_update_all();
			}
			select =0; 
			
		}
		
		#if 0
		if(number_txt!=0)
		{
			switch(number_txt)
			{
				case 1:
				BD1_Tx(BD1_TYPE_TXSQ,trans_1,strlen(trans_1));
				break;
				case 2:
				 BD1_Tx(BD1_TYPE_TXSQ,trans_2,strlen(trans_2));	
				break;
				case 3:
				 BD1_Tx(BD1_TYPE_TXSQ,trans_3,strlen(trans_3));	
				break;
				case 4:
				 BD1_Tx(BD1_TYPE_TXSQ,trans_4,strlen(trans_4));	
				break;
			}
		}
		#endif

		

		break;
	case KeyValueUP:
		select=Have_txt;
		number_txt++;
		if(number_txt>3)
		{
			number_txt=0;
		}
		//rt_kprintf("why do this  who can tell me %d !!!\r\n",number_txt);
		dis_data(number_txt);
		break;
	case KeyValueDown:
		select=Have_txt;
		number_txt--;
		if(number_txt<0)
		{
			number_txt=3;
		}
		//rt_kprintf("why do this  2 %d !!!\r\n",number_txt);
		dis_data(number_txt);
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
MENUITEM	Menu_9_2_TongXin=
{
    "通信申请",
	8,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};

