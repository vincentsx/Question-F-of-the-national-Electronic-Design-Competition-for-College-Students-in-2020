# 2020年全国大学生电子设计竞赛F题简易无接触温度测量及身份识别装置

## 演示视频

[演示视频](https://www.bilibili.com/video/BV1Wf4y1B76i)

* 开发环境 IAR7.2 for msp430f5529
* 运行平台 msp430f5529  

### LMT70：直接使用的ADC 引脚  P6.1  

### MLX90614&emsp;&emsp;I2C0:&ensp;SDA&emsp;SCL
### &emsp;&emsp;&emsp;&emsp;&emsp;&ensp;P3.0&emsp;P3.1
### &emsp;&emsp;&emsp;&emsp;&emsp;&ensp;P4.1&emsp;P4.2  

### OLED(本程序使用LCD)   D0    D1    RES    DC
###                      P4.3  P4.0  P3.7   P8.2  

### UCA1                 TX    RX
###                      P4.4   P4.5  

### LCD                  SCL   SDI   CS    RST   DC
###                      P3.2  P2.2  P2.7  P2.4  P2.0  

## 引脚可根据需求设置
* LMT70数据的采集主要在 LMT70.c 与 LMT70.h 中 
* MLX90614数据的采集主要在 msp430f5529_i2c.c 末尾处
* 滤波算法主要在 Filter.c 与 Filter.h 中