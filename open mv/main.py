# Hello World Example
#
# Welcome to the OpenMV IDE! Click on the green run arrow button below to run the script!
from pyb import millis
from math import pi, isnan
import sensor, time, image, math, pyb, lcd
import json
from pyb import UART
from pyb import Servo
RED_LED_PIN = 1
BLUE_LED_PIN = 3

TS = 3

lcd.init()

threshold_index = 0 # threshold_index的数值对应了列表thresholds[]中的色块LAB

# 口罩的LAB数值写入列表thresholds[]中 支持写入多组
thresholds = [(30, 66, -2, -32, -29, -11),  #口罩1
              (30, 66, -2, -32, -29, -11),  #粉色
              (30, 66, -2, -32, -29, -11)]  #浅黄
sensor.set_auto_gain(False)      # 关闭白平衡
sensor.set_auto_whitebal(False)  # 关闭自动增益
clock = time.clock()


uart = UART(3, 115200)

class collect_face:
    def collect(self):
        sensor.reset() # Initialize the camera sensor.
        sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
        sensor.set_framesize(sensor.B128X128) # or sensor.QQVGA (or others)
        sensor.set_windowing((92,112))
        sensor.skip_frames(10) # Let new settings take affect.
        sensor.skip_frames(time = 2000)
        num = 4 #设置被拍摄者序号，第一个人的图片保存到s1文件夹，第二个人的图片保存到s2文件夹，以此类推。每次更换拍摄者时，修改num值。
        n = 17 #设置每个人拍摄图片数量。
        #连续拍摄n张照片，每间隔3s拍摄一次。
        while(n):
            #红灯亮
            pyb.LED(RED_LED_PIN).on()
            sensor.skip_frames(time = 3000) # Give the user time to get ready.等待3s，准备一下表情。
            #红灯灭，蓝灯亮
            pyb.LED(RED_LED_PIN).off()
            pyb.LED(BLUE_LED_PIN).on()
            #保存截取到的图片到SD卡
            print(n)
            sensor.snapshot().save("singtown/s%s/%s.pgm" % (num, n) ) # or "example.bmp" (or others)
            n -= 1
            pyb.LED(BLUE_LED_PIN).off()
            print("Done! Reset the camera to see the saved image.")

class face_rendering:

    #def __init__(self)
        #self.num =0
    def min(self,pmin, a, s):
        if a<pmin:
            pmin=a
            self.num=s
        return pmin
    def start_face_rendering(self):
        sensor.reset() # Initialize the camera sensor.
        sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
        sensor.set_framesize(sensor.B128X128) # or sensor.QQVGA (or others)
        sensor.set_windowing((92,112))
        sensor.skip_frames(10) # Let new settings take affect.
        sensor.skip_frames(time = 5000) #等待5s
        s3 = Servo(3) # servo on position 1 (P7)
        #将蓝灯赋值给变量led
        led = pyb.LED(3) # Red LED = 1, Green LED = 2, Blue LED = 3, IR LEDs = 4.
        #SUB = "s1"
        NUM_SUBJECTS = 4 #图像库中不同人数，一共6人
        NUM_SUBJECTS_IMGS = 17 #每人有20张样本图片
        # 拍摄当前人脸。
        img = sensor.snapshot()
        #img = image.Image("singtown/%s/1.pgm"%(SUB))
        d0 = img.find_lbp((0, 0, img.width(), img.height()))
        #d0为当前人脸的lbp特征
        img = None
        pmin = 999999
        self.num=0

        for s in range(1, NUM_SUBJECTS+1):
            dist = 0
            for i in range(2, NUM_SUBJECTS_IMGS+1):
                img = image.Image("singtown/s%d/%d.pgm"%(s, i))
                d1 = img.find_lbp((0, 0, img.width(), img.height()))
                #d1为第s文件夹中的第i张图片的lbp特征
                dist += image.match_descriptor(d0, d1)#计算d0 d1即样本图像与被检测人脸的特征差异度。
            print("Average dist for subject %d: %d"%(s, dist/NUM_SUBJECTS_IMGS))
            pmin = self.min(pmin, dist/NUM_SUBJECTS_IMGS, s)#特征差异度越小，被检测人脸与此样本更相似更匹配。
            print(pmin)

        print(self.num) # num为当前最匹配的人的编号。
        #TS=3 没戴口罩
        if (pmin>5000) & (TS==3):
            uart.write("-- NO People! --")
            led.off()
        if (pmin>5000) & (TS==1):

            uart.write("-- NO People! --")
            led.off()
        if pmin<=5000:
            if self.num==1:     #匹配到了people_One
                    uart.write("People One      ")
            if self.num==2:
                    uart.write("People Two      ")
            if self.num==3:
                    uart.write("People Three    ")
            if self.num==4:
                    uart.write("People New      ")
            led.on()            #亮灯
            led1.off()
            time.sleep(3500)     #延时1500ms
            led.off()
            for i in range(1,460):
                s3.speed(50) # for continuous rotation servos
                time.sleep(15)
            s3.speed(0)
            time.sleep(1500)
            for i in range(1,230):
                s3.speed(-50)
                time.sleep(15)
            s3.speed(0)

t = collect_face()
y = face_rendering(0)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
led1 = pyb.LED(1)
c= "hg"
while(1):
    TEXT1 = 3
    TS = TEXT1
    clock.tick()
    img = sensor.snapshot()
    blob = img.find_blobs([thresholds[threshold_index]], area_threshold=300)
    if blob: #如果找到了目标颜色
        uart.write("6")
        TEXT1 = 1
        TS = TEXT1
        for b in blob:
            #迭代找到的目标颜色区域
            img.draw_rectangle(b[0:4]) # rect
            img.draw_cross(b[5], b[6]) # cx, cy
    if TEXT1 == 3:
        img.draw_string(120, 100, "NO MASK!!!!")
        img.draw_string(120, 115, "WEAR MASK!!!")
        uart.write("5")
        lcd.display(img)
    lcd.display(img)

    if uart.any():
        c=uart.readline()
        print(c[0])

    #c= '1'
    if c[0]== 49:
        print("gh")
        led1.on()
        y.start_face_rendering()
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QVGA)
    if c[0]== 50:
        print("gh")
        t.collect()
        uart.write("Study YES!      ")
        sensor.reset()
        sensor.set_pixformat(sensor.RGB565)
        sensor.set_framesize(sensor.QVGA)
       # led.on()
    '''
    if c[0]== 51:
        print("gh")
        clock.tick()
        img = sensor.snapshot()
        blob = img.find_blobs([thresholds[threshold_index]], area_threshold=300)
        text = 2;
        if blob: #如果找到了口罩
            text = 1
            if text == 1:
                uart.write("Mask YES      ")
        if text == 2:#如果没有找到口罩
            uart.write("Mask ON      ")
    '''
    c="51"
    lcd.display(sensor.snapshot())

