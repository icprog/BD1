#include "Menu_Include.h"

#define shuzu_num   210
#define Locking      1
#define Unlocking    2



char *number[]={"1:","2.","3.","4.","5.","6.","7.","8.","9.","10.","11.","12.","13.","14.","15.",};
static u8 main_screen=0;//������
static u8 sub_screen=0;//��������
static u8 main_flag =0;//1�������ɼ� 2���������ɼ�
static u8 sub_flag=0;//��������־0�����ɼ� 1���ɼ�
static u8 lock_menu=0;

u8 info_cont[shuzu_num];
u8 key_ok = Unlocking;
u8 len_txt=0;



static void msg( void *p)
{
}
static void show(void)
{
	lcd_fill(0);
	lcd_text12(0,5,"��ȷ�ϼ�����ͨ������",20,LCD_MODE_SET);
	//lcd_text12(0,18,"??",4,LCD_MODE_SET);
	lcd_update_all();
	lock_menu=1;
	//��ʼ��Ϊ���״̬
	main_screen=0;
	sub_screen=0;
	main_flag =0;
	sub_flag=0;
	
	
}


static void keypress(unsigned int key)
{
switch(KeyValue)
	{
	case KeyValueMenu:
		//�������ɼӻ�������Ļ�ɼӵ�ʱ��˵����������˵�
		if((main_flag ==1)||(lock_menu==1))
		{
			pMenuItem=&Menu_9_Urgent;//scr_CarMulTrans;
			pMenuItem->show();
			CounterBack=0;
			main_flag=0;
		}
		else//����������������
		{
			key_ok=Unlocking;
			main_flag=1;
			sub_flag=0;
			len_txt=Read_save_data(info_cont, main_screen,LCD_MODE_SET);
		}
		break;
	case KeyValueOk:
		/*
		BD1_Tx(BD1_TYPE_SJSC,0,2);
		lcd_fill(0);
		lcd_text12(2,8,"ʱ���ȡָ���ѷ���",18,LCD_MODE_SET);
		lcd_update_all();
		//SD_ACKflag.f_Worklist_SD_0701H=1;//����ѡ���͵����˵���־
		
		  //������ӷ���ϵͳ���ı�־
		*/
		lock_menu=0;//һ�����κΰ������¾�Ӧ��Ϊ0
		if(key_ok==Unlocking)
		{
			if(main_flag==0)
			{
				len_txt=Read_save_data(info_cont, main_screen,LCD_MODE_SET);
				if(len_txt==RT_ERROR)
				{
					main_flag =0;
				}
			}
			if(main_flag<=2)
			{
				main_flag++;
			}
			if(main_flag ==1)
			{
				sub_flag =0;//�����ɼӣ��������ɼ�
			}
			
			if(main_flag ==2)//��ʱ�������ԼӼ�
			{
				len_txt=Read_save_data(info_cont, main_screen,LCD_MODE_INVERT);
				sub_flag =1;
				sub_screen=0;//�������ʾʱ��������0��ʼ
				key_ok=Locking;
			}
		}
		break;
	case KeyValueUP:
		lock_menu=0;
		if((sub_flag==0)&&(main_screen!=0))
		{
			main_flag =1;//�����ɷ����ȴ���ȷ�Ϸ�ɫ
			main_screen=main_screen-1;
			if(main_screen<1)
			{
				main_screen =0;
			}
			len_txt=Read_save_data(info_cont, main_screen,LCD_MODE_SET);
			rt_kprintf("main_screen--%d\r\n",main_screen);
		}
		if((sub_flag ==1)&&(sub_screen!=0))
		{
			sub_screen=sub_screen-1;
			if(sub_screen<1)
			{
				sub_screen=0;
			}
			show_screen(info_cont, sub_screen, len_txt);
			rt_kprintf("sub_screen--%d\r\n",main_screen);
		}
		break;
	case KeyValueDown:
		lock_menu=0;
		if(sub_flag==0)
		{
		    main_flag=1;//�����ɷ����ȴ���ȷ�Ϸ�ɫ
			main_screen=main_screen+1;
			if(main_screen>15)
			{
				main_screen =15;
			}
			len_txt=Read_save_data(info_cont, main_screen,LCD_MODE_SET);
			rt_kprintf("main_screen--%d\r\n",main_screen);
		}
		if(sub_flag ==1)
		{
			sub_screen=sub_screen+1;
			if(sub_screen>6)
			{
				sub_screen=6;
			}
			show_screen(info_cont, sub_screen, len_txt);
			rt_kprintf("sub_screen--%d\r\n",main_screen);
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
    "ͨ����Ϣ�鿴",
	12,
	&show,
	&keypress,
	&timetick,
	&msg,
	(void*)0
};
