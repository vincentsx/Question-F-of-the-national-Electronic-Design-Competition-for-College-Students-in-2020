# 2020年全国大学生电子设计竞赛F题简易无接触温度测量及身份识别装置

## 演示视频

[演示视频](https://www.bilibili.com/video/BV1Wf4y1B76i)

* 开发环境 IAR7.2 for msp430f5529

* 运行平台 msp430f5529   

![]([https://github.com/rtrrsr/Question-F-of-the-national-Electronic-Design-Competition-for-College-Students-in-2020/blob/master/1.jpg))  

## 引脚可根据需求设置
* LMT70数据的采集主要在 LMT70.c 与 LMT70.h 中 
* MLX90614数据的采集主要在 msp430f5529_i2c.c 末尾处
* 滤波算法主要在 Filter.c 与 Filter.h 中