/*
    App Gsm uart
*/

#include <rtthread.h> 
#include <rthw.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "board.h"
#include <serial.h>

#include  <stdlib.h>//数字转换成字符串
#include  <stdio.h>
#include  <string.h>
#include "App_moduleConfig.h"



 //---------------变量声明-------------------------------------------
    //第四个字节  0x81: 320x240      0x82 : 640 x480
 u8   Take_photo[10]={0x40,0x40,0x61,0x81,0x02,0X00,0X00,0X02,0X0D,0X0A}; ;   //----  报警拍照命令
 u8   Fectch_photo[10]={0x40,0x40,0x62,0x81,0x02,0X00,0XFF,0XFF,0X0D,0X0A};;   //----- 报警取图命令 


 u8    _485_CameraData_Enable=0;// 有图片数据过来   1: data come  0:   no data 
 u8 	 _485_content[600];
 u16	 _485_content_wr=0;


 
static  u16	 PackageLen=0;//记录每次接收的byte数  
 u8  OpenDoor_StartTakeFlag=0; // 车开关开始拍照状态 ，	开始拍照时	set   1   拍照结束后  0 
 u8   Opendoor_transFLAG=0x02;	   //	车门打开拍照后是否上传标志位  



//----------- _485 rx-----
ALIGN(RT_ALIGN_SIZE)
u8    _485_dev_rx[_485_dev_SIZE];       
u16  _485dev_wr=0;      




struct rt_device  Device_485;
MSG_Q_TYPE  _485_MsgQue_sruct;
_485REC_Struct 	 _485_RXstatus;	  


//=====================================================
_Big_lcd Big_lcd;
u8 judge[1]={0};
extern u8 dwlcd;
//++++++++++++++++++++++++++++++++++++++=DWLCD+++++++++++++++++++
/******************************************************************************
 * Function: Lcd_write () 
 * DESCRIPTION: - 向LCD写数据 (密码没有输入完全时使用此函数要在判定车台配置是否完成)
 * Input: statu	LCD状态	type 写数据类型
 * Input: data 数据
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 19-jan-2014
* -------------------------------
 ******************************************************************************/
u8  Lcd_write(u8 statu,u8 type,u8 data)
{
	//TX_485const;

	memset(Big_lcd.Txinfo,0,sizeof(Big_lcd.Txinfo));
	memset(Big_lcd.Txinfo,0,sizeof(Big_lcd.Txinfo));
	Big_lcd.TxInfolen=0;
	Big_lcd.Txinfo[0]=0x88;//协议头
	Big_lcd.Txinfo[1]=0x55;
	if(statu !=LCD_IDLE)
	{
		//RX_485const;
		return RT_EBUSY;
	}
	statu =LCD_BUSY;
	
	switch (type)
	{   
		case  LCD_SETTIME:    //  ??  
			/*  AA 55 0A 80 1F 5A 12 09 01 03 11 53 01     -----??
				?: 12 09 01 03 11 53 01 ??  12? 09?01? ??3 11:53:01
			*/
			Big_lcd.Txinfo[2]=0x0A; 
			Big_lcd.Txinfo[3]=0x80;
			Big_lcd.Txinfo[4]=0x1F;  //  ??? 
			Big_lcd.Txinfo[5]=0x5A;	  
			Big_lcd.Txinfo[6]=((((time_now.year))/10)<<4)+(((time_now.year))%10);  	   	
			Big_lcd.Txinfo[7]=((time_now.month/10)<<4)+(time_now.month%10);  
			Big_lcd.Txinfo[8]=((time_now.day/10)<<4)+(time_now.day%10); 
			Big_lcd.Txinfo[9]=0x01; 
			Big_lcd.Txinfo[10]=((time_now.hour/10)<<4)+(time_now.hour%10);
			Big_lcd.Txinfo[11]=((time_now.min/10)<<4)+(time_now.min%10);
			Big_lcd.Txinfo[12]=((time_now.sec/10)<<4)+(time_now.sec%10);       
			rt_device_write(&Device_485,0,( const char*)Big_lcd.Txinfo,13);		
		break;	  
		case  LCD_SETSPD:   //   ????
			// AA 55 07 82 00 01 01 30  00 F3 
			Big_lcd.Txinfo[2]=0x05; 
			Big_lcd.Txinfo[3]=0x82;
			Big_lcd.Txinfo[4]=0x10;
			Big_lcd.Txinfo[5]=0x00;
			Big_lcd.Txinfo[6]=((Speed_gps/10)>>8);     // ??
			Big_lcd.Txinfo[7]=(u8)(Speed_gps/10);  	
			rt_device_write(&Device_485,0,( const char*)Big_lcd.Txinfo,8);				

		break;
		case  LCD_GPSNUM:  //  GPS ??   AA 55 05 82 10 01 00 03  ???20
			Big_lcd.Txinfo[2]=5;   // ?? 
			Big_lcd.Txinfo[3]=0x82;
			Big_lcd.Txinfo[4]=0x10;  // LCD ID  
			Big_lcd.Txinfo[5]=0x01; 
			Big_lcd.Txinfo[6]=0x00;  //Data
			Big_lcd.Txinfo[7]= data;//Satelite_num; 

			rt_device_write(&Device_485,0,( const char*)Big_lcd.Txinfo,8); 	
		break;
		case    LCD_GSMNUM://   GSM ??  AA 55 05 82 10 02 00 09  ???31
			Big_lcd.Txinfo[2]=5;   // ?? 
			Big_lcd.Txinfo[3]=0x82;
			Big_lcd.Txinfo[4]=0x10;  // LCD ID  
			Big_lcd.Txinfo[5]=0x02;  
			Big_lcd.Txinfo[6]=0x00;  //Data
			Big_lcd.Txinfo[7]=data;//ModuleSQ;   
			rt_device_write(&Device_485,0,( const char*)Big_lcd.Txinfo,8); 	  
		break;
		case LCD_PAGE:
			Big_lcd.Txinfo[2]= 0X04;
			Big_lcd.Txinfo[3]= 0X80;
			Big_lcd.Txinfo[4]= 0X03;
			Big_lcd.Txinfo[5]= 0X00;
			Big_lcd.Txinfo[6]=data;
			rt_device_write(&Device_485,0,( const char*)Big_lcd.Txinfo,7);
		break;
		case LCD_Text_clear:
			Big_lcd.Txinfo[2]= 0X05;
			Big_lcd.Txinfo[3]= 0X82;
			Big_lcd.Txinfo[4]= 0X44;
			Big_lcd.Txinfo[5]= 0X00;
			Big_lcd.Txinfo[6]= 0x00;
			Big_lcd.Txinfo[7]= 0x00;
			rt_device_write(&Device_485,0,( const char*)Big_lcd.Txinfo,8);
		break;
	}
	//-------------------------
	OutPrint_HEX("w_lcd_bd:", Big_lcd.Txinfo, Big_lcd.Txinfo[2]+3);
	Big_lcd.status=LCD_IDLE;          
	Big_lcd.TxInfolen=0; //长度清零	
	delay_ms(5);
	
	
	return RT_EOK;
	}


