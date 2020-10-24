/*******************************************************************************
*            开发环境 IAR7.2 for msp430f5529
*            运行平台 msp430f5529
*-------------------------------------------------------------------------------
* LMT70               直接使用的ADC 引脚  P6.1  
*-------------------------------------------------------------------------------
* MLX90614            I2C0:  SDA      SCL
*                            P3.0     P3.1
*                            P4.1     P4.2
*-------------------------------------------------------------------------------
* OLED(本程序使用LCD)   D0    D1    RES    DC
*                      P4.3  P4.0  P3.7   P8.2
*-------------------------------------------------------------------------------
* UCA1                 TX    RX
*                      P44   P45
*-------------------------------------------------------------------------------
* LCD                  SCL   SDI   CS    RST   DC
*                      P3.2  P2.2  P2.7  P2.4  P2.0
*
* 引脚可根据需求设置
*-------------------------------------------------------------------------------
* LMT70数据的采集主要在 LMT70.c 与 LMT70.h 中 
* MLX90614数据的采集主要在 msp430f5529_i2c.c 末尾处
* 滤波算法主要在 Filter.c 与 Filter.h 中
********************************************************************************/

#include "include.h"
#include "stdio.h"

//温度阈值
#define MIN_Temp 30.0
#define MAX_Temp 46.0

//初始温度阈值
float Threshold_temp = 32.0;

//锁定温度显示数值
float Lcd_stop_temp = 0.0;

//以下参数用于串口接收数据
char databuf[256];
static int num = 0;

//勿改 可能引发玄学错误
uint16_t data=1000;
float fdata=1.0;

//勿改 可能引发玄学错误
__interrupt void TIMER1_A0_IRQ_Handler()
{
   GPIO_TurnBit  (P4, 7);
   data++;  
   fdata+=0.1;
}

//串口
__interrupt void UART_RX_IRQ_Handler()
{ 
  if(UART_GetITStatus(UART1,UART_RX_IRQn) == TRUE)   //清除串口某一个中断标志
  {
    //传输速率高时，接收中断里不要做其他事件，否则会丢失数据，另外主时钟也要设高一点
    databuf[num]=UART_GetChar     (UART1);                 //读取一个字节1个字节
    num++;
    UART_ClearITPendingBit(UART1,UART_RX_IRQn);    //清除串口某一个中断标志
  }
}

//功能按键初始化
void function_key()
{
  KEY_Init(KEY1,NULL); 	                //按键初始化函数 P2.1  Threshold_temp ++
  KEY_Init(KEY2,NULL); 	                //按键初始化函数 P1.1  Threshold_temp --
  KEY_Init(KEY3,NULL); 	                //按键初始化函数 P8.1  串口通信 字符串1 识别人脸
  KEY_Init(KEY4,NULL); 	                //按键初始化函数 P6.2  KEY4 串口通信 字符串2 学习人脸
  KEY_Init(KEY5,NULL); 	                //按键初始化函数 P6.3
  //KEY_Init(KEY6,NULL); 	        //按键初始化函数 P6.4
}

//温度报警
void Temp_warn()
{
  if(Mlx90614_Filter() > Threshold_temp)
  {
    GPIO_ResetBits  (PORT6, GPIO_Pin_0);     //输出高电平 蜂鸣器响
  }
  if(Mlx90614_Filter() <= Threshold_temp)
  {
    GPIO_SetBits(PORT6, GPIO_Pin_0);         //输出低电平 蜂鸣器不响
  }
}

//激光准星
void JGZX()
{
  GPIO_ResetBits  (PORT7, GPIO_Pin_0);
  DELAY_MS(400);
  GPIO_SetBits(PORT7, GPIO_Pin_0);
}

