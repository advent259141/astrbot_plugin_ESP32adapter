#include <Arduino.h>
#include <ESP32Servo.h>

// 舵机对象定义
Servo servoLeft;   // 引脚39的舵机（左腿）
Servo servoRight;  // 引脚38的舵机（右腿）

// 引脚定义
const int SERVO_PIN_LEFT = 39;
const int SERVO_PIN_RIGHT = 38;

void setup() {
    // 初始化串口，用于调试输出
    Serial.begin(9600);
    Serial.println("开始初始化步行机器人舵机...");
    
    // 将舵机连接到指定引脚
    servoLeft.attach(SERVO_PIN_LEFT);
    servoRight.attach(SERVO_PIN_RIGHT);
    
    // 等待舵机连接稳定
    delay(500);
    
    // 设置初始角度
    Serial.println("设置舵机初始角度...");
    servoLeft.write(180);   // 左腿初始化为180度
    servoRight.write(0);    // 右腿初始化为0度
    
    // 等待舵机转动到指定位置
    delay(1000);
    
    servoLeft.write(90);
    servoRight.write(90); 
    delay(1000);
    Serial.println("站立完成");
}

void loop() {
    // 四足前进步态
    Serial.println("开始前进...");
    
    // 第一步：右腿前进(0-90度)，左腿后退(90-0度)
    Serial.println("右腿前进，左腿后退");
    servoRight.write(45);   // 右腿前进到中间位置
    servoLeft.write(45);    // 左腿后退到中间位置
    delay(500);
    
    // 第二步：右腿继续前进，左腿继续后退
    servoRight.write(30);   // 右腿更多前进
    servoLeft.write(30);    // 左腿更多后退
    delay(500);
    
    // 第三步：切换方向 - 右腿后退(90-180度)，左腿前进(90-180度)
    Serial.println("右腿后退，左腿前进");
    servoRight.write(120);  // 右腿开始后退
    servoLeft.write(120);   // 左腿开始前进
    delay(500);
    
    // 第四步：右腿继续后退，左腿继续前进
    servoRight.write(150);  // 右腿更多后退
    servoLeft.write(150);   // 左腿更多前进
    delay(500);
    
    // 回到中间位置准备下一个循环
    Serial.println("回到中间位置");
    servoRight.write(90);   // 右腿中间位置
    servoLeft.write(90);    // 左腿中间位置
    delay(500);
    
    Serial.println("一个步态循环完成");
    delay(1000);
}
