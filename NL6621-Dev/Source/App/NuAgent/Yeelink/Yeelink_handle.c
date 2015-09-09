#include "common.h"
#include <stdlib.h>

SENSOR_DATA   sensordata;
XPG_GLOBALVAR g_Yeelink_GlobalVar; //Yeelinkȫ�ֲ���
extern char * send_buf;
extern unsigned char TCPClient_reset_flag;


void Yeelink_initGPIO(void)
{
    //GPIO09
	BSP_GPIOPinMux(USER_GPIO_IDX_LED);		    /* led indicator */
	BSP_GPIOSetDir(USER_GPIO_IDX_LED, 1);		/* output */
	BSP_GPIOSetValue(USER_GPIO_IDX_LED, 1);	    /* low level */

	//GPIO30
	BSP_GPIOPinMux(USER_GPIO_IDX_LED3);		    /* led indicator */
	BSP_GPIOSetDir(USER_GPIO_IDX_LED3, 1);		/* output */
	BSP_GPIOSetValue(USER_GPIO_IDX_LED3, 1);	    /* low level */
	

	/* DirectConfig gpio is valied when set to low level */
	BSP_GPIOPinMux(USER_GPIO_DIRECTCONFIG);  
	BSP_GPIOSetDir(USER_GPIO_DIRECTCONFIG, GPIO_DIRECTION_INPUT); 
}

//��ʼ��yeelink������
void Yeelink_Global_Status_Init(void)
{
	/* Initialize Light resource */
	g_Yeelink_GlobalVar.http_sockettype = 0;

	g_Yeelink_GlobalVar.led_State = LED_OFF;
}

//yeelink��ʼ����GPIO,������ȡ�ȵ�
void Yeelink_Init(void)
{
	Yeelink_initGPIO();

    Yeelink_Global_Status_Init();
 
    return;
}

char* yeelink_post(const char *device_id,const char *sensors_id, int value)
{
    char str_tmp[128] = {0};
    
    // Http���ݣ�������
    char http_content[32] = {0};


    memset(send_buf,0,CLOUD_SEND_BUFFER_SIZE);

    sprintf(str_tmp,"/v1.0/device/%s/sensor/%s/datapoints",device_id,sensors_id);
    // ȷ��HTTP���ύ���� {"value":20}

    sprintf( http_content , "{\"value\":%d}" , (int)value);	


    // ȷ�� HTTP�����ײ�
    // ����POST /v1.0/device/98d19569e0474e9abf6f075b8b5876b9/1/1/datapoints/add HTTP/1.1\r\n
    sprintf( send_buf , "POST %s HTTP/1.1\r\n",str_tmp);
    // �������� ���� Host: api.machtalk.net\r\n
    sprintf( str_tmp , "Host:%s\r\n" , YEELINK_SERVER);
    strcat( send_buf , str_tmp);

    // �������� ���� APIKey: d8a605daa5f4c8a3ad086151686dce64
    sprintf( str_tmp , "U-ApiKey:%s\r\n" , YEELINK_APIKEY);//�Լ���APIKey
    strcat( send_buf , str_tmp);
    //
    strcat( send_buf , "Accept: */*\r\n");
    // �����ύ�����ݵĳ��� ���� Content-Length:12\r\n
    sprintf( str_tmp , "Content-Length:%d\r\n" ,strlen(http_content) );
    strcat( send_buf , str_tmp);
    // ���ӱ������ʽ Content-Type:application/x-www-form-urlencoded\r\n
    strcat( send_buf , "Content-Type: application/x-www-form-urlencoded\r\n");
    strcat( send_buf , "Connection: close\r\n");
    // HTTP�ײ���HTTP���� �ָ�����
    strcat( send_buf , "\r\n");
    // HTTP��������
    strcat( send_buf , http_content);

    return send_buf;
}



char* yeelink_get(const char *device_id,const char *sensors_id,char *value)
{
    char str_tmp[128] = {0};

    memset(send_buf,0,CLOUD_SEND_BUFFER_SIZE);

    sprintf(str_tmp,"/v1.0/device/%s/sensor/%s/datapoints",device_id,sensors_id);
    // ȷ�� HTTP�����ײ�
    // ����POST /v1.0/device/98d19569e0474e9abf6f075b8b5876b9/1/1/datapoints/add HTTP/1.1\r\n
    sprintf( send_buf , "GET %s HTTP/1.1\r\n",str_tmp);
    // �������� ���� Host: api.machtalk.net\r\n
    sprintf( str_tmp , "Host:%s\r\n" , YEELINK_SERVER);
    strcat( send_buf , str_tmp);
 
    // �������� ���� APIKey: "af735029b2e431481dd0e6ef5bee6a27"
    sprintf( str_tmp , "U-ApiKey:%s\r\n" , YEELINK_APIKEY);//��Ҫ�滻Ϊ�Լ���APIKey
    strcat( send_buf , str_tmp);
    //
    strcat( send_buf , "Accept: */*\r\n");
    // ���ӱ������ʽ Content-Type:application/x-www-form-urlencoded\r\n
    strcat( send_buf , "Content-Type: application/x-www-form-urlencoded\r\n");
    strcat( send_buf , "Connection: close\r\n");
    // HTTP�ײ���HTTP���� �ָ�����
    strcat( send_buf , "\r\n");

    return send_buf;
}