//按键操作
void Key_set()
{
  //KEY1增加阈值 Threshold_temp
     if(KEY_isPressed(KEY1) && (Threshold_temp < MAX_Temp) && (Threshold_temp >= MIN_Temp))
    {
      Threshold_temp ++;
    }
    //KEY2减小阈值 Threshold_temp
    if(KEY_isPressed(KEY2) && (Threshold_temp <= MAX_Temp) && (Threshold_temp > MIN_Temp))
    {
      Threshold_temp --;
    }
    //KEY3 串口通信 字符串1 识别人脸
    if(KEY_isPressed(KEY3))
    {
      UART_Printf   (UART1,"1");
      LCD_Printf_8x16(0,55,"Judge Face&Mask!");
      //OLED_PrintfAt(FONT_ASCII_8X16,6,0,"Judge Face&Mask!");
      
    }
    //KEY4 串口通信 字符串2 学习人脸
    if(KEY_isPressed(KEY4))
    {
     UART_Printf   (UART1,"2");
     LCD_Printf_8x16(0,55,"Learning Faces!  ");
     //OLED_PrintfAt(FONT_ASCII_8X16,6,0,"Learning Faces!  ");
    }
    /********************************************
    //KEY5 串口通信 字符串3 判断是否戴口罩
    if(KEY_isPressed(KEY5))
    {
     UART_Printf   (UART1,"3");
     OLED_PrintfAt(FONT_ASCII_8X16,6,0,"Judge Mask!      ");
    }
    ********************************************/
    //KEY5 按下后锁定显示
    if(KEY_isPressed(KEY5))
    {
      Lcd_stop_temp = Mlx90614_Filter();
      LCD_Printf_8x16(0,87,"Temp: %.1f C",Lcd_stop_temp);       //显示MLX90614温度数据
      JGZX();
      //OLED_DispChineseAt(FONT_CHINESE_16X16,2,0,"温度");          //39
      //OLED_PrintfAt(FONT_ASCII_8X16,2,32,":%.1f",Oled_stop_temp);//显示MLX90614温度数据
      //OLED_DispChineseAt(FONT_CHINESE_16X16,2,72,"℃");
    }
}

//蜂鸣器响一下
void FMQS()
{
  //蜂鸣器
  GPIO_ResetBits  (PORT6, GPIO_Pin_0);     //输出高电平 蜂鸣器响
  DELAY_MS(200);
  GPIO_SetBits(PORT6, GPIO_Pin_0);         //输出低电平 蜂鸣器不响
}

//串口接收open mv数据
void open_mv_data()
{
  //戴口罩    databuf[0] = '6'
  //不戴口罩  databuf[0] = '5'
  //UART_Printf   (UART1," Hello-CTGU!\n UART-TEST!\n");  //串口发送数据
  if(num > 0)
  {
    //OLED_PrintfAt(FONT_ASCII_8X16,0,0,"%s",databuf);    //将串口接收的数据显示出来 注意接收与显示的数据均是字符串均是字符串
    if(databuf[0] == '5')
    {
      LCD_Printf_8x16(0,103,"NO MASK!!!      ");
    }
    if(databuf[0] == '6')
    {
      LCD_Printf_8x16(0,103,"MASK YES        ");
    }
    if(databuf[0] == '-')
    {
      LCD_Printf_8x16(0,103,"%s",databuf);      ////将串口接收的数据显示出来 注意接收与显示的数据
      GPIO_ResetBits  (PORT6, GPIO_Pin_0);     //输出高电平 蜂鸣器响
      DELAY_MS(6000);
      GPIO_SetBits(PORT6, GPIO_Pin_0);         //输出低电平 蜂鸣器不响
    }
    if(databuf[0] == 'P' || databuf[0] == 'S' || databuf[0] == 'N')
    {
      FMQS();
      LCD_Printf_8x16(0,103,"%s",databuf);      ////将串口接收的数据显示出来 注意接收与显示的数据
      DELAY_MS(5000);
    }
    for(int i=0;i < num;i++)
    {
      databuf[i] = 0;
    }
    num = 0;
  }
}

//绘制小熊
void Little_bear()
{
  LCD_Draw_Circle(44,47,5,WHITE);           //耳朵
  LCD_Draw_Circle(84,47,5,WHITE);           //耳朵
  LCD_Draw_Circle(64,26,20,WHITE);          //脸
  //LCD_Draw_Circle(64,28,18,WHITE);          //画圆
  LCD_Draw_Dot(54,32,WHITE);
  LCD_Draw_Dot(55,32,WHITE);
  LCD_Draw_Dot(56,32,WHITE);
  LCD_Draw_Dot(53,32,WHITE);
  LCD_Draw_Dot(52,32,WHITE);
  
  LCD_Draw_Dot(54,31,WHITE);
  LCD_Draw_Dot(55,31,WHITE);
  LCD_Draw_Dot(56,31,WHITE);
  LCD_Draw_Dot(53,31,WHITE);
  LCD_Draw_Dot(52,31,WHITE);
  
  LCD_Draw_Dot(74,32,WHITE);
  LCD_Draw_Dot(73,32,WHITE);
  LCD_Draw_Dot(72,32,WHITE);
  LCD_Draw_Dot(71,32,WHITE);
  LCD_Draw_Dot(70,32,WHITE);
  
  LCD_Draw_Dot(74,31,WHITE);
  LCD_Draw_Dot(73,31,WHITE);
  LCD_Draw_Dot(72,31,WHITE);
  LCD_Draw_Dot(71,31,WHITE);
  LCD_Draw_Dot(70,31,WHITE);
  
  LCD_Draw_Line(93,28,93,40,WHITE);         //惊叹号
  LCD_Draw_Line(94,28,94,40,WHITE);
  LCD_Draw_Dot(93,20,WHITE);
  LCD_Draw_Dot(93,21,WHITE);
  LCD_Draw_Dot(94,20,WHITE);
  LCD_Draw_Dot(94,21,WHITE);
  
  LCD_Draw_Line(102,28,102,40,WHITE);
  LCD_Draw_Line(103,28,103,40,WHITE);
  LCD_Draw_Dot(102,20,WHITE);
  LCD_Draw_Dot(102,21,WHITE);
  LCD_Draw_Dot(103,20,WHITE);
  LCD_Draw_Dot(103,21,WHITE);

  LCD_Draw_Line(111,28,111,40,WHITE);
  LCD_Draw_Line(112,28,112,40,WHITE);
  LCD_Draw_Dot(111,20,WHITE);
  LCD_Draw_Dot(111,21,WHITE);
  LCD_Draw_Dot(112,20,WHITE);
  LCD_Draw_Dot(112,21,WHITE);
  
  LCD_Draw_Line(60,16,68,16,WHITE);         //嘴巴
}