/******************************************************************************
 * Function: Lcd_init () 
 * DESCRIPTION: - DWLCD初始化 
 * Input: 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * -----------------------------------
* Created By wxg 19-jan-2014
* -------------------------------
 ******************************************************************************/
void  Lcd_init(void)
{
	Big_lcd.CounterTimer=0;
	memset(Big_lcd.RxInfo,0,sizeof(Big_lcd.RxInfo));
	Big_lcd.status=LCD_IDLE;
	memset(Big_lcd.Txinfo,0,sizeof(Big_lcd.Txinfo));
	memset(Big_lcd.TXT_content,0,sizeof(Big_lcd.TXT_content));
}



void  Lcd_Data_Process(void) 
{
	u16 data_id =0,i=0; 
	u16 ContentLen=0;
	u8  dwin_reg[150];
	OutPrint_HEX("RX_form_lcd:",Big_lcd.RxInfo,Big_lcd.RxInfo[2]+3);
	//AA 55 08 83 30 00 02 1E FF FF 00               
	//      08 ???       83  : ??  33 00 : ??   02 1E FF FF 00: ?? 
	
	if(Big_lcd.RxInfolen>=3)//是有效数据
	{
	    ContentLen  = Big_lcd.RxInfo[6]*2;//内容的长度
		data_id=((u16)Big_lcd.RxInfo[4]<<8)+(u16)Big_lcd.RxInfo[5];
		
	//   Get useful  infomation    
	memset(dwin_reg,0,sizeof(dwin_reg));
	while((0xFF !=Big_lcd.RxInfo[7+i])&&(2*ContentLen >i))//找到后面的数据
	{
		dwin_reg[i] = Big_lcd.RxInfo[7+i];
		i++;
	}
	//rt_kprintf("data_id.......%d\r\n",data_id);
	switch(data_id)
	{
   case 0x1010:
   	    rt_kprintf("the length dwin_reg..%d\r\n",strlen(dwin_reg));
   	  if(strcmp(dwin_reg,"001100")==0)
   	  	{
   	  		Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
			judge[0] = 0x01;
   	  	}
	 
   	break;
	case 0x1020:
		if(strlen(dwin_reg) ==11)
		{

		memset(SimID_12D,0,sizeof(SimID_12D));
		SimID_12D[0]='0';
		memcpy(SimID_12D+1,dwin_reg,11);								 
		DF_WriteFlashSector(DF_SIMID_12D,0,SimID_12D,13); 
		delay_ms(80); 		  
		//rt_kprintf("\r\n 显示屏设备SIM_ID设置为 : %s", SimID_12D);   
		DF_ReadFlash(DF_SIMID_12D,0,SimID_12D,13); 
		SIMID_Convert_SIMCODE();  // 转换
		
		OutPrint_HEX("sim",dwin_reg,12);
		judge[0] = judge[0]|0x02;
		rt_kprintf("judge[0]..%d\r\n",judge[0]);
		Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
		}
		else
	  	{
	    Lcd_write(Big_lcd.status,LCD_PAGE,0x17);
	  	}
	break;

	case 0x1030:
		if(strlen(dwin_reg) ==17)//VIN
		{
			OutPrint_HEX("VIN", dwin_reg, 17);
			judge[0] = judge[0]|0x04;
			memset(Vechicle_Info.Vech_VIN,0,sizeof(Vechicle_Info.Vech_VIN));
			memcpy(Vechicle_Info.Vech_VIN,dwin_reg,17);
			DF_WriteFlashSector(DF_Vehicle_Struct_offset,0,(u8*)&Vechicle_Info,sizeof(Vechicle_Info));
			rt_kprintf("judge[0]..%d\r\n",judge[0]);
			Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
			
		}
		else
	  	{
	    Lcd_write(Big_lcd.status,LCD_PAGE,0x17);
	  	}
	break;
	case 0x1040:
		if(strlen(dwin_reg)==8)//车牌
		{
			OutPrint_HEX("che pai ", dwin_reg, 8);
			judge[0] = judge[0]|0x08;
			chepai(dwin_reg);
			rt_kprintf("judge[0]..%d\r\n",judge[0]);
			Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
		}
	 else
	  	{
	    Lcd_write(Big_lcd.status,LCD_PAGE,0x17);
	  	}
	break;
	case 0x1050://车辆颜色
		if(dwin_reg[1]!=0)
			{
		        if(0x0A ==dwin_reg[1])
		        {
		        	Vechicle_Info.Dev_Color=0;
					DF_WriteFlashSector(DF_Vehicle_Struct_offset,0,(u8*)&Vechicle_Info,sizeof(Vechicle_Info));
					judge[0] = judge[0]|0x10;
					rt_kprintf("judge[0]..%d\r\n",judge[0]);
					Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
		        }
				else
				{
				    Vechicle_Info.Dev_Color = dwin_reg[1];
					DF_WriteFlashSector(DF_Vehicle_Struct_offset,0,(u8*)&Vechicle_Info,sizeof(Vechicle_Info));
					judge[0] = judge[0]|0x10;
					rt_kprintf("judge[0]..%d\r\n",judge[0]);
					Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
				}
			}
		else
			{
				Lcd_write(Big_lcd.status,LCD_PAGE,0x17);
			}
		
	
	break;
	case 0x1060:
		if(dwin_reg[1]!=0)//速度获取类型
		{
			if(0x0A ==dwin_reg[1])
			{
				JT808Conf_struct.Speed_GetType = 0;
				Api_Config_Recwrite_Large(jt808,0,(u8*)&JT808Conf_struct,sizeof(JT808Conf_struct));
				judge[0] = judge[0]|0x20;
				rt_kprintf("judge[0]..%d\r\n",judge[0]);
				Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
			}
			else
			{
				JT808Conf_struct.Speed_GetType = dwin_reg[1];
				Api_Config_Recwrite_Large(jt808,0,(u8*)&JT808Conf_struct,sizeof(JT808Conf_struct));
				
				judge[0] = judge[0]|0x20;
				rt_kprintf("judge[0]..%d\r\n",judge[0]);
				Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
			}
		}
		else
		{
			Lcd_write(Big_lcd.status,LCD_PAGE,0x17);
		}
	
		
	break;
	case 0x1070:
		if(strlen(dwin_reg)!=0)//车辆类型
		{
		    memset(Vechicle_Info.Vech_Type,0,13);
			memcpy(Vechicle_Info.Vech_Type,dwin_reg,strlen(dwin_reg));
			DF_WriteFlashSector(DF_Vehicle_Struct_offset,0,(u8*)&Vechicle_Info,sizeof(Vechicle_Info));
			OutPrint_HEX("lei xing ...", dwin_reg, 50);
		    judge[0] = judge[0]|0x40;
			rt_kprintf("judge[0]..%d\r\n",judge[0]);
			Lcd_write(Big_lcd.status,LCD_PAGE,0x14);
		}
		else
		{
			Lcd_write(Big_lcd.status,LCD_PAGE,0x17);
		}

	break;
	case 0x1080:
		if(0x7f == judge[0])
			{
			
			  Lcd_write(Big_lcd.status,LCD_PAGE,0x01);//回到显示界面
			  Login_Menu_Flag=1;    // clear  first flag 	 Login_Menu_Flag=0;     //  输入界面为0   
   				DF_WriteFlashSector(DF_LOGIIN_Flag_offset,0,&Login_Menu_Flag,1);
			  pMenuItem=&Menu_1_Idle;
			  rt_kprintf("judge[0]..%d\r\n",judge[0]);
			}
		else
			{
			  Lcd_write(Big_lcd.status,LCD_PAGE,17);
			}
	break;
	case 0x1090:
	break;
	case 0x4400:
				OutPrint_HEX("DWLCD:",dwin_reg ,strlen(dwin_reg));
				memset(Big_lcd.TXT_content,0,sizeof(Big_lcd.TXT_content));
				memcpy(Big_lcd.TXT_content,dwin_reg,strlen(dwin_reg));
				Lcd_write(Big_lcd.status, LCD_PAGE, 26);
				
	break;
	case 0x4500:
				//rt_kprintf("Big_lcd.TXT_content_len  %d\r\n",strlen(Big_lcd.TXT_content));
				if(strlen(Big_lcd.TXT_content)!=0)	
				{	
				
				if(BD_TX.flag_send==RT_EOK)
					{
						
						BD1_Tx(BD1_TYPE_TXSQ,Big_lcd.TXT_content,strlen(Big_lcd.TXT_content));
						BD_FRE.Frequency =0;//从新计数吧
						BD_TX.flag_send=RT_EBUSY;
					}
					else
					{
						dwlcd =1;
						BD_TX.status = bd1_send_Artificial;
						Lcd_write(Big_lcd.status, LCD_PAGE, 27);//等待入站
						
					}
					
					
				}
	break;	
//-------------------------------------------------------车辆初始化-----------------------		

	default:
	break;
	
	}
	
}
else
{
	return ;
}

}


