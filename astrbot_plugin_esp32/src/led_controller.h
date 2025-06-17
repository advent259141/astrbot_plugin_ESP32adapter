#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

class LedController {
private:
    int pin;
    bool state;
    int brightness;  // 亮度百分比 (0-100)
    int pwmChannel;

public:
    LedController(int ledPin, int channel = 0);
    void init();
    void setState(bool on);
    void setBrightness(int brightness);
    void toggle();
    bool getState() const;
    int getBrightness() const;
    String getStatusString() const;
};

#endif
