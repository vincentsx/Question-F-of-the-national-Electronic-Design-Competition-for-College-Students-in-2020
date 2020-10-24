#ifndef LMT70_H_
#define LMT70_H_



extern float Temp_adc();                  //转换一次并读取LMT70的结果
extern float Temp_1();                    //一阶线性函数计算温度 适用于20℃-30℃
extern float Temp_2();                    //二阶线性函数计算温度 适用于-55℃-150℃
extern float Temp_3();                    //三阶线性函数计算温度 适用于-55℃-150℃

#endif