//主程序
void main()
{
  DisableInterrupts();            //禁止总中断
  SMCLK_OUT();
  LED_Init(LED_ALL);              //LED灯初始化
  LCD_INIT();                     //LCD初始化   128 160
  //OLED_Init();                  //OLED初始化
  
  Set_Vector_Handler(VECTOR_TIMER_A1_CCR0, TIMER1_A0_IRQ_Handler);    //设置定时器TA1的中断服务程序为：TIMER1_A0_IRQ_Handler
  TIMER_Interval_Ms(TIMER_A1, 500);                                   //设置TA1为定时中断，中断时间为500Ms
  TIMER_ITConfig (TIMER_A1, TIMER_CCR0_IRQn, ENABLE);                 //使能TIMER的某一个中断
  
  //以下为用户需求部分初始化
  
  //ADC_Init(ADC_CH1,ADC_VREF_2_5V,ADC_12bit);       //初始化ADC通道,P6.1
  I2C_MasterInit   (I2C0, 0x00,50000);            //初始化I2C0模块为主机
  UART_Init        (UART1,115200);                              //初始化UART1模块,波特率115200，波特率高时最好把主时钟 设高些
  Set_Vector_Handler(VECTOR_UART1,UART_RX_IRQ_Handler);         //设置接收中断向量
  UART_ITConfig  (UART1,UART_RX_IRQn,TRUE);                     //开串口接收中断
  function_key();                               //功能按键初始化
  GPIO_MultiBits_Init(P6,BIT0,GPO);             //蜂鸣器 P6.0
  GPIO_MultiBits_Init(P7,BIT0,GPO);             //激光准星
  GPIO_SetBits(PORT7, GPIO_Pin_0);
  //GPIO_ResetBits  (PORT6, GPIO_Pin_0);        //输出高电平 蜂鸣器响
  GPIO_SetBits(PORT6, GPIO_Pin_0);              //输出低电平 蜂鸣器不响
  
  Little_bear();         //绘制小熊
  LCD_Show_Picture(34,119,93,159,logxx);    //显示电赛log 127 159
  
  
  EnableInterrupts();
  while(1)
  {
     Key_set();           //按键操作
     Temp_warn();         //温度报警
     open_mv_data();      //串口接收open mv数据
     //OLED_PrintfAt(FONT_ASCII_6X8,0,0,"LMT70: %.3f C",Lmt70_Filter());       //显示LMT70温度数据
     //OLED_PrintfAt(FONT_ASCII_8X16,2,0,"Temp:%.2f C",Mlx90614_Filter());     //显示MLX90614温度数据
     
     //LCD_Printf_8x16(0,103,"%s",databuf);//显示一个字符串 一行最多16个字符
     //LCD_Printf_8x16(0,103,"People 3 Mask OK");
     
     LCD_Printf_8x16(0,87,"Temp: ");            //温度
     LCD_Printf_8x16(0,71,"Threshold: %.0f C",Threshold_temp);    //显示温度阈值
     //LCD_Printf_8x16(0,55,"Judge Face&Mask!");
     
     //OLED_DispChineseAt(FONT_CHINESE_16X16,4,0,"阈值");
     //OLED_PrintfAt(FONT_ASCII_8X16,4,32,":%.0f",Threshold_temp);     //显示温度阈值
     
     DELAY_MS(200);
  }
}
