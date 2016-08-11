#include "rec.h" 
#include "Rs485.h"
 
PicInfo gPicInformation;	
FileInfoStruct_Flash flashFileInfo;
u8 Target_ID=0;
u8 gPkgIndex=0;
u8 gFileRxPkgCount=0;
u16 gFileRxLastPkgLength=0;
u32 file_byte = 0;
u32 rec_byte = 0;
u32 each_byte = 0;

char sndback = FALSE;
char keepres = 0;
char finished = FALSE;
u8 status;
u16 waittime = 0;
u8 err_cnt=0;

void CAN_Process(void)
{
	u8 res=0;
	u8 resp[8] =      {0x52,0x65,0x63,0x65,0x69,0x76,0x65};			//R, e, c, e, i, v, e
  u8 resFailed[8] = {0x46,0x61,0x69,0x6C,0x65,0x64,0x4D};//F  a	i  l  e  d	M
	u8 resWating[8] = {0x57,0x61,0x69,0x74,0x69,0x6e,0x69};//Waiting
	u8 canresp[8] =   {'J','a','n','e','S','U','S'};		      //Receive   		//46,61,69,6C,65,64,4D	
	u8 canonoff[8] = {0x52,0x65,0x61};
	u8 canbuf[8];
	u8 key;   //
	u8 tmpchr;//
	
	if(keepres)	 //��������
	{
		if(sndback)
		{
			if (waittime<50)	
			{
				res = Can_Send_Msg(resp,3);
			}
			else	
			{
				res = Can_Send_Msg(resFailed,3);//����3���ֽ�
				waittime=0;	err_cnt++;Delay_mS(40);
				if (err_cnt>15)		
				{
					err_cnt=0;	finished = TRUE; keepres = FALSE; status = 0x61;	//'a'
				}
			}
			sndback = FALSE;
		}
		key=Can_Receive_Msg(canbuf,gDeviceCANAddr);
		if (key)
		{
			CAN_LED_Tog();
			waittime = 0; err_cnt=0;
			jcopy(canbuf, key, each_byte);
			rec_byte+=key;
			each_byte+=key;
			if (each_byte>=1017)//�����յ���1K�ֽڴ����ļ�ϵͳ��
			{
				if(gPkgIndex+1<gFileRxPkgCount)
				{
					jWriteFlashC(&flashFileInfo,each_byte);
					flashFileInfo.F_Start+=each_byte;
					gPkgIndex++;
				}
				each_byte = 0; 	sndback = TRUE;
			}
			if (rec_byte>=file_byte) 	
			{
				//�����ļ��ɹ�
				finished = TRUE; keepres = FALSE;
			}
		} 
		else {			 //���û����һ��1024������ʱ������	 waittime+1��
			waittime++;Delay_mS(10);
			if (waittime>200)	{
				if (each_byte>0) rec_byte-=each_byte; 
				each_byte=0;  sndback = TRUE;
			}
		}
	}	
	else 	//�������������ļ�ʱ
	{
		if (finished)	//����ͼƬ�ļ��ɹ�����
		{
				if(gPkgIndex+1==gFileRxPkgCount)
				{
					jWriteFlashC(&flashFileInfo,each_byte);
					flashFileInfo.F_Start = FileInfo_PIC1;			
					gPkgIndex++;
					finished = FALSE; status = 'e';	sndback = TRUE;
				}
				else
				{
					finished = FALSE; sndback = FALSE;  status = 'a';	
				}
		}
		if (sndback)
		{
			if ((status==0x6B)&&(sndback)) //'k' Ask for Reply		  //������ذ��������
			{
				res=Read_Ir(Target_ID);
				resWating[7]=res;
				res = Can_Send_Msg(resWating,8);//����7���ֽ� //Waiting				
				sndback = FALSE;  status = 'a';	
			}
			else if ((status==0x73)&&(sndback))//'s'��ʼ����ͼƬ���� 
			{
				flashFileInfo.F_Start = PIC1_Info;
				flashFileInfo.F_Size = sizeof(gPicInformation);
				F_Open_Flash(&flashFileInfo);
				
				gPicInformation.PicSize=(canbuf[4]<<24)+(canbuf[5]<<16)+(canbuf[6]<<8)+(canbuf[7]);
				gPicInformation.x = 0;
				gPicInformation.y = 0;
				gPicInformation.height =240;
				gPicInformation.weith = 400;
				
        Save_Pic_Size(PIC1_Info,gPicInformation);
				
				flashFileInfo.F_Start = FileInfo_PIC1;
				flashFileInfo.F_Size = gPicInformation.PicSize;
				gFileRxPkgCount=gPicInformation.PicSize/1024;
				gFileRxLastPkgLength=gPicInformation.PicSize%1024;
				if(gFileRxLastPkgLength!=0)gFileRxPkgCount+=1;
				else gFileRxLastPkgLength=1024;
				
				res = Can_Send_Msg(resp,3);//����3���ֽ�
				sndback = FALSE;
				if (rec_byte==0)
				{
					each_byte = 0; keepres = TRUE;	   //��ʼ���������ļ�
				}
			}
			else if ((status==0x65)&&(sndback)) //'e'����ͼƬ����
			{
				res = Can_Send_Msg(canresp,3);
				sndback = FALSE;
				clearjpgbuff();
				Update_Pic(Target_ID);
				status = 'a'; each_byte=0; 	rec_byte=0;
			}
		}
		else
		{
			//if(status==0x61)	////////'a'	
			//{	
				key=Can_Receive_Msg(canbuf,gDeviceCANAddr);
				if(key)//���յ�������
				{
					CAN_LED_Tog();
					if ((canbuf[0]==0x4A)
						&&(canbuf[1]==0x61)
						&&(canbuf[2]==0x73)
						&&((canbuf[3]>0x2F)&&(canbuf[3]<0x70)))	//��ʼ����ͼƬ�ļ�4A 61 73 6D 69 6E 65
					{
						file_byte=(canbuf[4]<<24)+(canbuf[5]<<16)+(canbuf[6]<<8)+(canbuf[7]);
						tmpchr = canbuf[3];
						if(tmpchr>0x2F && tmpchr<0x50)
						{
							Target_ID = tmpchr - 0x30;
							canonoff[3]=TRUNONOFF_LCD(Target_ID,CMD_TRUNON_LCD);
				      res = Can_Send_Msg(canonoff,4);
						}
						else 
						{
							Target_ID = tmpchr - 0x50;
							canonoff[3]=TRUNONOFF_LCD(Target_ID,CMD_TRUNOFF_LCD);
							res = Can_Send_Msg(canonoff,4);
						}
						if(file_byte>0){each_byte = 0;rec_byte = 0;gPkgIndex = 0;status = 's'; sndback = TRUE;}
					} 
					else if ((canbuf[0]==0x4A)
						&&(canbuf[1]==0x61)
						&&(canbuf[2]==0x61)
						&&(canbuf[3]==0x73)) //"Jaas mean asking for Reply"
					{
						  Target_ID = canbuf[4];
							status = 'k'; sndback = TRUE;
					}
				}
			//}
		}
	}
}
