#include "led_controller.h"

LedController::LedController(int ledPin, int channel) 
    : pin(ledPin), pwmChannel(channel), state(false), brightness(100) {
}

void LedController::init() {
    pinMode(pin, OUTPUT);
    
    // 配置PWM通道 (1000Hz, 8位分辨率)
    ledcSetup(pwmChannel, 1000, 8);
    ledcAttachPin(pin, pwmChannel);
    
    // 初始状态：LED关闭
    setState(false);
}

void LedController::setState(bool on) {
    state = on;
    if (on) {
        setBrightness(brightness);
    } else {
        setBrightness(0);
    }
}

void LedController::setBrightness(int newBrightness) {
    // 限制亮度范围
    if (newBrightness < 0) newBrightness = 0;
    if (newBrightness > 100) newBrightness = 100;
    
    if (state || newBrightness == 0) {
        // 转换为PWM值 (0-255)
        int pwmValue = map(newBrightness, 0, 100, 0, 255);
        
        // 设置PWM输出
        ledcWrite(pwmChannel, pwmValue);
        
        Serial.println("LED亮度设置为: " + String(newBrightness) + "% (PWM: " + String(pwmValue) + ")");
    }
    
    if (newBrightness > 0) {
        brightness = newBrightness;
        state = true;
    }
}

void LedController::toggle() {
    setState(!state);
}

bool LedController::getState() const {
    return state;
}

int LedController::getBrightness() const {
    return brightness;
}

String LedController::getStatusString() const {
    if (state) {
        return "LED当前状态：点亮，亮度" + String(brightness) + "%";
    } else {
        return "LED当前状态：关闭";
    }
}
