#ifndef __YEELINK_HANDLE_H_
#define __YEELINK_HANDLE_H_




//����
#define YEELINK_SERVER      "api.yeelink.net"

//API KEY
#define YEELINK_APIKEY      "af735029b2e431481dd0e6ef5bee6a27"

//�豸����
#define Control_LED         "196272"
    //�豸�´�����ID��
    #define KEY_ID          "214772"
	#define LED2_ID         "241611"

//�������������
#define AirTest_ID      "339895"
   //��������ָ��
   #define  AQI_ID      "376376"
   //�۳�Ũ��
   #define  Dust_ID     "376377"
   //����ʪ��
   #define  Humidity_ID "376378"
   //��ȩŨ��
   #define  Methanal_ID  "376382"
   //�����¶�
   #define  Temp_ID      "376383"

typedef struct{
   float C_LOW,C_HIGH;
   int   I_LOW,I_HIGH;
}AQI_DATA;

typedef struct{
    int Humidity;
	int Methanal;
	int Temperature;
	int Dust;
	int AQI;
}SENSOR_DATA;



typedef struct  _XPG_GLOBALVAR
{

    char http_sockettype; //yeelink��������

	char a_mac[13]; //MAC��ַ

#define LED_ON   1
#define LED_OFF  0
	char led_State;//LED״̬

    
}XPG_GLOBALVAR;



//LED ON
#define LED2_ON()  BSP_GPIOSetValue(USER_GPIO_IDX_LED, GPIO_LOW_LEVEL)
//LED OFF
#define LED2_OFF() BSP_GPIOSetValue(USER_GPIO_IDX_LED, GPIO_HIGH_LEVEL)

//LED ON
#define LED3_ON()  BSP_GPIOSetValue(USER_GPIO_IDX_LED3, GPIO_LOW_LEVEL)
//LED OFF
#define LED3_OFF() BSP_GPIOSetValue(USER_GPIO_IDX_LED3, GPIO_HIGH_LEVEL)

//Launch directconfig mode
#define KEY2_IN()  BSP_GPIOGetValue(USER_GPIO_DIRECTCONFIG)


/* zhou mask for debug */
#if 0
#define Yeelink_Printf(level, fmt, args...)
#else
#define Yeelink_Printf(level, fmt, args...)  printf(fmt, ##args);printf("\r\n")
#endif



extern void Yeelink_Init(void);
extern VOID Yeelink_Run(VOID *arg);
int GetSensorData(void);


#endif

