#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoController {
private:
    Servo servoLeft;   // 左腿舵机 (引脚39) - 两条左腿并联
    Servo servoRight;  // 右腿舵机 (引脚38) - 两条右腿并联
    int leftPin, rightPin;
    int currentLeftAngle, currentRightAngle;
    
public:
    ServoController();
    void init(int leftLegPin = 39, int rightLegPin = 38);
    void moveLeftLeg(int angle);
    void moveRightLeg(int angle);
    void moveLegs(int leftAngle, int rightAngle);
    void standUp();          // 站立姿态
    void walkForward();      // 前进步态循环
    void walkBackward();     // 后退步态循环
    void stopWalk();         // 停止并回到中心位置
    void leftLegForward();   // 左腿前进 (180-90度)
    void leftLegBackward();  // 左腿后退 (90-0度)
    void rightLegForward();  // 右腿前进 (0-90度)
    void rightLegBackward(); // 右腿后退 (90-180度)
    int getCurrentLeftAngle();
    int getCurrentRightAngle();
    String getStatusString() const;
    void detachServos();
};

#endif