//++++++++++++++++++++++++++++++++++++++=DWLCD++++++++++++++++++++++++++++++++++++++++++++

void  Photo_TakeCMD_Update(u8 CameraNum)
{
      Take_photo[4]=CameraNum;    // Set take  Camra  Num
      Take_photo[5]=0x00;
}

void  Photo_FetchCMD_Update(u8 CameraNum)
{
     Fectch_photo[4]=CameraNum; // Set  fetch  Camra  Num 
     Fectch_photo[5]=0x00;
}

void _485_delay_us(u16 j)
{
u8 i;
  while(j--)
  	{
  	i=3;
  	while(i--);
  	}  
}
void _485_delay_ms(u16 j)
{
  while(j--)
  	{
         _485_delay_us(1000);   
  	} 
   
}


/* write one character to serial, must not trigger interrupt */
 void rt_hw_485_putc(const char c)
{
	/*
		to be polite with serial console add a line feed
		to the carriage return character
	*/
	//USART_SendData(USART2,  c); 
	//while (!(USART2->SR & USART_FLAG_TXE));  
	//USART2->DR = (c & 0x1FF);  
	USART_SendData(USART2,  c);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){} 
}

void rt_hw_485_output(const char *str)
{
	/* empty console output */
	TX_485const; 
	_485_delay_ms(1);
		//--------  add by  nathanlnw ---------
       while (*str)
	{
		rt_hw_485_putc (*str++);
	}
       //--------  add by  nathanlnw  --------	
       _485_delay_ms(3); 
       RX_485const; 	 
}

