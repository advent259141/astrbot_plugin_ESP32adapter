#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OledDisplay {
private:
    Adafruit_SSD1306 display;
    int screenWidth;
    int screenHeight;
    int sdaPin;
    int sclPin;
    int screenAddress;
    bool initialized;

    // 表情绘制私有方法
    void drawHappyFace();
    void drawSadFace();
    void drawAngryFace();
    void drawSurprisedFace();
    void drawSleepyFace();
    void drawHeartEyes();
    void drawCoolFace();
    void drawThinkingFace();

public:
    OledDisplay(int width, int height, int sda, int scl, int address);
    bool init();
    void displayEmotion(String emotion);
    void displayText(String text);
    void clear();
    bool isInitialized() const;
};

#endif
