#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "led_controller.h"
#include "servo_controller.h"
#include "oled_display.h"
#include "websocket_client.h"

class MessageHandler {
private:
    LedController* ledController;
    ServoController* servoController;
    OledDisplay* oledDisplay;
    WebSocketClientManager* wsClient;

public:
    MessageHandler(LedController* led, ServoController* servo, OledDisplay* oled, WebSocketClientManager* ws);
    void handleMessage(String message);
    
private:
    void handleWelcomeMessage(JsonDocument& doc);
    void handleAstrBotMessage(JsonDocument& doc);
    void handleCustomCommand(JsonDocument& doc);
    void handleLedControl(JsonDocument& doc);
    void handleServoControl(JsonDocument& doc);
    void handleOledControl(JsonDocument& doc);
    void handleHeartbeatAck(JsonDocument& doc);
    
    void processCustomCommand(String command);
    void processTextCommands(String messageText);
};

#endif