void rt_hw_485_Output_Data(const char *Instr, unsigned int len)  
{
        unsigned int  Info_len485=0;

	Info_len485=len;
    	/* empty console output */
	TX_485const; 
	_485_delay_ms(1);
		//--------  add by  nathanlnw ---------
       while (Info_len485)
	{
		rt_hw_485_putc (*Instr++); 
		Info_len485--;
	}
       //--------  add by  nathanlnw  --------	
       _485_delay_ms(3); 
       RX_485const; 	 
}

static u8  CHKendTake_ReadyToSend(void)
{ /*  结束多路拍照，同时触发上报处理   */


    if(Max_CameraNum==Camera_Number) 
    {
		  /*
			  Taking End, Start Transfering 
		   */
		   MultiTake.Taking=0;	// Taking  State  Over	   
		   
		   //------ 判断车门开关拍照是否上传状态 ------
		   if((OpenDoor_StartTakeFlag==1)&&(Opendoor_transFLAG==0))
			{
				 MultiTake_End();	// 车门开关不上传
				 OpenDoor_StartTakeFlag=0;
			} 
		   else
				Check_MultiTakeResult_b4Trans();

		 return true;  
     }
	else
		 return false;
}

void  OpenDoor_TakePhoto(void)
{
   	    //------------------------------ 开关车门拍照状态检测 -----------------------------
	  if(DoorLight_StatusGet())  // PA1
	  	{
	            DoorOpen.currentState=1;
		    //   rt_kprintf( "\r\n   门高!\r\n "); 
		     //--------------------------------------
	    if( (Car_Status[2]&0x20)==0x00)
	      	{
				Car_Status[2]|=0x20; 
				PositionSD_Enable();					
				Current_UDP_sd=1;	
				rt_kprintf("\r\n  前门开启\r\n");	  
		      	}	
			   Car_Status[2]|=0x20; //  Bit(13)     Set  1  表示  前门开

			 //--------------------------------------
	  	}
	  else
	  	{
	            DoorOpen.currentState=0;
 		    // rt_kprintf( "\r\n   门低!\r\n ");   
 		    //-------------------------------
 		     if( Car_Status[2]&0x20)
	      	{
				Car_Status[2]&=~0x20;
				PositionSD_Enable();					
				Current_UDP_sd=1;	
				rt_kprintf("\r\n  前门关闭\r\n");	 	 
	      	}
	            Car_Status[2]&=~0x20; //  Bit(4)     Set  0  表示  飞翼开  0 营运状态 	

			//-------------------------------
 	  	}
	  if((DoorOpen.currentState!=DoorOpen.BakState)&&(DataLink_Status())) 
	  	{	   
	  	   rt_kprintf( "\r\n开关车门状态变化 \r\n");				 
		   if((CameraState.status==other)&&(Photo_sdState.photo_sending==0)&&(0==MultiTake.Taking)&&(0==MultiTake.Transfering))
		   	{
                    //----------  多路摄像头拍照 -------------
					MultiTake_Start();    
					//Start_Camera(1);    
					rt_kprintf( "\r\n   变化拍照!\r\n ");	   			 
		   	} 
	  }
	  DoorOpen.BakState=DoorOpen.currentState; //  update state        
}

