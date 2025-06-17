#include "servo_controller.h"

ServoController::ServoController() 
    : leftPin(39), rightPin(38), currentLeftAngle(90), currentRightAngle(90) {
}

void ServoController::init(int leftLegPin, int rightLegPin) {
    leftPin = leftLegPin;
    rightPin = rightLegPin;
    
    Serial.begin(9600);
    Serial.println("开始初始化舵机腿部控制...");
    
    // 将舵机连接到指定引脚
    servoLeft.attach(leftPin);
    servoRight.attach(rightPin);
    
    // 等待舵机连接稳定
    delay(500);
    
    // 设置初始站立姿态
    standUp();
}

void ServoController::moveLeftLeg(int angle) {
    // 限制角度范围
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    servoLeft.write(angle);
    currentLeftAngle = angle;
    
    Serial.println("左腿角度设置为: " + String(angle) + "度");
}

void ServoController::moveRightLeg(int angle) {
    // 限制角度范围
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    servoRight.write(angle);
    currentRightAngle = angle;
    
    Serial.println("右腿角度设置为: " + String(angle) + "度");
}

void ServoController::moveLegs(int leftAngle, int rightAngle) {
    moveLeftLeg(leftAngle);
    moveRightLeg(rightAngle);
}

void ServoController::standUp() {
    Serial.println("设置舵机初始站立角度...");
    
    // 左腿初始化为180度，右腿初始化为0度
    moveLeftLeg(180);
    moveRightLeg(0);
    delay(1000);
    
    // 然后移动到中心站立位置
    moveLeftLeg(90);
    moveRightLeg(90);
    delay(1000);
    
    Serial.println("站立完成");
}

void ServoController::walkForward() {
    Serial.println("开始前进步态...");
    
    // 第一步：右腿前进，左腿后退
    Serial.println("右腿前进，左腿后退");
    moveRightLeg(45);   // 右腿前进到中间位置
    moveLeftLeg(45);    // 左腿后退到中间位置
    delay(500);
    
    // 第二步：右腿继续前进，左腿继续后退
    moveRightLeg(30);   // 右腿更多前进
    moveLeftLeg(30);    // 左腿更多后退
    delay(500);
    
    // 第三步：切换方向 - 右腿后退，左腿前进
    Serial.println("右腿后退，左腿前进");
    moveRightLeg(120);  // 右腿开始后退
    moveLeftLeg(120);   // 左腿开始前进
    delay(500);
    
    // 第四步：右腿继续后退，左腿继续前进
    moveRightLeg(150);  // 右腿更多后退
    moveLeftLeg(150);   // 左腿更多前进
    delay(500);
    
    // 回到中间位置准备下一个循环
    Serial.println("回到中间位置");
    moveRightLeg(90);   // 右腿中间位置
    moveLeftLeg(90);    // 左腿中间位置
    delay(500);
    
    Serial.println("一个前进步态循环完成");
}

void ServoController::walkBackward() {
    Serial.println("开始后退步态...");
    
    // 后退步态与前进相反
    // 第一步：左腿前进，右腿后退
    Serial.println("左腿前进，右腿后退");
    moveLeftLeg(120);   // 左腿前进
    moveRightLeg(120);  // 右腿后退
    delay(500);
    
    moveLeftLeg(150);   // 左腿更多前进
    moveRightLeg(150);  // 右腿更多后退
    delay(500);
    
    // 第二步：切换方向
    Serial.println("左腿后退，右腿前进");
    moveLeftLeg(45);    // 左腿后退
    moveRightLeg(45);   // 右腿前进
    delay(500);
    
    moveLeftLeg(30);    // 左腿更多后退
    moveRightLeg(30);   // 右腿更多前进
    delay(500);
    
    // 回到中间位置
    moveLeftLeg(90);
    moveRightLeg(90);
    delay(500);
    
    Serial.println("一个后退步态循环完成");
}

void ServoController::stopWalk() {
    Serial.println("停止步行，回到站立位置");
    moveLeftLeg(90);
    moveRightLeg(90);
    delay(1000);
}

void ServoController::leftLegForward() {
    // 左腿前进：从90度到180度
    for(int angle = currentLeftAngle; angle <= 180; angle += 5) {
        moveLeftLeg(angle);
        delay(50);
    }
}

void ServoController::leftLegBackward() {
    // 左腿后退：从90度到0度
    for(int angle = currentLeftAngle; angle >= 0; angle -= 5) {
        moveLeftLeg(angle);
        delay(50);
    }
}

void ServoController::rightLegForward() {
    // 右腿前进：从90度到0度
    for(int angle = currentRightAngle; angle >= 0; angle -= 5) {
        moveRightLeg(angle);
        delay(50);
    }
}

void ServoController::rightLegBackward() {
    // 右腿后退：从90度到180度
    for(int angle = currentRightAngle; angle <= 180; angle += 5) {
        moveRightLeg(angle);
        delay(50);
    }
}

int ServoController::getCurrentLeftAngle() {
    return currentLeftAngle;
}

int ServoController::getCurrentRightAngle() {
    return currentRightAngle;
}

String ServoController::getStatusString() const {
    return "左腿角度: " + String(currentLeftAngle) + "度, 右腿角度: " + String(currentRightAngle) + "度";
}

void ServoController::detachServos() {
    servoLeft.detach();
    servoRight.detach();
    Serial.println("舵机已断开连接");
}