VOID Yeelink_Run(VOID *arg)
{
   int ret;
   char *buffer = NULL;

   while(1){
	   if(TCPClient_reset_flag == 1)
	   {  
 	       //�õ�������������ֵ
	   	   GetSensorData();

	       //�۳�Ũ��    376377
		   buffer = yeelink_post(AirTest_ID,Dust_ID,sensordata.Dust);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));

	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Dust=>Send failed(ret:%d).\n", ret);
	       } else {
		      TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Dust=>Send success.\n");
	       }	
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);


		   //�����¶�    376378
		   buffer = yeelink_post(AirTest_ID,Temp_ID,sensordata.Temperature);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Temperature=>Send data failed(ret:%d).\n", ret);
	       } else {
		   	  TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Temperature=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(10);
		   OSTimeDly(100);	
	  
		  //��ȩŨ��    376382
		   buffer = yeelink_post(AirTest_ID,Methanal_ID,sensordata.Methanal);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Methanal=>Send data failed(ret:%d).\n", ret);
	       } else {	
		   	  TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Methanal=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);

		  //����ʪ��    376383
		   buffer = yeelink_post(AirTest_ID,Humidity_ID,(int)sensordata.Humidity);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"Humidity=>Send data failed(ret:%d).\n", ret);
	       } else {
			  TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"Humidity=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);

		  //��������ָ�� 376376
		   buffer = yeelink_post(AirTest_ID,AQI_ID,sensordata.AQI);
           ret = Socket_TCPClientSendData(buffer,strlen(buffer));
	       if (ret <= 0) {
	          Yeelink_Printf(GAGENT_WARNING,"AQI=>Send data failed(ret:%d).\n", ret);
	       } else {
		      TCPClient_reset_flag = 2;
	          Yeelink_Printf(GAGENT_WARNING,"AQI=>Send data success.\n");
	       }		
		   while(TCPClient_reset_flag != 1)
		      OSTimeDly(100);
	   }
  	   OSTimeDly(100); 	   
   }
}

//�������������
int make_rand(int start, int end)
{
	int value = 0;
	if (end <= start) 
		return -1;
	srand(OSTimeGet());
	value = ((rand() % (end - start + 1)) + start);

	return value;
}

AQI_DATA  aqi_data[7] = {0,     15.4,   0,   50,  //��
                         15.5,  40.4,  51,  100,  //��
						 40.5,  65.4,  101, 150,  //�����Ⱦ
						 65.5,  150.4, 151, 200,  //�ж���Ⱦ
						 150.5, 250.4, 201, 300,  //�ض���Ⱦ
						 250.5, 350.4, 301, 400,  //������Ⱦ
						 350.5, 500.4, 401, 500,  //�ж���     
						 };


//����AQIֵ
static int Calculation_AQI(int _dust)
{
	int aqi;
	char i;
	float  dust = (float)_dust;

	for(i=0;i<7;i++)
	{
	   //�ж���ȾŨ�ȷ�Χ
	   if(aqi_data[i].C_LOW <= dust && dust <= aqi_data[i].C_HIGH)
	   {
	       aqi = (int)((aqi_data[i].I_HIGH - aqi_data[i].I_LOW)/(aqi_data[i].C_HIGH - aqi_data[i].C_LOW)*(dust - aqi_data[i].C_LOW));

		   aqi += aqi_data[i].I_LOW;
	   
	       return aqi;
	   }
	}

	return 	aqi_data[6].I_HIGH;
}

//�õ��۳�Ũ��
static int GetDustData(void)
{
	int Dust;
	
	Dust = make_rand(5, 500);
	Yeelink_Printf(GAGENT_WARNING, "Dust:%d\n", Dust);
	
	return Dust;
}

//�õ������¶�
static int GetTemData(void)
{
    int temp;

	temp = make_rand(-20, 40);
    Yeelink_Printf(GAGENT_WARNING, "Temperature:%d\n", temp);
	
	return temp;
}
//�õ�����ʪ��
static int GetHumData(void)
{
    int Hum;

	Hum = make_rand(0, 100);
	Yeelink_Printf(GAGENT_WARNING, "Humidity:%d\n", Hum);
	
	return Hum;
}


//�õ���ȩŨ��
static int GetMethanalData(void)
{
    int methanal;

	methanal = make_rand(1, 1000);
    Yeelink_Printf(GAGENT_WARNING, "Methanal:%d\n", methanal);
	
	return methanal;
}

//��������AQI
static int GetAQIData(void)
{
    int aqi;

	aqi = Calculation_AQI(sensordata.Dust);
	Yeelink_Printf(GAGENT_WARNING,"AQI:%d\n", aqi);
	
	return aqi;
}

//�õ�����������
int GetSensorData(void)
{
    sensordata.Humidity = GetHumData();
    sensordata.Dust = GetDustData();
	sensordata.Temperature = GetTemData();
	sensordata.Methanal = GetMethanalData();
	sensordata.AQI = GetAQIData();

    return 0;
}