void  _485_RxHandler(u8 data)
{
    //      Large   LCD   Data 

	//rt_interrupt_enter( );
	_485_dev_rx[_485dev_wr++]=data;
#if 1
	switch(_485_RXstatus._485_receiveflag)
	{
		case  IDLE_485:
			if((0x88!=_485_dev_rx[0])&&(0x40!=_485_dev_rx[0]))
			{	  //    ?D?? μúò???×??úê?·?o?·¨￡?·??ò?±?ó??3y	
				_485dev_wr=0;
				break;	  
			}								  
			switch (_485dev_wr)	 
			{   
				case  2 :   if((0x55!=_485_dev_rx[1])&&(0x40!=_485_dev_rx[1])) 
					_485dev_wr=0; 												
				break;
				case   3:
				//----------  Check  LCD  --------------------------
				if((0x88==_485_dev_rx[0])&&(0x55==_485_dev_rx[1]))  
				{     //AA 55 08 83 30 00 02 1E FF FF 00                     08 ê?3¤?è(°üo?1BYTE ??á?2 BYTES  μ??·)
					_485_RXstatus._485_RxLen=_485_dev_rx[2];
					_485_RXstatus._485_receiveflag=LARGE_LCD; 			
				}	
				else
					if(0x40!=_485_dev_rx[0])	
					_485dev_wr=0; 	
				break;
				case    8:      //        -------  Camera  Data  -----------
					if((0x40==_485_dev_rx[0])&&(0x40==_485_dev_rx[1])&&(0x63==_485_dev_rx[2])) 
					{     //  40  40  63  FF  FF  00  02           00  02  ê?3¤?è  (D???)
						   _485_RXstatus._485_RxLen=((u16)_485_dev_rx[6]<<8)+(u16)_485_dev_rx[5];
						//----------------------------------  						   
						_485dev_wr=0;  //clear now  , the bytes  receiving  later  is  pic data 	
						_485_RXstatus._485_receiveflag=CAMERA_Related;  //  return Idle
					}	  
				else
					_485dev_wr=0;   // clear
				break;	 
			}


		break;
		case  LARGE_LCD:
		if(_485dev_wr>=(_485_dev_rx[2]+3))  //  AA  55  08    
		{
			//  send msg_queue
			//_485_MsgQue_sruct.info=_485_dev_rx;
			//_485_MsgQue_sruct.len=_485dev_wr;	
			//-------------------------- 
			memset(Big_lcd.RxInfo,0,sizeof(Big_lcd.RxInfo));
			memcpy(Big_lcd.RxInfo,_485_dev_rx,(_485_dev_rx[2])+3); 
			Big_lcd.RxInfolen=(_485_dev_rx[2]+3);     
			//-------------------------- 
			_485dev_wr=0;  // clear	  
			_485_RXstatus._485_receiveflag=IDLE_485;  
			Big_lcd.RX_enable=LCD_BUSY;
		}					
		break;  
		case   CAMERA_Related:
		if(_485dev_wr>=_485_RXstatus._485_RxLen) 
		{
			memset(_485_content,0,sizeof(_485_content));
			//-------------------------------------------------- 					  
			memcpy(_485_content,_485_dev_rx,_485_RXstatus._485_RxLen); 
			_485_content_wr=_485_RXstatus._485_RxLen; // Packet info len 
			_485_CameraData_Enable=1;   // í???êy?Y1yà′á?
			//---------------------------------------------------
			_485dev_wr=0; // clear 
			_485_RXstatus._485_receiveflag=IDLE_485;
		} 
		break; 
		default:
			_485dev_wr=0;  
			_485_RXstatus._485_receiveflag=	IDLE_485; 
		break ;
	} 

#endif
 // rt_interrupt_leave( );   
}

void  _485_RxHandler_01(void)
{
    #if 0
	OutPrint_HEX("485",_485_dev_rx , strlen(_485_dev_rx));
	memset(_485_dev_rx,0,sizeof(_485_dev_rx));
	switch(_485_RXstatus._485_receiveflag)
	{
		case  IDLE_485:
			if((0x88!=_485_dev_rx[0])&&(0x40!=_485_dev_rx[0]))
			{	  //    ?D?? μúò???×??úê?·?o?·¨￡?·??ò?±?ó??3y	
				_485dev_wr=0;
				break;	  
			}								  
			switch (_485dev_wr)	 
			{   
				case  2 :   if((0x55!=_485_dev_rx[1])&&(0x40!=_485_dev_rx[1])) 
					_485dev_wr=0; 												
				break;
				case   3:
				//----------  Check  LCD  --------------------------
				if((0x88==_485_dev_rx[0])&&(0x55==_485_dev_rx[1]))  
				{     //AA 55 08 83 30 00 02 1E FF FF 00                     08 ê?3¤?è(°üo?1BYTE ??á?2 BYTES  μ??·)
					_485_RXstatus._485_RxLen=_485_dev_rx[2];
					_485_RXstatus._485_receiveflag=LARGE_LCD; 			
				}	
				else
					if(0x40!=_485_dev_rx[0])	
					_485dev_wr=0; 	
				break;
				case    8:      //        -------  Camera  Data  -----------
					if((0x40==_485_dev_rx[0])&&(0x40==_485_dev_rx[1])&&(0x63==_485_dev_rx[2])) 
					{     //  40  40  63  FF  FF  00  02           00  02  ê?3¤?è  (D???)
						   _485_RXstatus._485_RxLen=((u16)_485_dev_rx[6]<<8)+(u16)_485_dev_rx[5];
						//----------------------------------  						   
						_485dev_wr=0;  //clear now  , the bytes  receiving  later  is  pic data 	
						_485_RXstatus._485_receiveflag=CAMERA_Related;  //  return Idle
					}	  
				else
					_485dev_wr=0;   // clear
				break;	 
			}


		break;
		case  LARGE_LCD:
		if(_485dev_wr>=(_485_RXstatus._485_RxLen+3))  //  AA  55  08    
		{
			//  send msg_queue
			_485_MsgQue_sruct.info=_485_dev_rx;
			_485_MsgQue_sruct.len=_485dev_wr;	
			//-------------------------- 
			memset(Big_lcd.RxInfo,0,sizeof(Big_lcd.RxInfo));
			memcpy(Big_lcd.RxInfo,_485_dev_rx,_485_dev_rx[2]+3); 
			Big_lcd.RxInfolen=_485_dev_rx[2]+3;     
			//-------------------------- 
			_485dev_wr=0;  // clear	  
			_485_RXstatus._485_receiveflag=IDLE_485;  
			Big_lcd.RX_enable=LCD_BUSY;
		}					
		break;  
		case   CAMERA_Related:
		if(_485dev_wr>=_485_RXstatus._485_RxLen) 
		{
			memset(_485_content,0,sizeof(_485_content));
			//-------------------------------------------------- 					  
			memcpy(_485_content,_485_dev_rx,_485_RXstatus._485_RxLen); 
			_485_content_wr=_485_RXstatus._485_RxLen; // Packet info len 
			_485_CameraData_Enable=1;   // í???êy?Y1yà′á?
			//---------------------------------------------------
			_485dev_wr=0; // clear 
			_485_RXstatus._485_receiveflag=IDLE_485;
		} 
		break; 
		default:
			_485dev_wr=0;  
			_485_RXstatus._485_receiveflag=	IDLE_485; 
		break ;
	} 
	#endif
}


void  Pic_Data_Process(void) 
{
      u8  tmp[40]; 
      u8   pic_buf[600];
	    u16 i=0;
  

           // 1.    Judge   last  package  
            PackageLen=_485_content_wr;
             if(PackageLen<PageSIZE)
			  last_package=1;    	
	    else   
	      if((last_package==0)&&(_485_content[510]==0xFF)&&(_485_content[511]==0xD9))
		  {
		            last_package=1;        
		  }          
	   //  2.    Block ++	    
	      CameraState.block_counter++;
	  //   3.    Check   first   packet
            if(CameraState.create_Flag==1)    // 如果是第一包则创建文件 hhmmss_x.jpg
              {                  
                     CameraState.create_Flag=0; // clear 
					 memset(tmp,0,sizeof(tmp));
					 memset(PictureName,0,sizeof(PictureName));
					 //-----------  创建图片文件处理  -------------					 
					 /*
						 每张图片占32个page    其中第1个page 为图片索引，后边127个Page为图片内容	
						 SST25 开辟个区域做图片缓存 
					 */
                       DF_Read_RecordAdd(pic_write,pic_read,TYPE_PhotoAdd);					 
					 //pic_current_page=(pic_write<<5)+PicStart_offset; //计算图片起始page 
					if(Camera_Number==1) 
					{
					     pic_current_page=PicStart_offset; //起始page 固定为缓存起始地址
					     //擦除一个64K的区域用于图片存储  
					  
					     Api_DFdirectory_Delete(camera_1);
					  
					}
					else
					if(Camera_Number==2) 
					{
						 pic_current_page=PicStart_offset2; //起始page 固定为缓存起始地址
						 //擦除一个64K的区域用于图片存储  					   
					     Api_DFdirectory_Delete(camera_2);
					   ; 	 
					}	
					else
					if(Camera_Number==3) 
					{
						 pic_current_page=PicStart_offset3; //起始page 固定为缓存起始地址
						 //擦除一个64K的区域用于图片存储  
					     Api_DFdirectory_Delete(camera_3);   	 
					}
					else
					if(Camera_Number==4) 
					{
					     pic_current_page=PicStart_offset4; //起始page 固定为缓存起始地址
						 //擦除一个64K的区域用于图片存储  
					     Api_DFdirectory_Delete(camera_4); 	   
					}	
					 DF_delay_ms(150);  
					 WatchDog_Feed(); 
					 pic_current_page++;  // 图片内容从 第二个page 开始 第一个Page 存储的是图片索引 
					 pic_PageIn_offset=0; // 页内偏移清空 
					 pic_size=0; // 清除图片大小
					 //------------------------------------------					 
					 memset(PictureName,0,sizeof(PictureName));    
					 sprintf((char*)PictureName,"%d%d%d-%d.jpg",time_now.hour,time_now.min,time_now.sec,Camera_Number);  	
					 rt_kprintf("\r\n              创建图片名称: %s \r\n                  图片地址: %d ",PictureName,pic_write);   
					 WatchDog_Feed();  
        
		              //----- TF -------		         
				/*if(Udisk_Test_workState==1)	 	
		              {  
                               // creat_file(PictureName); //TF卡创建文件 
                                 rt_kprintf("\r\n   udisk  创建文件成功!");
                                 udisk_fd=open((const char*)PictureName, O_RDWR|O_CREAT, 0);  // 创建U 盘文件
                                 rt_kprintf(" \r\n udiskfile: %s  open res=%d   \r\n",PictureName, udisk_fd);  
		              }  */		              
				 WatchDog_Feed();         
                  // -----    写图片索引 -------
                 Save_MediaIndex(0,PictureName,Camera_Number,0);  					  
             }  
			
	       //  4.   填写存储图片内容数据  --------------------		
	       WatchDog_Feed();  
		   DF_WriteFlashDirect(pic_current_page,0,_485_content, PackageLen);// 写一次一个Page 512Bytes
		   delay_ms(150);   
		   //rt_kprintf(" \r\n ---- write  pic_current_page=%d  \r\n",pic_current_page);   		   
		   rt_kprintf(" \r\n ---- pkg=%d  \r\n",CameraState.block_counter);     
		  
            //---  read compare 
		    memset(pic_buf,0,600);
		    DF_ReadFlash(pic_current_page,0,pic_buf, PackageLen); 
			delay_ms(10);
		    for(i=0;i<PackageLen;i++)
		   {		if(pic_buf[i]!=_485_content[i])
		    	       {
		    	          rt_kprintf(" \r\n ----read not equal write  where i=%d  Rd[i]=%2X  WR[i]=%2X \r\n",i,pic_buf[i],_485_content[i]); 
						  DF_WriteFlashDirect(pic_current_page,0,_485_content, PackageLen);// 再写一次一个Page 512Bytes
		                  delay_ms(100);  
					      break;		  
		    	       }
		   }	 
                      //----- TF -------
		       /*       if((Udisk_Test_workState==1)&&(udisk_fd))
		              {
		                  fd_res=write(udisk_fd,_485_content, PackageLen);
	                         rt_kprintf("\r\n  wr--1 :%s  resualt=%d\r\n",PictureName,fd_res);	
				    if(fd_res<=0)
				        close(udisk_fd);
		              } 
		        */  
				   pic_size+=PackageLen;// 图片大小累加	 				   
				   pic_current_page++; //写一页加一
				  // pic_PageIn_offset+=PackageLen;  
				 //  DF_delay_ms(50);   
	   //   5.   最后一包 ，即拍照结束
		  if(last_package==1)
		 {
			   //f_close(&FileCameraIn);  //  拍照完成关闭图片文件
			   memset(_485_content,0,sizeof(_485_content)); 
			   _485_content_wr=0;			  

			   //-------------  图片拍照结束 相关处理  ------------------------------------
			   //  1. 写图片索引
			   if(Camera_Number==1)
				   pic_current_page=PicStart_offset; //计算图片起始page 
			   else
			   if(Camera_Number==2)
			         pic_current_page=PicStart_offset2; //计算图片起始page 
			   else
			   if(Camera_Number==3)
			         pic_current_page=PicStart_offset3; //计算图片起始page 
			   else
			   if(Camera_Number==4)
			         pic_current_page=PicStart_offset4; //计算图片起始page        
			   PictureName[18]=Camera_Number;
			   memcpy(PictureName+19,(u8*)&pic_size,4);	 			   
			   DF_WriteFlashDirect(pic_current_page,0,PictureName, 23);  
			   DF_delay_ms(8); 
			   
	               //  5.1   更新图片读写记录
	               pic_write++;
				   if(pic_write>=Max_PicNum)
				   	  pic_write=0;    
	               DF_Write_RecordAdd(pic_write,pic_read,TYPE_PhotoAdd);  
	               //--------------------------------------------------------------------------                  
	               rt_kprintf("\r\n        PicSize: %d Bytes\r\n    Camera  %d   End\r\n",pic_size,Camera_Number); 
		        SingleCamra_TakeResualt_BD=0;	    //  单路摄像头拍照		
		        SD_ACKflag.f_BD_CentreTakeAck_0805H=1;  //  发送中心拍照命令应答
		        //----------  Normal process ---------------------
			  End_Camera(); 

				
		        // 5.2   拍照完成后检查有没有多路 拍-----------Multi Take process--------------------
				   if(1==MultiTake.Taking)
				   {
					   switch(Camera_Number)
						 {
							case  1: 
	                                   //-------- old process---------------
	                                   MultiTake.TakeResult[0]=Take_Success;   // 表示第一路摄像头拍照失败
									   //--------- new   process-------------
	                                   MultiTake.Take_retry=0;
	                                   //-------------------------------------------
									   if(CHKendTake_ReadyToSend())// 检查是否是最后的拍照线路
									   	    break;
									   //----------拍照下一路摄像头-----------									   
										   Camera_Number=2;  
									   //-------------------------
									    Start_Camera(Camera_Number);
									 
								    break;
							case  2: 
	                                   //-------- old process---------------
	                                   MultiTake.TakeResult[1]=Take_Success;   // 表示第一路摄像头拍照失败
									   //--------- new   process-------------
	                                   MultiTake.Take_retry=0;
	                                   //-------------------------------------------
									   if(CHKendTake_ReadyToSend())// 检查是否是最后的拍照线路
									   	    break;
									   //----------拍照下一路摄像头-----------									   
										   Camera_Number=3;  
									   //-------------------------
									    Start_Camera(Camera_Number);
									 
								    break;
						   case  3: 
	                                   //-------- old process---------------
	                                   MultiTake.TakeResult[2]=Take_Success;   // 表示第一路摄像头拍照失败
									   //--------- new   process-------------
	                                   MultiTake.Take_retry=0;
	                                   //-------------------------------------------
									   if(CHKendTake_ReadyToSend())// 检查是否是最后的拍照线路
									   	    break;
									   //----------拍照下一路摄像头-----------									   
										   Camera_Number=4;   
									   //-------------------------
									    Start_Camera(Camera_Number);
									 
								    break;				
							case  4: 
	                                   //-------- old process---------------
	                                   MultiTake.TakeResult[3]=Take_Success;   // 表示第一路摄像头拍照失败
									   //--------- new   process-------------
	                                   MultiTake.Take_retry=0;
									   
	                                   //-------------------------------------------
									   if(CHKendTake_ReadyToSend())// 检查是否是最后的拍照线路
									   	    break;
									
								    break;					
						    default:
								    MultiTake_End();
	                                                     break;
								  
					   	 }		

					}		
				   else
				   if((0==MultiTake.Taking)&&(0==MultiTake.Transfering)) 
				    {    
				       //------ 判断车门开关拍照是否上传状态 ------
					    if((OpenDoor_StartTakeFlag==1)&&(Opendoor_transFLAG==0)) 
			                    {
			                         MultiTake_End();   // 车门开关不上传
			                         OpenDoor_StartTakeFlag=0;
							    }     
			                   else
			                   	{
									rt_kprintf("\r\n Single Camera !\r\n"); 
									if(Camera_Take_not_trans==0)  
											Photo_send_start(Camera_Number);  //在不是多路拍照的情况下拍完就可以上传了
									 else
									   Camera_Take_not_trans=0;					   
			                   	}
				   	}	
				    //  拍照结束  
	    }
	 else
	  {
	     
		 //------- change state  -------
		 CameraState.status=transfer;
		 CameraState.OperateFlag=0;   // clear 
	  
		  TX_485const_Enable=1;  // 发送485 命令
		   _485_RXstatus._485_receiveflag=IDLE_485;   
                 //rt_kprintf("\r\n  Head info_len : %d\r\n",_485_content_wr); 
		  memset(_485_content,0,sizeof(_485_content));
		  _485_content_wr=0;			  
		  //rt_kprintf("\r\n  One Packet Over!\r\n"); 	 
		 
	  } 			  
}


static rt_err_t   Device_485_init( rt_device_t dev )
{
      GPIO_InitTypeDef  GPIO_InitStructure;
      USART_InitTypeDef USART_InitStructure;    
     NVIC_InitTypeDef NVIC_InitStructure;	  


       //  1 . Clock
 		  
	/* Enable USART2 and GPIOA clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Enable USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

      //   2.  GPIO    
       GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;   
	GPIO_Init(GPIOA, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); 

     //  3.  Interrupt
        	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
    //   4.  uart  Initial
       USART_InitStructure.USART_BaudRate = 57600;  //485
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);      

	    /* -------------- 485  操作相关 -----------------	*/
	    /*
			   STM32 Pin	 		   Remark
				 PC4		  		    485_Rx_Tx 控制   0:Rx    1: Tx
				 PD0		  		    485 电源	1: ON  0:OFF
		*/
	  
	   /*  管脚初始化 设置为 推挽输出 */

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	 // ------------- PD10     --------------------------------
	GPIO_InitStructure.GPIO_Pin  =GPIO_Pin_8;		//--------- 485 外设置的电  
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
    Power_485CH1_ON;  // 第一路485的电	       上电工作      

	 // Power_485CH1_OFF;  
	 
 //------------------- PC4------------------------------	
	 GPIO_InitStructure.GPIO_Pin  =GPIO_Pin_4;				 //--------- 485const	收发控制线 
	 GPIO_Init(GPIOC, &GPIO_InitStructure); 
	 RX_485const;  
	 return RT_EOK;
}

static rt_err_t Device_485_open( rt_device_t dev, rt_uint16_t oflag )  
{
         return RT_EOK;
}
static rt_err_t Device_485_close( rt_device_t dev )
{
        return RT_EOK;
}
static rt_size_t Device_485_read( rt_device_t dev, rt_off_t pos, void* buff, rt_size_t count )
{

        return RT_EOK;
}
static rt_size_t Device_485_write( rt_device_t dev, rt_off_t pos, const void* buff, rt_size_t count )
 {
	
	 unsigned int  Info_len485=0;
	 const char		*p	= (const char*)buff;
	

	Info_len485=(unsigned int)count;
	
    	/* empty console output */
		//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		//USART_ClearFlag(USART2, USART_IT_RXNE);
	TX_485const; 
	_485_delay_ms(1);
		//--------  add by  nathanlnw ---------
  while (Info_len485)
	{
		rt_hw_485_putc (*p++);   
		Info_len485--;
	}
       //--------  add by  nathanlnw  --------	
       _485_delay_ms(5);
	   
       RX_485const; 
	   //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
        return RT_EOK;
  }
//FINSH_FUNCTION_EXPORT_ALIAS(Device_485_write,W_485, buzzer_onoff[1|0]); 

static rt_err_t Device_485_control( rt_device_t dev, rt_uint8_t cmd, void *arg )
{
       return RT_EOK;
 }

/* init 485 */
void _485_startup(void)
{

	Device_485.type	= RT_Device_Class_Char;
	Device_485.init	= Device_485_init;
	Device_485.open	=  Device_485_open;
	Device_485.close	=  Device_485_close;
	Device_485.read	=  Device_485_read;
	Device_485.write	=  Device_485_write;
	Device_485.control =Device_485_control;

	rt_device_register( &Device_485, "485", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE );
	rt_device_init( &Device_485 );
	
}


